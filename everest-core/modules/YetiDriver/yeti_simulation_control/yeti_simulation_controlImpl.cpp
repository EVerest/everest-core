#include "yeti_simulation_controlImpl.hpp"

namespace module {
namespace yeti_simulation_control {

Everest::json simulation_feedback_to_json(const SimulationFeedback& s) {
    Everest::json j;

    j["pwm_duty_cycle"] = s.pwmDutyCycle;
    j["relais_on"] = s.relais_on;
    j["evse_pwm_running"] = s.evse_pwm_running;
    j["evse_pwm_voltage_hi"] = s.evse_pwm_voltage_hi;
    j["evse_pwm_voltage_lo"] = s.evse_pwm_voltage_lo;

    return j;
}

SimulationData json_to_simulation_data(Object& v) {
    SimulationData s;

    s.cp_voltage = v["cp_voltage"];
    s.diode_fail = v["diode_fail"];
    s.error_e = v["error_e"];
    s.pp_resistor = v["pp_resistor"];
    s.rcd_current = v["rcd_current"];

    s.currentL1 = v["currents"]["L1"];
    s.currentL2 = v["currents"]["L2"];
    s.currentL3 = v["currents"]["L3"];
    s.currentN = v["currents"]["N"];

    s.voltageL1 = v["voltages"]["L1"];
    s.voltageL2 = v["voltages"]["L2"];
    s.voltageL3 = v["voltages"]["L3"];

    s.freqL1 = v["frequencies"]["L1"];
    s.freqL2 = v["frequencies"]["L2"];
    s.freqL3 = v["frequencies"]["L3"];

    return s;
}

void yeti_simulation_controlImpl::init() {
    mod->serial.signalSimulationFeedback.connect(
        [this](const SimulationFeedback& s) { publish_simulation_feedback(simulation_feedback_to_json(s)); });
}

void yeti_simulation_controlImpl::ready() {
}

void yeti_simulation_controlImpl::handle_enable(bool& value) {
    EVLOG(info) << "void YetiDriverModule::Yeti_simulation_control::enable: " << value;
    mod->serial.enableSimulation(value);
};

void yeti_simulation_controlImpl::handle_setSimulationData(Object& value) {
    mod->serial.setSimulationData(json_to_simulation_data(value));
};

} // namespace yeti_simulation_control
} // namespace module
