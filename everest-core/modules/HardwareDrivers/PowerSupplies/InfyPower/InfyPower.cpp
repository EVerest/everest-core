// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "InfyPower.hpp"

namespace module {

void InfyPower::init() {
    acdc = std::make_unique<InfyCanDevice>();
    acdc->set_can_device(config.can_device);
    acdc->set_config_values(config.module_addresses, config.group_address, config.device_connection_timeout_s,
                            config.controller_address);
    invoke_init(*p_main);
}

void InfyPower::ready() {
    invoke_ready(*p_main);
}

} // namespace module
