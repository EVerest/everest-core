// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "example_userImpl.hpp"

namespace module {
namespace example_user {

void example_userImpl::init() {
    mod->r_example_multiple.at(0)->subscribe_max_current(
        [](double max_current) { EVLOG_info << "0: Got max current: " << max_current; });
    mod->r_example_multiple.at(1)->subscribe_max_current(
        [](double max_current) { EVLOG_info << "1: Got max current: " << max_current; });

    mod->r_example_multiple.at(0)->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            EVLOG_info << "error raised: " << error.type << ": " << error.message;
        },
        [this](const Everest::error::Error& error) {
            EVLOG_info << "error cleared: " << error.type << ": " << error.message;
        });

    mod->r_example_multiple.at(1)->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            EVLOG_info << "error raised: " << error.type << ": " << error.message;
        },
        [this](const Everest::error::Error& error) {
            EVLOG_info << "error cleared: " << error.type << ": " << error.message;
        });

    Everest::error::ErrorCallback error_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received global error: " << error.type << ": " << error.message;
    };
    Everest::error::ErrorCallback error_cleared_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received global error cleared: " << error.type << ": " << error.message;
    };
    subscribe_global_all_errors(error_callback, error_cleared_callback);
}

void example_userImpl::ready() {
    mod->r_example->call_uses_something("hello_there");

    mod->r_example_multiple.at(0)->call_uses_something("hello there");
    mod->r_example_multiple.at(1)->call_uses_something("hello there");
    // mod->r_example_multiple.at(2)->call_uses_something("hello there");
}

} // namespace example_user
} // namespace module
