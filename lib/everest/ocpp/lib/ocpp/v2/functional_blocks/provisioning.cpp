// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/provisioning.hpp>

#include <set>

#include <ocpp/common/constants.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/v2/component_state_manager.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/notify_report_requests_splitter.hpp>

#include <ocpp/v2/functional_blocks/availability.hpp>
#include <ocpp/v2/functional_blocks/diagnostics.hpp>
#include <ocpp/v2/functional_blocks/meter_values.hpp>
#include <ocpp/v2/functional_blocks/security.hpp>
#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>
#include <ocpp/v2/functional_blocks/transaction.hpp>

#include <ocpp/v2/messages/BootNotification.hpp>
#include <ocpp/v2/messages/GetBaseReport.hpp>
#include <ocpp/v2/messages/GetReport.hpp>
#include <ocpp/v2/messages/GetVariables.hpp>
#include <ocpp/v2/messages/NotifyReport.hpp>
#include <ocpp/v2/messages/Reset.hpp>
#include <ocpp/v2/messages/SetNetworkProfile.hpp>
#include <ocpp/v2/messages/SetVariables.hpp>

const auto DEFAULT_MAX_MESSAGE_SIZE = 65000;
const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);

namespace ocpp::v2 {

namespace {
bool component_variable_change_requires_websocket_option_update_without_reconnect(
    const ComponentVariable& component_variable);
}

Provisioning::Provisioning(const FunctionalBlockContext& functional_block_context,
                           MessageQueue<MessageType>& message_queue, OcspUpdaterInterface& ocsp_updater,
                           AvailabilityInterface& availability, MeterValuesInterface& meter_values,
                           SecurityInterface& security, DiagnosticsInterface& diagnostics,
                           TransactionInterface& transaction, std::optional<TimeSyncCallback> time_sync_callback,
                           std::optional<BootNotificationCallback> boot_notification_callback,
                           std::optional<ValidateNetworkProfileCallback> validate_network_profile_callback,
                           IsResetAllowedCallback is_reset_allowed_callback, ResetCallback reset_callback,
                           StopTransactionCallback stop_transaction_callback,
                           std::optional<VariableChangedCallback> variable_changed_callback,
                           TariffAndCostInterface& tariff_and_cost,
                           std::atomic<RegistrationStatusEnum>& registration_status) :
    context(functional_block_context),
    message_queue(message_queue),
    ocsp_updater(ocsp_updater),
    availability(availability),
    meter_values(meter_values),
    security(security),
    diagnostics(diagnostics),
    transaction(transaction),
    tariff_and_cost(tariff_and_cost),
    time_sync_callback(time_sync_callback),
    boot_notification_callback(boot_notification_callback),
    validate_network_profile_callback(validate_network_profile_callback),
    is_reset_allowed_callback(is_reset_allowed_callback),
    reset_callback(reset_callback),
    stop_transaction_callback(stop_transaction_callback),
    variable_changed_callback(variable_changed_callback),
    registration_status(registration_status) {
}

void Provisioning::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::BootNotificationResponse) {
        this->handle_boot_notification_response(json_message);
    } else if (message.messageType == MessageType::SetVariables) {
        this->handle_set_variables_req(json_message);
    } else if (message.messageType == MessageType::GetVariables) {
        this->handle_get_variables_req(message);
    } else if (message.messageType == MessageType::GetBaseReport) {
        this->handle_get_base_report_req(json_message);
    } else if (message.messageType == MessageType::GetReport) {
        this->handle_get_report_req(message);
    } else if (message.messageType == MessageType::Reset) {
        this->handle_reset_req(json_message);
    } else if (message.messageType == MessageType::SetNetworkProfile) {
        this->handle_set_network_profile_req(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void Provisioning::boot_notification_req(const BootReasonEnum& reason, const bool initiated_by_trigger_message) {
    EVLOG_debug << "Sending BootNotification";
    BootNotificationRequest req;

    ChargingStation charging_station;
    charging_station.model =
        this->context.device_model.get_value<std::string>(ControllerComponentVariables::ChargePointModel);
    charging_station.vendorName =
        this->context.device_model.get_value<std::string>(ControllerComponentVariables::ChargePointVendor);
    charging_station.firmwareVersion.emplace(
        this->context.device_model.get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
    charging_station.serialNumber.emplace(
        this->context.device_model.get_value<std::string>(ControllerComponentVariables::ChargeBoxSerialNumber));

    auto iccid = this->context.device_model.get_optional_value<std::string>(ControllerComponentVariables::ICCID);
    auto imsi = this->context.device_model.get_optional_value<std::string>(ControllerComponentVariables::IMSI);

    if (iccid.has_value() || imsi.has_value()) {
        Modem modem;
        if (iccid.has_value()) {
            modem.iccid.emplace(iccid.value());
        }
        if (imsi.has_value()) {
            modem.imsi.emplace(imsi.value());
        }
        charging_station.modem.emplace(modem);
    }

    req.reason = reason;
    req.chargingStation = charging_station;

    const ocpp::Call<BootNotificationRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call, initiated_by_trigger_message);
}

void Provisioning::stop_bootnotification_timer() {
    this->boot_notification_timer.stop();
}

void Provisioning::on_variable_changed(const SetVariableData& set_variable_data) {
    this->handle_variable_changed(set_variable_data);
}

std::vector<GetVariableResult>
Provisioning::get_variables(const std::vector<GetVariableData>& get_variable_data_vector) {
    std::vector<GetVariableResult> response;
    for (const auto& get_variable_data : get_variable_data_vector) {
        GetVariableResult get_variable_result;
        get_variable_result.component = get_variable_data.component;
        get_variable_result.variable = get_variable_data.variable;
        get_variable_result.attributeType = get_variable_data.attributeType.value_or(AttributeEnum::Actual);
        const auto request_value_response = this->context.device_model.request_value<std::string>(
            get_variable_data.component, get_variable_data.variable,
            get_variable_data.attributeType.value_or(AttributeEnum::Actual));
        if (request_value_response.status == GetVariableStatusEnum::Accepted and
            request_value_response.value.has_value()) {
            get_variable_result.attributeValue = request_value_response.value.value();
        }
        get_variable_result.attributeStatus = request_value_response.status;

        // B09.FR.28: Return per-slot Identity when GetVariables asks for SecurityCtrlr.Identity
        const ComponentVariable cv = {get_variable_data.component, get_variable_data.variable, std::nullopt};
        if (cv == ControllerComponentVariables::SecurityCtrlrIdentity &&
            get_variable_result.attributeStatus == GetVariableStatusEnum::Accepted) {
            const auto active_slot_opt =
                this->context.device_model.get_optional_value<int>(ControllerComponentVariables::ActiveNetworkProfile);
            if (active_slot_opt.has_value()) {
                const auto slot_cv = NetworkConfigurationComponentVariables::get_component_variable(
                    active_slot_opt.value(), NetworkConfigurationComponentVariables::Identity);
                if (const auto slot_id = this->context.device_model.get_optional_value<std::string>(slot_cv);
                    slot_id.has_value() && !slot_id->empty()) {
                    get_variable_result.attributeValue = slot_id.value();
                }
            }
        }

        response.push_back(get_variable_result);
    }
    return response;
}

std::map<SetVariableData, SetVariableResult>
Provisioning::set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) {
    // set variables and allow setting of ReadOnly variables
    const auto response = this->set_variables_internal(set_variable_data_vector, source, true);
    this->handle_variables_changed(response);
    return response;
}

void Provisioning::notify_report_req(const int request_id, const std::vector<ReportData>& report_data) {
    NotifyReportRequest req;
    req.requestId = request_id;
    req.seqNo = 0;
    req.generatedAt = ocpp::DateTime();
    req.reportData.emplace(report_data);
    req.tbc = false;

    if (report_data.size() <= 1) {
        const ocpp::Call<NotifyReportRequest> call(req);
        this->context.message_dispatcher.dispatch_call(call);
    } else {
        NotifyReportRequestsSplitter splitter{
            req,
            this->context.device_model.get_optional_value<size_t>(ControllerComponentVariables::MaxMessageSize)
                .value_or(DEFAULT_MAX_MESSAGE_SIZE),
            []() { return ocpp::create_message_id(); }};
        for (const auto& msg : splitter.create_call_payloads()) {
            this->message_queue.push_call(msg);
        }
    }
}

void Provisioning::handle_boot_notification_response(CallResult<BootNotificationResponse> call_result) {
    EVLOG_info << "Received BootNotificationResponse: " << call_result.msg
               << "\nwith messageId: " << call_result.uniqueId;

    const auto msg = call_result.msg;

    this->registration_status = msg.status;

    if (this->registration_status == RegistrationStatusEnum::Accepted) {
        this->message_queue.set_registration_status_accepted();
        // B01.FR.06 Only use boot timestamp if TimeSource contains Heartbeat
        if (this->time_sync_callback.has_value() and
            this->context.device_model.get_value<std::string>(ControllerComponentVariables::TimeSource)
                    .find("Heartbeat") != std::string::npos) {
            this->time_sync_callback.value()(msg.currentTime);
        }

        this->context.connectivity_manager.confirm_successful_connection();

        // set timers
        if (msg.interval > 0) {
            this->availability.set_heartbeat_timer_interval(std::chrono::seconds(msg.interval));
            if (ControllerComponentVariables::HeartbeatInterval.variable.has_value()) {
                this->context.device_model.set_value(ControllerComponentVariables::HeartbeatInterval.component,
                                                     ControllerComponentVariables::HeartbeatInterval.variable.value(),
                                                     AttributeEnum::Actual, std::to_string(msg.interval),
                                                     VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS);
            }
        }

        // in case the BootNotification.req was triggered by a TriggerMessage.req the timer might still run
        this->boot_notification_timer.stop();

        this->security.init_certificate_expiration_check_timers();
        this->meter_values.update_aligned_data_interval();
        this->context.component_state_manager.send_status_notification_all_connectors();
        this->ocsp_updater.start();
    } else {
        auto retry_interval = DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL;
        if (msg.interval > 0) {
            retry_interval = std::chrono::seconds(msg.interval);
        }
        this->boot_notification_timer.timeout(
            [this, msg]() {
                this->boot_notification_req(BootReasonEnum::PowerUp); // FIXME(piet): Choose correct reason here
            },
            retry_interval);
    }

    if (this->boot_notification_callback.has_value()) {
        // call the registered boot notification callback
        boot_notification_callback.value()(call_result.msg);
    }
}

void Provisioning::handle_set_variables_req(Call<SetVariablesRequest> call) {
    const auto msg = call.msg;

    SetVariablesResponse response;

    // set variables but do not allow setting ReadOnly variables
    const auto set_variables_response =
        this->set_variables_internal(msg.setVariableData, VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, false);
    for (const auto& [single_set_variable_data, single_set_variable_result] : set_variables_response) {
        response.setVariableResult.push_back(single_set_variable_result);
    }

    const ocpp::CallResult<SetVariablesResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    // post handling of changed variables after the SetVariables.conf has been queued
    this->handle_variables_changed(set_variables_response);
}

void Provisioning::handle_get_variables_req(const EnhancedMessage<MessageType>& message) {
    const Call<GetVariablesRequest> call = message.call_message;
    const auto msg = call.msg;

    const auto max_variables_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::ItemsPerMessageGetVariables);
    const auto max_bytes_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::BytesPerMessageGetVariables);

    // B06.FR.16
    if (msg.getVariableData.size() > max_variables_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    // B06.FR.17
    if (message.message_size > max_bytes_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    GetVariablesResponse response;
    response.getVariableResult = this->get_variables(msg.getVariableData);

    const ocpp::CallResult<GetVariablesResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Provisioning::handle_get_base_report_req(Call<GetBaseReportRequest> call) {
    const auto msg = call.msg;
    GetBaseReportResponse response;
    response.status = GenericDeviceModelStatusEnum::Accepted;

    const ocpp::CallResult<GetBaseReportResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        const auto report_data = this->context.device_model.get_base_report_data(msg.reportBase);
        this->notify_report_req(msg.requestId, report_data);
    }
}

void Provisioning::handle_get_report_req(const EnhancedMessage<MessageType>& message) {
    const Call<GetReportRequest> call = message.call_message;
    const auto msg = call.msg;
    std::vector<ReportData> report_data;
    GetReportResponse response;

    const auto max_items_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::ItemsPerMessageGetReport);
    const auto max_bytes_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::BytesPerMessageGetReport);

    // B08.FR.17
    if (msg.componentVariable.has_value() and msg.componentVariable->size() > max_items_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    // B08.FR.18
    if (message.message_size > max_bytes_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    // if a criteria is not supported then send a not supported response.
    auto sup_criteria =
        this->context.device_model.get_optional_value<std::string>(ControllerComponentVariables::SupportedCriteria);
    if (sup_criteria.has_value() and msg.componentCriteria.has_value()) {
        for (const auto& criteria : msg.componentCriteria.value()) {
            const auto variable_ = conversions::component_criterion_enum_to_string(criteria);
            if (sup_criteria.value().find(variable_) == std::string::npos) {
                EVLOG_info << "This criteria is not supported: " << variable_;
                response.status = GenericDeviceModelStatusEnum::NotSupported;
                break;
                // TODO: maybe consider adding the reason why in statusInfo
            }
        }
    }

    if (response.status != GenericDeviceModelStatusEnum::NotSupported) {

        // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage /
        // BytesPerMessage
        report_data = this->context.device_model.get_custom_report_data(msg.componentVariable, msg.componentCriteria);
        if (report_data.empty()) {
            response.status = GenericDeviceModelStatusEnum::EmptyResultSet;
        } else {
            response.status = GenericDeviceModelStatusEnum::Accepted;
        }
    }

    const ocpp::CallResult<GetReportResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        this->notify_report_req(msg.requestId, report_data);
    }
}

void Provisioning::handle_set_network_profile_req(Call<SetNetworkProfileRequest> call) {
    const auto msg = call.msg;

    SetNetworkProfileResponse response;
    StatusInfo status_info;

    if (!this->validate_network_profile_callback.has_value()) {
        const auto warning = "No callback registered to validate network profile";
        EVLOG_warning << warning;
        status_info.reasonCode = "InternalError";
        status_info.additionalInfo = warning;
        response.statusInfo = status_info;
        response.status = SetNetworkProfileStatusEnum::Rejected;
        const ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    if (msg.connectionData.securityProfile <
        this->context.device_model.get_value<int>(ControllerComponentVariables::SecurityProfile)) {
        const auto warning = "CSMS attempted to set a network profile with a lower securityProfile";
        EVLOG_warning << warning;
        status_info.reasonCode = "NoSecurityDowngrade";
        status_info.additionalInfo = warning;
        response.statusInfo = status_info;
        response.status = SetNetworkProfileStatusEnum::Rejected;
        const ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    if (this->validate_network_profile_callback.value()(msg.configurationSlot, msg.connectionData) !=
        SetNetworkProfileStatusEnum::Accepted) {
        const auto warning = "CSMS attempted to set a network profile that could not be validated.";
        EVLOG_warning << warning;
        status_info.reasonCode = "InvalidNetworkConf";
        status_info.additionalInfo = warning;
        response.statusInfo = status_info;
        response.status = SetNetworkProfileStatusEnum::Rejected;
        const ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // Write the profile to NetworkConfiguration device model variables and refresh the cache (B09.FR.09 / B09.FR.11/12)
    if (!this->context.connectivity_manager.set_network_profile(msg.configurationSlot, msg.connectionData,
                                                                VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL)) {
        const auto warning = "CSMS attempted to set a network profile that could not be written to the device model";
        EVLOG_warning << warning;
        status_info.reasonCode = "InvalidNetworkConf";
        status_info.additionalInfo = warning;
        response.statusInfo = status_info;
        response.status = SetNetworkProfileStatusEnum::Rejected;
        const ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    std::string tech_info = "Received and stored a new network connection profile at configurationSlot: " +
                            std::to_string(msg.configurationSlot);
    EVLOG_info << tech_info;

    const auto& security_event = ocpp::security_events::RECONFIGURATIONOFSECURITYPARAMETERS;
    this->security.security_event_notification_req(CiString<50>(security_event), CiString<255>(tech_info), true,
                                                   utils::is_critical(security_event));

    response.status = SetNetworkProfileStatusEnum::Accepted;
    const ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Provisioning::handle_reset_req(Call<ResetRequest> call) {
    EVLOG_debug << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;

    ResetResponse response;

    // Check if there is an active transaction (on the given evse or if not
    // given, on one of the evse's)
    bool transaction_active = false;
    std::set<std::int32_t> evse_active_transactions;
    std::set<std::int32_t> evse_no_transactions;
    if (msg.evseId.has_value() and this->context.evse_manager.get_evse(msg.evseId.value()).has_active_transaction()) {
        transaction_active = true;
        evse_active_transactions.emplace(msg.evseId.value());
    } else {
        for (const auto& evse : this->context.evse_manager) {
            if (evse.has_active_transaction()) {
                transaction_active = true;
                evse_active_transactions.emplace(evse.get_id());
            } else {
                evse_no_transactions.emplace(evse.get_id());
            }
        }
    }

    const auto is_reset_allowed = [&]() {
        if (!this->is_reset_allowed_callback(msg.evseId, msg.type)) {
            return false;
        }

        // We dont need to check AllowReset if evseId is not set and can directly return true
        if (!msg.evseId.has_value()) {
            return true;
        }

        // B11.FR.10
        const auto allow_reset_cv =
            EvseComponentVariables::get_component_variable(msg.evseId.value(), EvseComponentVariables::AllowReset);
        // allow reset if AllowReset is not set or set to   true
        return this->context.device_model.get_optional_value<bool>(allow_reset_cv).value_or(true);
    };

    if (is_reset_allowed()) {
        // reset is allowed
        response.status = ResetStatusEnum::Accepted;
    } else {
        response.status = ResetStatusEnum::Rejected;
    }

    if (response.status == ResetStatusEnum::Accepted and transaction_active and msg.type == ResetEnum::OnIdle) {
        std::optional<std::int32_t> reset_scheduled_evseid = std::nullopt;
        // B12.FR.07
        reset_scheduled_evseid = msg.evseId;

        // B12.FR.01: We have to wait until transactions have ended.
        // B12.FR.07
        this->transaction.schedule_reset(reset_scheduled_evseid);
        response.status = ResetStatusEnum::Scheduled;
    }

    const ocpp::CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    // Reset response is sent, now set evse connectors to unavailable and / or
    // stop transaction (depending on reset type)
    if (response.status != ResetStatusEnum::Rejected and transaction_active) {
        if (msg.type == ResetEnum::Immediate) {
            // B12.FR.08 and B12.FR.04
            for (const std::int32_t evse_id : evse_active_transactions) {
                stop_transaction_callback(evse_id, ReasonEnum::ImmediateReset);
            }
        } else if (msg.type == ResetEnum::OnIdle and !evse_no_transactions.empty()) {
            for (const std::int32_t evse_id : evse_no_transactions) {
                auto& evse = this->context.evse_manager.get_evse(evse_id);
                set_evse_connectors_unavailable(evse, false);
            }
        }
    }

    if (response.status == ResetStatusEnum::Accepted) {
        this->reset_callback(call.msg.evseId, ResetEnum::Immediate);
    }
}

void Provisioning::handle_variable_changed(const SetVariableData& set_variable_data) {
    const ComponentVariable component_variable = {set_variable_data.component, set_variable_data.variable,
                                                  std::nullopt};

    if (set_variable_data.attributeType.has_value() and
        set_variable_data.attributeType.value() != AttributeEnum::Actual) {
        return;
    }

    if (component_variable == ControllerComponentVariables::BasicAuthPassword) {
        if (this->context.device_model.get_value<int>(ControllerComponentVariables::SecurityProfile) < 3) {
            // TODO: A01.FR.11 log the change of BasicAuth in Security Log
            this->context.connectivity_manager.set_websocket_authorization_key(set_variable_data.attributeValue.get());
        }
        // B09.FR.27: Clear per-slot BasicAuthPassword from active NetworkConfiguration slots
        const auto priority_str = this->context.device_model.get_optional_value<std::string>(
            ControllerComponentVariables::NetworkConfigurationPriority);
        if (priority_str.has_value()) {
            for (const auto& slot_str : ocpp::split_string(priority_str.value(), ',')) {
                const int slot = std::stoi(slot_str);
                const auto cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::BasicAuthPassword);
                this->context.device_model.set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "",
                                                     VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
            }
            EVLOG_info << "Cleared per-slot BasicAuthPassword from active NetworkConfiguration slots (B09.FR.27)";
        }
    }
    if (component_variable == ControllerComponentVariables::SecurityCtrlrIdentity) {
        // B09.FR.26: Clear per-slot Identity from active NetworkConfiguration slots
        const auto priority_str = this->context.device_model.get_optional_value<std::string>(
            ControllerComponentVariables::NetworkConfigurationPriority);
        if (priority_str.has_value()) {
            for (const auto& slot_str : ocpp::split_string(priority_str.value(), ',')) {
                const int slot = std::stoi(slot_str);
                const auto cv = NetworkConfigurationComponentVariables::get_component_variable(
                    slot, NetworkConfigurationComponentVariables::Identity);
                this->context.device_model.set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "",
                                                     VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
            }
            EVLOG_info << "Cleared per-slot Identity from active NetworkConfiguration slots (B09.FR.26)";
        }
    }
    if (component_variable == ControllerComponentVariables::HeartbeatInterval and
        this->registration_status == RegistrationStatusEnum::Accepted) {
        try {
            this->availability.set_heartbeat_timer_interval(
                std::chrono::seconds(std::stoi(set_variable_data.attributeValue.get())));
        } catch (const std::invalid_argument& e) {
            EVLOG_error << "Invalid argument exception while updating the heartbeat interval: " << e.what();
        } catch (const std::out_of_range& e) {
            EVLOG_error << "Out of range exception while updating the heartbeat interval: " << e.what();
        }
    }
    if (component_variable == ControllerComponentVariables::AlignedDataInterval) {
        this->meter_values.update_aligned_data_interval();
    }

    if (component_variable_change_requires_websocket_option_update_without_reconnect(component_variable)) {
        EVLOG_debug << "Reconfigure websocket due to relevant change of ControllerComponentVariable";
        this->context.connectivity_manager.set_websocket_connection_options_without_reconnect();
    }

    if (component_variable == ControllerComponentVariables::MessageAttemptInterval) {
        if (component_variable.variable.has_value()) {
            this->message_queue.update_transaction_message_retry_interval(
                this->context.device_model.get_value<int>(ControllerComponentVariables::MessageAttemptInterval));
        }
    }

    if (component_variable == ControllerComponentVariables::MessageAttempts) {
        if (component_variable.variable.has_value()) {
            this->message_queue.update_transaction_message_attempts(
                this->context.device_model.get_value<int>(ControllerComponentVariables::MessageAttempts));
        }
    }

    if (component_variable == ControllerComponentVariables::TariffFallbackMessage ||
        component_variable == ControllerComponentVariables::OfflineTariffFallbackMessage) {
        // SetVariables can only be received while connected, so publish the online price.
        this->tariff_and_cost.publish_default_price(this->context.connectivity_manager.is_websocket_connected());
    }

    // TODO(piet): other special handling of changed variables can be added here...
}

void Provisioning::handle_variables_changed(const std::map<SetVariableData, SetVariableResult>& set_variable_results) {
    // Track which NetworkConfiguration slots were modified
    std::set<int32_t> modified_slots;

    // iterate over set_variable_results
    for (const auto& [set_variable_data, set_variable_result] : set_variable_results) {
        if (set_variable_result.attributeStatus == SetVariableStatusEnum::Accepted) {
            std::optional<MutabilityEnum> mutability = this->context.device_model.get_mutability(
                set_variable_data.component, set_variable_data.variable,
                set_variable_data.attributeType.value_or(AttributeEnum::Actual));
            // If a nullopt is returned for whatever reason, assume it's write-only to prevent leaking secrets
            if (!mutability.has_value() || (mutability.value() == MutabilityEnum::WriteOnly)) {
                EVLOG_info << "Write-only " << set_variable_data.component.name << ":"
                           << set_variable_data.variable.name << " changed";
            } else {
                EVLOG_info << set_variable_data.component.name << ":" << set_variable_data.variable.name
                           << " changed to " << set_variable_data.attributeValue.get();
            }

            // Track which NetworkConfiguration DM component slots were modified via SetVariables
            if (set_variable_data.component.name == "NetworkConfiguration" &&
                set_variable_data.component.instance.has_value()) {
                try {
                    const int32_t slot = std::stoi(set_variable_data.component.instance.value().get());
                    modified_slots.insert(slot);
                } catch (const std::exception& e) {
                    EVLOG_warning << "Error parsing NetworkConfiguration slot: " << e.what();
                }
            }

            // handles required behavior specified within OCPP2.0.1 (e.g. reconnect when BasicAuthPassword has changed)
            this->handle_variable_changed(set_variable_data);
            // notifies libocpp user application that a variable has changed
            if (this->variable_changed_callback.has_value()) {
                this->variable_changed_callback.value()(set_variable_data);
            }
        }
    }

    // Refresh the connectivity manager's in-memory profile cache if any NetworkConfiguration slot changed
    if (!modified_slots.empty()) {
        this->context.connectivity_manager.reload_network_profiles();
    }

    // process all triggered monitors, after a possible disconnect
    this->diagnostics.process_triggered_monitors();
}

std::optional<std::string> Provisioning::validate_set_variable(const SetVariableData& set_variable_data) {
    const ComponentVariable cv = {set_variable_data.component, set_variable_data.variable, std::nullopt};

    // B09.FR.21/22: Reject changes to the currently active NetworkConfiguration slot
    if (cv.component.name == "NetworkConfiguration" && cv.component.instance.has_value() && cv.variable.has_value()) {
        try {
            const int slot = std::stoi(cv.component.instance.value().get());
            const auto active_slot_opt =
                this->context.device_model.get_optional_value<int>(ControllerComponentVariables::ActiveNetworkProfile);

            if (active_slot_opt.has_value() && slot == active_slot_opt.value()) {
                EVLOG_warning << "Cannot set NetworkConfiguration variable for slot " << slot
                              << " which is the currently active slot";
                return "PriorityNetworkConf";
            }

            // B09.FR.35: Reject security profile downgrades
            if (cv.variable.value().name == "SecurityProfile") {
                try {
                    const int new_profile = std::stoi(set_variable_data.attributeValue.get());
                    const int active_profile =
                        this->context.device_model.get_value<int>(ControllerComponentVariables::SecurityProfile);
                    if (new_profile < active_profile) {
                        EVLOG_warning << "Cannot downgrade SecurityProfile from " << active_profile << " to "
                                      << new_profile;
                        return "NoSecurityDowngrade";
                    }
                } catch (const std::exception& e) {
                    EVLOG_warning << "Error validating SecurityProfile: " << e.what();
                    return "InvalidNetworkConf";
                }
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Error validating NetworkConfiguration: " << e.what();
            return "InvalidNetworkConf";
        }
    }

    if (cv == ControllerComponentVariables::NetworkConfigurationPriority) {
        const auto network_configuration_priorities = ocpp::split_string(set_variable_data.attributeValue.get(), ',');
        const auto active_security_profile =
            this->context.device_model.get_value<int>(ControllerComponentVariables::SecurityProfile);

        try {
            for (const auto& configuration_slot : network_configuration_priorities) {
                const int slot = std::stoi(configuration_slot);
                const auto profile_opt = NetworkConfigurationComponentVariables::read_profile_from_device_model(
                    this->context.device_model, slot);

                if (!profile_opt.has_value()) {
                    EVLOG_warning << "Could not find network profile for configurationSlot: " << configuration_slot;
                    return "InvalidNetworkConf";
                }

                const auto& network_profile = *profile_opt;

                // Validate URL scheme matches security profile
                const std::string url = network_profile.ocppCsmsUrl.get();
                const bool is_wss = url.find("wss://") == 0;
                const bool is_ws = url.find("ws://") == 0;
                if (is_wss && network_profile.securityProfile < 2) {
                    EVLOG_warning << "configurationSlot " << configuration_slot
                                  << ": wss:// URL requires securityProfile >= 2, got "
                                  << network_profile.securityProfile;
                    return "InvalidNetworkConf";
                }
                if (is_ws && network_profile.securityProfile >= 2) {
                    EVLOG_warning << "configurationSlot " << configuration_slot
                                  << ": ws:// URL is not allowed with securityProfile >= 2, got "
                                  << network_profile.securityProfile;
                    return "InvalidNetworkConf";
                }

                if (network_profile.securityProfile <= active_security_profile) {
                    continue;
                }

                if (network_profile.securityProfile == 3 and
                    this->context.evse_security
                            .get_leaf_certificate_info(ocpp::CertificateSigningUseEnum::ChargingStationCertificate)
                            .status != ocpp::GetCertificateInfoStatus::Accepted) {
                    EVLOG_warning << "SecurityProfile of configurationSlot: " << configuration_slot
                                  << " is 3 but no CSMS Leaf Certificate is installed";
                    return "InvalidNetworkConf";
                }
                if (network_profile.securityProfile >= 2 and
                    !this->context.evse_security.is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
                    EVLOG_warning << "SecurityProfile of configurationSlot: " << configuration_slot
                                  << " is >= 2 but no CSMS Root Certifciate is installed";
                    return "InvalidNetworkConf";
                }
            }
        } catch (const std::invalid_argument& e) {
            EVLOG_warning << "NetworkConfigurationPriority contains at least one value which is not an integer: "
                          << set_variable_data.attributeValue.get();
            return "InvalidNetworkConf";
        }
    }
    return std::nullopt;
    // TODO(piet): other special validating of variables requested to change can be added here...
}

std::map<SetVariableData, SetVariableResult>
Provisioning::set_variables_internal(const std::vector<SetVariableData>& set_variable_data_vector,
                                     const std::string& source, const bool allow_read_only) {
    std::map<SetVariableData, SetVariableResult> response;

    // iterate over the set_variable_data_vector
    for (const auto& set_variable_data : set_variable_data_vector) {
        SetVariableResult set_variable_result;
        set_variable_result.component = set_variable_data.component;
        set_variable_result.variable = set_variable_data.variable;
        set_variable_result.attributeType = set_variable_data.attributeType.value_or(AttributeEnum::Actual);

        // validates variable against business logic of the spec
        const auto validation_result = this->validate_set_variable(set_variable_data);
        if (!validation_result.has_value()) {
            // attempt to set the value includes device model validation
            set_variable_result.attributeStatus =
                this->context.device_model.set_value(set_variable_data.component, set_variable_data.variable,
                                                     set_variable_data.attributeType.value_or(AttributeEnum::Actual),
                                                     set_variable_data.attributeValue.get(), source, allow_read_only);
        } else {
            set_variable_result.attributeStatus = SetVariableStatusEnum::Rejected;
            StatusInfo status_info;
            status_info.reasonCode = validation_result.value();
            set_variable_result.attributeStatusInfo = status_info;
        }
        response[set_variable_data] = set_variable_result;
    }

    return response;
}

namespace {
/**
 * Determine for a component variable whether it affects the Websocket Connection Options (cf.
 * get_ws_connection_options); return true if it is furthermore writable and does not require a reconnect
 *
 * @param component_variable
 * @return
 */
bool component_variable_change_requires_websocket_option_update_without_reconnect(
    const ComponentVariable& component_variable) {

    return component_variable == ControllerComponentVariables::RetryBackOffRandomRange or
           component_variable == ControllerComponentVariables::RetryBackOffRepeatTimes or
           component_variable == ControllerComponentVariables::RetryBackOffWaitMinimum or
           component_variable == ControllerComponentVariables::NetworkProfileConnectionAttempts or
           component_variable == ControllerComponentVariables::WebSocketPingInterval;
}
} // namespace
} // namespace ocpp::v2
