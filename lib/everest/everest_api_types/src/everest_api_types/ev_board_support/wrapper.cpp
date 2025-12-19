// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ev_board_support/wrapper.hpp"
#include "ev_board_support/API.hpp"
#include "evse_board_support/wrapper.hpp"

namespace everest::lib::API::V1_0::types::ev_board_support {

EvCpState_Internal to_internal_api(EvCpState_External const& val) {
    using SrcT = EvCpState_External;
    using TarT = EvCpState_Internal;

    switch (val) {
    case SrcT::A:
        return TarT::A;
    case SrcT::B:
        return TarT::B;
    case SrcT::C:
        return TarT::C;
    case SrcT::D:
        return TarT::D;
    case SrcT::E:
        return TarT::E;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ev_board_support::EvCpState_External");
}

EvCpState_External to_external_api(EvCpState_Internal const& val) {
    using SrcT = EvCpState_Internal;
    using TarT = EvCpState_External;

    switch (val) {
    case SrcT::A:
        return TarT::A;
    case SrcT::B:
        return TarT::B;
    case SrcT::C:
        return TarT::C;
    case SrcT::D:
        return TarT::D;
    case SrcT::E:
        return TarT::E;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::ev_board_support::EvCpState_Internal");
}

BspMeasurement_Internal to_internal_api(BspMeasurement_External const& val) {
    BspMeasurement_Internal result;

    result.proximity_pilot = evse_board_support::to_internal_api(val.proximity_pilot);
    result.cp_pwm_duty_cycle = val.cp_pwm_duty_cycle;
    result.rcd_current_mA = val.rcd_current_mA;

    return result;
}

BspMeasurement_External to_external_api(BspMeasurement_Internal const& val) {
    BspMeasurement_External result;

    result.proximity_pilot = evse_board_support::to_external_api(val.proximity_pilot);
    result.cp_pwm_duty_cycle = val.cp_pwm_duty_cycle;
    result.rcd_current_mA = val.rcd_current_mA;

    return result;
}

} // namespace everest::lib::API::V1_0::types::ev_board_support
