// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <variant>

#include "common.hpp"

namespace iso15118::message_20 {

struct DC_ChargeParameterDiscoveryRequest {
    Header header;

    struct DC_CPDReqEnergyTransferMode {
        RationalNumber max_charge_power;
        RationalNumber min_charge_power;
        RationalNumber max_charge_current;
        RationalNumber min_charge_current;
        RationalNumber max_voltage;
        RationalNumber min_voltage;
        std::optional<uint8_t> target_soc;
    };

    struct BPT_DC_CPDReqEnergyTransferMode : DC_CPDReqEnergyTransferMode {
        RationalNumber max_discharge_power;
        RationalNumber min_discharge_power;
        RationalNumber max_discharge_current;
        RationalNumber min_discharge_current;
    };

    std::variant<DC_CPDReqEnergyTransferMode, BPT_DC_CPDReqEnergyTransferMode> transfer_mode;
};

struct DC_ChargeParameterDiscoveryResponse {
    Header header;
    ResponseCode response_code;

    DC_ChargeParameterDiscoveryResponse() :
        transfer_mode(std::in_place_type<DC_ChargeParameterDiscoveryResponse::DC_CPDResEnergyTransferMode>){};

    struct DC_CPDResEnergyTransferMode {
        RationalNumber max_charge_power;
        RationalNumber min_charge_power;
        RationalNumber max_charge_current;
        RationalNumber min_charge_current;
        RationalNumber max_voltage;
        RationalNumber min_voltage;
        std::optional<RationalNumber> power_ramp_limit;
    };

    struct BPT_DC_CPDResEnergyTransferMode : DC_CPDResEnergyTransferMode {
        RationalNumber max_discharge_power;
        RationalNumber min_discharge_power;
        RationalNumber max_discharge_current;
        RationalNumber min_discharge_current;
    };

    std::variant<DC_CPDResEnergyTransferMode, BPT_DC_CPDResEnergyTransferMode> transfer_mode;
};

} // namespace iso15118::message_20
