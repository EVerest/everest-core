// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MODULE_CONFIG_HPP
#define UTILS_MODULE_CONFIG_HPP

#include <nlohmann/json.hpp>

#include <utils/mqtt_abstraction.hpp>

namespace Everest {
/// \brief get config from manager via mqtt
nlohmann::json get_module_config(std::shared_ptr<MQTTAbstraction> mqtt, const std::string& module_id);
} // namespace Everest

#endif // UTILS_MODULE_CONFIG_HPP
