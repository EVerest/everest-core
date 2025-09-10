// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <generated/types/error_history.hpp>
#include <utils/error.hpp>

namespace error_converter {

types::error_history::ErrorObject framework_to_internal_api(Everest::error::Error const& val);
types::error_history::Severity framework_to_internal_api(Everest::error::Severity const& val);
types::error_history::ImplementationIdentifier framework_to_internal_api(ImplementationIdentifier const& val);
types::error_history::State framework_to_internal_api(Everest::error::State const& val);
} // namespace error_converter
