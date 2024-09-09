// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "state.hpp"
#include <chrono>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace module::state {
using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::time_point_cast;

PowermeterData::PowermeterData() :
    time_stamp{time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count() / 1000} {
}
ModuleState::ModuleState() :
    time_stamp{time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count() / 1000} {
}

std::string state_to_string(state::ModuleState& module_state) {
    using state::State;

    const auto pwm = (module_state.pwm_running ? '2' : '1');

    switch (module_state.current_state) {
    case State::STATE_DISABLED:
        return "Disabled";
    case State::STATE_A:
        return "A" + std::to_string(pwm);
    case State::STATE_B:
        return "B" + std::to_string(pwm);
    case State::STATE_C:
        return "C" + std::to_string(pwm);
    case State::STATE_D:
        return "D" + std::to_string(pwm);
    case State::STATE_E:
        return "E" + std::to_string(pwm);
    case State::STATE_F:
        return "F" + std::to_string(pwm);
    default:
        return "";
    }
}

void to_json(nlohmann::json& json, const PowermeterData& powermeter_data) {
    json = nlohmann::json{{"time_stamp", powermeter_data.time_stamp},
                          {"totalWattHr", powermeter_data.totalWattHr},
                          {"wattL1", powermeter_data.wattL1},
                          {"vrmsL1", powermeter_data.vrmsL1},
                          {"irmsL1", powermeter_data.irmsL1},
                          {"wattHrL1", powermeter_data.wattHrL1},
                          {"tempL1", powermeter_data.tempL1},
                          {"freqL1", powermeter_data.freqL1},

                          {"wattL2", powermeter_data.wattL2},
                          {"vrmsL2", powermeter_data.vrmsL2},
                          {"irmsL2", powermeter_data.irmsL2},
                          {"wattHrL2", powermeter_data.wattHrL2},
                          {"tempL2", powermeter_data.tempL2},
                          {"freqL2", powermeter_data.freqL2},

                          {"wattL3", powermeter_data.wattL3},
                          {"vrmsL3", powermeter_data.vrmsL3},
                          {"irmsL3", powermeter_data.irmsL3},
                          {"wattHrL3", powermeter_data.wattHrL3},
                          {"tempL3", powermeter_data.tempL3},
                          {"freqL3", powermeter_data.freqL3},

                          {"irmsN", powermeter_data.irmsN}};
}

} // namespace module::state