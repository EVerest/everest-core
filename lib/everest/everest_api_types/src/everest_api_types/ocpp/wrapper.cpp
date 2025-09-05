// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp/wrapper.hpp"
#include "ocpp/API.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::ocpp {

namespace {
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
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

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}

} // namespace

AttributeEnum_Internal to_internal_api(AttributeEnum_External const& val) {
    using SrcT = AttributeEnum_External;
    using TarT = AttributeEnum_Internal;
    switch (val) {
    case SrcT::Actual:
        return TarT::Actual;
    case SrcT::Target:
        return TarT::Target;
    case SrcT::MinSet:
        return TarT::MinSet;
    case SrcT::MaxSet:
        return TarT::MaxSet;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::AttributeEnum_External");
}

AttributeEnum_External to_external_api(AttributeEnum_Internal const& val) {
    using SrcT = AttributeEnum_Internal;
    using TarT = AttributeEnum_External;
    switch (val) {
    case SrcT::Actual:
        return TarT::Actual;
    case SrcT::Target:
        return TarT::Target;
    case SrcT::MinSet:
        return TarT::MinSet;
    case SrcT::MaxSet:
        return TarT::MaxSet;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::AttributeEnum_Internal");
}

GetVariableStatusEnumType_Internal to_internal_api(GetVariableStatusEnumType_External const& val) {
    using SrcT = GetVariableStatusEnumType_External;
    using TarT = GetVariableStatusEnumType_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownComponent:
        return TarT::UnknownComponent;
    case SrcT::UnknownVariable:
        return TarT::UnknownVariable;
    case SrcT::NotSupportedAttributeType:
        return TarT::NotSupportedAttributeType;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnumType_External");
}

GetVariableStatusEnumType_External to_external_api(GetVariableStatusEnumType_Internal const& val) {
    using SrcT = GetVariableStatusEnumType_Internal;
    using TarT = GetVariableStatusEnumType_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownComponent:
        return TarT::UnknownComponent;
    case SrcT::UnknownVariable:
        return TarT::UnknownVariable;
    case SrcT::NotSupportedAttributeType:
        return TarT::NotSupportedAttributeType;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnumType_Internal");
}

SetVariableStatusEnumType_Internal to_internal_api(SetVariableStatusEnumType_External const& val) {
    using SrcT = SetVariableStatusEnumType_External;
    using TarT = SetVariableStatusEnumType_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownComponent:
        return TarT::UnknownComponent;
    case SrcT::UnknownVariable:
        return TarT::UnknownVariable;
    case SrcT::NotSupportedAttributeType:
        return TarT::NotSupportedAttributeType;
    case SrcT::RebootRequired:
        return TarT::RebootRequired;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnumType_External");
}

SetVariableStatusEnumType_External to_external_api(SetVariableStatusEnumType_Internal const& val) {
    using SrcT = SetVariableStatusEnumType_Internal;
    using TarT = SetVariableStatusEnumType_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownComponent:
        return TarT::UnknownComponent;
    case SrcT::UnknownVariable:
        return TarT::UnknownVariable;
    case SrcT::NotSupportedAttributeType:
        return TarT::NotSupportedAttributeType;
    case SrcT::RebootRequired:
        return TarT::RebootRequired;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnumType_Internal");
}

DataTransferStatus_Internal to_internal_api(DataTransferStatus_External const& val) {
    using SrcT = DataTransferStatus_External;
    using TarT = DataTransferStatus_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownMessageId:
        return TarT::UnknownMessageId;
    case SrcT::UnknownVendorId:
        return TarT::UnknownVendorId;
    case SrcT::Offline:
        return TarT::Offline;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::DataTransferStatus_External");
}

DataTransferStatus_External to_external_api(DataTransferStatus_Internal const& val) {
    using SrcT = DataTransferStatus_Internal;
    using TarT = DataTransferStatus_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::UnknownMessageId:
        return TarT::UnknownMessageId;
    case SrcT::UnknownVendorId:
        return TarT::UnknownVendorId;
    case SrcT::Offline:
        return TarT::Offline;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::DataTransferStatus_Internal");
}

RegistrationStatus_Internal to_internal_api(RegistrationStatus_External const& val) {
    using SrcT = RegistrationStatus_External;
    using TarT = RegistrationStatus_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Pending:
        return TarT::Pending;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::RegistrationStatus_External");
}

RegistrationStatus_External to_external_api(RegistrationStatus_Internal const& val) {
    using SrcT = RegistrationStatus_Internal;
    using TarT = RegistrationStatus_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Pending:
        return TarT::Pending;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::RegistrationStatus_Internal");
}

TransactionEvent_Internal to_internal_api(TransactionEvent_External const& val) {
    using SrcT = TransactionEvent_External;
    using TarT = TransactionEvent_Internal;
    switch (val) {
    case SrcT::Started:
        return TarT::Started;
    case SrcT::Updated:
        return TarT::Updated;
    case SrcT::Ended:
        return TarT::Ended;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::ocpp::TransactionEvent_External");
}

TransactionEvent_External to_external_api(TransactionEvent_Internal const& val) {
    using SrcT = TransactionEvent_Internal;
    using TarT = TransactionEvent_External;
    switch (val) {
    case SrcT::Started:
        return TarT::Started;
    case SrcT::Updated:
        return TarT::Updated;
    case SrcT::Ended:
        return TarT::Ended;
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

SecurityEvent_Internal to_internal_api(SecurityEvent_External const& val) {
    SecurityEvent_Internal result;
    result.type = val.type;
    result.info = val.info;
    return result;
}

SecurityEvent_External to_external_api(SecurityEvent_Internal const& val) {
    SecurityEvent_External result;
    result.type = val.type;
    result.info = val.info;
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

} // namespace everest::lib::API::V1_0::types::ocpp
