// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "board_support_common.hpp"

namespace module {

types::ev_board_support::EvCpState mcu_cp_state_to_everest_cp_state(CpState s) {
    switch (s) {
    case CpState_STATE_A:
        return types::ev_board_support::EvCpState::A;
    case CpState_STATE_B:
        return types::ev_board_support::EvCpState::B;
    case CpState_STATE_C:
        return types::ev_board_support::EvCpState::C;
    case CpState_STATE_D:
        return types::ev_board_support::EvCpState::D;
    case CpState_STATE_E:
        return types::ev_board_support::EvCpState::E;
    case CpState_STATE_F:
        // fixme(joro): this needs to be fixed in ev_board_support types definition (in everest-core)
        // also needs DF state proto+everest type definitions!
        return types::ev_board_support::EvCpState::E;
    default:
        return types::ev_board_support::EvCpState::E;
    }
}

CpState everest_cp_state_to_mcu_cp_state(types::ev_board_support::EvCpState s) {
    switch (s) {
    case types::ev_board_support::EvCpState::A:
        return CpState_STATE_A;
    case types::ev_board_support::EvCpState::B:
        return CpState_STATE_B;
    case types::ev_board_support::EvCpState::C:
        return CpState_STATE_C;
    case types::ev_board_support::EvCpState::D:
        return CpState_STATE_D;
    case types::ev_board_support::EvCpState::E:
        return CpState_STATE_E;
    default:
        return CpState_STATE_E;
        // fixme(joro): add F/DF to types/proto definitions and add their handling here aswell
    }
}

types::board_support_common::Ampacity mcu_pp_state_to_everest_pp_state(PpState s) {
    switch (s) {
    case PpState_STATE_NC:
        return types::board_support_common::Ampacity::None;
    case PpState_STATE_13A:
        return types::board_support_common::Ampacity::A_13;
    case PpState_STATE_20A:
        return types::board_support_common::Ampacity::A_20;
    case PpState_STATE_32A:
        return types::board_support_common::Ampacity::A_32;
    case PpState_STATE_70A:
        return types::board_support_common::Ampacity::A_63_3ph_70_1ph;
    default:
        return types::board_support_common::Ampacity::None;
    }
}

} // namespace module
