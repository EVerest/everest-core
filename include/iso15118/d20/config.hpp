// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include <iso15118/d20/limits.hpp>
#include <iso15118/message/common_types.hpp>

namespace iso15118::d20 {

struct ControlMobilityNeedsModes {
    message_20::datatypes::ControlMode control_mode;
    message_20::datatypes::MobilityNeedsMode mobility_mode;
};

struct EvseSetupConfig {
    std::string evse_id;
    std::vector<message_20::datatypes::ServiceCategory> supported_energy_services;
    std::vector<message_20::datatypes::Authorization> authorization_services;
    bool enable_certificate_install_service;
    d20::DcTransferLimits dc_limits;
    std::vector<ControlMobilityNeedsModes> control_mobility_modes;
};

// This should only have EVSE information
struct SessionConfig {
    explicit SessionConfig(EvseSetupConfig);

    std::string evse_id;

    bool cert_install_service;
    std::vector<message_20::datatypes::Authorization> authorization_services;

    std::vector<message_20::datatypes::ServiceCategory> supported_energy_transfer_services;
    std::vector<message_20::datatypes::ServiceCategory> supported_vas_services;

    std::vector<message_20::datatypes::DcParameterList> dc_parameter_list;
    std::vector<message_20::datatypes::DcBptParameterList> dc_bpt_parameter_list;

    std::vector<message_20::datatypes::InternetParameterList> internet_parameter_list;
    std::vector<message_20::datatypes::ParkingParameterList> parking_parameter_list;

    DcTransferLimits dc_limits;

    std::vector<ControlMobilityNeedsModes> supported_control_mobility_modes;
};

} // namespace iso15118::d20
