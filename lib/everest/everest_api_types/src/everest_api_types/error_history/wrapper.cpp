// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "error_history/wrapper.hpp"
#include "error_history/API.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::error_history {

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
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(toInternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toInternalApi(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(toExternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toInternalApi(val); });
}

} // namespace

State_Internal toInternalApi(State_External const& val) {
    using SrcT = State_External;
    using TarT = State_Internal;
    switch (val) {
    case SrcT::Active:
        return TarT::Active;
    case SrcT::ClearedByModule:
        return TarT::ClearedByModule;
    case SrcT::ClearedByReboot:
        return TarT::ClearedByReboot;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::error_history::State_External");
}

State_External toExternalApi(State_Internal const& val) {
    using SrcT = State_Internal;
    using TarT = State_External;
    switch (val) {
    case SrcT::Active:
        return TarT::Active;
    case SrcT::ClearedByModule:
        return TarT::ClearedByModule;
    case SrcT::ClearedByReboot:
        return TarT::ClearedByReboot;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::error_history::State_Internal");
}

SeverityFilter_Internal toInternalApi(SeverityFilter_External const& val) {
    using SrcT = SeverityFilter_External;
    using TarT = SeverityFilter_Internal;
    switch (val) {
    case SrcT::HIGH_GE:
        return TarT::HIGH_GE;
    case SrcT::MEDIUM_GE:
        return TarT::MEDIUM_GE;
    case SrcT::LOW_GE:
        return TarT::LOW_GE;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::error_history::SeverityFilter_External");
}

SeverityFilter_External toExternalApi(SeverityFilter_Internal const& val) {
    using SrcT = SeverityFilter_Internal;
    using TarT = SeverityFilter_External;
    switch (val) {
    case SrcT::HIGH_GE:
        return TarT::HIGH_GE;
    case SrcT::MEDIUM_GE:
        return TarT::MEDIUM_GE;
    case SrcT::LOW_GE:
        return TarT::LOW_GE;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::error_history::SeverityFilter_Internal");
}

Severity_Internal toInternalApi(Severity_External const& val) {
    using SrcT = Severity_External;
    using TarT = Severity_Internal;
    switch (val) {
    case SrcT::High:
        return TarT::High;
    case SrcT::Medium:
        return TarT::Medium;
    case SrcT::Low:
        return TarT::Low;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::error_history::Severity_External");
}

Severity_External toExternalApi(Severity_Internal const& val) {
    using SrcT = Severity_Internal;
    using TarT = Severity_External;
    switch (val) {
    case SrcT::High:
        return TarT::High;
    case SrcT::Medium:
        return TarT::Medium;
    case SrcT::Low:
        return TarT::Low;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::error_history::Severity_Internal");
}

ImplementationIdentifier_Internal toInternalApi(ImplementationIdentifier_External const& val) {
    ImplementationIdentifier_Internal result;
    result.module_id = val.module_id;
    result.implementation_id = val.implementation_id;
    return result;
}

ImplementationIdentifier_External toExternalApi(ImplementationIdentifier_Internal const& val) {
    ImplementationIdentifier_External result;
    result.module_id = val.module_id;
    result.implementation_id = val.implementation_id;
    return result;
}

TimeperiodFilter_Internal toInternalApi(TimeperiodFilter_External const& val) {
    TimeperiodFilter_Internal result;
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    return result;
}

TimeperiodFilter_External toExternalApi(TimeperiodFilter_Internal const& val) {
    TimeperiodFilter_External result;
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    return result;
}

FilterArguments_Internal toInternalApi(FilterArguments_External const& val) {
    FilterArguments_Internal result;
    result.state_filter = optToInternal(val.state_filter);
    result.origin_filter = optToInternal(val.origin_filter);
    result.type_filter = val.type_filter;
    result.severity_filter = optToInternal(val.severity_filter);
    result.timeperiod_filter = optToInternal(val.timeperiod_filter);
    result.handle_filter = val.handle_filter;
    return result;
}

FilterArguments_External toExternalApi(FilterArguments_Internal const& val) {
    FilterArguments_External result;
    result.state_filter = optToExternal(val.state_filter);
    result.origin_filter = optToExternal(val.origin_filter);
    result.type_filter = val.type_filter;
    result.severity_filter = optToExternal(val.severity_filter);
    result.timeperiod_filter = optToExternal(val.timeperiod_filter);
    result.handle_filter = val.handle_filter;
    return result;
}

ErrorObject_Internal toInternalApi(ErrorObject_External const& val) {
    ErrorObject_Internal result;
    result.type = val.type;
    result.description = val.description;
    result.message = val.message;
    result.severity = toInternalApi(val.severity);
    result.origin = toInternalApi(val.origin);
    result.timestamp = val.timestamp;
    result.uuid = val.uuid;
    result.state = toInternalApi(val.state);
    result.sub_type = val.sub_type;
    return result;
}

ErrorObject_External toExternalApi(ErrorObject_Internal const& val) {
    ErrorObject_External result;
    result.type = val.type;
    result.description = val.description;
    result.message = val.message;
    result.severity = toExternalApi(val.severity);
    result.origin = toExternalApi(val.origin);
    result.timestamp = val.timestamp;
    result.uuid = val.uuid;
    result.state = toExternalApi(val.state);
    result.sub_type = val.sub_type;
    return result;
}

ErrorList_Internal toInternalApi(ErrorList_External const& val) {
    ErrorList_Internal result;
    result = vecToInternal(val.errors);
    return result;
}

ErrorList_External toExternalApi(ErrorList_Internal const& val) {
    ErrorList_External result;
    result.errors = vecToExternal(val);
    return result;
}

} // namespace everest::lib::API::V1_0::types::error_history
