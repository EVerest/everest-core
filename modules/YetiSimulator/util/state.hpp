// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

// NOLINTBEGIN: ignore things like public access or magic values
#include "nlohmann/json.hpp"
#include <string>

namespace module::state {

struct PowermeterData {
    PowermeterData();
    int64_t time_stamp;

    double totalWattHr = 0.0;

    double wattL1 = 0.0;
    double vrmsL1 = 230.0;
    double irmsL1 = 0.0;
    double wattHrL1 = 0.0;
    double tempL1 = 25.0;
    double freqL1 = 50.0;

    double wattL2 = 0.0;
    double vrmsL2 = 230.0;
    double irmsL2 = 0.0;
    double wattHrL2 = 0.0;
    double tempL2 = 25.0;
    double freqL2 = 50.0;

    double wattL3 = 0.0;
    double vrmsL3 = 230.0;
    double irmsL3 = 0.0;
    double wattHrL3 = 0.0;
    double tempL3 = 25.0;
    double freqL3 = 50.0;

    double irmsN = 0.0;
};

struct SimulationData {
    double cp_voltage = 12.0;
    bool diode_fail = false;
    bool error_e = false;
    double pp_resistor = 220.1;
    double rcd_current = 0.1;

    struct Currents {
        double L1 = 0.0;
        double L2 = 0.0;
        double L3 = 0.0;
        double N = 0.0;
    };

    Currents currents;

    struct Voltages {
        double L1 = 230.0;
        double L2 = 230.0;
        double L3 = 230.0;
    };

    Voltages voltages;

    struct Frequencies {
        double L1 = 50.0;
        double L2 = 50.0;
        double L3 = 50.0;
    };

    Frequencies frequencies;
};

struct SimdataSetting {
    double cp_voltage = 12.0;
    double pp_resistor = 220.1;
    double impedance = 500.0;
    double rcd_current = 0.1;
    bool diode_fail = false;
    bool error_e = false;

    struct Voltages {
        double L1 = 230.0;
        double L2 = 230.0;
        double L3 = 230.0;
    };

    Voltages voltages;

    struct Currents {
        double L1 = 0.0;
        double L2 = 0.0;
        double L3 = 0.0;
        double N = 0.0;
    };

    Currents currents;

    struct Frequencies {
        double L1 = 50.0;
        double L2 = 50.0;
        double L3 = 50.0;
    };

    Frequencies frequencies;
};

struct WattHr {
    double L1 = 0.0;
    double L2 = 0.0;
    double L3 = 0.0;
};

struct TelemetryData {

    struct PowerPathControllerVersion {
        std::string timestamp;
        std::string type = "power_path_controller_version";
        int hardware_version = 3;
        std::string software_version = "1.01";
        std::string date_manufactured = "20220304";
        int64_t operating_time_h = 2330;
        int64_t operating_time_h_warning = 5000;
        int64_t operating_time_h_error = 6000;
        bool error = false;
    };

    PowerPathControllerVersion power_path_controller_version;

    struct PowerPathController {
        std::string timestamp;
        std::string type = "power_path_controller";
        double cp_voltage_high = 0.0;
        double cp_voltage_low = 0.0;
        double cp_pwm_duty_cycle = 0.0;
        std::string cp_state = "A1";
        double pp_ohm = 220.1;
        double supply_voltage_12V = 12.1;
        double supply_voltage_minus_12V = -11.9;
        double temperature_controller = 33;
        double temperature_car_connector = 65;
        int64_t watchdog_reset_count = 1;
        bool error = false;
    };

    PowerPathController power_path_controller;

    struct PowerSwitch {
        std::string timestamp;
        std::string type = "power_switch";
        int64_t switching_count = 0;
        int64_t switching_count_warning = 30000;
        int64_t switching_count_error = 50000;
        bool is_on = false;
        int64_t time_to_switch_on_ms = 110;
        int64_t time_to_switch_off_ms = 100;
        double temperature_C = 20;
        bool error = false;
        bool error_over_current = false;
    };

    PowerSwitch power_switch;

    struct Rcd {
        std::string timestamp;
        std::string type = "rcd";
        bool enabled = true;
        double current_mA = 2.5;
        bool triggered = false;
        bool error = false;
    };

    Rcd rcd;
};

enum class State {
    STATE_DISABLED = 0,
    STATE_A = 1,
    STATE_B = 2,
    STATE_C = 3,
    STATE_D = 4,
    STATE_E = 5,
    STATE_F = 6,
    Event_PowerOn = 8,
    Event_PowerOff = 9,
};

struct ModuleState {
    ModuleState();

    PowermeterData powermeter_data;
    SimulationData simulation_data;
    SimdataSetting simdata_setting;
    TelemetryData telemetry_data;

    int64_t pubCnt = 0;

    bool power_on_allowed = false;

    bool relais_on = false;
    State current_state = State::STATE_DISABLED;
    State last_state = State::STATE_DISABLED;
    int64_t time_stamp;
    bool use_three_phases = true;
    bool simplified_mode = false;

    bool has_ventilation = false;

    bool rcd_error = false;
    bool rcd_error_reported = false;

    bool simulation_enabled = false;
    double pwm_duty_cycle = 0;
    bool pwm_running = false;
    bool pwm_error_f = false;
    bool last_pwm_running = false;
    bool use_three_phases_confirmed = true;
    double pwm_voltage_hi = 12.1;
    double pwm_voltage_lo = 12.1;

    std::string country_code = "DE";
    int64_t last_pwm_update = 0;

    WattHr watt_hr;

    int64_t powermeter_sim_last_time_stamp = 0L;

    double ev_max_current = 0.0;
    int ev_three_phases = 3;
};

std::string state_to_string(state::ModuleState& module_state);

void to_json(nlohmann::json& json, const PowermeterData& powermeter_data);

} // namespace module::state

// NOLINTEND
