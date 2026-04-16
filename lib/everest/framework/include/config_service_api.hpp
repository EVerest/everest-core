// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

/// \file Class to implement the ConfigServiceAPI, which provides an API to manage the configuration of EVerest and its
/// modules.

#ifndef CONFIG_SERVICE_API_HPP_
#define CONFIG_SERVICE_API_HPP_

#include <utils/mqtt_abstraction.hpp>
#include <utils/config_service.hpp>
#include <everest_api_types/utilities/Topics.hpp>

namespace ev_API = everest::lib::API;

namespace Everest::config {

class ConfigServiceAPI {
public:
    ConfigServiceAPI(MQTTAbstraction& mqtt_abstraction, ConfigService& config_service);

private:
    MQTTAbstraction& mqtt_abstraction;
    ConfigService& config_service;

    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish);

    ev_API::Topics topics;
};

} // namespace Everest::config

#endif // CONFIG_SERVICE_API_HPP_
