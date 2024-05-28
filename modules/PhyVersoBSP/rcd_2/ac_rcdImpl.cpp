// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ac_rcdImpl.hpp"

namespace module {
namespace rcd_2 {

void ac_rcdImpl::init() {
    mod->serial.signal_error_flags.connect([this](int connector, ErrorFlags error_flags) {
        if ((connector == 2) && error_flags.rcd_triggered) {
            EVLOG_info << "[2] RCD triggered: " << error_flags.rcd_triggered;
            // TODO: publish somewhere
        }
    });
}

void ac_rcdImpl::ready() {
}

void ac_rcdImpl::handle_self_test() {
    mod->serial.set_rcd_test(2, true);
}

bool ac_rcdImpl::handle_reset() {
    mod->serial.reset_rcd(2, true);
    return true;
}

} // namespace rcd_2
} // namespace module
