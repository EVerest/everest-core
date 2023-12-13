// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { timeEnd } = require('console');
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');
const { inherits } = require('util');

const STATE_DISABLED = 0;
const STATE_A = 1;
const STATE_B = 2;
const STATE_C = 3;
const STATE_D = 4;
const STATE_E = 5;
const STATE_F = 6;
const STATE_DF = 7;

const Event_PowerOn = 8;
const Event_PowerOff = 9;
const Event_Error_Relais = 10;
const Event_Error_RCD = 11;
const Event_Error_VentilationNotAvailable = 12;
const Event_PermanentFault = 13;

var module_id;
let global_info;

function publish_ac_nr_of_phases_available(mod, n) {
  mod.provides.board_support.publish.ac_nr_of_phases_available(n);
}

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  global_info = info;
  // register commands
  setup.provides.yeti_simulation_control.register.enable(enable_simulation);
  setup.provides.yeti_simulation_control.register.setSimulationData((mod, args) => {
    mod.simulation_data = args.value;
  });

  setup.provides.powermeter.register.stop_transaction((mod, args) => ({
    status: 'NOT_SUPPORTED',
    error: 'YetiDriver does not support stop transaction request.',
  }));
  setup.provides.powermeter.register.start_transaction((mod, args) => ({ status: 'OK' }));

  setup.provides.board_support.register.setup((mod, args) => {
    mod.three_phases = args.three_phases;
    mod.has_ventilation = args.has_ventilation;
    mod.country_code = args.country_code;
    publish_ac_nr_of_phases_available(mod, (mod.use_three_phases_confirmed ? 3 : 1));
  });

  setup.provides.board_support.register.ac_set_overcurrent_limit_A((mod, args) => {
  });

  setup.provides.board_support.register.enable((mod, args) => {
    if (args.value) {
      if (mod.current_state === STATE_DISABLED) mod.state = STATE_A;
    } else mod.current_state = STATE_DISABLED;
  });

  setup.provides.board_support.register.pwm_on((mod, args) => { pwmOn(mod, args.value / 100.0); });
  setup.provides.board_support.register.pwm_off((mod, args) => { pwmOff(mod); });
  setup.provides.board_support.register.pwm_F((mod, args) => { pwmF(mod); });
  setup.provides.board_support.register.evse_replug((mod, args) => {
    evlog.error('Replugging not supported');
  });

  setup.provides.board_support.register.allow_power_on((mod, args) => {
    mod.power_on_allowed = args.value.allow_power_on;
  });

  // setup.provides.board_support.register.force_unlock((mod, args) => /* lock/unlock not implemented */ true);
  setup.provides.board_support.register.ac_switch_three_phases_while_charging((mod, args) => {
    mod.use_three_phases = args.value;
    mod.use_three_phases_confirmed = args.value;
    ac_publish_nr_of_phases_available(mod, (mod.use_three_phases_confirmed ? 3 : 1));
  });
  setup.provides.board_support.register.get_hw_capabilities((mod, args) => ({
    max_current_A_import: 32.0,
    min_current_A_import: 6.0,
    max_phase_count_import: 3,
    min_phase_count_import: 1,
    max_current_A_export: 16.0,
    min_current_A_export: 0.,
    max_phase_count_export: 3,
    min_phase_count_export: 1,
    supports_changing_phases_during_charging: true,
  }));
  setup.provides.board_support.register.ac_read_pp_ampacity((mod, args) => {
    let amp = { ampacity: read_pp_ampacity(mod) };
    return amp;
  });

  // subscribe vars of used modules
}).then((mod) => {
  mod.pubCnt = 0;
  clearData(mod);
  setInterval(simulation_loop, 250, mod);
  if (global_info.telemetry_enabled) {
    setInterval(telemetry_slow, 15000, mod);
    setInterval(telemetry_fast, 1000, mod);
  }
});

function telemetry_slow(mod) {
  const date = new Date();
  mod.telemetry_data.power_path_controller_version.timestamp = date.toISOString();
  mod.telemetry.publish('livedata', 'power_path_controller_version', mod.telemetry_data.power_path_controller_version);
}

function telemetry_fast(mod) {
  const date = new Date();
  mod.telemetry_data.power_path_controller.timestamp = date.toISOString();
  mod.telemetry_data.power_path_controller.cp_voltage_high = mod.cpHi;
  mod.telemetry_data.power_path_controller.cp_voltage_low = mod.cpLo;
  mod.telemetry_data.power_path_controller.cp_pwm_duty_cycle = mod.pwm_duty_cycle * 100.0;
  mod.telemetry_data.power_path_controller.cp_state = stateToString(mod);

  mod.telemetry_data.power_path_controller.temperature_controller = mod.powermeter.tempL1;
  mod.telemetry_data.power_path_controller.temperature_car_connector = mod.powermeter.tempL1 * 2.;
  mod.telemetry_data.power_path_controller.watchdog_reset_count = 0;
  mod.telemetry_data.power_path_controller.error = false;

  mod.telemetry_data.power_switch.timestamp = date.toISOString();
  mod.telemetry_data.power_switch.is_on = mod.relais_on;
  mod.telemetry_data.power_switch.time_to_switch_on_ms = 110;
  mod.telemetry_data.power_switch.time_to_switch_off_ms = 100;
  mod.telemetry_data.power_switch.temperature_C = 20;
  mod.telemetry_data.power_switch.error = false;
  mod.telemetry_data.power_switch.error_over_current = false;

  mod.telemetry_data.rcd.timestamp = date.toISOString();
  mod.telemetry_data.rcd.current_mA = mod.rcd_current;

  mod.telemetry.publish('livedata', 'power_path_controller', mod.telemetry_data.power_path_controller);
  mod.telemetry.publish('livedata', 'power_switch', mod.telemetry_data.power_switch);
  mod.telemetry.publish('livedata', 'rcd', mod.telemetry_data.rcd);

}

function publish_event(mod, event) {
  //console.log("------------ EVENT PUB " + event);
  mod.provides.board_support.publish.event({ event: event_to_enum(event) });
}


function enable_simulation(mod, args) {
  if (mod.simulation_enabled && !args.value) {
    publish_event(mod, Event_A);
    clearData(mod);
  }
  mod.simulation_enabled = args.value;
}

function simulation_loop(mod) {
  if (mod.simulation_enabled) {
    check_error_rcd(mod);
    read_from_car(mod);
    simulate_powermeter(mod);
    simulation_statemachine(mod);
    publish_yeti_simulation_control(mod);
  }

  // console.error(mod);
  mod.pubCnt++;
  switch (mod.pubCnt) {
    case 1:
      publish_powermeter(mod);
      publish_telemetry(mod);
      break;
    case 2:
      break;
    case 3:
      publish_keepalive(mod);
      break;

    default:
    case 4:
      mod.pubCnt = 0;
      break;
  }
}

// state machine for the evse
function simulation_statemachine(mod) {
  if (mod.last_state != mod.current_state) {
    publish_event(mod, mod.current_state);
  }

  switch (mod.current_state) {
    case STATE_DISABLED:
      powerOff();
      power_on_allowed = false;
      break;

    case STATE_A:
      mod.use_three_phases_confirmed = mod.use_three_phases;
      pwmOff(mod);
      reset_powermeter(mod);
      mod.simplified_mode = false;

      if (mod.last_state != STATE_A && mod.last_state != STATE_DISABLED
        && mod.last_state != STATE_F) {
        powerOff(mod);

        // If car was unplugged, reset RCD flag.
        mod.rcd_current = 0.1;
        mod.rcd_error = false;
      }
      break;
    case STATE_B:
      // Table A.6: Sequence 7 EV stops charging
      // Table A.6: Sequence 8.2 EV supply equipment
      // responds to EV opens S2 (w/o PWM)

      if (mod.last_state != STATE_A && mod.last_state != STATE_B) {

        // Need to switch off according to Table A.6 Sequence 8.1 within
        powerOff(mod);
      }

      // Table A.6: Sequence 1.1 Plug-in
      if (mod.last_state === STATE_A) {
        mod.simplified_mode = false;
      }

      break;
    case STATE_C:
      // Table A.6: Sequence 1.2 Plug-in
      if (mod.last_state === STATE_A) {
        mod.simplified_mode = true;
      }

      if (!mod.pwm_running) { // C1
        // Table A.6 Sequence 10.2: EV does not stop drawing power even
        // if PWM stops. Stop within 6 seconds (E.g. Kona1!)
        // This is implemented in EvseManager
        if (!mod.power_on_allowed) powerOff(mod);
      } else { // C2
        if (mod.power_on_allowed) {
          // Table A.6: Sequence 4 EV ready to charge.
          // Must enable power within 3 seconds.
          powerOn(mod);
        }
      }
      break;
    case STATE_D:
      // Table A.6: Sequence 1.2 Plug-in (w/ventilation)
      if (mod.last_state === STATE_A) {
        mod.simplified_mode = true;
      }

      if (!mod.pwm_running) {
        // Table A.6 Sequence 10.2: EV does not stop drawing power
        // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
        /* if (mod.last_pwm_running) // FIMXE implement 6 second timer
            startTimer(6000);
        if (timerElapsed()) { */
        // force power off under load
        powerOff(mod);
        // }
      } else {
        if (mod.power_on_allowed && !mod.relais_on) {
          // Table A.6: Sequence 4 EV ready to charge.
          // Must enable power within 3 seconds.
          if (mod.has_ventilation) powerOn(mod);
        }
      }
      break;
    case STATE_E:
      powerOff(mod);
      pwmOff(mod);
      break;
    case STATE_F:
      powerOff(mod);
      break;
    case STATE_DF:
      powerOff(mod);
      break;
  }
  mod.last_state = mod.current_state;
  mod.last_pwm_running = mod.pwm_running;
}

function check_error_rcd(mod) {
  if (mod.rcd_enabled && mod.simulation_data.rcd_current > 5.0) {
    if (!mod.rcd_error_reported) {
      mod.provides.rcd.publish.fault_dc();
      mod.rcd_error_reported = true;
    }
  } else {
    mod.rcd_error_reported = false;
  }
  mod.provides.rcd.publish.rcd_current_mA = mod.simulation_data.rcd_current;
}

function event_to_enum(event) {
  switch (event) {
    case STATE_A:
      return 'A';
    case STATE_B:
      return 'B';
    case STATE_C:
      return 'C';
    case STATE_D:
      return 'D';
    case STATE_E:
      return 'E';
    case STATE_F:
      return 'F';
    case STATE_DISABLED:
      return 'F';
    case Event_PowerOn:
      return 'PowerOn';
    case Event_PowerOff:
      return 'PowerOff';
    case STATE_DF:
      return 'ErrorDF';
    case Event_Error_Relais:
      return 'ErrorRelais';
    case Event_Error_RCD:
      return 'ErrorRCD';
    case Event_Error_VentilationNotAvailable:
      return 'VentilationNotAvailable';
    case Event_PermanentFault:
      return 'PermanentFault';
    default:
      evlog.error("Invalid event: " + event);
      return 'invalid';
  }
}

function pwmOn(mod, dutycycle) {
  if (dutycycle > 0.0) {
    mod.pwm_duty_cycle = dutycycle;
    mod.pwm_running = true;
    mod.pwm_error_f = false;
  } else {
    pwmOff(mod);
  }
}

function pwmOff(mod) {
  mod.pwm_duty_cycle = 1.0;
  mod.pwm_running = false;
  mod.pwm_error_f = false;
}

function pwmF(mod) {
  mod.pwm_duty_cycle = 1.0;
  mod.pwm_running = false;
  mod.pwm_error_f = true;
}

function powerOn(mod) {
  if (!mod.relais_on) {
    publish_event(mod, Event_PowerOn);
    mod.relais_on = true;
    mod.telemetry_data.power_switch.switching_count++;
  }
}

function powerOff(mod) {
  if (mod.relais_on) {
    publish_event(mod, Event_PowerOff);
    mod.telemetry_data.power_switch.switching_count++;
    mod.relais_on = false;
  }
}

// Translate ADC readings for lo and hi part of PWM to IEC61851 states.
function read_from_car(mod) {
  if (mod.pwm_running) {
    mod.pwm_voltage_hi = mod.simulation_data.cp_voltage;
    mod.pwm_voltage_lo = -12.0;
  } else {
    mod.pwm_voltage_hi = mod.simulation_data.cp_voltage;
    mod.pwm_voltage_lo = mod.pwm_voltage_hi;
  }

  if (mod.pwm_error_f) {
    mod.pwm_voltage_hi = -12.0;
    mod.pwm_voltage_lo = -12.0;
  }
  if (mod.simulation_data.error_e) {
    mod.pwm_voltage_hi = 0.0;
    mod.pwm_voltage_lo = 0.0;
  }
  if (mod.simulation_data.diode_fail) {
    mod.pwm_voltage_lo = -mod.pwm_voltage_hi;
  }

  const cpLo = mod.pwm_voltage_lo;
  const cpHi = mod.pwm_voltage_hi;

  // sth is wrong with negative signal
  if (mod.pwm_running && !is_voltage_in_range(cpLo, -12.0)) {
    // CP-PE short or signal somehow gone
    if (is_voltage_in_range(cpLo, 0.0) && is_voltage_in_range(cpHi, 0.0)) mod.current_state = STATE_E;
    // Diode fault
    else if (is_voltage_in_range(cpHi + cpLo, 0.0)) {
      mod.current_state = STATE_DF;
    } else return;
  } else if (is_voltage_in_range(cpHi, 12.0)) {
    // +12V State A IDLE (open circuit)
    mod.current_state = STATE_A;
  } else if (is_voltage_in_range(cpHi, 9.0)) {
    mod.current_state = STATE_B;
  } else if (is_voltage_in_range(cpHi, 6.0)) {
    mod.current_state = STATE_C;
  } else if (is_voltage_in_range(cpHi, 3.0)) {
    mod.current_state = STATE_D;
  } else if (is_voltage_in_range(cpHi, -12.0)) {
    mod.current_state = STATE_F;
  } else {

  }
}

// checks if voltage is within center+-interval
function is_voltage_in_range(voltage, center) {
  const interval = 1.1;
  return ((voltage > center - interval) && (voltage < center + interval));
}

function clearData(mod) {
  // Power meter data
  mod.powermeter = {
    time_stamp: Math.round(new Date().getTime() / 1000),
    totalWattHr: 0.0,

    wattL1: 0.0,
    vrmsL1: 230.0,
    irmsL1: 0.0,
    wattHrL1: 0.0,
    tempL1: 25.0,
    freqL1: 50.0,

    wattL2: 0.0,
    vrmsL2: 230.0,
    irmsL2: 0.0,
    wattHrL2: 0.0,
    tempL2: 25.0,
    freqL2: 50.0,

    wattL3: 0.0,
    vrmsL3: 230.0,
    irmsL3: 0.0,
    wattHrL3: 0.0,
    tempL3: 25.0,
    freqL3: 50.0,

    irmsN: 0.0,
  };

  mod.power_on_allowed = false;

  mod.relais_on = false;
  mod.current_state = STATE_DISABLED;
  mod.last_state = STATE_DISABLED;
  mod.time_stamp = Math.round(new Date().getTime() / 1000);
  mod.use_three_phases = true;
  mod.simplified_mode = false;

  mod.has_ventilation = false;

  mod.rcd_current = 0.1;
  mod.rcd_enabled = true;
  mod.rcd_error = false;
  mod.rcd_error_reported = false;

  mod.simulation_enabled = false;
  mod.pwm_duty_cycle = 0;
  mod.pwm_running = false;
  mod.pwm_error_f = false;
  mod.last_pwm_running = false;
  mod.use_three_phases_confirmed = true;
  mod.pwm_voltage_hi = 12.1;
  mod.pwm_voltage_lo = 12.1;

  mod.simulation_data = {
    cp_voltage: 12,
    diode_fail: false,
    error_e: false,
    pp_resistor: 220.1,
    rcd_current: 0.1,

    currents: {
      L1: 0.0,
      L2: 0.0,
      L3: 0.0,
      N: 0.0,
    },

    voltages: {
      L1: 230.0,
      L2: 230.0,
      L3: 230.0,
    },

    frequencies: {
      L1: 50.0,
      L2: 50.0,
      L3: 50.0,
    },

  };

  mod.country_code = 'DE';
  mod.lastPwmUpdate = 0;

  mod.wattHr = {
    L1: 0.0,
    L2: 0.0,
    L3: 0.0,
  };
  mod.powermeter_sim_last_time_stamp = 0;

  mod.telemetry_data = {

    power_path_controller_version: {
      timestamp: "",
      type: "power_path_controller_version",
      hardware_version: 3,
      software_version: "1.01",
      date_manufactured: "20220304",
      operating_time_h: 2330,
      operating_time_h_warning: 5000,
      operating_time_h_error: 6000,
      error: false
    },

    power_path_controller: {
      timestamp: "",
      type: "power_path_controller",
      cp_voltage_high: 0.0,
      cp_voltage_low: 0.0,
      cp_pwm_duty_cycle: 0.0,
      cp_state: "A1",
      pp_ohm: 220.1,
      supply_voltage_12V: 12.1,
      supply_voltage_minus_12V: -11.9,
      temperature_controller: 33,
      temperature_car_connector: 65,
      watchdog_reset_count: 1,
      error: false
    },

    power_switch: {
      timestamp: "",
      type: "power_switch",
      switching_count: 0,
      switching_count_warning: 30000,
      switching_count_error: 50000,
      is_on: false,
      time_to_switch_on_ms: 110,
      time_to_switch_off_ms: 100,
      temperature_C: 20,
      error: false,
      error_over_current: false
    },

    rcd: {
      timestamp: "",
      type: "rcd",
      enabled: true,
      current_mA: 2.5,
      triggered: false,
      error: false
    }
  }

}

function reset_powermeter(mod) {
  mod.wattHr = {
    L1: 0.0,
    L2: 0.0,
    L3: 0.0,
  };
  mod.powermeter_sim_last_time_stamp = 0;
}

function stateToString(mod) {
  let pwm = (mod.pwm_running ? "2" : "1");
  switch (mod.state) {
    case STATE_DISABLED:
      return 'Disabled';
      break;
    case STATE_A:
      return 'A' + pwm;
      break;
    case STATE_B:
      return 'B' + pwm;
      break;
    case STATE_C:
      return 'C' + pwm;
      break;
    case STATE_D:
      return 'D' + pwm;
      break;
    case STATE_E:
      return 'E';
      break;
    case STATE_F:
      return 'F';
      break;
    case STATE_DF:
      return 'DF';
      break;
  }
}

function power_meter_external(p) {
  const date = new Date();
  return ({
    timestamp: date.toISOString(),
    meter_id: 'YETI_POWERMETER',
    phase_seq_error: false,
    energy_Wh_import: {
      total: p.totalWattHr,
      L1: p.wattHrL1,
      L2: p.wattHrL2,
      L3: p.wattHrL3,
    },
    power_W: {
      total: p.wattL1 + p.wattL2 + p.wattL3,
      L1: p.wattL1,
      L2: p.wattL2,
      L3: p.wattL3,
    },
    voltage_V: {
      L1: p.vrmsL1,
      L2: p.vrmsL2,
      L3: p.vrmsL3,
    },
    current_A: {
      L1: p.irmsL1,
      L2: p.irmsL2,
      L3: p.irmsL3,
      N: p.irmsN,
    },
    frequency_Hz: {
      L1: p.freqL1,
      L2: p.freqL2,
      L3: p.freqL3,
    },
  });
}

function publish_powermeter(mod) {
  mod.provides.powermeter.publish.powermeter(power_meter_external(mod.powermeter));

  // Deprecated external stuff
  mod.mqtt.publish('/external/powermeter/vrmsL1', mod.powermeter.vrmsL1);
  mod.mqtt.publish('/external/powermeter/phaseSeqError', false);
  mod.mqtt.publish('/external/powermeter/time_stamp', mod.powermeter.time_stamp);
  mod.mqtt.publish('/external/powermeter/tempL1', mod.powermeter.tempL1);
  mod.mqtt.publish(
    '/external/powermeter/totalKw',
    (mod.powermeter.wattL1 + mod.powermeter.wattL2 + mod.powermeter.wattL3) / 1000.0
  );
  mod.mqtt.publish(
    '/external/powermeter/totalKWattHr',
    (mod.powermeter.wattHrL1 + mod.powermeter.wattHrL2 + mod.powermeter.wattHrL3) / 1000.0
  );
  mod.mqtt.publish('/external/powermeter_json', JSON.stringify(mod.powermeter));

  mod.mqtt.publish(`/external/${mod.info.id}/powermeter/tempL1`, mod.powermeter.tempL1);
  mod.mqtt.publish(
    `/external/${mod.info.id}/powermeter/totalKw`,
    (mod.powermeter.wattL1 + mod.powermeter.wattL2 + mod.powermeter.wattL3) / 1000.0
  );
  mod.mqtt.publish(
    `/external/${mod.info.id}/powermeter/totalKWattHr`,
    (mod.powermeter.wattHrL1 + mod.powermeter.wattHrL2 + mod.powermeter.wattHrL3) / 1000.0
  );
}

function publish_keepalive(mod) {
  mod.mqtt.publish('/external/keepalive_json', JSON.stringify(
    {
      hw_revision: 0,
      hw_type: 0,
      protocol_version_major: 0,
      protocol_version_minor: 1,
      sw_version_string: 'simulation',
      time_stamp: Math.round(new Date().getTime() / 1000),
    }
  ));
}

function publish_telemetry(mod) {
  mod.provides.board_support.publish.telemetry({
    evse_temperature_C: mod.powermeter.tempL1,
    fan_rpm: 1500.0,
    supply_voltage_12V: 12.01,
    supply_voltage_minus_12V: -11.8,
    relais_on: mod.relais_on,
  });
}

function publish_yeti_simulation_control(mod) {
  mod.provides.yeti_simulation_control.publish.enabled(mod.simulation_enabled);
  mod.provides.yeti_simulation_control.publish.simulation_feedback({
    pwm_duty_cycle: mod.pwm_duty_cycle,
    relais_on: (mod.relais_on ? (mod.use_three_phases_confirmed ? 3 : 1) : 0),
    evse_pwm_running: mod.pwm_running,
    evse_pwm_voltage_hi: mod.pwm_voltage_hi,
    evse_pwm_voltage_lo: mod.pwm_voltage_lo,
  });
}

function simulate_powermeter(mod) {
  const time_stamp = new Date().getTime();
  if (mod.powermeter_sim_last_time_stamp == 0) mod.powermeter_sim_last_time_stamp = time_stamp;
  const deltat = time_stamp - mod.powermeter_sim_last_time_stamp;
  mod.powermeter_sim_last_time_stamp = time_stamp;

  const wattL1 = mod.simulation_data.voltages.L1 * mod.simulation_data.currents.L1 * (mod.relais_on ? 1 : 0);
  const wattL2 = mod.simulation_data.voltages.L2 * mod.simulation_data.currents.L2
    * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);
  const wattL3 = mod.simulation_data.voltages.L3 * mod.simulation_data.currents.L3
    * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);

  mod.wattHr.L1 += wattL1 * deltat / 1000.0 / 3600.0;
  mod.wattHr.L2 += wattL2 * deltat / 1000.0 / 3600.0;
  mod.wattHr.L3 += wattL3 * deltat / 1000.0 / 3600.0;

  mod.powermeter = {
    time_stamp: Math.round(time_stamp / 1000),
    totalWattHr: Math.round(mod.wattHr.L1 + mod.wattHr.L2 + mod.wattHr.L3),

    wattL1: Math.round(wattL1),
    vrmsL1: mod.simulation_data.voltages.L1,
    irmsL1: mod.simulation_data.currents.L1,
    wattHrL1: Math.round(mod.wattHr.L1),
    tempL1: 25.0 + (wattL1 + wattL2 + wattL3) * 0.003,
    freqL1: mod.simulation_data.frequencies.L1,

    wattL2: Math.round(wattL2),
    vrmsL2: mod.simulation_data.voltages.L2,
    irmsL2: mod.simulation_data.currents.L1,
    wattHrL2: Math.round(mod.wattHr.L2),
    tempL2: 25.0 + (wattL1 + wattL2 + wattL3) * 0.003,
    freqL2: mod.simulation_data.frequencies.L2,

    wattL3: Math.round(wattL3),
    vrmsL3: mod.simulation_data.voltages.L3,
    irmsL3: mod.simulation_data.currents.L3,
    wattHrL3: Math.round(mod.wattHr.L3),
    tempL3: 25.0 + (wattL1 + wattL2 + wattL3) * 0.003,
    freqL3: mod.simulation_data.frequencies.L3,

    irmsN: mod.simulation_data.currents.N,
  };
}

function read_pp_ampacity(mod) {
  let pp_resistor = mod.simulation_data.pp_resistor;
  if (pp_resistor < 80.0 || pp_resistor > 2460) {
    evlog.error(`PP resistor value "${pp_resistor}" Ohm seems to be outside the allowed range.`);
    return "None"
  }

  // PP resistor value in spec, use a conservative interpretation of the resistance ranges
  if (pp_resistor > 936.0 && pp_resistor <= 2460.0) {
    return "A_13";
  } else if (pp_resistor > 308.0 && pp_resistor <= 936.0) {
    return "A_20";
  } else if (pp_resistor > 140.0 && pp_resistor <= 308.0) {
    return "A_32";
  } else if (pp_resistor > 80.0 && pp_resistor <= 140.0) {
    return "A_63";
  }

  return "None";
}
