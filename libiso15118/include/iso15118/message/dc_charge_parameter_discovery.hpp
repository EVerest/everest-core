// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <variant>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

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

} // namespace datatypes

struct DC_ChargeParameterDiscoveryRequest {
    Header header;
    std::variant<datatypes::DC_CPDReqEnergyTransferMode, datatypes::BPT_DC_CPDReqEnergyTransferMode> transfer_mode;
};

struct DC_ChargeParameterDiscoveryResponse {
    Header header;
    datatypes::ResponseCode response_code;

    std::variant<datatypes::DC_CPDResEnergyTransferMode, datatypes::BPT_DC_CPDResEnergyTransferMode> transfer_mode =
        datatypes::DC_CPDResEnergyTransferMode();
};

} // namespace iso15118::message_20
