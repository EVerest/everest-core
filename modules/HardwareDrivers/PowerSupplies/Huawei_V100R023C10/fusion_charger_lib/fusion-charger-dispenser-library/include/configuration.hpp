// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <cstdint>

#include <chrono>
#include <string>

#include "callbacks.hpp"
#include "fusion_charger/modbus/registers/raw.hpp"
#include "telemetry.hpp"
#include "tls_util.hpp"

typedef fusion_charger::modbus_driver::raw_registers::ConnectorType ConnectorType;

struct DispenserConfig {
    std::string psu_host;
    std::uint16_t psu_port;
    std::string eth_interface;

    std::uint16_t manufacturer;
    std::uint16_t model;
    std::uint16_t protocol_version;
    std::uint16_t hardware_version;
    std::string software_version;

    std::uint16_t charging_connector_count;
    std::string esn;

    std::chrono::milliseconds modbus_timeout_ms = std::chrono::seconds(60);

    bool send_secure_goose = true;        // if set to true send secured goose frames,
                                          // if false only send unsecured frames
    bool allow_unsecured_goose = false;   // if set to true allow unsecured goose frames from the PSU, if
                                          // false only allow secured frames (hmac not verified)
    bool verify_secure_goose_hmac = true; // if set to true verify the HMAC of secured goose frames, if false
                                          // do not verify the HMAC

    // Optional TLS configuration
    // If not set plain TCP will be used
    std::optional<tls_util::MutualTlsClientConfig> tls_config;

    std::chrono::milliseconds module_placeholder_allocation_timeout;

    std::shared_ptr<fusion_charger::telemetry::TelemetryPublisherBase> telemetry_publisher =
        std::make_shared<fusion_charger::telemetry::TelemetryPublisherNull>();
};

struct ConnectorConfig {
    std::uint16_t global_connector_number;
    ConnectorType connector_type = ConnectorType::CCS2;

    // Maximum current that the connector can deliver in A
    float max_rated_charge_current = 0.0;

    // Maximum output power that the connector can deliver in W
    float max_rated_output_power = 0.0;

    ConnectorCallbacks connector_callbacks;
};
