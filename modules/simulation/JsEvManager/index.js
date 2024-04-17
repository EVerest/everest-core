// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');

let globalconf;

function simdata_reset_defaults(mod) {
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

  mod.pp = '';
  mod.rcd_current_mA = 0;
  mod.pwm_duty_cycle = 100;

  mod.dc_power_on = false;
  mod.last_state = '';
  mod.last_pwm_duty_cycle = 0;
}

function current_command(mod) {
  return mod.simCommands[mod.loopCurrentCommand];
}

// Prepare next command
function next_command(mod) {
  mod.loopCurrentCommand += 1;
  if (mod.loopCurrentCommand < mod.simCommands.length) {
    evlog.info(current_command(mod));
    return true;
  }
  return false;
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
    if (c === undefined || args.length !== c.numargs) {
      // Ignoreing unknown command / command with wrong parameter count
      evlog.warning(`Ignoring command ${cmdname} with ${args.length} arguments`);
    } else {
      compiledCommands.push({ cmd: c.cmd, args, exec: c.exec });
    }
  }
  return compiledCommands;
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

  mod.simCommands = parseSimCommands(mod, str);
  mod.loopCurrentCommand = -1;
  if (next_command(mod)) mod.executionActive = true;
}

// state machine for the car simulator
function car_statemachine(mod) {
  let new_in_state = false;
  if (mod.state !== mod.last_state) new_in_state = true;
  mod.last_state = mod.state;

  switch (mod.state) {
    case 'unplugged':
      if (new_in_state) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'A' });
        mod.uses.ev_board_support.call.allow_power_on({ value: false });

        // Wait for physical plugin (ev BSP sees state A on CP and not Disconnected)

        // If we have auto_exec configured, restart simulation when it was unplugged
        evlog.info('Unplug detected, restarting simulation.');
        mod.slac_state = 'UNMATCHED';
        mod.uses_list.ev[0].call.stop_charging();
        if (globalconf.module.auto_exec) execute_charging_session(mod, { value: globalconf.module.auto_exec_commands });
      }
      break;
    case 'pluggedin':
      if (new_in_state) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'B' });
        mod.uses.ev_board_support.call.allow_power_on({ value: false });
      }
      break;
    case 'charging_regulated':
      if (new_in_state || mod.pwm_duty_cycle !== mod.last_pwm_duty_cycle) {
        mod.last_pwm_duty_cycle = mod.pwm_duty_cycle;
        // do not draw power if EVSE paused by stopping PWM

        if (mod.pwm_duty_cycle > 7.0 && mod.pwm_duty_cycle < 97.0) {
          mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'C' });
        } else {
          mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'B' });
        }
      }
      break;

    case 'charging_fixed':
      // Todo(sl): What to do here
      if (new_in_state) {
        // Also draw power if EVSE stopped PWM - this is a break the rules mode to test the charging implementation!
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'C' });
      }
      break;

    case 'error_e':
      if (new_in_state) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'E' });
        mod.uses.ev_board_support.call.allow_power_on({ value: false });
      }
      break;
    case 'diode_fail':
      if (new_in_state) {
        mod.uses.ev_board_support.call.diode_fail({ value: true });
        mod.uses.ev_board_support.call.allow_power_on({ value: false });
      }
      break;
    case 'iso_power_ready':
      if (new_in_state) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'C' });
      }
      break;
    case 'iso_charging_regulated':
      if (new_in_state) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'C' });
      }
      break;
    case 'bcb_toggle':
      if (mod.bcb_toggle_C === true) {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'C' });
        mod.bcb_toggle_C = false;
      } else {
        mod.uses.ev_board_support.call.set_cp_state({ cp_state: 'B' });
        mod.bcb_toggle_C = true;
        mod.bcb_toggles += 1;
      }
      break;
    default:
      mod.state = 'unplugged';
      break;
  }
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
        // If we have auto_exec configured, restart simulation when it is done
        if (globalconf.module.auto_exec) execute_charging_session(mod, { value: globalconf.module.auto_exec_commands });
        break;
      }
    } else break; // command blocked, wait for timer to run this function again
  }

  car_statemachine(mod);
}

// Command enable
function enable(mod, { value }) {
  if (mod === undefined || mod.provides === undefined) {
    evlog.warning('Already received data, but framework is not ready yet');
    return;
  }

  // ignore if not changed
  if (mod.enabled === value) return;

  simdata_reset_defaults(mod);

  mod.uses.ev_board_support.call.allow_power_on({ value: false });

  mod.uses.ev_board_support.call.set_ac_max_current({ current: globalconf.module.max_curent });
  mod.uses.ev_board_support.call.set_three_phases({ three_phases: globalconf.module.three_phases });

  // Start/Stop execution timer
  if (value) {
    mod.enabled = true;
    mod.loopInterval = setInterval(simulation_loop, 250, mod);
  } else {
    mod.enabled = false;
    if (!(mod.loopInterval === undefined)) clearInterval(mod.loopInterval);
  }

  // Enable/Disable HIL
  mod.uses.ev_board_support.call.enable({ value });

  // Publish new simualtion enabled/disabled
  mod.provides.main.publish.enabled(value);
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

function registerCmd(mod, name, numargs, execfunction) {
  mod.registeredCmds[name] = { cmd: name, numargs, exec: execfunction };
}

// Register all available sim commands
function registerAllCmds(mod) {
  mod.registeredCmds = [];

  registerCmd(mod, 'sleep', 1, (mod, c) => {
    if (c.timeLeft === undefined) {
      c.timeLeft = c.args[0] * 4 + 1;
    }
    c.timeLeft -= 1;
    return (!(c.timeLeft > 0));
  });

  registerCmd(mod, 'iec_wait_pwr_ready', 0, (mod) => {
    mod.state = 'pluggedin';
    if (mod.pwm_duty_cycle > 7.0 && mod.pwm_duty_cycle < 97.0) {
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_wait_pwm_is_running', 0, (mod) => {
    mod.state = 'pluggedin';
    // AC ISO can also start with nominal dutycycle
    if (mod.pwm_duty_cycle > 4.0 && mod.pwm_duty_cycle < 97.0) {
      return true;
    }
    return false;
  });

  registerCmd(mod, 'draw_power_regulated', 2, (mod, c) => {
    mod.uses.ev_board_support.call.set_ac_max_current({ current: c.args[0] });
    if (c.args[1] === 3) mod.uses.ev_board_support.call.set_three_phases({ three_phases: true });
    else mod.uses.ev_board_support.call.set_three_phases({ three_phases: false });
    mod.state = 'charging_regulated';
    return true;
  });

  // Todo(sl). Check power_fixed again
  registerCmd(mod, 'draw_power_fixed', 2, (mod, c) => {
    mod.uses.ev_board_support.call.set_ac_max_current({ current: c.args[0] });
    if (c.args[1] === 3) mod.uses.ev_board_support.call.set_three_phases({ three_phases: true });
    else mod.uses.ev_board_support.call.set_three_phases({ three_phases: false });
    mod.state = 'charging_fixed';
    return true;
  });

  registerCmd(mod, 'pause', 0, (mod) => {
    mod.state = 'pluggedin';
    return true;
  });

  registerCmd(mod, 'unplug', 0, (mod) => {
    mod.state = 'unplugged';
    return true;
  });

  registerCmd(mod, 'error_e', 0, (mod) => {
    mod.state = 'error_e';
    return true;
  });

  registerCmd(mod, 'diode_fail', 0, (mod) => {
    mod.state = 'diode_fail';
    return true;
  });

  registerCmd(mod, 'rcd_current', 1, (mod, c) => {
    mod.uses.ev_board_support.call.set_rcd_error({ rcd_current_mA: c.args[0] });
    return true;
  });

  registerCmd(mod, 'iso_wait_slac_matched', 0, (mod) => {
    mod.state = 'pluggedin';
    if (mod.slac_state === undefined) {
      evlog.debug('Slac undefined');
    }
    if (mod.slac_state === 'UNMATCHED') {
      evlog.debug('Slac UNMATCHED');
      if (mod.uses_list.slac.length > 0) {
        evlog.debug('Slac trigger matching');
        mod.uses_list.slac[0].call.reset();
        mod.uses_list.slac[0].call.trigger_matching();
        mod.slac_state = 'TRIGGERED'; // make sure we do not trigger SLAC again before we get a state update
      }
    }
    if (mod.slac_state === 'MATCHED') {
      evlog.debug('Slac Matched');
      return true;
    }
    return false;
  });

  if (mod.uses_list.ev.length > 0) {
    registerCmd(mod, 'iso_start_v2g_session', 1, (mod, c) => {
      switch (c.args[0]) {
        case 'ac_single_phase_core': mod.energymode = 'AC_single_phase_core'; break;
        case 'ac_three_phase_core': mod.energymode = 'AC_three_phase_core'; break;
        case 'dc_core': mod.energymode = 'DC_core'; break;
        case 'dc_extended': mod.energymode = 'DC_extended'; break;
        case 'dc_combo_core': mod.energymode = 'DC_combo_core'; break;
        case 'dc_unique': mod.energymode = 'DC_unique'; break;
        default: return false;
      }

      mod.uses_list.ev[0].call.start_charging({ EnergyTransferMode: mod.energymode });

      return true;
    });
  }

  registerCmd(mod, 'iso_wait_pwr_ready', 0, (mod) => {
    if (mod.iso_pwr_ready === true) {
      mod.state = 'iso_power_ready';
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_dc_power_on', 0, (mod) => {
    mod.state = 'iso_power_ready';
    if (mod.dc_power_on === true) {
      mod.state = 'iso_charging_regulated';
      mod.uses.ev_board_support.call.allow_power_on({ value: true });
      return true;
    }
    return false;
  });

  registerCmd(mod, 'iso_draw_power_regulated', 2, (mod, c) => {
    mod.uses.ev_board_support.call.set_ac_max_current({ current: c.args[0] });
    if (c.args[1] === 3) mod.uses.ev_board_support.call.set_three_phases({ three_phases: true });
    else mod.uses.ev_board_support.call.set_three_phases({ three_phases: false });
    mod.state = 'iso_charging_regulated';
    return true;
  });

  if (mod.uses_list.ev.length > 0) {
    registerCmd(mod, 'iso_stop_charging', 0, (mod) => {
      mod.uses_list.ev[0].call.stop_charging();
      mod.uses.ev_board_support.call.allow_power_on({ value: false });
      mod.state = 'pluggedin';
      return true;
    });
  }

  if (mod.uses_list.ev.length > 0) {
    registerCmd(mod, 'iso_wait_for_stop', 1, (mod, c) => {
      if (c.timeLeft === undefined) {
        c.timeLeft = c.args[0] * 4 + 1; // First time set the timer
      }
      c.timeLeft -= 1;
      if (!(c.timeLeft > 0)) {
        mod.uses_list.ev[0].call.stop_charging();
        mod.uses.ev_board_support.call.allow_power_on({ value: false });
        mod.state = 'pluggedin';
        return true;
      }
      if (mod.iso_stopped === true) {
        evlog.info('POWER OFF iso stopped');
        mod.uses.ev_board_support.call.allow_power_on({ value: false });
        mod.state = 'pluggedin';
        return true;
      }
      return false;
    });
    registerCmd(mod, 'iso_wait_v2g_session_stopped', 0, (mod) => {
      if (mod.v2g_finished === true) {
        return true;
      }
      return false;
    });

    registerCmd(mod, 'iso_pause_charging', 0, (mod) => {
      mod.uses_list.ev[0].call.pause_charging();
      mod.state = 'pluggedin';
      mod.iso_pwr_ready = false;
      return true;
    });

    registerCmd(mod, 'iso_wait_for_resume', 0, () => false);

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

  registerCmd(mod, 'wait_for_real_plugin', 0, (mod) => {
    if (mod.actual_bsp_event === 'A') {
      evlog.info('Real plugin detected');
      mod.state = 'pluggedin';
      return true;
    }
    return false;
  });
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
    },
  };
}

function get_hlc_bpt_dc_parameters(mod) {
  return {
    EV_BPT_Parameters: {
      DischargeMaxCurrentLimit: mod.config.module.dc_discharge_max_current_limit,
      DischargeMaxPowerLimit: mod.config.module.dc_discharge_max_power_limit,
      DischargeTargetCurrent: mod.config.module.dc_discharge_target_current,
      DischargeMinimalSoC: mod.config.module.dc_discharge_v2g_minimal_soc,
    },
  };
}

boot_module(async ({
  setup, config, mqtt,
}) => {
  // Subscribe external cmds for nodered
  mqtt.subscribe(`everest_external/nodered/${config.module.connector_id}/carsim/cmd/enable`, (mod, en) => {
    enable(mod, { value: JSON.parse(en) });
  });
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
  setup.uses.ev_board_support.subscribe.bsp_event((mod, str) => {
    mod.actual_bsp_event = str;
    if (mod.actual_bsp_event === 'Disconnected' && mod.state !== 'unplugged') {
      // we were unplugged
      mod.executionActive = false;
      mod.state = 'unplugged';
    }
  });
  setup.uses.ev_board_support.subscribe.bsp_measurement((mod, args) => {
    mod.pp = args.proximity_pilot.ampacity;
    mod.rcd_current_mA = args.rcd_current_mA;
    mod.pwm_duty_cycle = args.cp_pwm_duty_cycle;
  });

  if (setup.uses_list.slac.length > 0) {
    setup.uses_list.slac[0].subscribe.state((mod, args) => { mod.slac_state = args; });
  }
  // ISO15118 ev setup
  if (setup.uses_list.ev.length > 0) {
    setup.uses_list.ev[0].subscribe.AC_EVPowerReady((mod, value) => { mod.iso_pwr_ready = value; });
    setup.uses_list.ev[0].subscribe.AC_EVSEMaxCurrent((mod, value) => { mod.evse_maxcurrent = value; });
    setup.uses_list.ev[0].subscribe.AC_StopFromCharger((mod) => { mod.iso_stopped = true; });
    setup.uses_list.ev[0].subscribe.V2G_Session_Finished((mod) => { mod.v2g_finished = true; });
    setup.uses_list.ev[0].subscribe.DC_PowerOn((mod) => { mod.dc_power_on = true; });
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

  if (globalconf.module.auto_enable) enable(mod, { value: true });
  if (globalconf.module.auto_exec) execute_charging_session(mod, { value: globalconf.module.auto_exec_commands });
});

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
