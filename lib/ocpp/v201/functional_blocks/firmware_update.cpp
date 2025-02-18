// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/functional_blocks/firmware_update.hpp>

#include <array>

#include <ocpp/v201/functional_blocks/availability.hpp>
#include <ocpp/v201/functional_blocks/security.hpp>
#include <ocpp/v201/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/evse_manager.hpp>

namespace ocpp::v201 {

// Firmware update end states.
const static std::array<FirmwareStatusEnum, 5> firmware_status_end_states = {
    FirmwareStatusEnum::DownloadFailed, FirmwareStatusEnum::InstallationFailed, FirmwareStatusEnum::Installed,
    FirmwareStatusEnum::InstallVerificationFailed, FirmwareStatusEnum::InvalidSignature};

FirmwareUpdate::FirmwareUpdate(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                               EvseManagerInterface& evse_manager, EvseSecurity& evse_security,
                               AvailabilityInterface& availability, SecurityInterface& security,
                               UpdateFirmwareRequestCallback update_firmware_request_callback,
                               std::optional<AllConnectorsUnavailableCallback> all_connectors_unavailable_callback) :
    message_dispatcher(message_dispatcher),
    device_model(device_model),
    evse_manager(evse_manager),
    evse_security(evse_security),
    availability(availability),
    security(security),
    update_firmware_request_callback(update_firmware_request_callback),
    all_connectors_unavailable_callback(all_connectors_unavailable_callback),
    firmware_status(FirmwareStatusEnum::Idle) {
}

void FirmwareUpdate::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    if (message.messageType == MessageType::UpdateFirmware) {
        this->handle_firmware_update_req(message.message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void FirmwareUpdate::on_firmware_update_status_notification(int32_t request_id,
                                                            const FirmwareStatusEnum& firmware_update_status) {
    if (this->firmware_status == firmware_update_status) {
        if (request_id == -1 or
            (this->firmware_status_id.has_value() and this->firmware_status_id.value() == request_id)) {
            // already sent, do not send again
            return;
        }
    }
    FirmwareStatusNotificationRequest req;
    req.status = firmware_update_status;
    // Firmware status and id are stored for future trigger message request.
    this->firmware_status = req.status;

    if (request_id != -1) {
        req.requestId = request_id; // L01.FR.20
        this->firmware_status_id = request_id;
    }

    ocpp::Call<FirmwareStatusNotificationRequest> call(req);
    this->message_dispatcher.dispatch_call_async(call);

    if (req.status == FirmwareStatusEnum::Installed) {
        std::string firmwareVersionMessage = "New firmware succesfully installed! Version: ";
        firmwareVersionMessage.append(
            this->device_model.get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
        this->security.security_event_notification_req(CiString<50>(ocpp::security_events::FIRMWARE_UPDATED),
                                                       std::optional<CiString<255>>(firmwareVersionMessage), true,
                                                       true); // L01.FR.31
    } else if (req.status == FirmwareStatusEnum::InvalidSignature) {
        this->security.security_event_notification_req(
            CiString<50>(ocpp::security_events::INVALIDFIRMWARESIGNATURE),
            std::optional<CiString<255>>("Signature of the provided firmware is not valid!"), true,
            true); // L01.FR.03 - critical because TC_L_06_CS requires this message to be sent
    }

    if (std::find(firmware_status_end_states.begin(), firmware_status_end_states.end(), req.status) !=
        firmware_status_end_states.end()) {
        // One of the end states is reached. Restore all connector states.
        this->restore_all_connector_states();
    }

    if (this->firmware_status_before_installing == req.status) {
        // FIXME(Kai): This is a temporary workaround, because the EVerest System module does not keep track of
        // transactions and can't inquire about their status from the OCPP modules. If the firmware status is expected
        // to become "Installing", but we still have a transaction running, the update will wait for the transaction to
        // finish, and so we send an "InstallScheduled" status. This is necessary for OCTT TC_L_15_CS to pass.
        const auto transaction_active = this->evse_manager.any_transaction_active(std::nullopt);
        if (transaction_active) {
            this->firmware_status = FirmwareStatusEnum::InstallScheduled;
            req.status = firmware_status;
            ocpp::Call<FirmwareStatusNotificationRequest> call(req);
            this->message_dispatcher.dispatch_call_async(call);
        }
        this->change_all_connectors_to_unavailable_for_firmware_update();
    }
}

void FirmwareUpdate::on_firmware_status_notification_request() {
    FirmwareStatusNotificationRequest request;
    switch (this->firmware_status) {
    case FirmwareStatusEnum::Idle:
    case FirmwareStatusEnum::Installed: // L01.FR.25
        request.status = FirmwareStatusEnum::Idle;
        // do not set requestId when idle: L01.FR.20
        break;

    default: // So not Idle or Installed                   // L01.FR.26
        request.status = this->firmware_status;
        request.requestId = this->firmware_status_id;
        break;
    }

    ocpp::Call<FirmwareStatusNotificationRequest> call(request);
    this->message_dispatcher.dispatch_call(call, true);
}

void FirmwareUpdate::handle_firmware_update_req(Call<UpdateFirmwareRequest> call) {
    EVLOG_debug << "Received UpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    if (call.msg.firmware.signingCertificate.has_value() or call.msg.firmware.signature.has_value()) {
        this->firmware_status_before_installing = FirmwareStatusEnum::SignatureVerified;
    } else {
        this->firmware_status_before_installing = FirmwareStatusEnum::Downloaded;
    }

    UpdateFirmwareResponse response;
    const auto msg = call.msg;
    bool cert_valid_or_not_set = true;

    // L01.FR.22 check if certificate is valid
    if (msg.firmware.signingCertificate.has_value() and
        this->evse_security.verify_certificate(msg.firmware.signingCertificate.value().get(),
                                               ocpp::LeafCertificateType::MF) !=
            ocpp::CertificateValidationResult::Valid) {
        response.status = UpdateFirmwareStatusEnum::InvalidCertificate;
        cert_valid_or_not_set = false;
    }

    if (cert_valid_or_not_set) {
        // execute firwmare update callback
        response = update_firmware_request_callback(msg);
    }

    ocpp::CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);

    if ((response.status == UpdateFirmwareStatusEnum::InvalidCertificate) or
        (response.status == UpdateFirmwareStatusEnum::RevokedCertificate)) {
        // L01.FR.02
        this->security.security_event_notification_req(
            CiString<50>(ocpp::security_events::INVALIDFIRMWARESIGNINGCERTIFICATE),
            std::optional<CiString<255>>("Provided signing certificate is not valid!"), true,
            true); // critical because TC_L_05_CS requires this message to be sent
    }
}

void FirmwareUpdate::change_all_connectors_to_unavailable_for_firmware_update() {
    ChangeAvailabilityResponse response;
    response.status = ChangeAvailabilityStatusEnum::Scheduled;

    ChangeAvailabilityRequest msg;
    msg.operationalStatus = OperationalStatusEnum::Inoperative;

    const auto transaction_active = this->evse_manager.any_transaction_active(std::nullopt);

    if (!transaction_active) {
        // execute change availability if possible
        for (auto& evse : this->evse_manager) {
            if (!evse.has_active_transaction()) {
                set_evse_connectors_unavailable(evse, false);
            }
        }
        // Check succeeded, trigger the callback if needed
        if (this->all_connectors_unavailable_callback.has_value() and
            this->evse_manager.are_all_connectors_effectively_inoperative()) {
            this->all_connectors_unavailable_callback.value()();
        }
    } else if (response.status == ChangeAvailabilityStatusEnum::Scheduled) {
        // put all EVSEs to unavailable that do not have active transaction
        for (auto& evse : this->evse_manager) {
            if (!evse.has_active_transaction()) {
                set_evse_connectors_unavailable(evse, false);
            } else {
                EVSE e;
                e.id = evse.get_id();
                msg.evse = e;
                this->availability.set_scheduled_change_availability_requests(evse.get_id(), {msg, false});
            }
        }
    }
}

void FirmwareUpdate::restore_all_connector_states() {
    for (auto& evse : this->evse_manager) {
        uint32_t number_of_connectors = evse.get_number_of_connectors();

        for (uint32_t i = 1; i <= number_of_connectors; ++i) {
            evse.restore_connector_operative_status(static_cast<int32_t>(i));
        }
    }
}

} // namespace ocpp::v201
