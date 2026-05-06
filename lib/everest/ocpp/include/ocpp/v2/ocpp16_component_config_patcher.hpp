// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <map>

#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v2/init_device_model_db.hpp>
#include <ocpp/v2/ocpp16_custom_config_mappings.hpp>

namespace ocpp::v2 {

/// \brief Patch OCPP 2.x component configuration with OCPP 1.6 charge point configuration values.
///
/// Applies compatibility overrides from the provided OCPP 1.6 configuration to the in-memory
/// OCPP 2.x component configuration map.
///
/// Note: Patching is applied only when the mapped component and variable already exist in
/// the provided component configuration.
///
/// Note: custom_config_mappings is used as fallback for OCPP 1.6 keys that have no standardized
/// OCPP 1.6 -> OCPP 2.x mapping.
///
/// Throws nothing. Missing mappings/variables/components are skipped with log output.
void patch_component_config_with_ocpp16(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                                        ocpp::v16::ChargePointConfiguration& ocpp16_config,
                                        const Ocpp16CustomConfigMappings& custom_config_mappings = {});

} // namespace ocpp::v2
