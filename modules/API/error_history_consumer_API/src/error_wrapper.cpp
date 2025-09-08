// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "error_wrapper.hpp"
#include "generated/types/error_history.hpp"
#include "utils/date.hpp"
#include "utils/error.hpp"

namespace basecamp {

namespace error_converter {

types::error_history::Severity framework_to_internal_api(Everest::error::Severity const& val) {
    using SrcT = Everest::error::Severity;
    using TarT = types::error_history::Severity;
    switch (val) {
    case SrcT::High:
        return TarT::High;
    case SrcT::Medium:
        return TarT::Medium;
    case SrcT::Low:
        return TarT::Low;
    }
    throw std::out_of_range("Unexpected value for Everest::error::Severity");
}

types::error_history::ImplementationIdentifier framework_to_internal_api(ImplementationIdentifier const& val) {
    types::error_history::ImplementationIdentifier result;
    result.implementation_id = val.implementation_id;
    result.module_id = val.module_id;
    return result;
}

types::error_history::State framework_to_internal_api(Everest::error::State const& val) {
    using SrcT = Everest::error::State;
    using TarT = types::error_history::State;
    switch (val) {
    case SrcT::Active:
        return TarT::Active;
    case SrcT::ClearedByModule:
        return TarT::ClearedByModule;
    case SrcT::ClearedByReboot:
        return TarT::ClearedByReboot;
    }
    throw std::out_of_range("Unexpected value for basecamp::API::V1_0::types::error_history::State_External");
}

types::error_history::ErrorObject framework_to_internal_api(Everest::error::Error const& val) {
    types::error_history::ErrorObject result;
    result.type = val.type;
    result.description = val.description;
    result.message = val.message;
    result.severity = framework_to_internal_api(val.severity);
    result.origin = framework_to_internal_api(val.origin);
    result.timestamp = Everest::Date::to_rfc3339(val.timestamp);
    result.uuid = val.uuid.to_string();
    result.state = framework_to_internal_api(val.state);
    if (not val.sub_type.empty()) {
        result.sub_type.emplace(val.sub_type);
    }
    return result;
}

} // namespace error_converter
} // namespace basecamp
