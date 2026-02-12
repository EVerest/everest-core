// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/json_codec.hpp"
#include "ev_board_support/API.hpp"
#include "ev_board_support/codec.hpp"
#include "ev_board_support/json_codec.hpp"
#include "evse_board_support/API.hpp"
#include "nlohmann/json.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::ev_board_support {

void to_json(json& j, EvCpState const& k) noexcept {
    switch (k) {
    case EvCpState::A:
        j = "A";
        return;
    case EvCpState::B:
        j = "B";
        return;
    case EvCpState::C:
        j = "C";
        return;
    case EvCpState::D:
        j = "D";
        return;
    case EvCpState::E:
        j = "E";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ev_board_support::EvCpState";
}

void from_json(json const& j, EvCpState& k) {
    std::string s = j;
    if (s == "A") {
        k = EvCpState::A;
        return;
    }
    if (s == "B") {
        k = EvCpState::B;
        return;
    }
    if (s == "C") {
        k = EvCpState::C;
        return;
    }
    if (s == "D") {
        k = EvCpState::D;
        return;
    }
    if (s == "E") {
        k = EvCpState::E;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::1_0::types::ev_board_support::EvCpState");
}

void to_json(json& j, BspMeasurement const& k) noexcept {
    j = json{
        {"proximity_pilot", k.proximity_pilot},
        {"cp_pwm_duty_cycle", k.cp_pwm_duty_cycle},
    };
    if (k.rcd_current_mA) {
        j["rcd_current_mA"] = k.rcd_current_mA.value();
    }
}

void from_json(json const& j, BspMeasurement& k) {
    k.proximity_pilot = j.at("proximity_pilot");
    k.cp_pwm_duty_cycle = j.at("cp_pwm_duty_cycle");

    if (j.contains("rcd_current_mA")) {
        k.rcd_current_mA.emplace(j.at("rcd_current_mA"));
    }
}

} // namespace everest::lib::API::V1_0::types::ev_board_support
