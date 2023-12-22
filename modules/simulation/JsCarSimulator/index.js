// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { config } = require('process');
const { setInterval } = require('timers');

let globalconf;

// Command enable
function enable(mod, {value}) {
  if (mod === undefined || mod.provides === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  // ignore if not changed
  if (mod.enabled == value) return;

  simdata_reset_defaults(mod);

  // Start/Stop execution timer
  if (value) {
    mod.enabled = true;
    mod.loopInterval = setInterval(simulation_loop, 250, mod);
  } else {
    mod.enabled = false;
    if (!(mod.loopInterval === undefined)) clearInterval(mod.loopInterval);
  }

  // Enable/Disable HIL
  mod.uses.simulation_control.call.enable({ value });

  // Publish new simualtion enabled/disabled
  mod.provides.main.publish.enabled(value);
}

// Command execute_charging_session
function execute_charging_session(mod, args) {
  if (mod === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  if (!mod.enabled) {
    evlog.warning('Simulation disabled, cannot execute charging simulation.');
    return;
  }

  if (mod.executionActive === true) {
    evlog.warning('Execution of charging session simulation already running, cannot start new one');
    return;
  }

  const str = args.value;

  // start values
  simdata_reset_defaults(mod);
  addNoise(mod);

  mod.simCommands = parseSimCommands(mod, str);
  mod.loopCurrentCommand = -1;
  if (next_command(mod)) mod.executionActive = true;
}

// Command modify_charging_session
function modify_charging_session(mod, args) {
  if (mod === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  if (!mod.enabled) {
    evlog.warning('Simulation disabled, cannot execute charging simulation.');
    return;
  }

  const str = args.value;

  mod.simCommands = parseSimCommands(mod, str);
  mod.loopCurrentCommand = -1;
  if (next_command(mod)) mod.executionActive = true;
}

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  // Subscribe external cmds for nodered
  mqtt.subscribe(`everest_external/nodered/${config.module.connector_id}/carsim/cmd/enable`, (mod, en) => { enable(mod, { value: JSON.parse(en) }); });
  mqtt.subscribe(`everest_external/nodered/${config.module.connector_id}/carsim/cmd/execute_charging_session`, (mod, str) => {
    execute_charging_session(mod, { value: str });
  });
  mqtt.subscribe(`everest_external/nodered/${config.module.connector_id}/carsim/cmd/modify_charging_session`, (mod, str) => {
    modify_charging_session(mod, { value: str });
  });

  // register commands
  setup.provides.main.register.enable(enable);
  setup.provides.main.register.executeChargingSession(execute_charging_session);

  // subscribe vars of used modules
  setup.uses.simulation_control.subscribe.simulation_feedback((mod, args) => { mod.simulation_feedback = args; });
  if (setup.uses_list.slac.length > 0) setup.uses_list.slac[0].subscribe.state((mod, args) => { mod.slac_state = args; });

  // ISO15118 ev setup
  if (setup.uses_list.ev.length > 0) {
    setup.uses_list.ev[0].subscribe.AC_EVPowerReady((mod, value) => { mod.iso_pwr_ready = value; });
    setup.uses_list.ev[0].subscribe.AC_EVSEMaxCurrent((mod, value) => { mod.evse_maxcurrent = value; });
    setup.uses_list.ev[0].subscribe.AC_StopFromCharger((mod) => { mod.iso_stopped = true; });
    setup.uses_list.ev[0].subscribe.V2G_Session_Finished((mod) => { mod.v2g_finished = true; });
  }

  globalconf = config;
}).then((mod) => {
  registerAllCmds(mod);
  mod.enabled = false;
  if (mod.uses_list.ev.length > 0) mod.uses_list.ev[0].call.set_dc_params(get_hlc_dc_parameters(mod));
  if (mod.uses_list.ev.length > 0 && globalconf.module.support_sae_j2847 === true) {
    mod.uses_list.ev[0].call.enable_sae_j2847_v2g_v2h();
    mod.uses_list.ev[0].call.set_bpt_dc_params(get_hlc_bpt_dc_parameters(mod));
  }
  if(globalconf.module.auto_enable) enable(mod, { value: true });
  if (globalconf.module.auto_exec) execute_charging_session(mod, { value: globalconf.module.auto_exec_commands });
});

function simdata_reset_defaults(mod) {
  mod.simdata = {
    pp_resistor: 220.1,
    diode_fail: false,
    error_e: false,
    cp_voltage: 12.0,
    rcd_current: 0.1,
    voltages: { L1: 230.0, L2: 230.0, L3: 230.0 },
    currents: {
      L1: 0.0, L2: 0.0, L3: 0.0, N: 0.0,
    },
    frequencies: { L1: 50.0, L2: 50.0, L3: 50.0 },
  };

  mod.simdata_setting = {
    cp_voltage: 12.0,
    pp_resistor: 220.1,
    impedance: 500.0,
    rcd_current: 0.1,
    voltages: { L1: 230.0, L2: 230.0, L3: 230.0 },
    currents: {
      L1: 0.0, L2: 0.0, L3: 0.0, N: 0.0,
    },
    frequencies: { L1: 50.0, L2: 50.0, L3: 50.0 },
  };

  mod.simCommands = [];
  mod.loopCurrentCommand = 0;
  mod.executionActive = false;

  mod.state = 'unplugged';

  mod.v2g_finished = false;
  mod.iso_stopped = false;
  mod.evse_maxcurrent = 0;
  mod.maxCurrent = 0;
  mod.payment = 'ExternalPayment';
  mod.energymode = 'AC_single_phase_core';
  mod.iso_pwr_ready = false;

  mod.bcb_toggles = 0;
  mod.bcb_toggle_C = true;

  mod.uses.simulation_control.call.setSimulationData({ value: mod.simdata });
}

// Prepare next command
function next_command(mod) {
  mod.loopCurrentCommand++;
  if (mod.loopCurrentCommand < mod.simCommands.length) {
    evlog.info(current_command(mod));
    return true;
  }
  return false;
}

function current_command(mod) {
  return mod.simCommands[mod.loopCurrentCommand];
}

// state machine for the car simulator
function car_statemachine(mod) {
  let amps1 = 0.0;
  let amps2 = 0.0;
  let amps3 = 0.0;
  let amps = 0;
  switch (mod.state) {
    case 'unplugged':
      drawPower(mod, 0, 0, 0, 0);
      mod.simdata_setting.cp_voltage = 12.0;
      mod.simdata.error_e = false;
      mod.simdata.diode_fail = false;
      break;
    case 'pluggedin':
      drawPower(mod, 0, 0, 0, 0);
      mod.simdata_setting.cp_voltage = 9.0;
      break;
    case 'charging_regulated':
      amps = dutyCycleToAmps(mod.simulation_feedback.pwm_duty_cycle);
      if (amps > mod.maxCurrent) amps = mod.maxCurrent;

      // do not draw power if EVSE paused by stopping PWM
      if (amps > 5.9) {
        mod.simdata_setting.cp_voltage = 6.0;
        if (mod.simulation_feedback.relais_on > 0 && mod.numPhases > 0) amps1 = amps;
        else amps1 = 0;
        if (mod.simulation_feedback.relais_on > 1 && mod.numPhases > 1) amps2 = amps;
        else amps2 = 0;
        if (mod.simulation_feedback.relais_on > 2 && mod.numPhases > 2) amps3 = amps;
        else amps3 = 0;

        drawPower(mod, amps1, amps2, amps3, 0.2);
      } else {
        mod.simdata_setting.cp_voltage = 9.0;
        drawPower(mod, amps1, amps2, amps3, 0.2);
      }
      break;

    case 'charging_fixed':
      // Also draw power if EVSE stopped PWM - this is a break the rules mode to test the charging implementation!
      mod.simdata_setting.cp_voltage = 6.0;

      amps = mod.maxCurrent;

      if (amps > mod.maxCurrent) amps = mod.maxCurrent;

      if (mod.simulation_feedback.relais_on > 0 && mod.numPhases > 0) amps1 = amps;
      else amps1 = 0;
      if (mod.simulation_feedback.relais_on > 1 && mod.numPhases > 1) amps2 = amps;
      else amps2 = 0;
      if (mod.simulation_feedback.relais_on > 2 && mod.numPhases > 2) amps3 = amps;
      else amps3 = 0;

      drawPower(mod, amps1, amps2, amps3, 0.2);
      break;

    case 'error_e':
      mod.simdata_setting.cp_voltage = 0.0;
      drawPower(mod, 0, 0, 0, 0);
      mod.simdata.error_e = true;
      break;
    case 'diode_fail':
      mod.simdata_setting.cp_voltage = 9.0;
      drawPower(mod, 0, 0, 0, 0);
      mod.simdata.diode_fail = true;
      break;
    case 'iso_power_ready':
      drawPower(mod, 0, 0, 0, 0);
      mod.simdata_setting.cp_voltage = 6.0;
      break;
    case 'iso_charging_regulated':
      amps = mod.evse_maxcurrent;
      if (amps > mod.maxCurrent) amps = mod.maxCurrent;

      mod.simdata_setting.cp_voltage = 6.0;
      if (mod.simulation_feedback.relais_on > 0 && mod.numPhases > 0) amps1 = amps;
      else amps1 = 0;
      if (mod.simulation_feedback.relais_on > 1 && mod.numPhases > 1) amps2 = amps;
      else amps2 = 0;
      if (mod.simulation_feedback.relais_on > 2 && mod.numPhases > 2) amps3 = amps;
      else amps3 = 0;

      drawPower(mod, amps1, amps2, amps3, 0.2);
      break;
    case 'bcb_toggle':
      drawPower(mod, 0, 0, 0, 0);
      if (mod.bcb_toggle_C === true) {
        mod.simdata_setting.cp_voltage = 6.0;
        mod.bcb_toggle_C = false;
      } else {
        mod.simdata_setting.cp_voltage = 9.0;
        mod.bcb_toggle_C = true;
        mod.bcb_toggles++;
      }
      break;
    default:
      mod.state = 'unplugged';
      break;
  }
}

// IEC61851 Table A.8
function dutyCycleToAmps(dc) {
  if (dc < 8.0 / 100.0) return 0;
  if (dc < 85.0 / 100.0) return dc * 100.0 * 0.6;
  if (dc < 96.0 / 100.0) return (dc * 100.0 - 64) * 2.5;
  if (dc < 97.0 / 100.0) return 80;
  return 0;
}

function drawPower(mod, l1, l2, l3, n) {
  mod.simdata_setting.currents.L1 = l1;
  mod.simdata_setting.currents.L2 = l2;
  mod.simdata_setting.currents.L3 = l3;
  mod.simdata_setting.currents.N = n;
}

function simulation_loop(mod) {
  // Execute sim commands until a command blocks or we are finished
  while (mod.executionActive) {
    const c = current_command(mod);

    if (c.exec(mod, c)) {
      // command was non-blocking, run next command immediately
      if (!next_command(mod)) {
        evlog.debug('Finished simulation.');
        simdata_reset_defaults(mod);
        mod.executionActive = false;
        break;
      }
    } else break; // command blocked, wait for timer to run this function again
  }

  car_statemachine(mod);
  addNoise(mod);

  // send new sim data to HIL
  mod.uses.simulation_control.call.setSimulationData({ value: mod.simdata });
}

function addNoise(mod) {
  const noise = (1 + (Math.random() - 0.5) * 0.02);
  const lonoise = (1 + (Math.random() - 0.5) * 0.005);
  const impedance = mod.simdata_setting.impedance / 1000.0;

  mod.simdata.currents.L1 = mod.simdata_setting.currents.L1 * noise;
  mod.simdata.currents.L2 = mod.simdata_setting.currents.L2 * noise;
  mod.simdata.currents.L3 = mod.simdata_setting.currents.L3 * noise;
  mod.simdata.currents.N = mod.simdata_setting.currents.N * noise;

  mod.simdata.voltages.L1 = mod.simdata_setting.voltages.L1 * noise - impedance * mod.simdata.currents.L1;
  mod.simdata.voltages.L2 = mod.simdata_setting.voltages.L2 * noise - impedance * mod.simdata.currents.L2;
  mod.simdata.voltages.L3 = mod.simdata_setting.voltages.L3 * noise - impedance * mod.simdata.currents.L3;

  mod.simdata.frequencies.L1 = mod.simdata_setting.frequencies.L1 * lonoise;
  mod.simdata.frequencies.L2 = mod.simdata_setting.frequencies.L2 * lonoise;
  mod.simdata.frequencies.L3 = mod.simdata_setting.frequencies.L3 * lonoise;

  mod.simdata.cp_voltage = mod.simdata_setting.cp_voltage * noise;
  mod.simdata.rcd_current = mod.simdata_setting.rcd_current * noise;
  mod.simdata.pp_resistor = mod.simdata_setting.pp_resistor * noise;
}

/*
 Parses simcmds string into simulation commands object

 Command functions return true if they are done or false if they need to
 be called again on the next sim loop (to implement blocking commands such as sleep)

*/
function parseSimCommands(mod, str) {
  // compile aliases here as well
  // and convert waiting times
  const simcmds = str.toLowerCase().replace(/\n/g, ';').split(';');
  const compiledCommands = [];

  for (cmd of simcmds) {
    cmd = cmd.replace(/,/g, ' ').split(' ');
    const cmdname = cmd.shift();
    const args = [];
    for (a of cmd) {
      if (isNaN(a)) args.push(a);
      else args.push(parseFloat(a));
    }

    const c = mod.registeredCmds[cmdname];
    if (c === undefined || args.length != c.numargs) {
      // Ignoreing unknown command / command with wrong parameter count
      evlog.warning(`Ignoring command ${cmdname} with ${args.length} arguments`);
    } else {
      compiledCommands.push({ cmd: c.cmd, args, exec: c.exec });
    }
  }
  return compiledCommands;
}

// Register all available sim commands
function registerAllCmds(mod) {
  mod.registeredCmds = [];

  registerCmd(mod, 'cp', 1, (mod, c) => {
    mod.simdata_setting.cp_voltage = c.args[0];
    return true;
  });

  registerCmd(mod, 'sleep', 1, (mod, c) => {
    if (c.timeLeft === undefined) c.timeLeft = c.args[0] * 4 + 1;
    return (!(c.timeLeft-- > 0));
  });

  registerCmd(mod, 'iec_wait_pwr_ready', 0, (mod, c) => {
    mod.state = 'pluggedin';
    if (mod.simulation_feedback === undefined) return false;
    if (mod.simulation_feedback.evse_pwm_running && dutyCycleToAmps(mod.simulation_feedback.pwm_duty_cycle) > 0) {
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_wait_pwm_is_running', 0, (mod, c) => {
    mod.state = 'pluggedin';
    if (mod.simulation_feedback === undefined) return false;
    if (mod.simulation_feedback.evse_pwm_running && mod.simulation_feedback.pwm_duty_cycle > 0.04
      && mod.simulation_feedback.pwm_duty_cycle < 0.97) {
      return true;
    }
    return false;
  });

  registerCmd(mod, 'draw_power_regulated', 2, (mod, c) => {
    mod.maxCurrent = c.args[0];
    mod.numPhases = c.args[1];
    mod.state = 'charging_regulated';
    return true;
  });

  registerCmd(mod, 'draw_power_fixed', 2, (mod, c) => {
    mod.maxCurrent = c.args[0];
    mod.numPhases = c.args[1];
    mod.state = 'charging_fixed';
    return true;
  });

  registerCmd(mod, 'pause', 0, (mod, c) => {
    mod.state = 'pluggedin';
    return true;
  });

  registerCmd(mod, 'unplug', 0, (mod, c) => {
    mod.state = 'unplugged';
    return true;
  });

  registerCmd(mod, 'error_e', 0, (mod, c) => {
    mod.state = 'error_e';
    return true;
  });

  registerCmd(mod, 'diode_fail', 0, (mod, c) => {
    mod.state = 'diode_fail';
    return true;
  });

  registerCmd(mod, 'rcd_current', 1, (mod, c) => {
    mod.simdata_setting.rcd_current = c.args[0];
    return true;
  });

  registerCmd(mod, 'pp_resistor', 1, (mod, c) => {
    mod.simdata_setting.pp_resistor = c.args[0];
    return true;
  });

  registerCmd(mod, 'iso_wait_slac_matched', 0, (mod, c) => {
    mod.state = 'pluggedin';
    if (mod.slac_state === undefined) return false;
    if (mod.slac_state === 'UNMATCHED') if (mod.uses_list.slac.length > 0) mod.uses_list.slac[0].call.enter_bcd();
    if (mod.slac_state === 'MATCHED') return true;
  });
  // --- wip

  if (mod.uses_list.ev.length > 0) registerCmd(mod, 'iso_start_v2g_session', 2, (mod, c) => {
    if (c.args[0] === 'externalpayment') mod.payment = 'ExternalPayment';
    else if (c.args[0] === 'contract') mod.payment = 'Contract';
    else return false;

    switch (c.args[1]) {
      case 'ac_single_phase_core': mod.energymode = 'AC_single_phase_core'; break;
      case 'ac_three_phase_core': mod.energymode = 'AC_three_phase_core'; break;
      case 'dc_core': mod.energymode = 'DC_core'; break;
      case 'dc_extended': mod.energymode = 'DC_extended'; break;
      case 'dc_combo_core': mod.energymode = 'DC_combo_core'; break;
      case 'dc_unique': mod.energymode = 'DC_unique'; break;
      default: return false;
    }
    // TODO_SL: Check NumPhases with EnergyMode

    args = { PaymentOption: mod.payment, EnergyTransferMode: mod.energymode };

    if (mod.uses_list.ev[0].call.start_charging(args) === true) {
      return true;
    }
    return false; // TODO:SL: Bleibt ewig in einer Schleife hängen, weil es nicht weiter geht
  });

  // not checked -----------------------------------------------------------

  registerCmd(mod, 'iso_wait_pwr_ready', 0, (mod, c) => {
    if (mod.iso_pwr_ready === true) {
      mod.state = 'iso_power_ready';
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_draw_power_regulated', 2, (mod, c) => {
    mod.maxCurrent = c.args[0];
    mod.numPhases = c.args[1];
    mod.state = 'iso_charging_regulated';
    return true;
  });

  if (mod.uses_list.ev.length > 0)
    registerCmd(mod, 'iso_stop_charging', 0, (mod, c) => {
      mod.uses_list.ev[0].call.stop_charging();
      mod.state = 'pluggedin';
      return true;
    });

  if (mod.uses_list.ev.length > 0)
    registerCmd(mod, 'iso_wait_for_stop', 1, (mod, c) => {
      if (c.timeLeft === undefined) {
        c.timeLeft = c.args[0] * 4 + 1; // First time set the timer
      }
      if (!(c.timeLeft-- > 0)) {
        mod.uses_list.ev[0].call.stop_charging();
        mod.state = 'pluggedin';
        return true;
      }
      if (mod.iso_stopped === true) {
        mod.state = 'pluggedin';
        return true;
      }
      return false;
    });

  registerCmd(mod, 'iso_wait_v2g_session_stopped', 0, (mod, c) => {
    if (mod.v2g_finished === true) {
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_pause_charging', 0, (mod, c) => {
    mod.uses_list.ev[0].call.pause_charging();
    mod.state = 'pluggedin';
    mod.iso_pwr_ready = false;
    return true;
  });

  registerCmd(mod, 'iso_wait_for_resume', 0, (mod, c) => {
    return false;
  });

  registerCmd(mod, 'iso_start_bcb_toogle', 1, (mod, c) => {
    mod.state = 'bcb_toggle';
    if (mod.bcb_toggles >= c.args[0] || mod.bcb_toggles === 3) {
      mod.bcb_toggles = 0;
      mod.state = 'pluggedin';
      return true;
    }
    return false;
  });
}

function registerCmd(mod, name, numargs, execfunction) {
  mod.registeredCmds[name] = { cmd: name, numargs, exec: execfunction };
}

function get_hlc_dc_parameters(mod) {
  return {
    EV_Parameters: {
      MaxCurrentLimit: mod.config.module.dc_max_current_limit,
      MaxPowerLimit: mod.config.module.dc_max_power_limit,
      MaxVoltageLimit: mod.config.module.dc_max_voltage_limit,
      EnergyCapacity: mod.config.module.dc_energy_capacity,
      TargetCurrent: mod.config.module.dc_target_current,
      TargetVoltage: mod.config.module.dc_target_voltage,
    }
  };
}

function get_hlc_bpt_dc_parameters(mod) {
  return {
    EV_BPT_Parameters: {
      DischargeMaxCurrentLimit: mod.config.module.dc_discharge_max_current_limit,
      DischargeMaxPowerLimit: mod.config.module.dc_discharge_max_power_limit,
      DischargeTargetCurrent: mod.config.module.dc_discharge_target_current,
      DischargeMinimalSoC: mod.config.module.dc_discharge_v2g_minimal_soc,
    }
  };
}

/*
 command reference:

 cp N - set cp to N volts - don't use except for bogus car simulation. Use iec_xxx commands for normal sessions
 sleep N - sleep N seconds
 iec_wait_pwr_ready - wait for power ready IEC mode only (i.e. a valid 10-95% PWM or so state B2)
 iso_wait_pwr_ready - wait for ISO power ready (B2 5% and ISO15118 EVCC successful connection, with fallback to IEC)
 draw_power_regulated|draw_power_fixed, A,N - switch to C and draw A amps from the N phases once relais turns on.
                    Auto 0 if phases are not provided by EVSE. if fixed, draw power independent of PWM DC or ISO.
                    If regulated, follow ISO or PWM current limits.
 pause - sets CP level to 9V and car state machine stops drawing power. use draw_power_xxx to resume

 unplug - alias to cp 12V

 error_e - signal Error E (CP-PE short)
 diode_fail - fail diode

 rcd_current N - set rcd_current to N mA
 pp_resistor N - apply N Ohms to PP

 In case EVSE reports error F state machine cancels execution and resets cp to 12V

 TODO:

 - implement remaining commands
 - only draw power on phases that are switched on (add #phases in feedback packet)
   -> implemented in C++, recompile!, change in js code!

*/
