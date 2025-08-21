// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ac_rcdImpl.hpp"

namespace module {
namespace rcd {

void ac_rcdImpl::init() {
    mod->serial.signalErrorFlags.connect([this](ErrorFlags error_flags) {
        if (error_flags.rcd_triggered and not last_error_flags.rcd_triggered) {
            Everest::error::Error error_object =
                this->error_factory->create_error("ac_rcd/DC", "", "RDC-MD triggered", Everest::error::Severity::High);
            this->raise_error(error_object);
        } else if (not error_flags.rcd_triggered and last_error_flags.rcd_triggered) {
            this->clear_error("ac_rcd/DC");
        }

        if (error_flags.rcd_selftest_failed and not last_error_flags.rcd_selftest_failed) {
            Everest::error::Error error_object = this->error_factory->create_error(
                "ac_rcd/Selftest", "", "RCD self-test failed", Everest::error::Severity::High);
            this->raise_error(error_object);
        } else if (not error_flags.rcd_selftest_failed and last_error_flags.rcd_selftest_failed) {
            this->clear_error("ac_rcd/Selftest");
        }

        last_error_flags = error_flags;
    });
}

void ac_rcdImpl::ready() {
}

void ac_rcdImpl::handle_self_test() {
    mod->serial.set_rcd_test(true);
}

bool ac_rcdImpl::handle_reset() {
    mod->serial.reset_rcd(true);
    return true;
}

} // namespace rcd
} // namespace module
