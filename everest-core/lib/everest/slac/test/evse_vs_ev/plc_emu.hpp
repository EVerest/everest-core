// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef TESTS_EVSE_VS_EV_PLC_EMU_HPP
#define TESTS_EVSE_VS_EV_PLC_EMU_HPP

#include "socket_pair_bridge.hpp"

void handle_ev_input(int ev_bridge_fd, int evse_bridge_fd);

void handle_evse_input(int evse_bridge_fd, int ev_bridge_fd);

#endif // TESTS_EVSE_VS_EV_PLC_EMU_HPP
