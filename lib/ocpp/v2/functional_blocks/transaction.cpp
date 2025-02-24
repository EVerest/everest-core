// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/transaction.hpp>

#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/utils.hpp>

#include <ocpp/v2/functional_blocks/authorization.hpp>
#include <ocpp/v2/functional_blocks/availability.hpp>
#include <ocpp/v2/functional_blocks/smart_charging.hpp>
#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>

#include <ocpp/v2/messages/GetTransactionStatus.hpp>
#include <ocpp/v2/messages/TransactionEvent.hpp>

namespace ocpp::v2 {
TransactionBlock::TransactionBlock(
    MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
    ConnectivityManagerInterface& connectivity_manager, EvseManagerInterface& evse_manager,
    MessageQueue<v2::MessageType>& message_queue, DatabaseHandlerInterface& database_handler,
    AuthorizationInterface& authorization, AvailabilityInterface& availability, SmartChargingInterface& smart_charging,
    TariffAndCostInterface& tariff_and_cost, StopTransactionCallback stop_transaction_callback,
    PauseChargingCallback pause_charging_callback, std::optional<TransactionEventCallback> transaction_event_callback,
    std::optional<TransactionEventResponseCallback> transaction_event_response_callback, ResetCallback reset_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    connectivity_manager(connectivity_manager),
    evse_manager(evse_manager),
    message_queue(message_queue),
    database_handler(database_handler),
    authorization(authorization),
    availability(availability),
    smart_charging(smart_charging),
    tariff_and_cost(tariff_and_cost),
    stop_transaction_callback(stop_transaction_callback),
    pause_charging_callback(pause_charging_callback),
    transaction_event_callback(transaction_event_callback),
    transaction_event_response_callback(transaction_event_response_callback),
    reset_callback(reset_callback),
    remote_start_id_per_evse{},
    reset_scheduled(false),
    reset_scheduled_evseids{} {
}

void TransactionBlock::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::TransactionEventResponse) {
        this->handle_transaction_event_response(message);
    } else if (message.messageType == MessageType::GetTransactionStatus) {
        this->handle_get_transaction_status(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void TransactionBlock::on_transaction_started(
    const int32_t evse_id, const int32_t connector_id, const std::string& session_id, const DateTime& timestamp,
    const TriggerReasonEnum trigger_reason, const MeterValue& meter_start, const std::optional<IdToken>& id_token,
    const std::optional<IdToken>& group_id_token, const std::optional<int32_t>& reservation_id,
    const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state) {
    auto& evse_handle = this->evse_manager.get_evse(evse_id);
    evse_handle.open_transaction(session_id, connector_id, timestamp, meter_start, id_token, group_id_token,
                                 reservation_id, charging_state);

    const auto meter_value = utils::get_meter_value_with_measurands_applied(
        meter_start, utils::get_measurands_vec(this->device_model.get_value<std::string>(
                         ControllerComponentVariables::SampledDataTxStartedMeasurands)));

    const auto& enhanced_transaction = evse_handle.get_transaction();
    Transaction transaction{enhanced_transaction->transactionId};
    transaction.chargingState = charging_state;
    if (remote_start_id.has_value()) {
        transaction.remoteStartId = remote_start_id.value();
        enhanced_transaction->remoteStartId = remote_start_id.value();
    }

    EVSE evse{evse_id};
    evse.connectorId.emplace(connector_id);

    std::optional<std::vector<MeterValue>> opt_meter_value;
    if (!meter_value.sampledValue.empty()) {
        opt_meter_value.emplace(1, meter_value);
    }

    this->transaction_event_req(TransactionEventEnum::Started, timestamp, transaction, trigger_reason,
                                enhanced_transaction->get_seq_no(), std::nullopt, evse, id_token, opt_meter_value,
                                std::nullopt, !this->connectivity_manager.is_websocket_connected(), reservation_id);
}

void TransactionBlock::on_transaction_finished(const int32_t evse_id, const DateTime& timestamp,
                                               const MeterValue& meter_stop, const ReasonEnum reason,
                                               const TriggerReasonEnum trigger_reason,
                                               const std::optional<IdToken>& id_token,
                                               const std::optional<std::string>& signed_meter_value,
                                               const ChargingStateEnum charging_state) {
    auto& evse_handle = this->evse_manager.get_evse(evse_id);
    auto& enhanced_transaction = evse_handle.get_transaction();
    if (enhanced_transaction == nullptr) {
        EVLOG_warning << "Received notification of finished transaction while no transaction was active";
        return;
    }

    enhanced_transaction->chargingState = charging_state;
    evse_handle.close_transaction(timestamp, meter_stop, reason);
    const auto transaction = enhanced_transaction->get_transaction();
    const auto transaction_id = enhanced_transaction->transactionId.get();

    std::optional<std::vector<ocpp::v2::MeterValue>> meter_values = std::nullopt;
    try {
        meter_values = std::make_optional(utils::get_meter_values_with_measurands_applied(
            this->database_handler.transaction_metervalues_get_all(enhanced_transaction->transactionId.get()),
            utils::get_measurands_vec(
                this->device_model.get_value<std::string>(ControllerComponentVariables::SampledDataTxEndedMeasurands)),
            utils::get_measurands_vec(
                this->device_model.get_value<std::string>(ControllerComponentVariables::AlignedDataTxEndedMeasurands)),
            timestamp,
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::SampledDataSignReadings)
                .value_or(false),
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::AlignedDataSignReadings)
                .value_or(false)));

        if (meter_values.value().empty()) {
            meter_values.reset();
        }
    } catch (const common::DatabaseException& e) {
        EVLOG_warning << "Could not get metervalues of transaction: " << e.what();
    }

    // E07.FR.02 The field idToken is provided when the authorization of the transaction has been ended
    const std::optional<IdToken> transaction_id_token =
        trigger_reason == ocpp::v2::TriggerReasonEnum::StopAuthorized ? id_token : std::nullopt;

    this->transaction_event_req(TransactionEventEnum::Ended, timestamp, enhanced_transaction->get_transaction(),
                                trigger_reason, enhanced_transaction->get_seq_no(), std::nullopt, std::nullopt,
                                transaction_id_token, meter_values, std::nullopt,
                                !this->connectivity_manager.is_websocket_connected(), std::nullopt);

    // K02.FR.05 The transaction is over, so delete the TxProfiles associated with the transaction.
    smart_charging.delete_transaction_tx_profiles(enhanced_transaction->get_transaction().transactionId);
    evse_handle.release_transaction();

    bool send_reset = false;
    if (this->reset_scheduled) {
        // Check if this evse needs to be reset or set to inoperative.
        if (!this->reset_scheduled_evseids.empty()) {
            // There is an evse id in the 'reset scheduled' list, it needs to be
            // reset because it has finished charging.
            if (this->reset_scheduled_evseids.find(evse_id) != this->reset_scheduled_evseids.end()) {
                send_reset = true;
            }
        } else {
            // No evse id is given, whole charging station needs a reset. Wait
            // for last evse id to stop charging.
            bool is_charging = false;
            for (auto const& evse : this->evse_manager) {
                if (evse.has_active_transaction()) {
                    is_charging = true;
                    break;
                }
            }

            if (is_charging) {
                set_evse_connectors_unavailable(evse_handle, false);
            } else {
                send_reset = true;
            }
        }
    }

    if (send_reset) {
        // Reset evse.
        if (reset_scheduled_evseids.empty()) {
            // This was the last evse that was charging, whole charging station
            // should be rest, send reset.
            this->reset_callback(std::nullopt, ResetEnum::OnIdle);
            this->reset_scheduled = false;
        } else {
            // Reset evse that just stopped the transaction.
            this->reset_callback(evse_id, ResetEnum::OnIdle);
            // Remove evse id that is just reset.
            this->reset_scheduled_evseids.erase(evse_id);

            // Check if there are more evse's that should be reset.
            if (reset_scheduled_evseids.empty()) {
                // No other evse's should be reset
                this->reset_scheduled = false;
            }
        }

        this->reset_scheduled_evseids.erase(evse_id);
    }

    this->availability.handle_scheduled_change_availability_requests(evse_id);
    this->availability.handle_scheduled_change_availability_requests(0);
}

void TransactionBlock::transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                                             const Transaction& transaction, const TriggerReasonEnum& trigger_reason,
                                             const int32_t seq_no, const std::optional<int32_t>& cable_max_current,
                                             const std::optional<EVSE>& evse, const std::optional<IdToken>& id_token,
                                             const std::optional<std::vector<MeterValue>>& meter_value,
                                             const std::optional<int32_t>& number_of_phases_used, const bool offline,
                                             const std::optional<int32_t>& reservation_id,
                                             const bool initiated_by_trigger_message) {
    TransactionEventRequest req;
    req.eventType = event_type;
    req.timestamp = timestamp;
    req.transactionInfo = transaction;
    req.triggerReason = trigger_reason;
    req.seqNo = seq_no;
    req.cableMaxCurrent = cable_max_current;
    req.evse = evse;
    req.idToken = id_token;
    req.meterValue = meter_value;
    req.numberOfPhasesUsed = number_of_phases_used;
    req.offline = offline;
    req.reservationId = reservation_id;

    ocpp::Call<TransactionEventRequest> call(req);

    // Check if id token is in the remote start map, because when a remote
    // start request is done, the first transaction event request should
    // always contain trigger reason 'RemoteStart'.
    auto it = std::find_if(
        remote_start_id_per_evse.begin(), remote_start_id_per_evse.end(),
        [&id_token, &evse](const std::pair<int32_t, std::pair<IdToken, int32_t>>& remote_start_per_evse) {
            if (id_token.has_value() and remote_start_per_evse.second.first.idToken == id_token.value().idToken) {

                if (remote_start_per_evse.first == 0) {
                    return true;
                }

                if (evse.has_value() and evse.value().id == remote_start_per_evse.first) {
                    return true;
                }
            }
            return false;
        });

    if (it != remote_start_id_per_evse.end()) {
        // Found remote start. Set remote start id and the trigger reason.
        call.msg.triggerReason = TriggerReasonEnum::RemoteStart;
        call.msg.transactionInfo.remoteStartId = it->second.second;

        remote_start_id_per_evse.erase(it);
    }

    this->message_dispatcher.dispatch_call(call, initiated_by_trigger_message);

    if (this->transaction_event_callback.has_value()) {
        this->transaction_event_callback.value()(req);
    }
}

void TransactionBlock::set_remote_start_id_for_evse(const int32_t evse_id, const IdToken id_token,
                                                    const int32_t remote_start_id) {
    remote_start_id_per_evse[evse_id] = {id_token, remote_start_id};
}

void TransactionBlock::schedule_reset(const std::optional<int32_t> reset_scheduled_evseid) {
    reset_scheduled = true;
    if (reset_scheduled_evseid.has_value()) {
        this->reset_scheduled_evseids.insert(reset_scheduled_evseid.value());
    }
}

void TransactionBlock::handle_transaction_event_response(const EnhancedMessage<MessageType>& message) {
    CallResult<TransactionEventResponse> call_result = message.message;
    const Call<TransactionEventRequest>& original_call = message.call_message;
    const auto& original_msg = original_call.msg;

    if (this->transaction_event_response_callback.has_value()) {
        this->transaction_event_response_callback.value()(original_msg, call_result.msg);
    }

    this->tariff_and_cost.handle_cost_and_tariff(call_result.msg, original_msg, message.message[CALLRESULT_PAYLOAD]);

    if (original_msg.eventType == TransactionEventEnum::Ended) {
        // nothing to do for TransactionEventEnum::Ended
        return;
    }

    const auto msg = call_result.msg;

    if (!msg.idTokenInfo.has_value()) {
        // nothing to do when the response does not contain idTokenInfo
        return;
    }

    if (!original_msg.idToken.has_value()) {
        EVLOG_error
            << "TransactionEvent.conf contains idTokenInfo when no idToken was part of the TransactionEvent.req";
        return;
    }

    const IdToken& id_token = original_msg.idToken.value();

    // C03.FR.0x and C05.FR.01: We SHALL NOT store central information in the Authorization Cache
    // C10.FR.05
    if (id_token.type != IdTokenEnum::Central and this->authorization.is_auth_cache_ctrlr_enabled()) {
        try {
            this->authorization.authorization_cache_insert_entry(utils::generate_token_hash(id_token),
                                                                 msg.idTokenInfo.value());
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not insert into authorization cache entry: " << e.what();
        }
        this->authorization.trigger_authorization_cache_cleanup();
    }

    if (msg.idTokenInfo.value().status == AuthorizationStatusEnum::Accepted) {
        // nothing to do in case status is accepted
        return;
    }

    for (auto& evse : this->evse_manager) {
        if (auto& transaction = evse.get_transaction();
            transaction != nullptr and transaction->transactionId == original_msg.transactionInfo.transactionId) {
            // Deal with invalid token for transaction
            auto evse_id = evse.get_id();
            if (this->device_model.get_value<bool>(ControllerComponentVariables::StopTxOnInvalidId)) {
                this->stop_transaction_callback(evse_id, ReasonEnum::DeAuthorized);
            } else {
                if (this->device_model.get_optional_value<int32_t>(ControllerComponentVariables::MaxEnergyOnInvalidId)
                        .has_value()) {
                    // Energy delivery to the EV SHALL be allowed until the amount of energy specified in
                    // MaxEnergyOnInvalidId has been reached.
                    evse.start_checking_max_energy_on_invalid_id();
                } else {
                    this->pause_charging_callback(evse_id);
                }
            }
            break;
        }
    }
}

void TransactionBlock::handle_get_transaction_status(const Call<GetTransactionStatusRequest> call) {
    const auto msg = call.msg;

    GetTransactionStatusResponse response;
    response.messagesInQueue = false;

    if (msg.transactionId.has_value()) {
        if (this->evse_manager.get_transaction_evseid(msg.transactionId.value()).has_value()) {
            response.ongoingIndicator = true;
        } else {
            response.ongoingIndicator = false;
        }
        if (this->message_queue.contains_transaction_messages(msg.transactionId.value())) {
            response.messagesInQueue = true;
        }
    } else if (!this->message_queue.is_transaction_message_queue_empty()) {
        response.messagesInQueue = true;
    }

    ocpp::CallResult<GetTransactionStatusResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}
} // namespace ocpp::v2
