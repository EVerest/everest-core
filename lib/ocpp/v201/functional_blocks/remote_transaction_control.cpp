// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/functional_blocks/remote_transaction_control.hpp>

#include <ocpp/v201/connectivity_manager.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/evse_manager.hpp>

#include <ocpp/v201/functional_blocks/availability.hpp>
#include <ocpp/v201/functional_blocks/firmware_update.hpp>
#include <ocpp/v201/functional_blocks/meter_values.hpp>
#include <ocpp/v201/functional_blocks/provisioning.hpp>
#include <ocpp/v201/functional_blocks/reservation.hpp>
#include <ocpp/v201/functional_blocks/security.hpp>
#include <ocpp/v201/functional_blocks/smart_charging.hpp>
#include <ocpp/v201/functional_blocks/transaction.hpp>

#include <ocpp/v201/messages/LogStatusNotification.hpp>
#include <ocpp/v201/messages/RequestStartTransaction.hpp>
#include <ocpp/v201/messages/RequestStopTransaction.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>

namespace ocpp::v201 {

RemoteTransactionControl::RemoteTransactionControl(
    MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
    ConnectivityManagerInterface& connectivity_manager, EvseManagerInterface& evse_manager,
    ComponentStateManagerInterface& component_state_manager, TransactionInterface& transaction,
    SmartChargingInterface& smart_charging, MeterValuesInterface& meter_values, AvailabilityInterface& availability,
    FirmwareUpdateInterface& firmware_update, SecurityInterface& security, ReservationInterface* reservation,
    ProvisioningInterface& provisioning, UnlockConnectorCallback unlock_connector_callback,
    RemoteStartTransactionCallback remote_start_transaction_callback, StopTransactionCallback stop_transaction_callback,
    std::atomic<RegistrationStatusEnum>& registration_status, std::atomic<UploadLogStatusEnum>& upload_log_status,
    std::atomic<int32_t>& upload_log_status_id) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    connectivity_manager(connectivity_manager),
    evse_manager(evse_manager),
    component_state_manager(component_state_manager),
    transaction(transaction),
    smart_charging(smart_charging),
    meter_values(meter_values),
    availability(availability),
    firmware_update(firmware_update),
    security(security),
    reservation(reservation),
    provisioning(provisioning),
    unlock_connector_callback(unlock_connector_callback),
    remote_start_transaction_callback(remote_start_transaction_callback),
    stop_transaction_callback(stop_transaction_callback),
    registration_status(registration_status),
    upload_log_status(upload_log_status),
    upload_log_status_id(upload_log_status_id) {
}

void RemoteTransactionControl::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::RequestStartTransaction:
        this->handle_remote_start_transaction_request(json_message);
        break;
    case MessageType::RequestStopTransaction:
        this->handle_remote_stop_transaction_request(json_message);
        break;
    case MessageType::UnlockConnector:
        this->handle_unlock_connector(json_message);
        break;
    case MessageType::TriggerMessage:
        this->handle_trigger_message(json_message);
        break;
    default:
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void RemoteTransactionControl::handle_unlock_connector(Call<UnlockConnectorRequest> call) {
    const UnlockConnectorRequest& msg = call.msg;
    UnlockConnectorResponse unlock_response;

    EVSE evse = {msg.evseId, std::nullopt, msg.connectorId};

    if (this->evse_manager.is_valid_evse(evse)) {
        if (!this->evse_manager.get_evse(msg.evseId).has_active_transaction()) {
            unlock_response = unlock_connector_callback(msg.evseId, msg.connectorId);
        } else {
            unlock_response.status = UnlockStatusEnum::OngoingAuthorizedTransaction;
        }
    } else {
        unlock_response.status = UnlockStatusEnum::UnknownConnector;
    }

    ocpp::CallResult<UnlockConnectorResponse> call_result(unlock_response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void RemoteTransactionControl::handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call) {
    auto msg = call.msg;

    RequestStartTransactionResponse response;
    response.status = RequestStartStopStatusEnum::Rejected;

    // Check if evse id is given.
    if (msg.evseId.has_value()) {
        const int32_t evse_id = msg.evseId.value();
        auto& evse = this->evse_manager.get_evse(evse_id);

        // F01.FR.23: Faulted or unavailable. F01.FR.24 / F02.FR.25: Occupied. Send rejected.
        const bool available = is_evse_connector_available(evse);

        // When available but there was a reservation for another token id or group token id:
        //    send rejected (F01.FR.21 & F01.FR.22)
        ocpp::ReservationCheckStatus reservation_status =
            is_evse_reserved_for_other(evse, call.msg.idToken, call.msg.groupIdToken);

        const bool is_reserved = (reservation_status == ocpp::ReservationCheckStatus::ReservedForOtherToken);

        if (!available or is_reserved) {
            // Note: we only support TxStartPoint PowerPathClosed, so we did not implement starting a
            // transaction first (and send TransactionEventRequest (eventType = Started). Only if a transaction
            // is authorized, a TransactionEventRequest will be sent. Because of this, F01.FR.13 is not
            // implemented as well, because in the current situation, this is an impossible state. (TODO: when
            // more TxStartPoints are supported, add implementation for F01.FR.13 as well).
            EVLOG_info << "Remote start transaction requested, but connector is not available or reserved.";
        } else {
            // F02: No active transaction yet and there is an available connector, so just send 'accepted'.
            response.status = RequestStartStopStatusEnum::Accepted;

            this->transaction.set_remote_start_id_for_evse(evse_id, msg.idToken, msg.remoteStartId);
        }

        // F01.FR.26 If a Charging Station with support for Smart Charging receives a
        // RequestStartTransactionRequest with an invalid ChargingProfile: The Charging Station SHALL respond
        // with RequestStartTransactionResponse with status = Rejected and optionally with reasonCode =
        // "InvalidProfile" or "InvalidSchedule".

        bool is_smart_charging_enabled =
            this->device_model.get_optional_value<bool>(ControllerComponentVariables::SmartChargingCtrlrEnabled)
                .value_or(false);

        if (is_smart_charging_enabled) {
            if (msg.chargingProfile.has_value()) {

                auto charging_profile = msg.chargingProfile.value();

                if (charging_profile.chargingProfilePurpose == ChargingProfilePurposeEnum::TxProfile) {

                    const auto add_profile_response = this->smart_charging.conform_validate_and_add_profile(
                        msg.chargingProfile.value(), evse_id, ChargingLimitSourceEnum::CSO,
                        AddChargingProfileSource::RequestStartTransactionRequest);
                    if (add_profile_response.status == ChargingProfileStatusEnum::Accepted) {
                        EVLOG_debug << "Accepting SetChargingProfileRequest";
                    } else {
                        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: "
                                    << add_profile_response.statusInfo->reasonCode.get()
                                    << "\nadditionalInfo: " << add_profile_response.statusInfo->additionalInfo->get();
                        response.statusInfo = add_profile_response.statusInfo;
                    }
                }
            }
        }
    } else {
        // F01.FR.07 RequestStartTransactionRequest does not contain an evseId. The Charging Station MAY reject the
        // RequestStartTransactionRequest. We do this for now (send rejected) (TODO: eventually support the charging
        // station to accept no evse id. If so: add token and remote start id for evse id 0 to
        // remote_start_id_per_evse, so we know for '0' it means 'all evse id's').
        EVLOG_warning << "No evse id given. Can not remote start transaction.";
    }

    if (response.status == RequestStartStopStatusEnum::Accepted) {
        response.status = this->remote_start_transaction_callback(
            msg, this->device_model.get_value<bool>(ControllerComponentVariables::AuthorizeRemoteStart));
    }

    const ocpp::CallResult<RequestStartTransactionResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void RemoteTransactionControl::handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call) {
    const auto msg = call.msg;

    RequestStopTransactionResponse response;
    std::optional<int32_t> evseid = this->evse_manager.get_transaction_evseid(msg.transactionId);

    if (evseid.has_value()) {
        // F03.FR.07: send 'accepted' if there was an ongoing transaction with the given transaction id
        response.status = RequestStartStopStatusEnum::Accepted;
    } else {
        // F03.FR.08: send 'rejected' if there was no ongoing transaction with the given transaction id
        response.status = RequestStartStopStatusEnum::Rejected;
    }

    if (response.status == RequestStartStopStatusEnum::Accepted) {
        response.status = this->stop_transaction_callback(evseid.value(), ReasonEnum::Remote);
    }

    const ocpp::CallResult<RequestStopTransactionResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

void RemoteTransactionControl::handle_trigger_message(Call<TriggerMessageRequest> call) {
    const TriggerMessageRequest& msg = call.msg;
    TriggerMessageResponse response;
    EvseInterface* evse_ptr = nullptr;

    response.status = TriggerMessageStatusEnum::Rejected;

    if (msg.evse.has_value()) {
        int32_t evse_id = msg.evse.value().id;
        evse_ptr = &this->evse_manager.get_evse(evse_id);
    }

    // F06.FR.04: First send the TriggerMessageResponse before sending the requested message
    //            so we split the functionality to be able to determine if we need to respond first.
    switch (msg.requestedMessage) {
    case MessageTriggerEnum::BootNotification:
        // F06.FR.17: Respond with rejected in case registration status is already accepted
        if (this->registration_status != RegistrationStatusEnum::Accepted) {
            response.status = TriggerMessageStatusEnum::Accepted;
        }
        break;

    case MessageTriggerEnum::LogStatusNotification:
    case MessageTriggerEnum::Heartbeat:
    case MessageTriggerEnum::FirmwareStatusNotification:
        response.status = TriggerMessageStatusEnum::Accepted;
        break;

    case MessageTriggerEnum::MeterValues:
        if (msg.evse.has_value()) {
            if (evse_ptr != nullptr and
                utils::meter_value_has_any_measurand(
                    evse_ptr->get_meter_value(), utils::get_measurands_vec(this->device_model.get_value<std::string>(
                                                     ControllerComponentVariables::AlignedDataMeasurands)))) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            const auto measurands = utils::get_measurands_vec(
                this->device_model.get_value<std::string>(ControllerComponentVariables::AlignedDataMeasurands));
            for (auto& evse : this->evse_manager) {
                if (utils::meter_value_has_any_measurand(evse.get_meter_value(), measurands)) {
                    response.status = TriggerMessageStatusEnum::Accepted;
                    break;
                }
            }
        }
        break;

    case MessageTriggerEnum::TransactionEvent:
        if (msg.evse.has_value()) {
            if (evse_ptr != nullptr and evse_ptr->has_active_transaction()) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            for (auto const& evse : this->evse_manager) {
                if (evse.has_active_transaction()) {
                    response.status = TriggerMessageStatusEnum::Accepted;
                    break;
                }
            }
        }
        break;

    case MessageTriggerEnum::StatusNotification:
        if (msg.evse.has_value() and msg.evse.value().connectorId.has_value()) {
            int32_t connector_id = msg.evse.value().connectorId.value();
            if (evse_ptr != nullptr and connector_id > 0 and connector_id <= evse_ptr->get_number_of_connectors()) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            // F06.FR.12: Reject if evse or connectorId is ommited
        }
        break;

    case MessageTriggerEnum::SignChargingStationCertificate:
        response.status = TriggerMessageStatusEnum::Accepted;
        break;
    case MessageTriggerEnum::SignV2GCertificate:
        if (this->device_model.get_optional_value<bool>(ControllerComponentVariables::V2GCertificateInstallationEnabled)
                .value_or(false)) {
            response.status = TriggerMessageStatusEnum::Accepted;
        } else {
            EVLOG_warning << "CSMS requested SignV2GCertificate but V2GCertificateInstallationEnabled is configured as "
                             "false, so the TriggerMessage is rejected!";
            response.status = TriggerMessageStatusEnum::Rejected;
        }

        break;
        // TODO:
        // PublishFirmwareStatusNotification
        // SignCombinedCertificate

    default:
        response.status = TriggerMessageStatusEnum::NotImplemented;
        break;
    }

    ocpp::CallResult<TriggerMessageResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if (response.status != TriggerMessageStatusEnum::Accepted) {
        return;
    }

    auto send_evse_message = [&](std::function<void(int32_t evse_id, EvseInterface & evse)> send) {
        if (evse_ptr != nullptr) {
            send(msg.evse.value().id, *evse_ptr);
        } else {
            for (auto& evse : this->evse_manager) {
                send(evse.get_id(), evse);
            }
        }
    };

    switch (msg.requestedMessage) {
    case MessageTriggerEnum::BootNotification:
        this->provisioning.boot_notification_req(BootReasonEnum::Triggered);
        break;

    case MessageTriggerEnum::MeterValues: {
        auto send_meter_value = [&](int32_t evse_id, EvseInterface& evse) {
            const auto meter_value =
                this->meter_values.get_latest_meter_value_filtered(evse.get_meter_value(), ReadingContextEnum::Trigger,
                                                                   ControllerComponentVariables::AlignedDataMeasurands);

            if (!meter_value.sampledValue.empty()) {
                this->meter_values.meter_values_req(evse_id, std::vector<ocpp::v201::MeterValue>(1, meter_value), true);
            }
        };
        send_evse_message(send_meter_value);
    } break;

    case MessageTriggerEnum::TransactionEvent: {
        auto send_transaction = [&](int32_t evse_id, EvseInterface& evse) {
            if (!evse.has_active_transaction()) {
                return;
            }

            const auto meter_value = this->meter_values.get_latest_meter_value_filtered(
                evse.get_meter_value(), ReadingContextEnum::Trigger,
                ControllerComponentVariables::SampledDataTxUpdatedMeasurands);

            std::optional<std::vector<MeterValue>> opt_meter_value;
            if (!meter_value.sampledValue.empty()) {
                opt_meter_value.emplace(1, meter_value);
            }
            const auto& enhanced_transaction = evse.get_transaction();
            this->transaction.transaction_event_req(
                TransactionEventEnum::Updated, DateTime(), enhanced_transaction->get_transaction(),
                TriggerReasonEnum::Trigger, enhanced_transaction->get_seq_no(), std::nullopt, std::nullopt,
                std::nullopt, opt_meter_value, std::nullopt, !this->connectivity_manager.is_websocket_connected(),
                std::nullopt, true);
        };
        send_evse_message(send_transaction);
    } break;

    case MessageTriggerEnum::StatusNotification:
        if (evse_ptr != nullptr and msg.evse.value().connectorId.has_value()) {
            this->component_state_manager.send_status_notification_single_connector(
                msg.evse.value().id, msg.evse.value().connectorId.value());
        }
        break;

    case MessageTriggerEnum::Heartbeat:
        this->availability.heartbeat_req(true);
        break;

    case MessageTriggerEnum::LogStatusNotification: {
        LogStatusNotificationRequest request;
        if (this->upload_log_status == UploadLogStatusEnum::Uploading) {
            request.status = UploadLogStatusEnum::Uploading;
            request.requestId = this->upload_log_status_id;
        } else {
            request.status = UploadLogStatusEnum::Idle;
        }

        ocpp::Call<LogStatusNotificationRequest> call(request);
        this->message_dispatcher.dispatch_call(call, true);
    } break;

    case MessageTriggerEnum::FirmwareStatusNotification: {
        this->firmware_update.on_firmware_status_notification_request();
    } break;

    case MessageTriggerEnum::SignChargingStationCertificate: {
        this->security.sign_certificate_req(ocpp::CertificateSigningUseEnum::ChargingStationCertificate, true);
    } break;

    case MessageTriggerEnum::SignV2GCertificate: {
        this->security.sign_certificate_req(ocpp::CertificateSigningUseEnum::V2GCertificate, true);
    } break;

    default:
        EVLOG_error << "Sent a TriggerMessageResponse::Accepted while not following up with a message";
        break;
    }
}

bool RemoteTransactionControl::is_evse_connector_available(EvseInterface& evse) const {
    if (evse.has_active_transaction()) {
        // If an EV is connected and has no authorization yet then the status is 'Occupied' and the
        // RemoteStartRequest should still be accepted. So this is the 'occupied' check instead.
        return false;
    }

    const uint32_t connectors = evse.get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum status =
            evse.get_connector(static_cast<int32_t>(i))->get_effective_connector_status();

        // At least one of the connectors is available / not faulted.
        if (status != ConnectorStatusEnum::Faulted and status != ConnectorStatusEnum::Unavailable) {
            return true;
        }
    }

    // Connectors are faulted or unavailable.
    return false;
}

ReservationCheckStatus
RemoteTransactionControl::is_evse_reserved_for_other(EvseInterface& evse, const IdToken& id_token,
                                                     const std::optional<IdToken>& group_id_token) const {
    if (this->reservation != nullptr) {
        return this->reservation->is_evse_reserved_for_other(evse, id_token, group_id_token);
    }

    return ReservationCheckStatus::NotReserved;
}

} // namespace ocpp::v201
