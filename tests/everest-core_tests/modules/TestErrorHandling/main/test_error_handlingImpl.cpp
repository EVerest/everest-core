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

void test_error_handlingImpl::handle_clear_error_by_uuid(std::string& uuid) {
    this->request_clear_error(Everest::error::ErrorHandle(uuid));
}

void test_error_handlingImpl::handle_clear_errors_by_type(std::string& type) {
    if (type == "test_errors/TestErrorA") {
        this->request_clear_all_test_errors_TestErrorA();
    } else if (type == "test_errors/TestErrorB") {
        this->request_clear_all_test_errors_TestErrorB();
    } else if (type == "test_errors/TestErrorC") {
        this->request_clear_all_test_errors_TestErrorC();
    } else if (type == "test_errors/TestErrorD") {
        this->request_clear_all_test_errors_TestErrorD();
    } else {
        throw std::runtime_error("unknown error type");
    }
}

void test_error_handlingImpl::handle_clear_all_errors() {
    this->request_clear_all_errors();
}

std::string test_error_handlingImpl::handle_raise_error(std::string& type, std::string& message,
                                                        std::string& severity) {
    Everest::error::Severity severity_enum = Everest::error::string_to_severity(severity);
    Everest::error::ErrorHandle handle;
    if (type == "test_errors/TestErrorA") {
        handle = this->raise_test_errors_TestErrorA(message, severity_enum);
    } else if (type == "test_errors/TestErrorB") {
        handle = this->raise_test_errors_TestErrorB(message, severity_enum);
    } else if (type == "test_errors/TestErrorC") {
        handle = this->raise_test_errors_TestErrorC(message, severity_enum);
    } else if (type == "test_errors/TestErrorD") {
        handle = this->raise_test_errors_TestErrorD(message, severity_enum);
    } else {
        throw std::runtime_error("unknown error type");
    }
    return handle.uuid;
}

} // namespace main
} // namespace module
