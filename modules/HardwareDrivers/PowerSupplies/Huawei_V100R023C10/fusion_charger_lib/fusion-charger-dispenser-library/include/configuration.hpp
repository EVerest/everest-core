// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <stdint.h>

#include <chrono>
#include <string>

#include "callbacks.hpp"
#include "fusion_charger/modbus/registers/raw.hpp"
#include "tls_util.hpp"

typedef fusion_charger::modbus_driver::raw_registers::ConnectorType ConnectorType;

struct DispenserConfig {
    std::string psu_host;
    uint16_t psu_port;
    std::string eth_interface;

    uint16_t manufacturer;
    uint16_t model;
    uint16_t protocol_version;
    uint16_t hardware_version;
    std::string software_version;

    uint16_t charging_connector_count;
    std::string esn;

    std::chrono::milliseconds modbus_timeout_ms = std::chrono::seconds(60);
    bool secure_goose = true;

    // Optional TLS configuration
    // If not set plain TCP will be used
    std::optional<tls_util::MutualTlsClientConfig> tls_config;

    std::chrono::milliseconds module_placeholder_allocation_timeout;
};

struct ConnectorConfig {
    uint16_t global_connector_number;
    ConnectorType connector_type = ConnectorType::CCS2;

    // Maximum current that the connector can deliver in A
    float max_rated_charge_current = 0.0;

    // Maximum output power that the connector can deliver in W
    float max_rated_output_power = 0.0;

    ConnectorCallbacks connector_callbacks;
};
