// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVERESTPY_MODULE_HPP
#define EVERESTPY_MODULE_HPP

#include <chrono>
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

    void init_done(const std::function<void()>& on_ready_handler) {
        this->handle->check_code();

        if (on_ready_handler) {
            handle->register_on_ready_handler(on_ready_handler);
        }

        const auto end_time = std::chrono::system_clock::now();
        EVLOG_info << "Module " << fmt::format(Everest::TERMINAL_STYLE_BLUE, "{}", this->module_id) << " initialized ["
                   << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - this->start_time).count()
                   << "ms]";

        handle->signal_ready();
    }

    void init_done() {
        init_done(nullptr);
    }

    Everest::Config& get_config() {
        return *config_;
    }

    json call_command(const Fulfillment& fulfillment, const std::string& cmd_name, json args);
    void publish_variable(const std::string& impl_id, const std::string& var_name, json value);
    void implement_command(const std::string& impl_id, const std::string& cmd_name, std::function<json(json)> handler);
    void subscribe_variable(const Fulfillment& fulfillment, const std::string& var_name,
                            std::function<void(json)> callback);
    void raise_error(const std::string& impl_id, const Everest::error::Error& error);
    void clear_error(const std::string& impl_id, const Everest::error::ErrorType& type, const bool clear_all = false);
    void clear_error(const std::string& impl_id, const Everest::error::ErrorType& type,
                     const Everest::error::ErrorSubType& sub_type);
    void clear_all_errors_of_impl(const std::string& impl_id);
    std::shared_ptr<Everest::error::ErrorStateMonitor> get_error_state_monitor_impl(const std::string& impl_id) const;
    std::shared_ptr<Everest::error::ErrorFactory> get_error_factory(const std::string& impl_id) const;
    void subscribe_error(const Fulfillment& fulfillment, const Everest::error::ErrorType& type,
                         const Everest::error::ErrorCallback& callback,
                         const Everest::error::ErrorCallback& clear_callback);
    void subscribe_all_errors(const Fulfillment& fulfillment, const Everest::error::ErrorCallback& callback,
                              const Everest::error::ErrorCallback& clear_callback);
    std::shared_ptr<Everest::error::ErrorStateMonitor>
    get_error_state_monitor_req(const Fulfillment& fulfillment) const;

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
    const std::chrono::time_point<std::chrono::system_clock> start_time;
    std::unique_ptr<Everest::RuntimeSettings> rs;
    std::shared_ptr<Everest::MQTTAbstraction> mqtt_abstraction;
    std::unique_ptr<Everest::Config> config_;

    std::unique_ptr<Everest::Everest> handle;

    // NOTE (aw): we're keeping the handlers local to the module instance and don't pass them by copy-construction
    // to "external" c/c++ code, so no GIL related problems should appear
    std::deque<std::function<json(json)>> command_handlers{};
    std::deque<std::function<void(json)>> subscription_callbacks{};
    std::deque<std::function<void(json)>> err_susbcription_callbacks{};
    std::deque<std::function<void(json)>> err_cleared_susbcription_callbacks{};

    static std::unique_ptr<Everest::Everest>
    create_everest_instance(const std::string& module_id, const Everest::Config& config,
                            const Everest::RuntimeSettings& rs,
                            std::shared_ptr<Everest::MQTTAbstraction> mqtt_abstraction);

    ModuleInfo module_info{};
    std::map<std::string, Interface> requirements;
    std::map<std::string, Interface> implementations;
    std::map<std::string, std::vector<Fulfillment>> fulfillments;
};

#endif // EVERESTPY_MODULE_HPP
