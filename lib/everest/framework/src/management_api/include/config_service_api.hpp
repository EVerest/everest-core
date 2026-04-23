// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

/// \file Class to implement the ConfigServiceAPI, which provides an API to manage the configuration of EVerest modules.

#pragma once

#include <everest_api_types/config_service/API.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <utils/config_service_interface.hpp>
#include <utils/mqtt_abstraction.hpp>

namespace ev_API = everest::lib::API;

namespace Everest::api::config_service {

class ConfigServiceAPI {
public:
    ConfigServiceAPI(MQTTAbstraction& mqtt_abstraction, Everest::config::ConfigServiceInterface& config_service);

private:
    MQTTAbstraction& mqtt_abstraction;
    Everest::config::ConfigServiceInterface& config_service;
    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void generate_api_cmd_list_all_slots();
    void generate_api_cmd_get_active_slot();
    void generate_api_cmd_mark_active_slot();

    void generate_api_cmd_delete_slot();
    void generate_api_cmd_duplicate_slot();
    void generate_api_cmd_load_from_yaml();
    void generate_api_cmd_set_config_parameters();
    void generate_api_cmd_get_configuration();

    void generate_api_var_active_slot();
    void generate_api_var_config_updates();

    void subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish);

    ev_API::Topics topics;
};

} // namespace Everest::api::config_service
