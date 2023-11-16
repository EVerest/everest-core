// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVERESTPY_MODULE_HPP
#define EVERESTPY_MODULE_HPP

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <framework/everest.hpp>

#include "misc.hpp"

class Module {
public:
    Module(const RuntimeSession&);
    Module(const std::string&, const RuntimeSession&);

    ModuleSetup say_hello();

    void init_done(std::function<void()> on_ready_handler) {
        if (on_ready_handler) {
            handle->register_on_ready_handler(std::move(on_ready_handler));
        }

        handle->signal_ready();
    }

    void init_done() {
        init_done(nullptr);
    }

    Everest::Config& get_config() {
        return session.get_config();
    }

    json call_command(const Fulfillment& fulfillment, const std::string& cmd_name, json args);
    void publish_variable(const std::string& impl_id, const std::string& var_name, json value);
    void implement_command(const std::string& impl_id, const std::string& cmd_name, std::function<json(json)> handler);
    void subscribe_variable(const Fulfillment& fulfillment, const std::string& var_name,
                            std::function<void(json)> callback);
    std::string raise_error(const std::string& impl_id, const std::string& error_type, const std::string& message,
                            const std::string& severity);
    json request_clear_error_uuid(const std::string& impl_id, const std::string& uuid);
    json request_clear_error_all_of_type(const std::string& impl_id, const std::string& error_type);
    json request_clear_error_all_of_module(const std::string& impl_id);
    void subscribe_error(const Fulfillment& fulfillment, const std::string& error_type, const JsonCallback& error_cb_,
                         const JsonCallback& error_cleared_cb_);
    void subscribe_all_errors(const Fulfillment& fulfillment, const JsonCallback& error_cb_,
                              const JsonCallback& error_cleared_cb_);

    const auto& get_fulfillments() const {
        return fulfillments;
    }

    const auto& get_info() const {
        return module_info;
    }

    const auto& get_requirements() const {
        return requirements;
    }

    const auto& get_implementations() const {
        return implementations;
    }

private:
    const std::string module_id;
    const RuntimeSession& session;

    std::unique_ptr<Everest::Everest> handle;

    // NOTE (aw): we're keeping the handlers local to the module instance and don't pass them by copy-construction
    // to "external" c/c++ code, so no GIL related problems should appear
    std::deque<std::function<json(json)>> command_handlers{};
    std::deque<std::function<void(json)>> subscription_callbacks{};
    std::deque<std::function<void(json)>> err_susbcription_callbacks{};
    std::deque<std::function<void(json)>> err_cleared_susbcription_callbacks{};

    static std::unique_ptr<Everest::Everest> create_everest_instance(const std::string& module_id,
                                                                     const RuntimeSession& session);

    ModuleInfo module_info{};
    std::map<std::string, Interface> requirements;
    std::map<std::string, Interface> implementations;
    std::map<std::string, std::vector<Fulfillment>> fulfillments;
};

#endif // EVERESTPY_MODULE_HPP
