// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/config/storage_types.hpp>

namespace everest::config {
bool ConfigurationParameterIdentifier::operator<(const ConfigurationParameterIdentifier& rhs) const {
    return (
        this->module_id < rhs.module_id ||
        (this->module_id == rhs.module_id && this->configuration_parameter_name < rhs.configuration_parameter_name) ||
        (this->module_id == rhs.module_id && this->configuration_parameter_name == rhs.configuration_parameter_name &&
         this->module_implementation_id < rhs.module_implementation_id));
}

} // namespace everest::config
