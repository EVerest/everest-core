// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "example_error_frameworkImpl.hpp"

namespace module {
namespace example_global_subscriber {

void example_error_frameworkImpl::init() {
    Everest::error::ErrorCallback error_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received error: " << error.type;
        this->test_state_monitor();
    };
    Everest::error::ErrorCallback error_cleared_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received error cleared: " << error.type;
        this->test_state_monitor();
    };
    subscribe_global_all_errors(error_callback, error_cleared_callback);
}

void example_error_frameworkImpl::test_state_monitor() {
    EVLOG_info << "Currently there are " << get_global_error_state_monitor()->get_ative_error_count()
               << " errors active.";
}

void example_error_frameworkImpl::ready() {
}

} // namespace example_global_subscriber
} // namespace module
