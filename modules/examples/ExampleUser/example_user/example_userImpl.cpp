// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "example_userImpl.hpp"

namespace module {
namespace example_user {

void example_userImpl::init() {
    Everest::error::ErrorCallback error_callback = [this](const Everest::error::Error& error) {
        EVLOG_error << "received error from error manager: " << error.type;
        this->received_error = error.uuid;
    };
    Everest::error::ErrorCallback error_cleared_callback = [](const Everest::error::Error& error) {
        EVLOG_info << "received error cleared from error manager: " << error.type;
    };
    mod->r_example->subscribe_error_example_ExampleErrorA(
        error_callback,
        error_cleared_callback
    );
    mod->r_example->subscribe_all_errors(
        error_callback,
        error_cleared_callback
    );
}

void example_userImpl::ready() {
    mod->r_example->call_uses_something("hello_there");
    std::this_thread::sleep_for(std::chrono::seconds(6));
    request_clear_error(this->received_error);
}

} // namespace example_user
} // namespace module
