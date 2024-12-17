// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/common/constants.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/functional_blocks/reservation.hpp>

namespace ocpp::v201 {
Reservation::Reservation(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                         EvseManagerInterface& evse_manager, ReserveNowCallback reserve_now_callback,
                         CancelReservationCallback cancel_reservation_callback,
                         const IsReservationForTokenCallback is_reservation_for_token_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    evse_manager(evse_manager),
    reserve_now_callback(reserve_now_callback),
    cancel_reservation_callback(cancel_reservation_callback),
    is_reservation_for_token_callback(is_reservation_for_token_callback) {
}

void Reservation::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    switch (message.messageType) {
    case MessageType::ReserveNow:
        this->handle_reserve_now_request(json_message);
        break;
    case MessageType::CancelReservation:
        this->handle_cancel_reservation_callback(json_message);
        break;
    default:
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void Reservation::on_reservation_status(const int32_t reservation_id, const ReservationUpdateStatusEnum status) {
    ReservationStatusUpdateRequest req;
    req.reservationId = reservation_id;
    req.reservationUpdateStatus = status;

    ocpp::Call<ReservationStatusUpdateRequest> call(req);
    this->message_dispatcher.dispatch_call(call);
}

ocpp::ReservationCheckStatus
Reservation::is_evse_reserved_for_other(const EvseInterface& evse, const IdToken& id_token,
                                        const std::optional<IdToken>& group_id_token) const {
    const std::optional<CiString<36>> no = std::nullopt;
    const std::optional<CiString<36>> groupIdToken = group_id_token.has_value() ? group_id_token.value().idToken : no;

    return this->is_reservation_for_token_callback(evse.get_id(), id_token.idToken, groupIdToken);
}

void Reservation::on_reserved(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager.get_evse(evse_id).submit_event(connector_id, ConnectorEvent::Reserve);
}

void Reservation::on_reservation_cleared(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager.get_evse(evse_id).submit_event(connector_id, ConnectorEvent::ReservationCleared);
}

void Reservation::handle_reserve_now_request(Call<ReserveNowRequest> call) {
    ReserveNowResponse response;
    response.status = ReserveNowStatusEnum::Rejected;
    bool reservation_available = true;

    std::string status_info;

    if (this->reserve_now_callback == nullptr) {
        reservation_available = false;
        status_info = "Reservation is not implemented";
    } else if (!this->device_model.get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrAvailable)
                    .value_or(false)) {
        status_info = "Reservation is not available";
        reservation_available = false;
    } else if (!this->device_model.get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrEnabled)
                    .value_or(false)) {
        reservation_available = false;
        status_info = "Reservation is not enabled";
    }

    if (!reservation_available) {
        // Reservation not available / implemented, return 'Rejected'.
        // H01.FR.01
        EVLOG_info << "Receiving a reservation request, but reservation is not enabled or implemented.";
        send_reserve_now_rejected_response(call.uniqueId, status_info);
        return;
    }

    // Check if we need a specific evse id during a reservation and if that is the case, if we recevied an evse id.
    const ReserveNowRequest request = call.msg;
    if (!request.evseId.has_value() &&
        !this->device_model.get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrNonEvseSpecific)
             .value_or(false)) {
        // H01.FR.19
        EVLOG_warning << "Trying to make a reservation, but no evse id was given while it should be sent in the "
                         "request when NonEvseSpecific is disabled.";
        send_reserve_now_rejected_response(
            call.uniqueId,
            "No evse id was given while it should be sent in the request when NonEvseSpecific is disabled");
        return;
    }

    const std::optional<int32_t> evse_id = request.evseId;

    if (evse_id.has_value()) {
        if (evse_id <= 0 || !evse_manager.does_evse_exist(evse_id.value())) {
            EVLOG_error << "Trying to make a reservation, but evse " << evse_id.value() << " is not a valid evse id.";
            send_reserve_now_rejected_response(call.uniqueId, "Evse id does not exist");
            return;
        }

        // Check if there is a connector available for this evse id.
        if (!this->evse_manager.does_connector_exist(static_cast<uint32_t>(evse_id.value()),
                                                     request.connectorType.value_or(ConnectorEnum::Unknown))) {
            EVLOG_info << "Trying to make a reservation for connector type "
                       << conversions::connector_enum_to_string(request.connectorType.value_or(ConnectorEnum::Unknown))
                       << " for evse " << evse_id.value() << ", but this connector type does not exist.";
            send_reserve_now_rejected_response(call.uniqueId, "Connector type does not exist");
            return;
        }
    } else {
        // No evse id. Just search for all evse's if there is something available for reservation
        const uint64_t number_of_evses = evse_manager.get_number_of_evses();
        if (number_of_evses == 0) {
            send_reserve_now_rejected_response(call.uniqueId, "No evse's found in charging station");
            EVLOG_error << "Trying to make a reservation, but number of evse's is 0";
            return;
        }

        bool connector_exists = false;
        for (uint64_t i = 1; i <= number_of_evses; i++) {
            if (this->evse_manager.does_connector_exist(i, request.connectorType.value_or(ConnectorEnum::Unknown))) {
                connector_exists = true;
                break;
            }
        }

        if (!connector_exists) {
            send_reserve_now_rejected_response(call.uniqueId, "Could not get status info from connector");
            return;
        }
    }

    // Connector exists and might or might not be available, but if the reservation id is already existing, reservation
    // should be overwritten.

    // Call reserve now callback and wait for the response.
    const ReserveNowRequest reservation_request = call.msg;
    response.status = this->reserve_now_callback(reservation_request);

    // Reply with the response from the callback.
    const ocpp::CallResult<ReserveNowResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if (response.status == ReserveNowStatusEnum::Accepted) {
        EVLOG_debug << "Reservation with id " << reservation_request.id << " for "
                    << (reservation_request.evseId.has_value()
                            ? " evse_id: " + std::to_string(reservation_request.evseId.value())
                            : "")
                    << " is accepted";
    }
}

void Reservation::handle_cancel_reservation_callback(Call<CancelReservationRequest> call) {

    CancelReservationResponse response;
    if (this->cancel_reservation_callback == nullptr ||
        !this->device_model.get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrAvailable)
             .value_or(false) ||
        !this->device_model.get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrEnabled)
             .value_or(false)) {
        // Reservation not available / implemented, return 'Rejected'.
        // H01.FR.01
        EVLOG_info << "Receiving a cancel reservation request, but reservation is not implemented.";
        response.status = CancelReservationStatusEnum::Rejected;
    } else {
        response.status =
            (this->cancel_reservation_callback(call.msg.reservationId) ? CancelReservationStatusEnum::Accepted
                                                                       : CancelReservationStatusEnum::Rejected);
    }

    const ocpp::CallResult<CancelReservationResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void Reservation::send_reserve_now_rejected_response(const MessageId& unique_id, const std::string& status_info) {
    ReserveNowResponse response;
    response.status = ReserveNowStatusEnum::Rejected;
    response.statusInfo = StatusInfo();
    response.statusInfo->additionalInfo = status_info;
    const ocpp::CallResult<ReserveNowResponse> call_result(response, unique_id);
    this->message_dispatcher.dispatch_call_result(call_result);
}

} // namespace ocpp::v201
