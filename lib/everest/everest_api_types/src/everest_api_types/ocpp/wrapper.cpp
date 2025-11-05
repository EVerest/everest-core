// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp/wrapper.hpp"
#include "ocpp/API.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::ocpp {

#define enum_case(A)                                                                                                   \
    case SrcT::A:                                                                                                      \
        return TarT::A

namespace {
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src,
                 ConvT const& converter) -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT, class ConvT>
auto srcToTarVec(std::optional<std::vector<SrcT>> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src.value()[0]));
    std::optional<std::vector<TarT>> result;
    if (src) {
        for (SrcT const& elem : src.value()) {
            result.value().push_back(converter(elem));
        }
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_internal_api(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::optional<std::vector<SrcT>> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}

} // namespace

AttributeEnum_Internal to_internal_api(AttributeEnum_External const& val) {
    using SrcT = AttributeEnum_External;
    using TarT = AttributeEnum_Internal;
    switch (val) {
        enum_case(Actual);
        enum_case(Target);
        enum_case(MinSet);
        enum_case(MaxSet);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::AttributeEnum_External");
}

AttributeEnum_External to_external_api(AttributeEnum_Internal const& val) {
    using SrcT = AttributeEnum_Internal;
    using TarT = AttributeEnum_External;
    switch (val) {
        enum_case(Actual);
        enum_case(Target);
        enum_case(MinSet);
        enum_case(MaxSet);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::AttributeEnum_Internal");
}

GetVariableStatusEnumType_Internal to_internal_api(GetVariableStatusEnumType_External const& val) {
    using SrcT = GetVariableStatusEnumType_External;
    using TarT = GetVariableStatusEnumType_Internal;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownComponent);
        enum_case(UnknownVariable);
        enum_case(NotSupportedAttributeType);
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnumType_External");
}

GetVariableStatusEnumType_External to_external_api(GetVariableStatusEnumType_Internal const& val) {
    using SrcT = GetVariableStatusEnumType_Internal;
    using TarT = GetVariableStatusEnumType_External;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownComponent);
        enum_case(UnknownVariable);
        enum_case(NotSupportedAttributeType);
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnumType_Internal");
}

SetVariableStatusEnumType_Internal to_internal_api(SetVariableStatusEnumType_External const& val) {
    using SrcT = SetVariableStatusEnumType_External;
    using TarT = SetVariableStatusEnumType_Internal;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownComponent);
        enum_case(UnknownVariable);
        enum_case(NotSupportedAttributeType);
        enum_case(RebootRequired);
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnumType_External");
}

SetVariableStatusEnumType_External to_external_api(SetVariableStatusEnumType_Internal const& val) {
    using SrcT = SetVariableStatusEnumType_Internal;
    using TarT = SetVariableStatusEnumType_External;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownComponent);
        enum_case(UnknownVariable);
        enum_case(NotSupportedAttributeType);
        enum_case(RebootRequired);
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnumType_Internal");
}

EventTriggerEnum_External to_external_api(EventTriggerEnum_Internal const& val) {
    using SrcT = EventTriggerEnum_Internal;
    using TarT = EventTriggerEnum_External;
    switch (val) {
        enum_case(Alerting);
        enum_case(Delta);
        enum_case(Periodic);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::EventTriggerEnum_Internal");
}

EventNotificationType_External to_external_api(EventNotificationType_Internal const& val) {
    using SrcT = EventNotificationType_Internal;
    using TarT = EventNotificationType_External;
    switch (val) {
        enum_case(HardWiredNotification);
        enum_case(HardWiredMonitor);
        enum_case(PreconfiguredMonitor);
        enum_case(CustomMonitor);
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::EventNotificationType_Internal");
}

DataTransferStatus_Internal to_internal_api(DataTransferStatus_External const& val) {
    using SrcT = DataTransferStatus_External;
    using TarT = DataTransferStatus_Internal;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownMessageId);
        enum_case(UnknownVendorId);
        enum_case(Offline);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::DataTransferStatus_External");
}

DataTransferStatus_External to_external_api(DataTransferStatus_Internal const& val) {
    using SrcT = DataTransferStatus_Internal;
    using TarT = DataTransferStatus_External;
    switch (val) {
        enum_case(Accepted);
        enum_case(Rejected);
        enum_case(UnknownMessageId);
        enum_case(UnknownVendorId);
        enum_case(Offline);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::DataTransferStatus_Internal");
}

RegistrationStatus_Internal to_internal_api(RegistrationStatus_External const& val) {
    using SrcT = RegistrationStatus_External;
    using TarT = RegistrationStatus_Internal;
    switch (val) {
        enum_case(Accepted);
        enum_case(Pending);
        enum_case(Rejected);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::RegistrationStatus_External");
}

RegistrationStatus_External to_external_api(RegistrationStatus_Internal const& val) {
    using SrcT = RegistrationStatus_Internal;
    using TarT = RegistrationStatus_External;
    switch (val) {
        enum_case(Accepted);
        enum_case(Pending);
        enum_case(Rejected);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::RegistrationStatus_Internal");
}

TransactionEvent_Internal to_internal_api(TransactionEvent_External const& val) {
    using SrcT = TransactionEvent_External;
    using TarT = TransactionEvent_Internal;
    switch (val) {
        enum_case(Started);
        enum_case(Updated);
        enum_case(Ended);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::TransactionEvent_External");
}

TransactionEvent_External to_external_api(TransactionEvent_Internal const& val) {
    using SrcT = TransactionEvent_Internal;
    using TarT = TransactionEvent_External;
    switch (val) {
        enum_case(Started);
        enum_case(Updated);
        enum_case(Ended);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::TransactionEvent_Internal");
}

CustomData_Internal to_internal_api(CustomData_External const& val) {
    CustomData_Internal result;
    result.vendor_id = val.vendor_id;
    result.data = val.data;
    return result;
}

CustomData_External to_external_api(CustomData_Internal const& val) {
    CustomData_External result;
    result.vendor_id = val.vendor_id;
    result.data = val.data;
    return result;
}

DataTransferRequest_Internal to_internal_api(DataTransferRequest_External const& val) {
    DataTransferRequest_Internal result;
    result.vendor_id = val.vendor_id;
    result.message_id = val.message_id;
    result.data = val.data;
    result.custom_data = optToInternal(val.custom_data);
    return result;
}

DataTransferRequest_External to_external_api(DataTransferRequest_Internal const& val) {
    DataTransferRequest_External result;
    result.vendor_id = val.vendor_id;
    result.message_id = val.message_id;
    result.data = val.data;
    result.custom_data = optToExternal(val.custom_data);
    return result;
}

DataTransferResponse_Internal to_internal_api(DataTransferResponse_External const& val) {
    DataTransferResponse_Internal result;
    result.status = to_internal_api(val.status);
    result.data = val.data;
    result.custom_data = optToInternal(val.custom_data);
    return result;
}

DataTransferResponse_External to_external_api(DataTransferResponse_Internal const& val) {
    DataTransferResponse_External result;
    result.status = to_external_api(val.status);
    result.data = val.data;
    result.custom_data = optToExternal(val.custom_data);
    return result;
}

EVSE_Internal to_internal_api(EVSE_External const& val) {
    EVSE_Internal result;
    result.id = val.id;
    result.connector_id = val.connector_id;
    return result;
}

EVSE_External to_external_api(EVSE_Internal const& val) {
    EVSE_External result;
    result.id = val.id;
    result.connector_id = val.connector_id;
    return result;
}

Component_Internal to_internal_api(Component_External const& val) {
    Component_Internal result;
    result.name = val.name;
    result.instance = val.instance;
    result.evse = optToInternal(val.evse);
    return result;
}

Component_External to_external_api(Component_Internal const& val) {
    Component_External result;
    result.name = val.name;
    result.instance = val.instance;
    result.evse = optToExternal(val.evse);
    return result;
}

Variable_Internal to_internal_api(Variable_External const& val) {
    Variable_Internal result;
    result.name = val.name;
    result.instance = val.instance;
    return result;
}

Variable_External to_external_api(Variable_Internal const& val) {
    Variable_External result;
    result.name = val.name;
    result.instance = val.instance;
    return result;
}

ComponentVariable_Internal to_internal_api(ComponentVariable_External const& val) {
    ComponentVariable_Internal result;
    result.component = to_internal_api(val.component);
    result.variable = to_internal_api(val.variable);
    return result;
}

ComponentVariable_External to_external_api(ComponentVariable_Internal const& val) {
    ComponentVariable_External result;
    result.component = to_external_api(val.component);
    result.variable = to_external_api(val.variable);
    return result;
}

GetVariableRequest_Internal to_internal_api(GetVariableRequest_External const& val) {
    GetVariableRequest_Internal result;
    result.component_variable = to_internal_api(val.component_variable);
    result.attribute_type = optToInternal(val.attribute_type);
    return result;
}

GetVariableRequest_External to_external_api(GetVariableRequest_Internal const& val) {
    GetVariableRequest_External result;
    result.component_variable = to_external_api(val.component_variable);
    result.attribute_type = optToExternal(val.attribute_type);
    return result;
}

GetVariableResult_Internal to_internal_api(GetVariableResult_External const& val) {
    GetVariableResult_Internal result;
    result.status = to_internal_api(val.status);
    result.component_variable = to_internal_api(val.component_variable);
    result.attribute_type = optToInternal(val.attribute_type);
    result.value = val.value;
    return result;
}

GetVariableResult_External to_external_api(GetVariableResult_Internal const& val) {
    GetVariableResult_External result;
    result.status = to_external_api(val.status);
    result.component_variable = to_external_api(val.component_variable);
    result.attribute_type = optToExternal(val.attribute_type);
    result.value = val.value;
    return result;
}

SetVariableRequest_Internal to_internal_api(SetVariableRequest_External const& val) {
    SetVariableRequest_Internal result;
    result.component_variable = to_internal_api(val.component_variable);
    result.value = val.value;
    result.attribute_type = optToInternal(val.attribute_type);
    return result;
}

SetVariableRequest_External to_external_api(SetVariableRequest_Internal const& val) {
    SetVariableRequest_External result;
    result.component_variable = to_external_api(val.component_variable);
    result.value = val.value;
    result.attribute_type = optToExternal(val.attribute_type);
    return result;
}

SetVariableResult_Internal to_internal_api(SetVariableResult_External const& val) {
    SetVariableResult_Internal result;
    result.status = to_internal_api(val.status);
    result.component_variable = to_internal_api(val.component_variable);
    result.attribute_type = optToInternal(val.attribute_type);
    return result;
}

SetVariableResult_External to_external_api(SetVariableResult_Internal const& val) {
    SetVariableResult_External result;
    result.status = to_external_api(val.status);
    result.component_variable = to_external_api(val.component_variable);
    result.attribute_type = optToExternal(val.attribute_type);
    return result;
}

GetVariableRequestList_Internal to_internal_api(GetVariableRequestList_External const& val) {
    GetVariableRequestList_Internal result;
    result = vecToInternal(val.items);
    return result;
}

GetVariableRequestList_External to_external_api(GetVariableRequestList_Internal const& val) {
    GetVariableRequestList_External result;
    result.items = vecToExternal(val);
    return result;
}

GetVariableResultList_Internal to_internal_api(GetVariableResultList_External const& val) {
    GetVariableResultList_Internal result;
    result = vecToInternal(val.items);
    return result;
}

GetVariableResultList_External to_external_api(GetVariableResultList_Internal const& val) {
    GetVariableResultList_External result;
    result.items = vecToExternal(val);
    return result;
}

SetVariableRequestList_Internal to_internal_api(SetVariableRequestList_External const& val) {
    SetVariableRequestList_Internal result;
    result = vecToInternal(val.items);
    return result;
}

SetVariableRequestList_External to_external_api(SetVariableRequestList_Internal const& val) {
    SetVariableRequestList_External result;
    result.items = vecToExternal(val);
    return result;
}

SetVariableResultList_Internal to_internal_api(SetVariableResultList_External const& val) {
    SetVariableResultList_Internal result;
    result = vecToInternal(val.items);
    return result;
}

SetVariableResultList_External to_external_api(SetVariableResultList_Internal const& val) {
    SetVariableResultList_External result;
    result.items = vecToExternal(val);
    return result;
}

MonitorVariableRequestList_Internal to_internal_api(MonitorVariableRequestList_External const& val) {
    MonitorVariableRequestList_Internal result;
    result = vecToInternal(val.items);
    return result;
}

SecurityEvent_Internal to_internal_api(SecurityEvent_External const& val) {
    SecurityEvent_Internal result;
    result.type = val.type;
    result.info = val.info;
    result.critical = val.critical;
    result.timestamp = val.timestamp;
    return result;
}

SecurityEvent_External to_external_api(SecurityEvent_Internal const& val) {
    SecurityEvent_External result;
    result.type = val.type;
    result.info = val.info;
    result.critical = val.critical;
    result.timestamp = result.timestamp;
    return result;
}

StatusInfoType_Internal to_internal_api(StatusInfoType_External const& val) {
    StatusInfoType_Internal result;
    result.reason_code = val.reason_code;
    result.additional_info = val.additional_info;
    return result;
}

StatusInfoType_External to_external_api(StatusInfoType_Internal const& val) {
    StatusInfoType_External result;
    result.reason_code = val.reason_code;
    result.additional_info = val.additional_info;
    return result;
}

BootNotificationResponse_Internal to_internal_api(BootNotificationResponse_External const& val) {
    BootNotificationResponse_Internal result;
    result.status = to_internal_api(val.status);
    result.current_time = val.current_time;
    result.interval = val.interval;
    result.status_info = optToInternal(val.status_info);
    return result;
}

BootNotificationResponse_External to_external_api(BootNotificationResponse_Internal const& val) {
    BootNotificationResponse_External result;
    result.status = to_external_api(val.status);
    result.current_time = val.current_time;
    result.interval = val.interval;
    result.status_info = optToExternal(val.status_info);
    return result;
}

OcppTransactionEvent_Internal to_internal_api(OcppTransactionEvent_External const& val) {
    OcppTransactionEvent_Internal result;
    result.transaction_event = to_internal_api(val.transaction_event);
    result.session_id = val.session_id;
    result.evse = optToInternal(val.evse);
    result.transaction_id = val.transaction_id;
    return result;
}

OcppTransactionEvent_External to_external_api(OcppTransactionEvent_Internal const& val) {
    OcppTransactionEvent_External result;
    result.transaction_event = to_external_api(val.transaction_event);
    result.session_id = val.session_id;
    result.evse = optToExternal(val.evse);
    result.transaction_id = val.transaction_id;
    return result;
}

EventData_External to_external_api(EventData_Internal const& val) {
    EventData_External result;
    result.component_variable = to_external_api(val.component_variable);
    result.event_id = val.event_id;
    result.timestamp = val.timestamp;
    result.trigger = to_external_api(val.trigger);
    result.actual_value = val.actual_value;
    result.event_notification_type = to_external_api(val.event_notification_type);
    result.cause = val.cause;
    result.tech_code = val.tech_code;
    result.tech_info = val.tech_info;
    result.cleared = val.cleared;
    result.transaction_id = val.transaction_id;
    result.variable_monitoring_id = val.variable_monitoring_id;
    return result;
}

V2XFreqWattPointType_External to_external_api(V2XFreqWattPointType_Internal const& val) {
    V2XFreqWattPointType_External result;
    result.frequency = val.frequency;
    result.power = val.power;
    return result;
}

V2XSignalWattPointCurve_External to_external_api(V2XSignalWattPointCurve_Internal const& val) {
    V2XSignalWattPointCurve_External result;
    result.signal = val.signal;
    result.power = val.power;
    return result;
}

OperationMode_External to_external_api(OperationMode_Internal const& val) {
    using SrcT = OperationMode_Internal;
    using TarT = OperationMode_External;
    switch (val) {
        enum_case(Idle);
        enum_case(ChargingOnly);
        enum_case(CentralSetpoint);
        enum_case(ExternalSetpoint);
        enum_case(ExternalLimits);
        enum_case(CentralFrequency);
        enum_case(LocalFrequency);
        enum_case(LocalLoadBalancing);
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::Operation_mode");
}

ChargingSchedulePeriod_External to_external_api(ChargingSchedulePeriod_Internal const& val) {
    ChargingSchedulePeriod_External result;
    result.start_period = val.start_period;
    result.limit = val.limit;
    result.limit_L2 = val.limit_L2;
    result.limit_L3 = val.limit_L3;
    result.number_phases = val.number_phases;
    result.stack_level = val.stack_level;
    result.phase_to_use = val.phase_to_use;
    result.discharge_limit = val.discharge_limit;
    result.discharge_limit_L2 = val.discharge_limit_L2;
    result.discharge_limit_L3 = val.discharge_limit_L3;
    result.setpoint = val.setpoint;
    result.setpoint_L2 = val.setpoint_L2;
    result.setpoint_L3 = val.setpoint_L3;
    result.setpoint_reactive = val.setpoint_reactive;
    result.setpoint_reactive_L2 = val.setpoint_reactive_L2;
    result.setpoint_reactive_L3 = val.setpoint_reactive_L3;
    if (val.preconditioning_request) {
        result.preconditioning_request = val.preconditioning_request.value() != 0;
    }
    result.evse_sleep = val.evse_sleep;
    result.v2x_baseline = val.v2x_baseline;
    result.operation_mode = optToExternal(val.operation_mode);
    result.v2x_freq_watt_curve = vecToExternal(val.v2x_freq_watt_curve);
    result.v2x_signal_watt_curve = vecToExternal(val.v2x_signal_watt_curve);
    return result;
}

ChargingSchedule_External to_external_api(ChargingSchedule_Internal const& val) {
    ChargingSchedule_External result;
    result.evse = val.evse;
    result.charging_rate_unit = val.charging_rate_unit;
    result.charging_schedule_period = vecToExternal(val.charging_schedule_period);
    result.duration = val.duration;
    result.start_schedule = val.start_schedule;
    result.min_charging_rate = val.min_charging_rate;
    return result;
}

ChargingSchedules_External to_external_api(ChargingSchedules_Internal const& val) {
    ChargingSchedules_External result;
    result.schedules = vecToExternal(val.schedules);
    return result;
}

} // namespace everest::lib::API::V1_0::types::ocpp
