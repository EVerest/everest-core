// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "test_error_handlingImpl.hpp"

#include <fmt/format.h>
#include <utils/error.hpp>
#include <utils/error/error_json.hpp>

namespace module {
namespace main {

void test_error_handlingImpl::init() {
    this->mod->r_error_raiser->subscribe_error_test_errors_TestErrorA(
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_subscribe_TestErrorA(json(error));
        },
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_cleared_subscribe_TestErrorA(json(error));
        });
    this->mod->r_error_raiser->subscribe_error_test_errors_TestErrorB(
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_subscribe_TestErrorB(json(error));
        },
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_cleared_subscribe_TestErrorB(json(error));
        });
    this->mod->r_error_raiser->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_subscribe_all(json(error));
        },
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_cleared_subscribe_all(json(error));
        });
    subscribe_global_all_errors(
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_subscribe_global_all(json(error));
        },
        [this](const Everest::error::Error& error) {
            EVLOG_debug << fmt::format("received error: {}", json(error).dump(2));
            this->publish_errors_cleared_subscribe_global_all(json(error));
        });
}

void test_error_handlingImpl::ready() {
}

void test_error_handlingImpl::handle_clear_error(std::string& type, std::string& sub_type) {
    this->clear_error(type, sub_type);
}


void test_error_handlingImpl::handle_clear_all_errors() {
    this->clear_all_errors_of_impl();
}

void test_error_handlingImpl::handle_raise_error(std::string& type, std::string& sub_type, std::string& message, std::string& severity) {
    Everest::error::Error error = this->create_error();
    error.type = type;
    error.sub_type = sub_type;
    error.message = message;
    error.severity = Everest::error::string_to_severity(severity);
    this->raise_error(error);
}

} // namespace main
} // namespace module
