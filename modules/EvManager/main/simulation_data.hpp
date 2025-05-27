// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "command_registry.hpp"
#include "simulation_command.hpp"
#include <generated/types/board_support_common.hpp>
#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

enum class SimState {
    UNPLUGGED,
    PLUGGED_IN,
    CHARGING_REGULATED,
    CHARGING_FIXED,
    ERROR_E,
    DIODE_FAIL,
    ISO_POWER_READY,
    ISO_CHARGING_REGULATED,
    BCB_TOGGLE,
    UNDEFINED,
};

enum class EnergyMode {
    AC,
    DC,
};

struct SimulationData {

    SimulationData() = default;

    SimState state{SimState::UNPLUGGED};
    SimState last_state{SimState::UNDEFINED};
    std::string slac_state{"UNMATCHED"};
    std::optional<size_t> sleep_ticks_left{};

    bool v2g_finished{false};
    bool iso_stopped{false};
    bool iso_d20_paused{false};
    size_t evse_maxcurrent{0};
    size_t max_current{0};
    std::string payment{"ExternalPayment"};

    EnergyMode energy_mode{EnergyMode::AC};
    std::optional<std::string> modify_charging_session_cmds{std::nullopt};

    bool iso_pwr_ready{false};

    size_t bcb_toggles{0};
    bool bcb_toggle_C{true};

    types::board_support_common::Ampacity pp;
    float rcd_current_ma{0.0f};
    float pwm_duty_cycle{0.0f};
    float last_pwm_duty_cycle{0.0f};

    bool dc_power_on{false};

    types::board_support_common::Event actual_bsp_event{types::board_support_common::Event::Disconnected};
};
