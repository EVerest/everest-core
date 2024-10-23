// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSE_BOARD_SUPPORT_COMMON_HPP
#define EVSE_BOARD_SUPPORT_COMMON_HPP

#include "phyverso.pb.h"
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#include <generated/types/led_state.hpp>

namespace module {
types::board_support_common::BspEvent to_bsp_event(CpState s);
types::board_support_common::BspEvent to_bsp_event(CoilState s);
types::led_state::LedState to_led_state(types::led_state::LedState);
} // namespace module

#endif // EVSE_BOARD_SUPPORT_COMMON_HPP
