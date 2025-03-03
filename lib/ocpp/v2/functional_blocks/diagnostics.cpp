// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/diagnostics.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/authorization.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/utils.hpp>

#include <ocpp/v2/messages/ClearVariableMonitoring.hpp>
#include <ocpp/v2/messages/CustomerInformation.hpp>
#include <ocpp/v2/messages/GetLog.hpp>
#include <ocpp/v2/messages/GetMonitoringReport.hpp>
#include <ocpp/v2/messages/NotifyCustomerInformation.hpp>
#include <ocpp/v2/messages/NotifyEvent.hpp>
#include <ocpp/v2/messages/NotifyMonitoringReport.hpp>
#include <ocpp/v2/messages/SetMonitoringBase.hpp>
#include <ocpp/v2/messages/SetMonitoringLevel.hpp>
#include <ocpp/v2/messages/SetVariableMonitoring.hpp>

const auto DEFAULT_MAX_CUSTOMER_INFORMATION_DATA_LENGTH = 51200;

namespace ocpp::v2 {

Diagnostics::Diagnostics(const FunctionalBlockContext& context, AuthorizationInterface& authorization,
                         GetLogRequestCallback get_log_request_callback,
                         std::optional<GetCustomerInformationCallback> get_customer_information_callback,
                         std::optional<ClearCustomerInformationCallback> clear_customer_information_callback) :
    context(context),
    authorization(authorization),
    monitoring_updater(
        context.device_model, [this](const std::vector<EventData>& events) { this->notify_event_req(events); },
        [this]() { return !this->context.connectivity_manager.is_websocket_connected(); }),
    get_log_request_callback(get_log_request_callback),
    get_customer_information_callback(get_customer_information_callback),
    clear_customer_information_callback(clear_customer_information_callback) {
}

void Diagnostics::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    const auto& json_message = message.message;

    if (message.messageType == MessageType::GetLog) {
        this->handle_get_log_req(json_message);
    } else if (message.messageType == MessageType::CustomerInformation) {
        this->handle_customer_information_req(json_message);
    } else if (message.messageType == MessageType::SetMonitoringBase) {
        this->handle_set_monitoring_base_req(json_message);
    } else if (message.messageType == MessageType::SetMonitoringLevel) {
        this->handle_set_monitoring_level_req(json_message);
    } else if (message.messageType == MessageType::SetVariableMonitoring) {
        this->handle_set_variable_monitoring_req(message);
    } else if (message.messageType == MessageType::GetMonitoringReport) {
        this->handle_get_monitoring_report_req(json_message);
    } else if (message.messageType == MessageType::ClearVariableMonitoring) {
        this->handle_clear_variable_monitoring_req(json_message);
    } else {
        throw MessageTypeNotImplementedException(message.messageType);
    }
}

void Diagnostics::notify_event_req(const std::vector<EventData>& events) {
    NotifyEventRequest req;
    req.eventData = events;
    req.generatedAt = DateTime();
    req.seqNo = 0;

    ocpp::Call<NotifyEventRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call);
}

void Diagnostics::stop_monitoring() {
    monitoring_updater.stop_monitoring();
}

void Diagnostics::start_monitoring() {
    monitoring_updater.start_monitoring();
}

void Diagnostics::process_triggered_monitors() {
    monitoring_updater.process_triggered_monitors();
}

void Diagnostics::notify_customer_information_req(const std::string& data, const int32_t request_id) {
    size_t pos = 0;
    int32_t seq_no = 0;
    while (pos < data.length() or (pos == 0 and data.empty())) {
        const auto req = [&]() {
            NotifyCustomerInformationRequest req;
            req.data = CiString<512>(data.substr(pos, 512));
            req.seqNo = seq_no;
            req.requestId = request_id;
            req.generatedAt = DateTime();
            req.tbc = data.length() - pos > 512;
            return req;
        }();

        ocpp::Call<NotifyCustomerInformationRequest> call(req);
        this->context.message_dispatcher.dispatch_call(call);

        pos += 512;
        seq_no++;
    }
}

void Diagnostics::notify_monitoring_report_req(const int request_id,
                                               const std::vector<MonitoringData>& montoring_data) {
    static constexpr int32_t MAXIMUM_VARIABLE_SEND = 10;

    if (montoring_data.size() <= MAXIMUM_VARIABLE_SEND) {
        NotifyMonitoringReportRequest req;
        req.requestId = request_id;
        req.seqNo = 0;
        req.generatedAt = ocpp::DateTime();
        req.monitor.emplace(montoring_data);
        req.tbc = false;

        ocpp::Call<NotifyMonitoringReportRequest> call(req);
        this->context.message_dispatcher.dispatch_call(call);
    } else {
        // Split for larger message sizes
        int32_t sequence_num = 0;
        auto generated_at = ocpp::DateTime();

        for (int32_t i = 0; i < montoring_data.size(); i += MAXIMUM_VARIABLE_SEND) {
            // If our next index is >= than the last index then we're finished
            bool last_part = ((i + MAXIMUM_VARIABLE_SEND) >= montoring_data.size());

            NotifyMonitoringReportRequest req;
            req.requestId = request_id;
            req.seqNo = sequence_num;
            req.generatedAt = generated_at;
            req.tbc = (!last_part);

            // Construct sub-message part
            std::vector<MonitoringData> sub_data;

            for (int32_t j = i; j < MAXIMUM_VARIABLE_SEND and j < montoring_data.size(); ++j) {
                sub_data.push_back(std::move(montoring_data[i + j]));
            }

            req.monitor = sub_data;

            ocpp::Call<NotifyMonitoringReportRequest> call(req);
            this->context.message_dispatcher.dispatch_call(call);

            sequence_num++;
        }
    }
}

void Diagnostics::handle_get_log_req(Call<GetLogRequest> call) {
    const GetLogResponse response = this->get_log_request_callback(call.msg);

    ocpp::CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Diagnostics::handle_customer_information_req(Call<CustomerInformationRequest> call) {
    CustomerInformationResponse response;
    const auto& msg = call.msg;
    response.status = CustomerInformationStatusEnum::Accepted;

    if (!msg.report and !msg.clear) {
        EVLOG_warning << "CSMS sent CustomerInformation.req with both report and clear flags being false";
        response.status = CustomerInformationStatusEnum::Rejected;
    }

    if (!msg.customerCertificate.has_value() and !msg.idToken.has_value() and !msg.customerIdentifier.has_value()) {
        EVLOG_warning << "CSMS sent CustomerInformation.req without setting one of customerCertificate, idToken, "
                         "customerIdentifier fields";
        response.status = CustomerInformationStatusEnum::Invalid;
    }

    ocpp::CallResult<CustomerInformationResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == CustomerInformationStatusEnum::Accepted) {
        std::string data = "";
        if (msg.report) {
            data += this->get_customer_information(msg.customerCertificate, msg.idToken, msg.customerIdentifier);
        }
        if (msg.clear) {
            this->clear_customer_information(msg.customerCertificate, msg.idToken, msg.customerIdentifier);
        }

        const auto max_customer_information_data_length =
            this->context.device_model
                .get_optional_value<int>(ControllerComponentVariables::MaxCustomerInformationDataLength)
                .value_or(DEFAULT_MAX_CUSTOMER_INFORMATION_DATA_LENGTH);
        if (data.length() > max_customer_information_data_length) {
            EVLOG_warning << "NotifyCustomerInformation.req data field is too large. Cropping it down to: "
                          << max_customer_information_data_length << "characters";
            data.erase(max_customer_information_data_length);
        }

        this->notify_customer_information_req(data, msg.requestId);
    }
}

void Diagnostics::handle_set_monitoring_base_req(Call<SetMonitoringBaseRequest> call) {
    SetMonitoringBaseResponse response;
    const auto& msg = call.msg;

    auto result = this->context.device_model.set_value(
        ControllerComponentVariables::ActiveMonitoringBase.component,
        ControllerComponentVariables::ActiveMonitoringBase.variable.value(), AttributeEnum::Actual,
        conversions::monitoring_base_enum_to_string(msg.monitoringBase), VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, true);

    if (result != SetVariableStatusEnum::Accepted) {
        EVLOG_warning << "Could not persist in device model new monitoring base: "
                      << conversions::monitoring_base_enum_to_string(msg.monitoringBase);
        response.status = GenericDeviceModelStatusEnum::Rejected;
    } else {
        response.status = GenericDeviceModelStatusEnum::Accepted;

        if (msg.monitoringBase == MonitoringBaseEnum::HardWiredOnly or
            msg.monitoringBase == MonitoringBaseEnum::FactoryDefault) {
            try {
                this->context.device_model.clear_custom_monitors();
            } catch (const DeviceModelError& e) {
                EVLOG_warning << "Could not clear custom monitors from DB: " << e.what();
                response.status = GenericDeviceModelStatusEnum::Rejected;
            }
        }
    }

    ocpp::CallResult<SetMonitoringBaseResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Diagnostics::handle_set_monitoring_level_req(Call<SetMonitoringLevelRequest> call) {
    SetMonitoringLevelResponse response;
    const auto& msg = call.msg;

    if (msg.severity < MonitoringLevelSeverity::MIN or msg.severity > MonitoringLevelSeverity::MAX) {
        response.status = GenericStatusEnum::Rejected;
    } else {
        auto result = this->context.device_model.set_value(
            ControllerComponentVariables::ActiveMonitoringLevel.component,
            ControllerComponentVariables::ActiveMonitoringLevel.variable.value(), AttributeEnum::Actual,
            std::to_string(msg.severity), VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, true);

        if (result != SetVariableStatusEnum::Accepted) {
            EVLOG_warning << "Could not persist in device model new monitoring level: " << msg.severity;
            response.status = GenericStatusEnum::Rejected;
        } else {
            response.status = GenericStatusEnum::Accepted;
        }
    }

    ocpp::CallResult<SetMonitoringLevelResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Diagnostics::handle_set_variable_monitoring_req(const EnhancedMessage<MessageType>& message) {
    Call<SetVariableMonitoringRequest> call = message.call_message;
    SetVariableMonitoringResponse response;
    const auto& msg = call.msg;

    const auto max_items_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::ItemsPerMessageSetVariableMonitoring);
    const auto max_bytes_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::BytesPerMessageSetVariableMonitoring);

    // N04.FR.09
    if (msg.setMonitoringData.size() > max_items_per_message) {
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    if (message.message_size > max_bytes_message) {
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    try {
        response.setMonitoringResult = this->context.device_model.set_monitors(msg.setMonitoringData);
    } catch (const DeviceModelError& e) {
        EVLOG_error << "Set monitors failed:" << e.what();
    }

    ocpp::CallResult<SetVariableMonitoringResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void Diagnostics::handle_get_monitoring_report_req(Call<GetMonitoringReportRequest> call) {
    GetMonitoringReportResponse response;
    const auto& msg = call.msg;

    const auto component_variables = msg.componentVariable.value_or(std::vector<ComponentVariable>());
    const auto max_variable_components_per_message =
        this->context.device_model.get_value<int>(ControllerComponentVariables::ItemsPerMessageGetReport);

    // N02.FR.07
    if (component_variables.size() > max_variable_components_per_message) {
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->context.message_dispatcher.dispatch_call_error(call_error);
        return;
    }

    auto criteria = msg.monitoringCriteria.value_or(std::vector<MonitoringCriterionEnum>());
    std::vector<MonitoringData> data{};

    try {
        data = this->context.device_model.get_monitors(criteria, component_variables);

        if (!data.empty()) {
            response.status = GenericDeviceModelStatusEnum::Accepted;
        } else {
            response.status = GenericDeviceModelStatusEnum::EmptyResultSet;
        }
    } catch (const DeviceModelError& e) {
        EVLOG_error << "Get variable monitoring failed:" << e.what();
        response.status = GenericDeviceModelStatusEnum::Rejected;
    }

    ocpp::CallResult<GetMonitoringReportResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        // Send the result with splits if required
        notify_monitoring_report_req(msg.requestId, data);
    }
}

void Diagnostics::handle_clear_variable_monitoring_req(Call<ClearVariableMonitoringRequest> call) {
    ClearVariableMonitoringResponse response;
    const auto& msg = call.msg;

    try {
        response.clearMonitoringResult = this->context.device_model.clear_monitors(msg.id);
    } catch (const DeviceModelError& e) {
        EVLOG_error << "Clear variable monitoring failed:" << e.what();
    }

    ocpp::CallResult<ClearVariableMonitoringResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

std::string Diagnostics::get_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                                  const std::optional<IdToken> id_token,
                                                  const std::optional<CiString<64>> customer_identifier) {
    std::stringstream s;

    // Retrieve possible customer information from application that uses this library
    if (this->get_customer_information_callback.has_value()) {
        s << this->get_customer_information_callback.value()(customer_certificate, id_token, customer_identifier);
    }

    // Retrieve information from auth cache
    if (id_token.has_value()) {
        const auto hashed_id_token = utils::generate_token_hash(id_token.value());
        try {
            const auto entry = this->authorization.authorization_cache_get_entry(hashed_id_token);
            if (entry.has_value()) {
                s << "Hashed id_token stored in cache: " + hashed_id_token + "\n";
                s << "IdTokenInfo: " << entry->id_token_info;
            }
        } catch (const common::DatabaseException& e) {
            EVLOG_warning << "Could not get authorization cache entry from database";
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        }
    }

    return s.str();
}

void Diagnostics::clear_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                             const std::optional<IdToken> id_token,
                                             const std::optional<CiString<64>> customer_identifier) {
    if (this->clear_customer_information_callback.has_value()) {
        this->clear_customer_information_callback.value()(customer_certificate, id_token, customer_identifier);
    }

    if (id_token.has_value()) {
        const auto hashed_id_token = utils::generate_token_hash(id_token.value());
        try {
            this->authorization.authorization_cache_delete_entry(hashed_id_token);
        } catch (const common::DatabaseException& e) {
            EVLOG_error << "Could not delete from table: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Exception while deleting from auth cache table: " << e.what();
        }
        this->authorization.update_authorization_cache_size();
    }
}

} // namespace ocpp::v2
