// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

/// \file Class to implement the LifecycleAPI, which provides an API to manage the lifecycle of EVerest modules

#pragma once

#include <everest_api_types/lifecycle/API.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <utils/config_service_interface.hpp>
#include <utils/mqtt_abstraction.hpp>

namespace ev_API = everest::lib::API;

namespace Everest::api::lifecycle {

class LifecycleAPI {
public:
    LifecycleAPI(MQTTAbstraction& mqtt_abstraction, ::Everest::config::ConfigServiceInterface& config_service,
                 bool config_api_enabled, bool readonly = true);

private:
    MQTTAbstraction& m_mqtt_abstraction;
    ::Everest::config::ConfigServiceInterface& m_config_service;

    ev_API::Topics m_topics;
    bool m_config_api_enabled;
    const bool m_readonly;

    void generate_api_cmd_stop_modules();
    void generate_api_cmd_start_modules();

    void generate_api_var_status();

    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish);
};

} // namespace Everest::api::lifecycle
