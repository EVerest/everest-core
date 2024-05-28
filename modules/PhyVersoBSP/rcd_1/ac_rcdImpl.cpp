// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ac_rcdImpl.hpp"

namespace module {
namespace rcd_1 {

void ac_rcdImpl::init() {
    mod->serial.signal_error_flags.connect([this](int connector, ErrorFlags error_flags) {
        if ((connector == 1) && error_flags.rcd_triggered) {
            EVLOG_info << "[1] RCD triggered: " << error_flags.rcd_triggered;
            // TODO: publish somewhere
        }
    });
}

void ac_rcdImpl::ready() {
}

void ac_rcdImpl::handle_self_test() {
    mod->serial.set_rcd_test(1, true);
}

bool ac_rcdImpl::handle_reset() {
    mod->serial.reset_rcd(1, true);
    return true;
}

} // namespace rcd_1
} // namespace module
