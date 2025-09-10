// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "dc_external_derateImpl.hpp"

namespace module {
namespace dc_external_derate {

void dc_external_derateImpl::init() {
}

void dc_external_derateImpl::ready() {
}

void dc_external_derateImpl::handle_set_external_derating(types::dc_external_derate::ExternalDerating& derate) {
    mod->set_external_derating(derate);
}

} // namespace dc_external_derate
} // namespace module
