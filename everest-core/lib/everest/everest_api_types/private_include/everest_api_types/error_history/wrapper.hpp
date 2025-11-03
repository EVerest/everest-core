// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/error_history/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/error_history.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::error_history {

using State_Internal = ::types::error_history::State;
using State_External = State;

State_Internal to_internal_api(State_External const& val);
State_External to_external_api(State_Internal const& val);

using SeverityFilter_Internal = ::types::error_history::SeverityFilter;
using SeverityFilter_External = SeverityFilter;

SeverityFilter_Internal to_internal_api(SeverityFilter_External const& val);
SeverityFilter_External to_external_api(SeverityFilter_Internal const& val);

using Severity_Internal = ::types::error_history::Severity;
using Severity_External = Severity;

Severity_Internal to_internal_api(Severity_External const& val);
Severity_External to_external_api(Severity_Internal const& val);

using ImplementationIdentifier_Internal = ::types::error_history::ImplementationIdentifier;
using ImplementationIdentifier_External = ImplementationIdentifier;

ImplementationIdentifier_Internal to_internal_api(ImplementationIdentifier_External const& val);
ImplementationIdentifier_External to_external_api(ImplementationIdentifier_Internal const& val);

using TimeperiodFilter_Internal = ::types::error_history::TimeperiodFilter;
using TimeperiodFilter_External = TimeperiodFilter;

TimeperiodFilter_Internal to_internal_api(TimeperiodFilter_External const& val);
TimeperiodFilter_External to_external_api(TimeperiodFilter_Internal const& val);

using FilterArguments_Internal = ::types::error_history::FilterArguments;
using FilterArguments_External = FilterArguments;

FilterArguments_Internal to_internal_api(FilterArguments_External const& val);
FilterArguments_External to_external_api(FilterArguments_Internal const& val);

using ErrorObject_Internal = ::types::error_history::ErrorObject;
using ErrorObject_External = ErrorObject;

ErrorObject_Internal to_internal_api(ErrorObject_External const& val);
ErrorObject_External to_external_api(ErrorObject_Internal const& val);

using ErrorList_Internal = std::vector<ErrorObject_Internal>;
using ErrorList_External = ErrorList;

ErrorList_Internal to_internal_api(ErrorList_External const& val);
ErrorList_External to_external_api(ErrorList_Internal const& val);

} // namespace everest::lib::API::V1_0::types::error_history
