// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/functional_blocks/availability.hpp>

#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/evse_manager.hpp>

#include <ocpp/v201/messages/StatusNotification.hpp>

namespace ocpp::v201 {
Availability::Availability(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                           EvseManagerInterface& evse_manager, ComponentStateManagerInterface& component_state_manager,
                           std::optional<TimeSyncCallback> time_sync_callback,
                           std::optional<AllConnectorsUnavailableCallback> all_connectors_unavailable_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    evse_manager(evse_manager),
    component_state_manager(component_state_manager),
    time_sync_callback(time_sync_callback),
    all_connectors_unavailable_callback(all_connectors_unavailable_callback) {
}

Availability::~Availability() {
    this->stop_heartbeat_timer();
}

void Availability::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::ChangeAvailability:
        this->handle_change_availability_req(json_message);
        break;
    case MessageType::HeartbeatResponse:
        this->handle_heartbeat_response(json_message);
        break;
    default:
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void Availability::status_notification_req(const int32_t evse_id, const int32_t connector_id,
                                           const ConnectorStatusEnum status, const bool initiated_by_trigger_message) {
    StatusNotificationRequest req;
    req.connectorId = connector_id;
    req.evseId = evse_id;
    req.timestamp = DateTime();
    req.connectorStatus = status;

    ocpp::Call<StatusNotificationRequest> call(req);
    this->message_dispatcher.dispatch_call(call, initiated_by_trigger_message);
}

void Availability::heartbeat_req(const bool initiated_by_trigger_message) {
    HeartbeatRequest req;

    heartbeat_request_time = std::chrono::steady_clock::now();
    ocpp::Call<HeartbeatRequest> call(req);
    this->message_dispatcher.dispatch_call(call, initiated_by_trigger_message);
}

void Availability::handle_scheduled_change_availability_requests(const int32_t evse_id) {
    if (this->scheduled_change_availability_requests.count(evse_id)) {
        EVLOG_info << "Found scheduled ChangeAvailability.req for evse_id:" << evse_id;
        const auto req = this->scheduled_change_availability_requests[evse_id].request;
        const auto persist = this->scheduled_change_availability_requests[evse_id].persist;
        if (!this->evse_manager.any_transaction_active(req.evse)) {
            EVLOG_info << "Changing availability of evse:" << evse_id;
            this->execute_change_availability_request(req, persist);
            this->scheduled_change_availability_requests.erase(evse_id);
            // Check succeeded, trigger the callback if needed
            if (this->all_connectors_unavailable_callback.has_value() and
                this->evse_manager.are_all_connectors_effectively_inoperative()) {
                this->all_connectors_unavailable_callback.value()();
            }
        } else {
            EVLOG_info << "Cannot change availability because transaction is still active";
        }
    }
}

void Availability::set_scheduled_change_availability_requests(const int32_t evse_id,
                                                              AvailabilityChange availability_change) {
    this->scheduled_change_availability_requests[evse_id] = availability_change;
}

void Availability::set_heartbeat_timer_interval(const std::chrono::seconds& interval) {
    this->heartbeat_timer.interval([this]() { this->heartbeat_req(); }, interval);
}

void Availability::stop_heartbeat_timer() {
    this->heartbeat_timer.stop();
}

void Availability::handle_change_availability_req(Call<ChangeAvailabilityRequest> call) {
    const auto msg = call.msg;
    ChangeAvailabilityResponse response;
    response.status = ChangeAvailabilityStatusEnum::Scheduled;

    // Sanity check: if we're addressing an EVSE or a connector, it must actually exist
    if (msg.evse.has_value() and !this->evse_manager.is_valid_evse(msg.evse.value())) {
        EVLOG_warning << "CSMS requested ChangeAvailability for invalid evse id or connector id";
        response.status = ChangeAvailabilityStatusEnum::Rejected;
        ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
        this->message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // Check if we have any transaction running on the EVSE (or any EVSE if we're addressing the whole CS)
    const auto transaction_active = this->evse_manager.any_transaction_active(msg.evse);
    // Check if we're already in the requested state
    const auto is_already_in_state = this->is_already_in_state(msg);

    // evse_id will be 0 if we're addressing the whole CS, and >=1 otherwise
    auto evse_id = 0;
    if (msg.evse.has_value()) {
        evse_id = msg.evse.value().id;
    }

    if (!transaction_active or is_already_in_state or
        (evse_id == 0 and msg.operationalStatus == OperationalStatusEnum::Operative)) {
        // If the chosen EVSE (or CS) has no transactions, we're already in the desired state,
        // or we're telling the whole CS to power on, we can accept the request - there's nothing stopping us.
        response.status = ChangeAvailabilityStatusEnum::Accepted;
        // Remove any scheduled availability requests for the evse_id.
        // This is relevant in case some of those requests become activated later - the current one overrides them.
        this->scheduled_change_availability_requests.erase(evse_id);
    } else {
        // We can't immediately perform the change, because we have a transaction running.
        // Schedule the request to run when the transaction finishes.
        this->scheduled_change_availability_requests[evse_id] = {msg, true};
    }

    // Respond to the CSMS before performing any changes to avoid StatusNotification.req being sent before
    // the ChangeAvailabilityResponse.
    ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if (!transaction_active) {
        // No transactions - execute the change now
        this->execute_change_availability_request(msg, true);
    } else if (response.status == ChangeAvailabilityStatusEnum::Scheduled) {
        // We can't execute the change now, but it's scheduled to run after transactions are finished.
        if (evse_id == 0) {
            // The whole CS is being addressed - we need to prevent further transactions from starting.
            // To do that, make all EVSEs without an active transaction Inoperative
            for (auto const& evse : this->evse_manager) {
                if (!evse.has_active_transaction()) {
                    // FIXME: This will linger after the update too! We probably need another mechanism...
                    this->set_evse_operative_status(evse.get_id(), OperationalStatusEnum::Inoperative, false);
                }
            }
        } else {
            // A single EVSE is being addressed. We need to prevent further transactions from starting on it.
            // To do that, make all connectors of the EVSE without an active transaction Inoperative.
            int number_of_connectors = this->evse_manager.get_evse(evse_id).get_number_of_connectors();
            for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
                if (!this->evse_manager.get_evse(evse_id).has_active_transaction(connector_id)) {
                    // FIXME: This will linger after the update too! We probably need another mechanism...
                    this->set_connector_operative_status(evse_id, connector_id, OperationalStatusEnum::Inoperative,
                                                         false);
                }
            }
        }
    }
}

void Availability::handle_heartbeat_response(CallResult<HeartbeatResponse> call) {
    if (this->time_sync_callback.has_value() and
        this->device_model.get_value<std::string>(ControllerComponentVariables::TimeSource).find("Heartbeat") !=
            std::string::npos) {
        // the received currentTime was the time the CSMS received the heartbeat request
        // to get a system time as accurate as possible keep the time-of-flight into account
        auto timeOfFlight = (std::chrono::steady_clock::now() - this->heartbeat_request_time) / 2;
        ocpp::DateTime currentTimeCompensated(call.msg.currentTime.to_time_point() + timeOfFlight);
        this->time_sync_callback.value()(currentTimeCompensated);
    }
}

bool Availability::is_already_in_state(const ChangeAvailabilityRequest& request) {
    // TODO: This checks against the individual status setting. What about effective/persisted status?
    if (!request.evse.has_value()) {
        // We're addressing the whole charging station
        return (this->component_state_manager.get_cs_individual_operational_status() == request.operationalStatus);
    }
    if (!request.evse.value().connectorId.has_value()) {
        // An EVSE is addressed
        return (this->component_state_manager.get_evse_individual_operational_status(request.evse.value().id) ==
                request.operationalStatus);
    }
    // A connector is being addressed
    return (this->component_state_manager.get_connector_individual_operational_status(
                request.evse.value().id, request.evse.value().connectorId.value()) == request.operationalStatus);
}

void Availability::execute_change_availability_request(ChangeAvailabilityRequest request, bool persist) {
    if (request.evse.has_value()) {
        if (request.evse.value().connectorId.has_value()) {
            this->set_connector_operative_status(request.evse.value().id, request.evse.value().connectorId.value(),
                                                 request.operationalStatus, persist);
        } else {
            this->set_evse_operative_status(request.evse.value().id, request.operationalStatus, persist);
        }
    } else {
        this->set_cs_operative_status(request.operationalStatus, persist);
    }
}

void Availability::set_cs_operative_status(OperationalStatusEnum new_status, bool persist) {
    this->component_state_manager.set_cs_individual_operational_status(new_status, persist);
}

void Availability::set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist) {
    this->evse_manager.get_evse(evse_id).set_evse_operative_status(new_status, persist);
}

void Availability::set_connector_operative_status(int32_t evse_id, int32_t connector_id,
                                                  OperationalStatusEnum new_status, bool persist) {
    this->evse_manager.get_evse(evse_id).set_connector_operative_status(connector_id, new_status, persist);
}
} // namespace ocpp::v201
