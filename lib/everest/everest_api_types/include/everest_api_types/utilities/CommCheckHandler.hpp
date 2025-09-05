// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "VarContainer.hpp"
#include "everest/logging.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

namespace everest::lib::API {

template <class InterfaceT> class CommCheckHandler {
public:
    using ErrorClearFtor = std::function<void()>;
    using ErrorRaiseFtor = std::function<void(std::string const&)>;
    using ErrorCheckFtor = std::function<bool()>;
    using HeartBeatFtor = std::function<bool()>;
    CommCheckHandler(std::string const& error_type_, std::string const& default_sub_type_,
                     std::shared_ptr<InterfaceT> const& interface) :
        error_type(error_type_), default_sub_type(default_sub_type_) {
        raise_error = [this, interface](std::string const& sub_type) {
            const std::string message{"Send communication_check to clear the error"};
            auto error =
                interface->error_factory->create_error(error_type, sub_type, message, Everest::error::Severity::High);
            try {
                return interface->raise_error(error);
            } catch (...) {
            }
        };
        clear_error = [this, interface]() {
            try {
                if (interface->error_state_monitor->is_error_active(error_type, default_sub_type)) {
                    interface->clear_error(error_type, default_sub_type);
                }
                if (interface->error_state_monitor->is_error_active(error_type, init_sub_type)) {
                    interface->clear_error(error_type, init_sub_type);
                }
            } catch (...) {
                EVLOG_info << "Failed to clear error: " << error_type;
            }
        };
        check_error = [this, interface]() {
            return interface->error_state_monitor->is_error_active(error_type, default_sub_type) ||
                   interface->error_state_monitor->is_error_active(error_type, init_sub_type);
        };
    }

    ~CommCheckHandler() {
        check_active.store(false);
    }

    void set_value(bool value) {
        comm_check_value.set(value);
    }

    void set_error() {
        if (check_active.load()) {
            raise_error(default_sub_type);
        }
    }

    void start(int timeout_s) {
        timeout = std::chrono::seconds(timeout_s);
        if (timeout_s < 1) {
            EVLOG_info << "No communication checks" << std::endl;
            check_active.store(false);
        } else {
            handler = std::thread([this]() { this->communication_check_handler(); });
        }
    }

    bool heartbeat(int interval_ms, HeartBeatFtor const& ftor) {
        if (interval_ms <= 0) {
            return false;
        }
        auto interval = std::chrono::milliseconds(interval_ms);
        heartbeat_active.store(true);
        auto action = [this, interval, ftor]() {
            while (heartbeat_active.load()) {
                auto now = std::chrono::steady_clock::now();
                auto result = ftor();
                if (not result) {
                    heartbeat_active.store(false);
                    break;
                }
                std::this_thread::sleep_until(now + interval);
            }
        };
        heartbeat_handler = std::thread(action);
        return true;
    }

private:
    void communication_check_handler() {
        raise_error(init_sub_type);
        while (check_active.load()) {
            auto check = comm_check_value.wait_for(timeout);
            if (check.has_value()) {
                check_active.store(check.value());
                if (check_error()) {
                    EVLOG_info << "Communication check: SUCCESS " << std::endl;
                    clear_error();
                }
            } else {
                if (!check_error()) {
                    EVLOG_info << "Communication check: FAILURE " << std::endl;
                    raise_error(default_sub_type);
                }
            }
        }
    }

    std::chrono::seconds timeout;
    ErrorRaiseFtor raise_error;
    ErrorClearFtor clear_error;
    ErrorCheckFtor check_error;
    VarContainer<bool> comm_check_value;
    std::thread handler;
    std::thread heartbeat_handler;
    std::atomic_bool check_active{true};
    std::atomic_bool heartbeat_active{false};
    const std::string error_type;
    const std::string init_sub_type{"Initial communication check"};
    const std::string default_sub_type;
};

} // namespace everest::lib::API
