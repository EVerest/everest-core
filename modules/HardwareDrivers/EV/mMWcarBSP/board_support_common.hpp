// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef EV_BOARD_SUPPORT_COMMON_HPP
#define EV_BOARD_SUPPORT_COMMON_HPP

#include "mMWcar.pb.h"
#include <generated/interfaces/ev_board_support/Implementation.hpp>

namespace module {
types::ev_board_support::EvCpState mcu_cp_state_to_everest_cp_state(CpState s);
CpState everest_cp_state_to_mcu_cp_state(types::ev_board_support::EvCpState s);
types::board_support_common::Ampacity mcu_pp_state_to_everest_pp_state(PpState s);
} // namespace module

#endif // EV_BOARD_SUPPORT_COMMON_HPP
