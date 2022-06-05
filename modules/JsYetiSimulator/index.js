// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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

const Event_CarPluggedIn = 0;
const Event_CarRequestedPower = 1;
const Event_PowerOn = 2;
const Event_PowerOff = 3;
const Event_CarRequestedStopPower = 4;
const Event_CarUnplugged = 5;
const Event_Error_E = 6;
const Event_Error_DF = 7;
const Event_Error_Relais = 8;
const Event_Error_RCD = 9;
const Event_Error_VentilationNotAvailable = 10;
const Event_EnterBCD = 11;
const Event_LeaveBCD = 12;
const Event_PermanentFault = 13;


boot_module(async ({ setup, info, config, mqtt }) => {
  // register commands
  setup.provides.yeti_simulation_control.register.enable(enable_simulation);
  setup.provides.yeti_simulation_control.register.setSimulationData((mod, args) => {/*evlog.error(args.value);*/mod.simulation_data = args.value; });

  setup.provides.yeti_extras.register.firmware_update((mod, args) => { });

  setup.provides.board_support.register.setup((mod, args) => {
    mod.three_phases = args.three_phases;
    mod.has_ventilation = args.has_ventilation;
    mod.country_code = args.country_code;
    mod.rcd_enabled = args.rcd_enabled;
    publish_nr_of_phases_available(mod, (mod.use_three_phases_confirmed ? 3 : 1));
  });

  setup.provides.board_support.register.enable((mod, args) => {
    if (args.value) {
      if (mod.current_state === STATE_DISABLED) mod.state = STATE_A;
    } else mod.current_state = STATE_DISABLED;
  });

  setup.provides.board_support.register.pwm_on((mod, args) => { pwmOn(mod, args.value); });
  setup.provides.board_support.register.pwm_off((mod, args) => { pwmOff(mod); });
  setup.provides.board_support.register.pwm_F((mod, args) => { pwmF(mod); });
  setup.provides.board_support.register.allow_power_on((mod, args) => {
    mod.power_on_allowed = args.value;
  });
  setup.provides.board_support.register.force_unlock((mod, args) => {/* lock/unlock not implemented */ return true; });
  setup.provides.board_support.register.switch_three_phases_while_charging((mod, args) => {
    mod.use_three_phases = args.value;
    mod.use_three_phases_confirmed = args.value;
    publish_nr_of_phases_available(mod, (mod.use_three_phases_confirmed ? 3 : 1));
  });
  setup.provides.board_support.register.get_hw_capabilities((mod, args) => {
    return ({
      'max_current_A': 32.0,
      'min_current_A': 6.0,
      'max_phase_count': 3,
      'min_phase_count': 1,
      'supports_changing_phases_during_charging': true
    });
  });

  // subscribe vars of used modules
}).then((mod) => {
  mod.pubCnt = 0;
  clearData(mod);
  setInterval(simulation_loop, 250, mod);
});

function publish_event(mod, event) {
  //console.log("------------ EVENT PUB "+event);
  mod.provides.board_support.publish.event(event_to_enum(event));
}

function enable_simulation(mod, args) {
  if (mod.simulation_enabled && !args.value) {
    publish_event(mod, Event_CarUnplugged);
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

  //console.error(mod);
  mod.pubCnt++;
  switch (mod.pubCnt) {
    case 1:
      publish_powermeter(mod);
      publish_telemetry(mod);
      break;
    case 2:
      publish_yeti_extras(mod);
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

      if (mod.last_state === STATE_B || mod.last_state === STATE_C || mod.last_state === STATE_D) {
        publish_event(mod, Event_LeaveBCD);
      }

      if (mod.last_state != STATE_A && mod.last_state != STATE_DISABLED &&
        mod.last_state != STATE_F) {
        publish_event(mod, Event_CarRequestedStopPower);
        powerOff(mod);
        publish_event(mod, Event_CarUnplugged);

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
        publish_event(mod, Event_CarRequestedStopPower);
        // Need to switch off according to Table A.6 Sequence 8.1 within
        powerOff(mod);
      }

      // Table A.6: Sequence 1.1 Plug-in
      if (mod.last_state === STATE_A) {
        publish_event(mod, Event_CarPluggedIn);
        mod.simplified_mode = false;
      }

      if (mod.last_state === STATE_A || mod.last_state === STATE_E || mod.last_state === STATE_F) {
        publish_event(mod, Event_EnterBCD);
      }
      break;
    case STATE_C:
      // Table A.6: Sequence 1.2 Plug-in
      if (mod.last_state === STATE_A) {
        publish_event(mod, Event_CarPluggedIn);
        mod.simplified_mode = true;
      }
      if (mod.last_state === STATE_B) {
        publish_event(mod, Event_CarRequestedPower);
      }
      if (mod.last_state === STATE_A || mod.last_state === STATE_E || mod.last_state === STATE_F) {
        publish_event(mod, Event_EnterBCD);
      }

      if (!mod.pwm_running) { // C1
        // Table A.6 Sequence 10.2: EV does not stop drawing power even
        // if PWM stops. Stop within 6 seconds (E.g. Kona1!)
        // if (mod.last_pwm_running) FIXME implement 6 second timer!
        //     startTimer(6000);
        // if (timerElapsed()) {
        // force power off under load
        powerOff(mod);
        //}
      } else { // C2
        if (mod.power_on_allowed) {
          // Table A.6: Sequence 4 EV ready to charge.
          // Must enable power within 3 seconds.
          powerOn(mod);

          // Simulate Request power Event here for simplified mode to
          // ensure that this mode behaves similar for higher layers.
          // Note this does not work with 5% mode correctly, but
          // simplified mode does not support HLC anyway.
          if (!mod.last_pwm_running && mod.simplified_mode)
            publish_event(mod, Event_CarRequestedPower);
        }
      }
      break;
    case STATE_D:
      // Table A.6: Sequence 1.2 Plug-in (w/ventilation)
      if (mod.last_state === STATE_A) {
        publish_event(mod, Event_CarPluggedIn);
        publish_event(mod, Event_CarRequestedPower);
        mod.simplified_mode = true;
      }
      if (mod.last_state === STATE_B) {
        publish_event(mod, Event_CarRequestedPower);
      }
      if (mod.last_state === STATE_A || mod.last_state === STATE_E || mod.last_state === STATE_F) {
        publish_event(mod, Event_EnterBCD);
      }

      if (!mod.pwm_running) {
        // Table A.6 Sequence 10.2: EV does not stop drawing power
        // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
        /*if (mod.last_pwm_running) // FIMXE implement 6 second timer
            startTimer(6000);
        if (timerElapsed()) {*/
        // force power off under load
        powerOff(mod);
        //}
      } else {
        if (mod.power_on_allowed && !mod.relais_on) {
          // Table A.6: Sequence 4 EV ready to charge.
          // Must enable power within 3 seconds.
          if (!mod.has_ventilation)
            publish_event(mod, Event_Error_VentilationNotAvailable);
          else
            powerOn(mod);
        }
        if (mod.last_state === STATE_C) {
          // Car switches to ventilation while charging.
          if (!mod.has_ventilation)
            publish_event(mod, Event_Error_VentilationNotAvailable);
        }
      }
      break;
    case STATE_E:
      if (mod.last_state != mod.current_state)
        publish_event(mod, Event_Error_E);
      if (mod.last_state === STATE_B || mod.last_state === STATE_C || mod.last_state === STATE_D) {
        publish_event(mod, Event_LeaveBCD);
      }
      powerOff(mod);
      pwmOff(mod);
      break;
    case STATE_F:
      if (mod.last_state === STATE_B || mod.last_state === STATE_C || mod.last_state === STATE_D) {
        publish_event(mod, Event_LeaveBCD);
      }
      powerOff(mod);
      break;
    case STATE_DF:
      if (mod.last_state != mod.current_state)
        publish_event(mod, Event_Error_DF);
      if (mod.last_state === STATE_B || mod.last_state === STATE_C || mod.last_state === STATE_D) {
        publish_event(mod, Event_LeaveBCD);
      }
      powerOff(mod);
      break;
  }
  mod.last_state = mod.current_state;
  mod.last_pwm_running = mod.pwm_running;
}

function check_error_rcd(mod) {
  if (mod.rcd_enabled && mod.simulation_data.rcd_current > 5.0) {
    publish_event(mod, Event_Error_RCD);
  }
}



function publish_nr_of_phases_available(mod, n) {
  //console.log("------------ NR PHASE PUB "+n);
  mod.provides.board_support.publish.nr_of_phases_available(n);
}

function event_to_enum(event) {
  switch (event) {
    case Event_CarPluggedIn:
      return 'CarPluggedIn';
    case Event_CarRequestedPower:
      return 'CarRequestedPower';
    case Event_PowerOn:
      return 'PowerOn';
    case Event_PowerOff:
      return 'PowerOff';
    case Event_CarRequestedStopPower:
      return 'CarRequestedStopPower';
    case Event_CarUnplugged:
      return 'CarUnplugged';
    case Event_Error_E:
      return 'ErrorE';
    case Event_Error_DF:
      return 'ErrorDF';
    case Event_Error_Relais:
      return 'ErrorRelais';
    case Event_Error_RCD:
      return 'ErrorRCD';
    case Event_Error_VentilationNotAvailable:
      return 'VentilationNotAvailable';
    case Event_EnterBCD:
      return 'EnterBCD';
    case Event_LeaveBCD:
      return 'LeaveBCD';
    case Event_PermanentFault:
      return 'PermanentFault';
    default:
      return 'invalid';
  }
}

function pwmOn(mod, dutycycle) {
  if (dutycycle > 0.) {
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
  }
}

function powerOff(mod) {
  if (mod.relais_on) {
    publish_event(mod, Event_PowerOff);
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

  var cpLo = mod.pwm_voltage_lo;
  var cpHi = mod.pwm_voltage_hi;

  // sth is wrong with negative signal
  if (mod.pwm_running && !is_voltage_in_range(cpLo, -12.)) {
    // CP-PE short or signal somehow gone
    if (is_voltage_in_range(cpLo, 0.) && is_voltage_in_range(cpHi, 0.))
      mod.current_state = STATE_E;
    // Diode fault
    else if (is_voltage_in_range(cpHi + cpLo, 0.)) {
      mod.current_state = STATE_DF;
    } else return;
  } else if (is_voltage_in_range(cpHi, 12.)) {
    // +12V State A IDLE (open circuit)
    mod.current_state = STATE_A;
  } else if (is_voltage_in_range(cpHi, 9.)) {
    mod.current_state = STATE_B;
  } else if (is_voltage_in_range(cpHi, 6.)) {
    mod.current_state = STATE_C;
  } else if (is_voltage_in_range(cpHi, 3.)) {
    mod.current_state = STATE_D;
  } else if (is_voltage_in_range(cpHi, -12.)) {
    mod.current_state = STATE_F;
  } else {
    return;
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

    irmsN: 0.0
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
      N: 0.0
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
    }

  };

  mod.country_code = 'DE';
  mod.lastPwmUpdate = 0;

  mod.wattHr = {
    L1: 0.0,
    L2: 0.0,
    L3: 0.0
  };
  mod.powermeter_sim_last_time_stamp = 0;
}

function reset_powermeter(mod) {
  mod.wattHr = {
    L1: 0.0,
    L2: 0.0,
    L3: 0.0
  };
  mod.powermeter_sim_last_time_stamp = 0;
}
/*
function stateToString(state) {
  switch (state) {
    case STATE_DISABLED:
      return 'Disabled';
    break;
    case STATE_A:
      return 'A';
    break;
    case STATE_B:
      return 'B';
    break;
    case STATE_C:
      return 'C';
    break;
    case STATE_D:
      return 'D';
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
}*/
function power_meter_external(p) {
  return ({
    timestamp: p.time_stamp,
    meter_id: 'YETI_POWERMETER',
    phase_seq_error: false,
    energy_Wh_import: {
      total: p.totalWattHr,
      L1: p.wattHrL1,
      L2: p.wattHrL2,
      L3: p.wattHrL3
    },
    power_W: {
      total: p.wattL1 + p.wattL2 + p.wattL3,
      L1: p.wattL1,
      L2: p.wattL2,
      L3: p.wattL3
    },
    voltage_V: {
      L1: p.vrmsL1,
      L2: p.vrmsL2,
      L3: p.vrmsL3
    },
    current_A: {
      L1: p.irmsL1,
      L2: p.irmsL2,
      L3: p.irmsL3,
      N: p.irmsN
    },
    frequency_Hz: {
      L1: p.freqL1,
      L2: p.freqL2,
      L3: p.freqL3
    }
  });
}

function publish_powermeter(mod) {

  mod.provides.powermeter.publish.powermeter(power_meter_external(mod.powermeter));

  // Publish raw packet for debugging purposes
  mod.provides.debug_powermeter.publish.debug_json(mod.powermeter);

  // Deprecated external stuff
  mod.mqtt.publish('/external/powermeter/vrmsL1', mod.powermeter.vrmsL1);
  mod.mqtt.publish('/external/powermeter/phaseSeqError', false);
  mod.mqtt.publish('/external/powermeter/time_stamp', mod.powermeter.time_stamp);
  mod.mqtt.publish('/external/powermeter/tempL1', mod.powermeter.tempL1);
  mod.mqtt.publish('/external/powermeter/totalKw', (mod.powermeter.wattL1 + mod.powermeter.wattL2 + mod.powermeter.wattL3) / 1000.);
  mod.mqtt.publish('/external/powermeter/totalKWattHr', (mod.powermeter.wattHrL1 + mod.powermeter.wattHrL2 + mod.powermeter.wattHrL3) / 1000.);
  mod.mqtt.publish('/external/powermeter_json', JSON.stringify(mod.powermeter));

  mod.mqtt.publish('/external/' + mod.info.id + '/powermeter/tempL1', mod.powermeter.tempL1);
  mod.mqtt.publish('/external/' + mod.info.id + '/powermeter/totalKw', (mod.powermeter.wattL1 + mod.powermeter.wattL2 + mod.powermeter.wattL3) / 1000.);
  mod.mqtt.publish('/external/' + mod.info.id + '/powermeter/totalKWattHr', (mod.powermeter.wattHrL1 + mod.powermeter.wattHrL2 + mod.powermeter.wattHrL3) / 1000.);
}

function publish_keepalive(mod) {
  mod.mqtt.publish('/external/keepalive_json', JSON.stringify(
    {
      'hw_revision': 0, 'hw_type': 0,
      'protocol_version_major': 0,
      'protocol_version_minor': 1,
      'sw_version_string': 'simulation',
      'time_stamp': Math.round(new Date().getTime() / 1000)
    }
  ));
}


function publish_telemetry(mod) {
  mod.provides.board_support.publish.telemetry({
    'temperature': mod.powermeter.tempL1,
    'fan_rpm': 1500.0,
    'supply_voltage_12V': 12.01,
    'supply_voltage_minus_12V': -11.8,
    'rcd_current': mod.rcd_current,
    'relais_on': mod.relais_on
  });
}

function publish_yeti_extras(mod) {
  mod.provides.yeti_extras.publish.time_stamp(Math.round(new Date().getTime() / 1000));
  mod.provides.yeti_extras.publish.hw_type(0);
  mod.provides.yeti_extras.publish.hw_revision(0);
  mod.provides.yeti_extras.publish.protocol_version_major(0);
  mod.provides.yeti_extras.publish.protocol_version_minor(1);
  mod.provides.yeti_extras.publish.sw_version_string('simulation');
}

function publish_yeti_simulation_control(mod) {
  mod.provides.yeti_simulation_control.publish.enabled(mod.simulation_enabled);
  mod.provides.yeti_simulation_control.publish.simulation_feedback({
    pwm_duty_cycle: mod.pwm_duty_cycle,
    relais_on: (mod.relais_on ? (mod.use_three_phases_confirmed ? 3 : 1) : 0),
    evse_pwm_running: mod.pwm_running,
    evse_pwm_voltage_hi: mod.pwm_voltage_hi,
    evse_pwm_voltage_lo: mod.pwm_voltage_lo
  });
  /*evlog.error({
    pwm_duty_cycle: mod.pwm_duty_cycle,
    relais_on: (mod.relais_on?(mod.use_three_phases_confirmed?3:1):0),
    evse_pwm_running: mod.pwm_running,
    evse_pwm_voltage_hi: mod.pwm_voltage_hi,
    evse_pwm_voltage_lo: mod.pwm_voltage_lo
  });*/
}

function simulate_powermeter(mod) {
  let time_stamp = new Date().getTime();
  if (mod.powermeter_sim_last_time_stamp == 0) mod.powermeter_sim_last_time_stamp = time_stamp;
  let deltat = time_stamp - mod.powermeter_sim_last_time_stamp;
  mod.powermeter_sim_last_time_stamp = time_stamp;

  let wattL1 = mod.simulation_data.voltages.L1 * mod.simulation_data.currents.L1 * (mod.relais_on ? 1 : 0);
  let wattL2 = mod.simulation_data.voltages.L2 * mod.simulation_data.currents.L2 * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);
  let wattL3 = mod.simulation_data.voltages.L3 * mod.simulation_data.currents.L3 * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);


  mod.wattHr.L1 += wattL1 * deltat / 1000. / 3600.;
  mod.wattHr.L2 += wattL2 * deltat / 1000. / 3600.;
  mod.wattHr.L3 += wattL3 * deltat / 1000. / 3600.;

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

    irmsN: mod.simulation_data.currents.N
  };
}
