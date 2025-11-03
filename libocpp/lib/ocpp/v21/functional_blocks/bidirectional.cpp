// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <ocpp/v21/functional_blocks/bidirectional.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/utils.hpp>

#include <ocpp/v21/messages/NotifyAllowedEnergyTransfer.hpp>

ocpp::v2::Bidirectional::Bidirectional(
    const FunctionalBlockContext& context,
    std::optional<NotifyAllowedEnergyTransferCallback> notify_allowed_energy_transfer_callback) :
    context(context), notify_allowed_energy_transfer_callback(notify_allowed_energy_transfer_callback) {
}

ocpp::v2::Bidirectional::~Bidirectional() {
}

void ocpp::v2::Bidirectional::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::NotifyAllowedEnergyTransfer) {
        this->handle_notify_allowed_energy_transfer(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void ocpp::v2::Bidirectional::handle_notify_allowed_energy_transfer(
    Call<v21::NotifyAllowedEnergyTransferRequest> notify_allowed_energy_transfer) {
    if (this->context.ocpp_version != OcppProtocolVersion::v21) {
        EVLOG_error
            << "Received NotifyAllowedEnergyTransferRequest when not using OCPP2.1 this is not normal, ignoring...";
        return;
    }
    ocpp::v21::NotifyAllowedEnergyTransferResponse response;
    response.status = NotifyAllowedEnergyTransferStatusEnum::Accepted;

    const auto evse_id =
        this->context.evse_manager.get_transaction_evseid(notify_allowed_energy_transfer.msg.transactionId);
    if (!evse_id.has_value()) {
        response.status = NotifyAllowedEnergyTransferStatusEnum::Rejected;
        ocpp::v2::StatusInfo status_info;
        status_info.reasonCode = "InvalidValue";
        status_info.additionalInfo = "No evse associated to that transactionId.";
        response.statusInfo = status_info;
        ocpp::CallResult<v21::NotifyAllowedEnergyTransferResponse> call_result(response,
                                                                               notify_allowed_energy_transfer.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    if (!this->context.device_model
             .get_optional_value<bool>(
                 V2xComponentVariables::get_component_variable(evse_id.value(), V2xComponentVariables::Enabled))
             .value_or(false)) {
        response.status = NotifyAllowedEnergyTransferStatusEnum::Rejected;
        ocpp::v2::StatusInfo status_info;
        status_info.reasonCode = "InvalidValue";
        status_info.additionalInfo = "EVSE does not support V2X or V2X is disabled.";
        response.statusInfo = status_info;
        ocpp::CallResult<v21::NotifyAllowedEnergyTransferResponse> call_result(response,
                                                                               notify_allowed_energy_transfer.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    const auto selected_protocol = this->context.device_model.get_optional_value<std::string>(
        ConnectedEvComponentVariables::get_component_variable(evse_id.value(),
                                                              ConnectedEvComponentVariables::ProtocolAgreed));
    const bool is_15118_20 = selected_protocol.has_value()
                                 ? selected_protocol.value().find("urn:iso:std:iso:15118:-20") != std::string::npos
                                 : false;
    if (!is_15118_20) {
        response.status = NotifyAllowedEnergyTransferStatusEnum::Rejected;
        ocpp::v2::StatusInfo status_info;
        status_info.reasonCode = "UnsupportedRequest";
        status_info.additionalInfo = "Impossible to trigger service renegotiation due to the ISO15118 version that "
                                     "does not support it. Ignoring command.";
        response.statusInfo = status_info;
        ocpp::CallResult<v21::NotifyAllowedEnergyTransferResponse> call_result(response,
                                                                               notify_allowed_energy_transfer.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    const bool service_renegotiation_supported =
        this->context.device_model
            .get_optional_value<bool>(ISO15118ComponentVariables::get_component_variable(
                evse_id.value(), ISO15118ComponentVariables::ServiceRenegotiationSupport))
            .value_or(false);
    if (!service_renegotiation_supported) {
        response.status = NotifyAllowedEnergyTransferStatusEnum::Rejected;
        ocpp::v2::StatusInfo status_info;
        status_info.reasonCode = "UnsupportedRequest";
        status_info.additionalInfo =
            "Impossible to trigger service renegotiation since it is not supported. Ignoring command.";
        response.statusInfo = status_info;
        ocpp::CallResult<v21::NotifyAllowedEnergyTransferResponse> call_result(response,
                                                                               notify_allowed_energy_transfer.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    bool update_result = this->notify_allowed_energy_transfer_callback.has_value()
                             ? this->notify_allowed_energy_transfer_callback.value()(
                                   notify_allowed_energy_transfer.msg.allowedEnergyTransfer,
                                   notify_allowed_energy_transfer.msg.transactionId)
                             : false;
    if (!update_result) {
        response.status = NotifyAllowedEnergyTransferStatusEnum::Rejected;
        ocpp::v2::StatusInfo status_info;
        status_info.reasonCode = "InternalError";
        status_info.additionalInfo = "Update of allowed energy transfer and/or service renegotiation was unsuccessful.";
        response.statusInfo = status_info;
    }
    ocpp::CallResult<v21::NotifyAllowedEnergyTransferResponse> call_result(response,
                                                                           notify_allowed_energy_transfer.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}
