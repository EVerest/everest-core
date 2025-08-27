// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <everest/staging/run_application/run_application.hpp>
#include <generated/types/iso15118_vas.hpp>

#include "ISO15118_vasImpl.hpp"

namespace module {
namespace iso15118_vas {

constexpr int InternetAccessServiceIdD2 = 3;
// constexpr int InternetAccessServiceIdD20 = 65;
const std::string INTERNET_SCRIPT = "internet_service.sh";

void ISO15118_vasImpl::init() {
    this->scripts_path = mod->info.paths.libexec;
    this->internet_service_running = false;
    if (!this->mod->r_evse_manager.empty()) {
        this->mod->r_evse_manager.at(0)->subscribe_session_event(
            [this](types::evse_manager::SessionEvent session_event) {
                if (session_event.event == types::evse_manager::SessionEventEnum::SessionFinished and
                    this->internet_service_running) {
                    this->stop_internet_service();
                }
            });
    }
}

void ISO15118_vasImpl::ready() {
    types::iso15118_vas::OfferedServices services;
    services.service_ids.reserve(1);
    services.service_ids.emplace_back(InternetAccessServiceIdD2);
    this->publish_offered_vas(services);
}

std::vector<types::iso15118_vas::ParameterSet> ISO15118_vasImpl::handle_get_service_parameters(int& service_id) {
    std::vector<types::iso15118_vas::ParameterSet> ret{};
    if (service_id == InternetAccessServiceIdD2) {
        // Fill ParameterSets based on table 107 for -2 or table 212 for -20 (same values)
        ret.reserve(2);

        // HTTP
        types::iso15118_vas::ParameterSet http_params;
        http_params.set_id = 3;
        http_params.parameters = {};
        http_params.parameters.reserve(2);

        types::iso15118_vas::Parameter http_param;
        http_param.name = "Protocol";
        types::iso15118_vas::ParameterValue http_protocol_name;
        http_protocol_name.finite_string = "http";
        http_param.value = http_protocol_name;

        types::iso15118_vas::Parameter http_port;
        http_port.name = "Port";
        types::iso15118_vas::ParameterValue http_port_value;
        http_port_value.int_value = 80;
        http_port.value = http_port_value;
        http_params.parameters.emplace_back(http_param);
        http_params.parameters.emplace_back(http_port);

        ret.emplace_back(http_params);

        // HTTPS
        types::iso15118_vas::ParameterSet https_params;
        https_params.set_id = 4;
        https_params.parameters = {};
        https_params.parameters.reserve(2);

        types::iso15118_vas::Parameter https_param;
        https_param.name = "Protocol";
        types::iso15118_vas::ParameterValue https_protocol_name;
        https_protocol_name.finite_string = "https";
        https_param.value = https_protocol_name;

        types::iso15118_vas::Parameter https_port;
        https_port.name = "Port";
        types::iso15118_vas::ParameterValue https_port_value;
        https_port_value.int_value = 443;
        https_port.value = https_port_value;
        https_params.parameters.emplace_back(https_param);
        https_params.parameters.emplace_back(https_port);

        ret.emplace_back(https_params);
    }
    return ret;
}

void ISO15118_vasImpl::handle_selected_services(std::vector<types::iso15118_vas::SelectedService>& selected_services) {
    const auto found = std::find_if(selected_services.cbegin(), selected_services.cend(),
                                    [](const types::iso15118_vas::SelectedService& service) {
                                        return service.service_id == InternetAccessServiceIdD2;
                                    });
    if (found != selected_services.cend()) {
        start_internet_service();
    }
}

void ISO15118_vasImpl::start_internet_service() {

    this->internet_service_running = true;
    EVLOG_info << "Starting internet service";

    this->start_internet_thread = std::thread([this]() {
        const auto internet_service_script = this->scripts_path / INTERNET_SCRIPT;

        const std::vector<std::string> args = {"enable", this->mod->config.ev_interface,
                                               this->mod->config.modem_interface};
        everest::staging::run_application::run_application(internet_service_script, args, [](const std::string&) {
            return everest::staging::run_application::CmdControl::Continue;
        });
    });
    this->start_internet_thread.detach();
}

void ISO15118_vasImpl::stop_internet_service() {
    this->internet_service_running = false;
    EVLOG_info << "Stopping internet service";
    this->stop_internet_thread = std::thread([this]() {
        const auto internet_service_script = this->scripts_path / INTERNET_SCRIPT;
        const std::vector<std::string> args = {"disable", this->mod->config.ev_interface,
                                               this->mod->config.modem_interface};

        everest::staging::run_application::run_application(internet_service_script, args, [](const std::string&) {
            return everest::staging::run_application::CmdControl::Continue;
        });
    });
    this->stop_internet_thread.detach();
}

} // namespace iso15118_vas
} // namespace module
