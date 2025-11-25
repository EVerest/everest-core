// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <everest/run_application/run_application.hpp>
#include <generated/types/iso15118_vas.hpp>

#include "ISO15118_vasImpl.hpp"
#include <sys/types.h>

namespace fs = std::filesystem;

namespace module {
namespace iso15118_vas {

constexpr int32_t InternetAccessServiceIdD2 = 3;
constexpr int HTTP_PARAM_SET_ID = 3;
constexpr int HTTPS_PARAM_SET_ID = 4;
constexpr int HTTP_PORT = 80;
constexpr int HTTPS_PORT = 443;
const std::string PARAM_NAME_PROTOCOL = "Protocol";
const std::string PARAM_NAME_PORT = "Port";
const std::string VALUE_HTTP = "http";
const std::string VALUE_HTTPS = "https";

ISO15118_vasImpl::~ISO15118_vasImpl() {
    this->stop_internet_service();
}

void ISO15118_vasImpl::init() {
    if (!this->mod->r_evse_manager.empty()) {
        this->mod->r_evse_manager.at(0)->subscribe_session_event(
            [this](types::evse_manager::SessionEvent session_event) {
                if (session_event.event == types::evse_manager::SessionEventEnum::SessionFinished and
                    this->internet_service_running) {
                    this->stop_internet_service();
                }
            });
    }
    const auto config_setup_script = fs::path(this->mod->config.vas_setup_script);
    if (config_setup_script.is_relative()) {
        this->internet_setup_script = (this->mod->info.paths.libexec / this->mod->config.vas_setup_script).string();
    } else {
        this->internet_setup_script = this->mod->config.vas_setup_script;
    }
}

void ISO15118_vasImpl::ready() {
    this->publish_offered_vas({{{InternetAccessServiceIdD2}}});
}

std::vector<types::iso15118_vas::ParameterSet> ISO15118_vasImpl::handle_get_service_parameters(int& service_id) {
    std::vector<types::iso15118_vas::ParameterSet> ret{};
    if (service_id == InternetAccessServiceIdD2) {
        ret.reserve(2);

        if (this->mod->config.http_support) {
            // HTTP
            types::iso15118_vas::ParameterSet http_params;
            http_params.set_id = HTTP_PARAM_SET_ID;
            http_params.parameters.reserve(2);

            types::iso15118_vas::Parameter http_param;
            http_param.name = PARAM_NAME_PROTOCOL;
            types::iso15118_vas::ParameterValue http_protocol_name;
            http_protocol_name.finite_string = VALUE_HTTP;
            http_param.value = http_protocol_name;

            types::iso15118_vas::Parameter http_port;
            http_port.name = PARAM_NAME_PORT;
            types::iso15118_vas::ParameterValue http_port_value;
            http_port_value.int_value = HTTP_PORT;
            http_port.value = http_port_value;
            http_params.parameters.emplace_back(http_param);
            http_params.parameters.emplace_back(http_port);

            ret.emplace_back(http_params);
        }

        if (this->mod->config.https_support) {
            // HTTPS
            types::iso15118_vas::ParameterSet https_params;
            https_params.set_id = HTTPS_PARAM_SET_ID;
            https_params.parameters.reserve(2);

            types::iso15118_vas::Parameter https_param;
            https_param.name = PARAM_NAME_PROTOCOL;
            types::iso15118_vas::ParameterValue https_protocol_name;
            https_protocol_name.finite_string = VALUE_HTTPS;
            https_param.value = https_protocol_name;

            types::iso15118_vas::Parameter https_port;
            https_port.name = PARAM_NAME_PORT;
            types::iso15118_vas::ParameterValue https_port_value;
            https_port_value.int_value = HTTPS_PORT;
            https_port.value = https_port_value;
            https_params.parameters.emplace_back(https_param);
            https_params.parameters.emplace_back(https_port);

            ret.emplace_back(https_params);
        }
    }
    return ret;
}

std::vector<int> ISO15118_vasImpl::get_selected_internet_ports(
    const std::vector<types::iso15118_vas::SelectedService>& selected_services) {
    std::vector<int> selected_ports;

    for (const auto& service : selected_services) {
        if (service.service_id == InternetAccessServiceIdD2) {
            if (this->mod->config.http_support and service.parameter_set_id == HTTP_PARAM_SET_ID) {
                if (std::find(selected_ports.begin(), selected_ports.end(), HTTP_PORT) == selected_ports.end()) {
                    selected_ports.push_back(HTTP_PORT);
                }
            } else if (this->mod->config.https_support and service.parameter_set_id == HTTPS_PARAM_SET_ID) {
                if (std::find(selected_ports.begin(), selected_ports.end(), HTTPS_PORT) == selected_ports.end()) {
                    selected_ports.push_back(HTTPS_PORT);
                }
            }
        }
    }
    return selected_ports;
}

void ISO15118_vasImpl::handle_selected_services(std::vector<types::iso15118_vas::SelectedService>& selected_services) {
    const auto ports_to_open = this->get_selected_internet_ports(selected_services);

    if (!ports_to_open.empty()) {
        std::string ports_str;
        for (size_t i = 0; i < ports_to_open.size(); ++i) {
            ports_str += std::to_string(ports_to_open[i]);
            if (i < ports_to_open.size() - 1) {
                ports_str += ",";
            }
        }
        start_internet_service(ports_str);
    }
}

void ISO15118_vasImpl::start_script(const std::string& script_name, const std::vector<std::string>& args) {
    auto output = everest::run_application::run_application(script_name, args);
    if (output.exit_code != 0) {
        EVLOG_warning << "Script: " << script_name << " exited with code: " << output.exit_code;
        EVLOG_warning << "Script output:";
        EVLOG_warning << output.output;
    }
}

void ISO15118_vasImpl::start_internet_service(const std::string& ports) {
    {
        std::lock_guard lock(internet_mutex);
        if (this->internet_service_running) {
            EVLOG_warning << "Internet service is already running.";
            return;
        }
        this->internet_service_running = true;
    }
    this->active_ports = ports;
    EVLOG_info << "Starting internet service for ports: " << this->active_ports;

    std::thread(&ISO15118_vasImpl::start_script, this, this->internet_setup_script,
                std::vector<std::string>{"up", this->mod->config.ev_interface, this->mod->config.modem_interface,
                                         this->active_ports})
        .detach();
}

void ISO15118_vasImpl::stop_internet_service() {
    {
        std::lock_guard lock(internet_mutex);
        if (!this->internet_service_running) {
            EVLOG_warning << "Internet service is not running.";
            return;
        }
        this->internet_service_running = false;
    }
    EVLOG_info << "Stopping internet service for ports: " << this->active_ports;

    std::thread(&ISO15118_vasImpl::start_script, this, this->internet_setup_script,
                std::vector<std::string>{"down", this->mod->config.ev_interface, this->mod->config.modem_interface,
                                         this->active_ports})
        .detach();
    this->active_ports.clear();
}

} // namespace iso15118_vas
} // namespace module
