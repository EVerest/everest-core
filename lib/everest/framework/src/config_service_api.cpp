// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <config_service_api.hpp>
#include <everest/logging.hpp>

namespace Everest::config {
ConfigServiceAPI::ConfigServiceAPI(MQTTAbstraction& mqtt_abstraction, ConfigService& config_service) :
    mqtt_abstraction(mqtt_abstraction), config_service(config_service) {

    EVLOG_info << "Starting ConfigServiceAPI";
    mqtt_abstraction.publish("CONFIGTOPIC", std::string("RUNNING"), QOS::QOS2, true);

}
} // namespace Everest::config