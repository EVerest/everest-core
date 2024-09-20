// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "ocpp_1_6_charge_pointImpl.hpp"

namespace module {
namespace main {

void ocpp_1_6_charge_pointImpl::init() {
}

void ocpp_1_6_charge_pointImpl::ready() {
}

bool ocpp_1_6_charge_pointImpl::handle_stop() {
    std::lock_guard<std::mutex> lock(this->m);
    mod->charging_schedules_timer->stop();
    bool success = mod->charge_point->stop();
    if (success) {
        this->mod->ocpp_stopped = true;
    }
    return success;
}
bool ocpp_1_6_charge_pointImpl::handle_restart() {
    std::lock_guard<std::mutex> lock(this->m);
    mod->charging_schedules_timer->interval(std::chrono::seconds(this->mod->config.PublishChargingScheduleIntervalS));
    bool success = mod->charge_point->restart();
    if (success) {
        this->mod->ocpp_stopped = false;
    }
    return success;
}

types::ocpp::KeyValue to_everest(const ocpp::v16::KeyValue& key_value) {
    types::ocpp::KeyValue _key_value;
    _key_value.key = key_value.key.get();
    _key_value.read_only = key_value.readonly;
    if (key_value.value.has_value()) {
        _key_value.value = key_value.value.value().get();
    }
    return _key_value;
}

types::ocpp::ConfigurationStatus to_everest(const ocpp::v16::ConfigurationStatus status) {
    switch (status) {
    case ocpp::v16::ConfigurationStatus::Accepted:
        return types::ocpp::ConfigurationStatus::Accepted;
    case ocpp::v16::ConfigurationStatus::Rejected:
        return types::ocpp::ConfigurationStatus::Rejected;
    case ocpp::v16::ConfigurationStatus::RebootRequired:
        return types::ocpp::ConfigurationStatus::RebootRequired;
    case ocpp::v16::ConfigurationStatus::NotSupported:
        return types::ocpp::ConfigurationStatus::NotSupported;
    default:
        EVLOG_warning << "Could not convert to ConfigurationStatus";
        return types::ocpp::ConfigurationStatus::Rejected;
    }
}

types::ocpp::GetConfigurationResponse to_everest(const ocpp::v16::GetConfigurationResponse& response) {
    types::ocpp::GetConfigurationResponse _response;
    std::vector<types::ocpp::KeyValue> configuration_keys;
    std::vector<std::string> unknown_keys;

    if (response.configurationKey.has_value()) {
        for (const auto& item : response.configurationKey.value()) {
            configuration_keys.push_back(to_everest(item));
        }
    }

    if (response.unknownKey.has_value()) {
        for (const auto& item : response.unknownKey.value()) {
            unknown_keys.push_back(item.get());
        }
    }

    _response.configuration_keys = configuration_keys;
    _response.unknown_keys = unknown_keys;
    return _response;
}

types::ocpp::GetConfigurationResponse ocpp_1_6_charge_pointImpl::handle_get_configuration_key(Array& keys) {
    ocpp::v16::GetConfigurationRequest request;
    std::vector<ocpp::CiString<50>> _keys;
    for (const auto& key : keys) {
        _keys.push_back(key);
    }
    request.key = _keys;

    const auto response = this->mod->charge_point->get_configuration_key(request);
    return to_everest(response);
}

types::ocpp::ConfigurationStatus ocpp_1_6_charge_pointImpl::handle_set_custom_configuration_key(std::string& key,
                                                                                                std::string& value) {
    const auto response = this->mod->charge_point->set_custom_configuration_key(key, value);
    return to_everest(response);
}

void ocpp_1_6_charge_pointImpl::handle_monitor_configuration_keys(Array& keys) {
    for (const auto& key : keys) {
        this->mod->charge_point->register_configuration_key_changed_callback(
            key,
            [this](const ocpp::v16::KeyValue key_value) { this->publish_configuration_key(to_everest(key_value)); });
    }
}

void ocpp_1_6_charge_pointImpl::handle_security_event(std::string& type, std::string& info) {
    this->mod->charge_point->on_security_event(type, info);
}

} // namespace main
} // namespace module
