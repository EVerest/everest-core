// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ac_rcdImpl.hpp"

namespace module {
namespace rcd_1 {

void ac_rcdImpl::init() {
    mod->serial.signal_error_flags.connect([this](int connector, ErrorFlags error_flags) {
        if ((connector == 1)) {
            if (error_flags.rcd_triggered and not last_error_flags.rcd_triggered) {
                Everest::error::Error error_object = this->error_factory->create_error(
                    "ac_rcd/DC", "", "Port 1 RDC-MD triggered", Everest::error::Severity::High);
                this->raise_error(error_object);
            } else if (not error_flags.rcd_triggered and last_error_flags.rcd_triggered) {
                this->clear_error("ac_rcd/DC");
            }

            last_error_flags = error_flags;
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
