// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "external_energy_limitsImpl.hpp"

namespace module {
namespace external_limits {

void external_energy_limitsImpl::init() {
}

void external_energy_limitsImpl::ready() {
}

void external_energy_limitsImpl::handle_set_external_limits(types::energy::ExternalLimits& value) {
    // your code for cmd set_external_limits goes here
    mod->signalExternalLimit(value);
};

} // namespace external_limits
} // namespace module
