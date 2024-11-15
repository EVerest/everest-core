// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <variant>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

struct AC_CPDReqEnergyTransferMode {
    RationalNumber max_charge_power;
    std::optional<RationalNumber> max_charge_power_L2;
    std::optional<RationalNumber> max_charge_power_L3;
    RationalNumber min_charge_power;
    std::optional<RationalNumber> min_charge_power_L2;
    std::optional<RationalNumber> min_charge_power_L3;
};

struct BPT_AC_CPDReqEnergyTransferMode : AC_CPDReqEnergyTransferMode {
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    RationalNumber min_discharge_power;
    std::optional<RationalNumber> min_discharge_power_L2;
    std::optional<RationalNumber> min_discharge_power_L3;
};

struct AC_CPDResEnergyTransferMode {
    RationalNumber max_charge_power;
    std::optional<RationalNumber> max_charge_power_L2;
    std::optional<RationalNumber> max_charge_power_L3;
    RationalNumber min_charge_power;
    std::optional<RationalNumber> min_charge_power_L2;
    std::optional<RationalNumber> min_charge_power_L3;
    RationalNumber nominal_frequency;
    std::optional<RationalNumber> max_power_asymmetry;
    std::optional<RationalNumber> power_ramp_limitation;
    std::optional<RationalNumber> present_active_power;
    std::optional<RationalNumber> present_active_power_L2;
    std::optional<RationalNumber> present_active_power_L3;
};

struct BPT_AC_CPDResEnergyTransferMode : AC_CPDResEnergyTransferMode {
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    RationalNumber min_discharge_power;
    std::optional<RationalNumber> min_discharge_power_L2;
    std::optional<RationalNumber> min_discharge_power_L3;
};

} // namespace datatypes

struct AC_ChargeParameterDiscoveryRequest {
    Header header;
    std::variant<datatypes::AC_CPDReqEnergyTransferMode, datatypes::BPT_AC_CPDReqEnergyTransferMode> transfer_mode;
};

struct AC_ChargeParameterDiscoveryResponse {
    Header header;
    datatypes::ResponseCode response_code;

    std::variant<datatypes::AC_CPDResEnergyTransferMode, datatypes::BPT_AC_CPDResEnergyTransferMode> transfer_mode =
        datatypes::AC_CPDResEnergyTransferMode();
};

} // namespace iso15118::message_20
