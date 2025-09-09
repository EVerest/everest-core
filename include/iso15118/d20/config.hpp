// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <iso15118/d20/limits.hpp>
#include <iso15118/message/common_types.hpp>

namespace iso15118::d20 {

struct ControlMobilityNeedsModes {
    message_20::datatypes::ControlMode control_mode;
    message_20::datatypes::MobilityNeedsMode mobility_mode;
};

struct AcSetupConfig {
    uint32_t voltage;
    std::vector<message_20::datatypes::AcConnector> connectors;
};

struct BptSetupConfig {
    message_20::datatypes::BptChannel bpt_channel;
    message_20::datatypes::GeneratorMode generator_mode;
    std::optional<message_20::datatypes::GridCodeIslandingDetectionMethod> grid_code_detection_method;
};

struct EvseSetupConfig {
    std::string evse_id;
    std::vector<message_20::datatypes::ServiceCategory> supported_energy_services;
    std::vector<message_20::datatypes::Authorization> authorization_services;
    std::vector<uint16_t> supported_vas_services;
    bool enable_certificate_install_service;
    d20::DcTransferLimits dc_limits;
    d20::AcTransferLimits ac_limits;
    std::vector<ControlMobilityNeedsModes> control_mobility_modes;
    std::optional<std::string> custom_protocol{std::nullopt};
    std::optional<AcSetupConfig> ac_setup_config{std::nullopt};
    std::optional<BptSetupConfig> bpt_setup_config{std::nullopt};
};

// This should only have EVSE information
struct SessionConfig {
    explicit SessionConfig(EvseSetupConfig);

    std::string evse_id;

    bool cert_install_service;
    std::vector<message_20::datatypes::Authorization> authorization_services;

    std::vector<message_20::datatypes::ServiceCategory> supported_energy_transfer_services;
    std::vector<std::uint16_t> supported_vas_services;

    std::vector<message_20::datatypes::AcParameterList> ac_parameter_list;
    std::vector<message_20::datatypes::AcBptParameterList> ac_bpt_parameter_list;
    std::vector<message_20::datatypes::DcParameterList> dc_parameter_list;
    std::vector<message_20::datatypes::DcBptParameterList> dc_bpt_parameter_list;

    std::vector<message_20::datatypes::McsParameterList> mcs_parameter_list;
    std::vector<message_20::datatypes::McsBptParameterList> mcs_bpt_parameter_list;

    std::vector<message_20::datatypes::InternetParameterList> internet_parameter_list;
    std::vector<message_20::datatypes::ParkingParameterList> parking_parameter_list;

    DcTransferLimits dc_limits;
    AcTransferLimits ac_limits;

    std::vector<ControlMobilityNeedsModes> supported_control_mobility_modes;

    std::optional<std::string> custom_protocol{std::nullopt};
};

} // namespace iso15118::d20
