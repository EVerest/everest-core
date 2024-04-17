// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');

const STATE_DISABLED = 0;
const STATE_A = 1;
const STATE_B = 2;
const STATE_C = 3;
const STATE_D = 4;
const STATE_E = 5;
const STATE_F = 6;

const Event_PowerOn = 8;
const Event_PowerOff = 9;

let global_info;

let active_errors = {
  DiodeFault: false,
  BrownOut: false,
  EnergyManagement: false,
  PermanentFault: false,
  MREC2GroundFailure: false,
  MREC3HighTemperature: false,
  MREC4OverCurrentFailure: false,
  MREC5OverVoltage: false,
  MREC6UnderVoltage: false,
  MREC8EmergencyStop: false,
  MREC10InvalidVehicleMode: false,
  MREC14PilotFault: false,
  MREC15PowerLoss: false,
  MREC17EVSEContactorFault: false,
  MREC18CableOverTempDerate: false,
  MREC19CableOverTempStop: false,
  MREC20PartialInsertion: false,
  MREC23ProximityFault: false,
  MREC24ConnectorVoltageHigh: false,
  MREC25BrokenLatch: false,
  MREC26CutCable: false,

  ac_rcd_MREC2GroundFailure: false,
  ac_rcd_VendorError: false,
  ac_rcd_Selftest: false,
  ac_rcd_AC: false,
  ac_rcd_DC: false,

  lock_ConnectorLockCapNotCharged: false,
  lock_ConnectorLockUnexpectedOpen: false,
  lock_ConnectorLockUnexpectedClose: false,
  lock_ConnectorLockFailedLock: false,
  lock_ConnectorLockFailedUnlock: false,
  lock_MREC1ConnectorLockFailure: false,
  lock_VendorError: false,
};

function publish_ac_nr_of_phases_available(mod, n) {
  mod.provides.board_support.publish.ac_nr_of_phases_available(n);
}

function read_pp_ampacity(mod) {
  const { pp_resistor } = mod.simulation_data;
  if (pp_resistor < 80.0 || pp_resistor > 2460) {
    evlog.error(`PP resistor value '${pp_resistor}' Ohm seems to be outside the allowed range.`);
    return 'None';
  }

  // PP resistor value in spec, use a conservative interpretation of the resistance ranges
  if (pp_resistor > 936.0 && pp_resistor <= 2460.0) {
    return 'A_13';
  }
  if (pp_resistor > 308.0 && pp_resistor <= 936.0) {
    return 'A_20';
  }
  if (pp_resistor > 140.0 && pp_resistor <= 308.0) {
    return 'A_32';
  }
  if (pp_resistor > 80.0 && pp_resistor <= 140.0) {
    return 'A_63';
  }
  return 'None';
}

function stateToString(mod) {
  const pwm = (mod.pwm_running ? '2' : '1');
  switch (mod.state) {
    case STATE_DISABLED:
      return 'Disabled';
    case STATE_A:
      return `A${pwm}`;
    case STATE_B:
      return `B${pwm}`;
    case STATE_C:
      return `C${pwm}`;
    case STATE_D:
      return `D${pwm}`;
    case STATE_E:
      return 'E';
    case STATE_F:
      return 'F';
    default:
      return '';
  }
}

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
  mod.telemetry_data.power_path_controller.temperature_car_connector = mod.powermeter.tempL1 * 2.0;
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
  mod.telemetry_data.rcd.current_mA = mod.simulation_data.rcd_current;

  mod.telemetry.publish('livedata', 'power_path_controller', mod.telemetry_data.power_path_controller);
  mod.telemetry.publish('livedata', 'power_switch', mod.telemetry_data.power_switch);
  mod.telemetry.publish('livedata', 'rcd', mod.telemetry_data.rcd);
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
    default:
      evlog.error(`Invalid event: ${event}`);
      return 'invalid';
  }
}

function publish_event(mod, event) {
  mod.provides.board_support.publish.event({ event: event_to_enum(event) });
}

function check_error_rcd(mod) {
  if (mod.simulation_data.rcd_current > 5.0) {
    if (!mod.rcd_error_reported) {
      mod.provides.rcd.raise.ac_rcd_DC('Simulated fault event', 'High');
      mod.rcd_error_reported = true;
    }
  } else {
    mod.rcd_error_reported = false;
  }
  mod.provides.rcd.publish.rcd_current_mA = mod.simulation_data.rcd_current;
}

function pwmOff(mod) {
  mod.pwm_duty_cycle = 1.0;
  mod.pwm_running = false;
  mod.pwm_error_f = false;
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

function pwmF(mod) {
  mod.pwm_duty_cycle = 1.0;
  mod.pwm_running = false;
  mod.pwm_error_f = true;
}

function powerOn(mod) {
  if (!mod.relais_on) {
    publish_event(mod, Event_PowerOn);
    mod.relais_on = true;
    mod.telemetry_data.power_switch.switching_count += 1;
  }
}

function powerOff(mod) {
  if (mod.relais_on) {
    publish_event(mod, Event_PowerOff);
    mod.telemetry_data.power_switch.switching_count += 1;
    mod.relais_on = false;
  }
}

function drawPower(mod, l1, l2, l3, n) {
  mod.simdata_setting.currents.L1 = l1;
  mod.simdata_setting.currents.L2 = l2;
  mod.simdata_setting.currents.L3 = l3;
  mod.simdata_setting.currents.N = n;
}

function addNoise(mod) {
  const noise = (1 + (Math.random() - 0.5) * 0.02);
  const lonoise = (1 + (Math.random() - 0.5) * 0.005);
  const impedance = mod.simdata_setting.impedance / 1000.0;

  mod.simulation_data.currents.L1 = mod.simdata_setting.currents.L1 * noise;
  mod.simulation_data.currents.L2 = mod.simdata_setting.currents.L2 * noise;
  mod.simulation_data.currents.L3 = mod.simdata_setting.currents.L3 * noise;
  mod.simulation_data.currents.N = mod.simdata_setting.currents.N * noise;

  mod.simulation_data.voltages.L1 = mod.simdata_setting.voltages.L1 * noise - impedance * mod.simulation_data.currents.L1;
  mod.simulation_data.voltages.L2 = mod.simdata_setting.voltages.L2 * noise - impedance * mod.simulation_data.currents.L2;
  mod.simulation_data.voltages.L3 = mod.simdata_setting.voltages.L3 * noise - impedance * mod.simulation_data.currents.L3;

  mod.simulation_data.frequencies.L1 = mod.simdata_setting.frequencies.L1 * lonoise;
  mod.simulation_data.frequencies.L2 = mod.simdata_setting.frequencies.L2 * lonoise;
  mod.simulation_data.frequencies.L3 = mod.simdata_setting.frequencies.L3 * lonoise;

  mod.simulation_data.cp_voltage = mod.simdata_setting.cp_voltage * noise;
  mod.simulation_data.rcd_current = mod.simdata_setting.rcd_current * noise;
  mod.simulation_data.pp_resistor = mod.simdata_setting.pp_resistor * noise;

  mod.simulation_data.diode_fail = mod.simdata_setting.diode_fail;
  mod.simulation_data.error_e = mod.simdata_setting.error_e;
}

// checks if voltage is within center+-interval
function is_voltage_in_range(voltage, center) {
  const interval = 1.1;
  return ((voltage > center - interval) && (voltage < center + interval));
}

// IEC61851 Table A.8
function dutyCycleToAmps(dc) {
  if (dc < 8.0 / 100.0) return 0;
  if (dc < 85.0 / 100.0) return dc * 100.0 * 0.6;
  if (dc < 96.0 / 100.0) return (dc * 100.0 - 64) * 2.5;
  if (dc < 97.0 / 100.0) return 80;
  return 0;
}

function error_BrownOut(mod, raise) {
  if (!active_errors.BrownOut) {
    mod.provides.board_support.raise.evse_board_support_BrownOut('Simulated fault event', 'High');
    active_errors.BrownOut = true;
  } else if (!raise && active_errors.BrownOut) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_BrownOut();
    active_errors.BrownOut = false;
  }
}

function error_EnergyManagement(mod, raise) {
  if (!active_errors.EnergyManagement) {
    mod.provides.board_support.raise.evse_board_support_EnergyManagement('Simulated fault event', 'High');
    active_errors.EnergyManagement = true;
  } else if (!raise && active_errors.EnergyManagement) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_EnergyManagement();
    active_errors.EnergyManagement = false;
  }
}

function error_PermanentFault(mod, raise) {
  if (!active_errors.PermanentFault) {
    mod.provides.board_support.raise.evse_board_support_PermanentFault('Simulated fault event', 'High');
    active_errors.PermanentFault = true;
  } else if (!raise && active_errors.PermanentFault) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_PermanentFault();
    active_errors.PermanentFault = false;
  }
}

function error_MREC2GroundFailure(mod, raise) {
  if (!active_errors.MREC2GroundFailure) {
    mod.provides.board_support.raise.evse_board_support_MREC2GroundFailure('Simulated fault event', 'High');
    active_errors.MREC2GroundFailure = true;
  } else if (!raise && active_errors.MREC2GroundFailure) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC2GroundFailure();
    active_errors.MREC2GroundFailure = false;
  }
}

function error_MREC3HighTemperature(mod, raise) {
  if (!active_errors.MREC3HighTemperature) {
    mod.provides.board_support.raise.evse_board_support_MREC3HighTemperature('Simulated fault event', 'High');
    active_errors.MREC3HighTemperature = true;
  } else if (!raise && active_errors.MREC3HighTemperature) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC3HighTemperature();
    active_errors.MREC3HighTemperature = false;
  }
}

function error_MREC4OverCurrentFailure(mod, raise) {
  if (!active_errors.MREC4OverCurrentFailure) {
    mod.provides.board_support.raise.evse_board_support_MREC4OverCurrentFailure('Simulated fault event', 'High');
    active_errors.MREC4OverCurrentFailure = true;
  } else if (!raise && active_errors.MREC4OverCurrentFailure) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC4OverCurrentFailure();
    active_errors.MREC4OverCurrentFailure = false;
  }
}

function error_MREC5OverVoltage(mod, raise) {
  if (!active_errors.MREC5OverVoltage) {
    mod.provides.board_support.raise.evse_board_support_MREC5OverVoltage('Simulated fault event', 'High');
    active_errors.MREC5OverVoltage = true;
  } else if (!raise && active_errors.MREC5OverVoltage) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC5OverVoltage();
    active_errors.MREC5OverVoltage = false;
  }
}

function error_MREC6UnderVoltage(mod, raise) {
  if (!active_errors.MREC6UnderVoltage) {
    mod.provides.board_support.raise.evse_board_support_MREC6UnderVoltage('Simulated fault event', 'High');
    active_errors.MREC6UnderVoltage = true;
  } else if (!raise && active_errors.MREC6UnderVoltage) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC6UnderVoltage();
    active_errors.MREC6UnderVoltage = false;
  }
}

function error_MREC8EmergencyStop(mod, raise) {
  if (!active_errors.MREC8EmergencyStop) {
    mod.provides.board_support.raise.evse_board_support_MREC8EmergencyStop('Simulated fault event', 'High');
    active_errors.MREC8EmergencyStop = true;
  } else if (!raise && active_errors.MREC8EmergencyStop) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC8EmergencyStop();
    active_errors.MREC8EmergencyStop = false;
  }
}

function error_MREC10InvalidVehicleMode(mod, raise) {
  if (!active_errors.MREC10InvalidVehicleMode) {
    mod.provides.board_support.raise.evse_board_support_MREC10InvalidVehicleMode('Simulated fault event', 'High');
    active_errors.MREC10InvalidVehicleMode = true;
  } else if (!raise && active_errors.MREC10InvalidVehicleMode) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC10InvalidVehicleMode();
    active_errors.MREC10InvalidVehicleMode = false;
  }
}

function error_MREC14PilotFault(mod, raise) {
  if (!active_errors.MREC14PilotFault) {
    mod.provides.board_support.raise.evse_board_support_MREC14PilotFault('Simulated fault event', 'High');
    active_errors.MREC14PilotFault = true;
  } else if (!raise && active_errors.MREC14PilotFault) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC14PilotFault();
    active_errors.MREC14PilotFault = false;
  }
}

function error_MREC15PowerLoss(mod, raise) {
  if (!active_errors.MREC15PowerLoss) {
    mod.provides.board_support.raise.evse_board_support_MREC15PowerLoss('Simulated fault event', 'High');
    active_errors.MREC15PowerLoss = true;
  } else if (!raise && active_errors.MREC15PowerLoss) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC15PowerLoss();
    active_errors.MREC15PowerLoss = false;
  }
}

function error_MREC17EVSEContactorFault(mod, raise) {
  if (!active_errors.MREC17EVSEContactorFault) {
    mod.provides.board_support.raise.evse_board_support_MREC17EVSEContactorFault('Simulated fault event', 'High');
    active_errors.MREC17EVSEContactorFault = true;
  } else if (!raise && active_errors.MREC17EVSEContactorFault) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC17EVSEContactorFault();
    active_errors.MREC17EVSEContactorFault = false;
  }
}

function error_MREC18CableOverTempDerate(mod, raise) {
  if (!active_errors.MREC18CableOverTempDerate) {
    mod.provides.board_support.raise.evse_board_support_MREC18CableOverTempDerate('Simulated fault event', 'High');
    active_errors.MREC18CableOverTempDerate = true;
  } else if (!raise && active_errors.MREC18CableOverTempDerate) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC18CableOverTempDerate();
    active_errors.MREC18CableOverTempDerate = false;
  }
}

function error_MREC19CableOverTempStop(mod, raise) {
  if (!active_errors.MREC19CableOverTempStop) {
    mod.provides.board_support.raise.evse_board_support_MREC19CableOverTempStop('Simulated fault event', 'High');
    active_errors.MREC19CableOverTempStop = true;
  } else if (!raise && active_errors.MREC19CableOverTempStop) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC19CableOverTempStop();
    active_errors.MREC19CableOverTempStop = false;
  }
}

function error_MREC20PartialInsertion(mod, raise) {
  if (!active_errors.MREC20PartialInsertion) {
    mod.provides.board_support.raise.evse_board_support_MREC20PartialInsertion('Simulated fault event', 'High');
    active_errors.MREC20PartialInsertion = true;
  } else if (!raise && active_errors.MREC20PartialInsertion) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC20PartialInsertion();
    active_errors.MREC20PartialInsertion = false;
  }
}

function error_MREC23ProximityFault(mod, raise) {
  if (!active_errors.MREC23ProximityFault) {
    mod.provides.board_support.raise.evse_board_support_MREC23ProximityFault('Simulated fault event', 'High');
    active_errors.MREC23ProximityFault = true;
  } else if (!raise && active_errors.MREC23ProximityFault) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC23ProximityFault();
    active_errors.MREC23ProximityFault = false;
  }
}

function error_MREC24ConnectorVoltageHigh(mod, raise) {
  if (!active_errors.MREC24ConnectorVoltageHigh) {
    mod.provides.board_support.raise.evse_board_support_MREC24ConnectorVoltageHigh('Simulated fault event', 'High');
    active_errors.MREC24ConnectorVoltageHigh = true;
  } else if (!raise && active_errors.MREC24ConnectorVoltageHigh) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC24ConnectorVoltageHigh();
    active_errors.MREC24ConnectorVoltageHigh = false;
  }
}

function error_MREC25BrokenLatch(mod, raise) {
  if (!active_errors.MREC25BrokenLatch) {
    mod.provides.board_support.raise.evse_board_support_MREC25BrokenLatch('Simulated fault event', 'High');
    active_errors.MREC25BrokenLatch = true;
  } else if (!raise && active_errors.MREC25BrokenLatch) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC25BrokenLatch();
    active_errors.MREC25BrokenLatch = false;
  }
}

function error_MREC26CutCable(mod, raise) {
  if (raise && !active_errors.MREC26CutCable) {
    mod.provides.board_support.raise.evse_board_support_MREC26CutCable('Simulated fault event', 'High');
    active_errors.MREC26CutCable = true;
  } else if (!raise && active_errors.MREC26CutCable) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_MREC26CutCable('Simulated fault event', 'High');
    active_errors.MREC26CutCable = false;
  }
}

function error_DiodeFault(mod, raise) {
  if (raise && !active_errors.DiodeFault) {
    mod.provides.board_support.raise.evse_board_support_DiodeFault('Simulated fault event', 'High');
    active_errors.DiodeFault = true;
  } else if (!raise && active_errors.DiodeFault) {
    mod.provides.board_support.request_clear_all_of_type.evse_board_support_DiodeFault();
    active_errors.DiodeFault = false;
  }
}

function error_ac_rcd_MREC2GroundFailure(mod, raise) {
  if (raise && !active_errors.ac_rcd_MREC2GroundFailure) {
    mod.provides.rcd.raise.ac_rcd_MREC2GroundFailure('Simulated fault event', 'High');
    active_errors.ac_rcd_MREC2GroundFailure = true;
  } else if (!raise && active_errors.ac_rcd_MREC2GroundFailure) {
    mod.provides.rcd.request_clear_all_of_type.ac_rcd_MREC2GroundFailure();
    active_errors.ac_rcd_MREC2GroundFailure = false;
  }
}

function error_ac_rcd_VendorError(mod, raise) {
  if (raise && !active_errors.ac_rcd_VendorError) {
    mod.provides.rcd.raise.ac_rcd_VendorError('Simulated fault event', 'High');
    active_errors.ac_rcd_VendorError = true;
  } else if (!raise && active_errors.ac_rcd_VendorError) {
    mod.provides.rcd.request_clear_all_of_type.ac_rcd_VendorError();
    active_errors.ac_rcd_VendorError = false;
  }
}

function error_ac_rcd_Selftest(mod, raise) {
  if (raise && !active_errors.ac_rcd_Selftest) {
    mod.provides.rcd.raise.ac_rcd_Selftest('Simulated fault event', 'High');
    active_errors.ac_rcd_Selftest = true;
  } else if (!raise && active_errors.ac_rcd_Selftest) {
    mod.provides.rcd.request_clear_all_of_type.ac_rcd_Selftest();
    active_errors.ac_rcd_Selftest = false;
  }
}

function error_ac_rcd_AC(mod, raise) {
  if (raise && !active_errors.ac_rcd_AC) {
    mod.provides.rcd.raise.ac_rcd_AC('Simulated fault event', 'High');
    active_errors.ac_rcd_AC = true;
  } else if (!raise && active_errors.ac_rcd_AC) {
    mod.provides.rcd.request_clear_all_of_type.ac_rcd_AC();
    active_errors.ac_rcd_AC = false;
  }
}

function error_ac_rcd_DC(mod, raise) {
  if (raise && !active_errors.ac_rcd_DC) {
    mod.provides.rcd.raise.ac_rcd_DC('Simulated fault event', 'High');
    active_errors.ac_rcd_DC = true;
  } else if (!raise && active_errors.ac_rcd_DC) {
    mod.provides.rcd.request_clear_all_of_type.ac_rcd_DC();
    active_errors.ac_rcd_DC = false;
  }
}

function error_lock_ConnectorLockCapNotCharged(mod, raise) {
  if (raise && !active_errors.lock_ConnectorLockCapNotCharged) {
    mod.provides.connector_lock.raise.connector_lock_ConnectorLockCapNotCharged('Simulated fault event', 'High');
    active_errors.lock_ConnectorLockCapNotCharged = true;
  } else if (!raise && active_errors.lock_ConnectorLockCapNotCharged) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_ConnectorLockCapNotCharged();
    active_errors.lock_ConnectorLockCapNotCharged = false;
  }
}

function error_lock_ConnectorLockUnexpectedOpen(mod, raise) {
  if (raise && !active_errors.lock_ConnectorLockUnexpectedOpen) {
    mod.provides.connector_lock.raise.connector_lock_ConnectorLockUnexpectedOpen('Simulated fault event', 'High');
    active_errors.lock_ConnectorLockUnexpectedOpen = true;
  } else if (!raise && active_errors.lock_ConnectorLockUnexpectedOpen) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_ConnectorLockUnexpectedOpen();
    active_errors.lock_ConnectorLockUnexpectedOpen = false;
  }
}

function error_lock_ConnectorLockUnexpectedClose(mod, raise) {
  if (raise && !active_errors.lock_ConnectorLockUnexpectedClose) {
    mod.provides.connector_lock.raise.connector_lock_ConnectorLockUnexpectedClose('Simulated fault event', 'High');
    active_errors.lock_ConnectorLockUnexpectedClose = true;
  } else if (!raise && active_errors.lock_ConnectorLockUnexpectedClose) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_ConnectorLockUnexpectedClose();
    active_errors.lock_ConnectorLockUnexpectedClose = false;
  }
}

function error_lock_ConnectorLockFailedLock(mod, raise) {
  if (raise && !active_errors.lock_ConnectorLockFailedLock) {
    mod.provides.connector_lock.raise.connector_lock_ConnectorLockFailedLock('Simulated fault event', 'High');
    active_errors.lock_ConnectorLockFailedLock = true;
  } else if (!raise && active_errors.ConnectorLockFailedLock) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_ConnectorLockFailedLock();
    active_errors.lock_ConnectorLockFailedLock = false;
  }
}

function error_lock_ConnectorLockFailedUnlock(mod, raise) {
  if (raise && !active_errors.lock_ConnectorLockFailedUnlock) {
    mod.provides.connector_lock.raise.connector_lock_ConnectorLockFailedUnlock('Simulated fault event', 'High');
    active_errors.lock_ConnectorLockFailedUnlock = true;
  } else if (!raise && active_errors.lock_ConnectorLockFailedUnlock) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_ConnectorLockFailedUnlock();
    active_errors.lock_ConnectorLockFailedUnlock = false;
  }
}

function error_lock_MREC1ConnectorLockFailure(mod, raise) {
  if (raise && !active_errors.lock_MREC1ConnectorLockFailure) {
    mod.provides.connector_lock.raise.connector_lock_MREC1ConnectorLockFailure('Simulated fault event', 'High');
    active_errors.lock_MREC1ConnectorLockFailure = true;
  } else if (!raise && active_errors.lock_MREC1ConnectorLockFailure) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_MREC1ConnectorLockFailure();
    active_errors.lock_MREC1ConnectorLockFailure = false;
  }
}

function error_lock_VendorError(mod, raise) {
  if (raise && !active_errors.lock_VendorError) {
    mod.provides.connector_lock.raise.connector_lock_VendorError('Simulated fault event', 'High');
    active_errors.lock_VendorError = true;
  } else if (!raise && active_errors.lock_VendorError) {
    mod.provides.connector_lock.request_clear_all_of_type.connector_lock_VendorError();
    active_errors.lock_VendorError = false;
  }
}

// Example of automatically reset errors up on disconnection of the vehicle.
// All other errors need to be cleared explicitly.
// Note that in real life the clearing of errors may differ between BSPs depending on the
// hardware implementation.
function clear_disconnect_errors(mod) {
  if (active_errors.DiodeFault) {
    error_DiodeFault(mod, false);
  }
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
      timestamp: '',
      type: 'power_path_controller_version',
      hardware_version: 3,
      software_version: '1.01',
      date_manufactured: '20220304',
      operating_time_h: 2330,
      operating_time_h_warning: 5000,
      operating_time_h_error: 6000,
      error: false,
    },

    power_path_controller: {
      timestamp: '',
      type: 'power_path_controller',
      cp_voltage_high: 0.0,
      cp_voltage_low: 0.0,
      cp_pwm_duty_cycle: 0.0,
      cp_state: 'A1',
      pp_ohm: 220.1,
      supply_voltage_12V: 12.1,
      supply_voltage_minus_12V: -11.9,
      temperature_controller: 33,
      temperature_car_connector: 65,
      watchdog_reset_count: 1,
      error: false,
    },

    power_switch: {
      timestamp: '',
      type: 'power_switch',
      switching_count: 0,
      switching_count_warning: 30000,
      switching_count_error: 50000,
      is_on: false,
      time_to_switch_on_ms: 110,
      time_to_switch_off_ms: 100,
      temperature_C: 20,
      error: false,
      error_over_current: false,
    },

    rcd: {
      timestamp: '',
      type: 'rcd',
      enabled: true,
      current_mA: 2.5,
      triggered: false,
      error: false,
    },
  };
  mod.ev_max_current = 0.0;
  mod.ev_three_phases = 3;
}

function reset_powermeter(mod) {
  mod.wattHr = {
    L1: 0.0,
    L2: 0.0,
    L3: 0.0,
  };
  mod.powermeter_sim_last_time_stamp = 0;
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

function publish_ev_board_support(mod) {
  const pp = { ampacity: read_pp_ampacity(mod) };

  mod.provides.ev_board_support.publish.bsp_measurement({
    cp_pwm_duty_cycle: mod.pwm_duty_cycle * 100.0,
    rcd_current_mA: mod.simulation_data.rcd_current,
    proximity_pilot: pp,
  });
}

function simulate_powermeter(mod) {
  const time_stamp = new Date().getTime();
  if (mod.powermeter_sim_last_time_stamp === 0) mod.powermeter_sim_last_time_stamp = time_stamp;
  const deltat = time_stamp - mod.powermeter_sim_last_time_stamp;
  mod.powermeter_sim_last_time_stamp = time_stamp;

  const wattL1 = mod.simulation_data.voltages.L1 * mod.simulation_data.currents.L1 * (mod.relais_on ? 1 : 0);
  const wattL2 = mod.simulation_data.voltages.L2 * mod.simulation_data.currents.L2
    * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);
  const wattL3 = mod.simulation_data.voltages.L3 * mod.simulation_data.currents.L3
    * (mod.relais_on && mod.use_three_phases_confirmed ? 1 : 0);

  mod.wattHr.L1 += (wattL1 * deltat) / 1000.0 / 3600.0;
  mod.wattHr.L2 += (wattL2 * deltat) / 1000.0 / 3600.0;
  mod.wattHr.L3 += (wattL3 * deltat) / 1000.0 / 3600.0;

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

// Translate ADC readings for lo and hi part of PWM to IEC61851 states.
function read_from_car(mod) {
  let amps1 = 0.0;
  let amps2 = 0.0;
  let amps3 = 0.0;

  let amps = dutyCycleToAmps(mod.pwm_duty_cycle);
  if (amps > mod.ev_max_current) amps = mod.ev_max_current;

  if (mod.relais_on === true && mod.ev_three_phases > 0) amps1 = amps;
  else amps1 = 0;
  if (mod.relais_on === true && mod.ev_three_phases > 1) amps2 = amps;
  else amps2 = 0;
  if (mod.relais_on === true && mod.ev_three_phases > 2) amps3 = amps;
  else amps3 = 0;

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
    if (is_voltage_in_range(cpLo, 0.0) && is_voltage_in_range(cpHi, 0.0)) {
      mod.current_state = STATE_E;
      drawPower(mod, 0, 0, 0, 0);
    } else if (is_voltage_in_range(cpHi + cpLo, 0.0)) { // Diode fault
      error_DiodeFault(mod, true);
      drawPower(mod, 0, 0, 0, 0);
    }
  } else if (is_voltage_in_range(cpHi, 12.0)) {
    // +12V State A IDLE (open circuit)
    // clear all errors that clear on disconnection
    clear_disconnect_errors(mod);
    mod.current_state = STATE_A;
    drawPower(mod, 0, 0, 0, 0);
  } else if (is_voltage_in_range(cpHi, 9.0)) {
    mod.current_state = STATE_B;
    drawPower(mod, 0, 0, 0, 0);
  } else if (is_voltage_in_range(cpHi, 6.0)) {
    mod.current_state = STATE_C;
    drawPower(mod, amps1, amps2, amps3, 0.2);
  } else if (is_voltage_in_range(cpHi, 3.0)) {
    mod.current_state = STATE_D;
    drawPower(mod, amps1, amps2, amps3, 0.2);
  } else if (is_voltage_in_range(cpHi, -12.0)) {
    mod.current_state = STATE_F;
    drawPower(mod, 0, 0, 0, 0);
  }
}

function enable_simulation(mod, args) {
  if (mod.simulation_enabled && !args.value) {
    publish_event(mod, STATE_A);
    clearData(mod);
  }
  mod.simulation_enabled = args.value;
}

// state machine for the evse
function simulation_statemachine(mod) {
  if (mod.last_state !== mod.current_state) {
    publish_event(mod, mod.current_state);
  }

  switch (mod.current_state) {
    case STATE_DISABLED:
      powerOff();
      mod.power_on_allowed = false;
      break;

    case STATE_A:
      mod.use_three_phases_confirmed = mod.use_three_phases;
      pwmOff(mod);
      reset_powermeter(mod);
      mod.simplified_mode = false;

      if (mod.last_state !== STATE_A && mod.last_state !== STATE_DISABLED
        && mod.last_state !== STATE_F) {
        powerOff(mod);

        // If car was unplugged, reset RCD flag.
        mod.simdata_setting.rcd_current = 0.1;
        mod.rcd_error = false;
      }
      break;
    case STATE_B:
      // Table A.6: Sequence 7 EV stops charging
      // Table A.6: Sequence 8.2 EV supply equipment
      // responds to EV opens S2 (w/o PWM)

      if (mod.last_state !== STATE_A && mod.last_state !== STATE_B) {
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
      } else if (mod.power_on_allowed) { // C2
        // Table A.6: Sequence 4 EV ready to charge.
        // Must enable power within 3 seconds.
        powerOn(mod);
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
      } else if (mod.power_on_allowed && !mod.relais_on && mod.has_ventilation) {
        // Table A.6: Sequence 4 EV ready to charge.
        // Must enable power within 3 seconds.
        powerOn(mod);
      }
      break;
    case STATE_E:
      powerOff(mod);
      pwmOff(mod);
      break;
    case STATE_F:
      powerOff(mod);
      break;
    default:
      break;
  }
  mod.last_state = mod.current_state;
  mod.last_pwm_running = mod.pwm_running;
}

function simulation_loop(mod) {
  if (mod.simulation_enabled) {
    check_error_rcd(mod);
    read_from_car(mod);
    simulation_statemachine(mod);
    addNoise(mod);
    simulate_powermeter(mod);
    publish_ev_board_support(mod);
  }

  // console.error(mod);
  mod.pubCnt += 1;
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
    case 4:
    default:
      mod.pubCnt = 0;
      break;
  }
}

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  global_info = info;
  // register commands
  setup.provides.powermeter.register.stop_transaction(() => ({
    status: 'NOT_SUPPORTED',
    error: 'YetiDriver does not support stop transaction request.',
  }));
  setup.provides.powermeter.register.start_transaction(() => ({ status: 'OK' }));

  setup.provides.board_support.register.setup((mod, args) => {
    mod.three_phases = args.three_phases;
    mod.has_ventilation = args.has_ventilation;
    mod.country_code = args.country_code;
    publish_ac_nr_of_phases_available(mod, (mod.use_three_phases_confirmed ? 3 : 1));
  });

  setup.provides.board_support.register.ac_set_overcurrent_limit_A(() => {
  });

  setup.provides.board_support.register.enable((mod, args) => {
    if (args.value) {
      if (mod.current_state === STATE_DISABLED) mod.state = STATE_A;
    } else mod.current_state = STATE_DISABLED;
  });

  setup.provides.board_support.register.pwm_on((mod, args) => { pwmOn(mod, args.value / 100.0); });
  setup.provides.board_support.register.pwm_off((mod) => { pwmOff(mod); });
  setup.provides.board_support.register.pwm_F((mod) => { pwmF(mod); });
  setup.provides.board_support.register.evse_replug(() => {
    evlog.error('Replugging not supported');
  });

  setup.provides.board_support.register.allow_power_on((mod, args) => {
    mod.power_on_allowed = args.value.allow_power_on;
  });

  setup.provides.board_support.register.ac_switch_three_phases_while_charging((mod, args) => {
    mod.use_three_phases = args.value;
    mod.use_three_phases_confirmed = args.value;
  });
  setup.provides.board_support.register.get_hw_capabilities(() => ({
    max_current_A_import: 32.0,
    min_current_A_import: 6.0,
    max_phase_count_import: 3,
    min_phase_count_import: 1,
    max_current_A_export: 16.0,
    min_current_A_export: 0.0,
    max_phase_count_export: 3,
    min_phase_count_export: 1,
    supports_changing_phases_during_charging: true,
    connector_type: 'IEC62196Type2Cable',
  }));
  setup.provides.board_support.register.ac_read_pp_ampacity((mod) => {
    const amp = { ampacity: read_pp_ampacity(mod) };
    return amp;
  });
  setup.provides.connector_lock.register.lock(() => {
    evlog.info('Lock connector');
  });
  setup.provides.connector_lock.register.unlock(() => {
    evlog.info('Unlock connector');
  });

  // bsp ev
  setup.provides.ev_board_support.register.enable(enable_simulation);

  setup.provides.ev_board_support.register.set_cp_state((mod, args) => {
    switch (args.cp_state) {
      case 'A':
        mod.simdata_setting.cp_voltage = 12.0;
        break;
      case 'B':
        mod.simdata_setting.cp_voltage = 9.0;
        break;
      case 'C':
        mod.simdata_setting.cp_voltage = 6.0;
        break;
      case 'D':
        mod.simdata_setting.cp_voltage = 3.0;
        break;
      case 'E':
        mod.simdata_setting.error_e = true;
        break;
      default:
        break;
    }
  });

  setup.provides.ev_board_support.register.diode_fail((mod, args) => {
    mod.simdata_setting.diode_fail = args.value;
  });

  // Right now the YetiSimulator have no option to control the dc powermeter.
  setup.provides.ev_board_support.register.allow_power_on((mod, args) => {
    evlog.debug(`EV Power On: ${args.value}`);
  });

  setup.provides.ev_board_support.register.set_ac_max_current((mod, args) => {
    mod.ev_max_current = args.current;
  });

  setup.provides.ev_board_support.register.set_three_phases((mod, args) => {
    if (args.three_phases) mod.ev_three_phases = 3.0;
    else mod.ev_three_phases = 1.0;
  });

  setup.provides.ev_board_support.register.set_rcd_error((mod, args) => {
    mod.simdata_setting.rcd_current = args.rcd_current_mA;
  });

  // Subscribe to nodered error injection
  mqtt.subscribe(`everest_external/nodered/${config.module.connector_id}/carsim/error`, (mod, en) => {
    const e = JSON.parse(en);
    const raise = e.raise === 'true';

    switch (e.error_type) {
      case 'DiodeFault':
        error_DiodeFault(mod, raise);
        break;
      case 'BrownOut':
        error_BrownOut(mod, raise);
        break;
      case 'EnergyManagement':
        error_EnergyManagement(mod, raise);
        break;
      case 'PermanentFault':
        error_PermanentFault(mod, raise);
        break;
      case 'MREC2GroundFailure':
        error_MREC2GroundFailure(mod, raise);
        break;
      case 'MREC3HighTemperature':
        error_MREC3HighTemperature(mod, raise);
        break;
      case 'MREC4OverCurrentFailure':
        error_MREC4OverCurrentFailure(mod, raise);
        break;
      case 'MREC5OverVoltage':
        error_MREC5OverVoltage(mod, raise);
        break;
      case 'MREC6UnderVoltage':
        error_MREC6UnderVoltage(mod, raise);
        break;
      case 'MREC8EmergencyStop':
        error_MREC8EmergencyStop(mod, raise);
        break;
      case 'MREC10InvalidVehicleMode':
        error_MREC10InvalidVehicleMode(mod, raise);
        break;
      case 'MREC14PilotFault':
        error_MREC14PilotFault(mod, raise);
        break;
      case 'MREC15PowerLoss':
        error_MREC15PowerLoss(mod, raise);
        break;
      case 'MREC17EVSEContactorFault':
        error_MREC17EVSEContactorFault(mod, raise);
        break;
      case 'MREC18CableOverTempDerate':
        error_MREC18CableOverTempDerate(mod, raise);
        break;
      case 'MREC19CableOverTempStop':
        error_MREC19CableOverTempStop(mod, raise);
        break;
      case 'MREC20PartialInsertion':
        error_MREC20PartialInsertion(mod, raise);
        break;
      case 'MREC23ProximityFault':
        error_MREC23ProximityFault(mod, raise);
        break;
      case 'MREC24ConnectorVoltageHigh':
        error_MREC24ConnectorVoltageHigh(mod, raise);
        break;
      case 'MREC25BrokenLatch':
        error_MREC25BrokenLatch(mod, raise);
        break;
      case 'MREC26CutCable':
        error_MREC26CutCable(mod, raise);
        break;
      case 'ac_rcd_MREC2GroundFailure':
        error_ac_rcd_MREC2GroundFailure(mod, raise);
        break;
      case 'ac_rcd_VendorError':
        error_ac_rcd_VendorError(mod, raise);
        break;
      case 'ac_rcd_Selftest':
        error_ac_rcd_Selftest(mod, raise);
        break;
      case 'ac_rcd_AC':
        error_ac_rcd_AC(mod, raise);
        break;
      case 'ac_rcd_DC':
        error_ac_rcd_DC(mod, raise);
        break;
      case 'lock_ConnectorLockCapNotCharged':
        error_lock_ConnectorLockCapNotCharged(mod, raise);
        break;
      case 'lock_ConnectorLockUnexpectedOpen':
        error_lock_ConnectorLockUnexpectedOpen(mod, raise);
        break;
      case 'lock_ConnectorLockUnexpectedClose':
        error_lock_ConnectorLockUnexpectedClose(mod, raise);
        break;
      case 'lock_ConnectorLockFailedLock':
        error_lock_ConnectorLockFailedLock(mod, raise);
        break;
      case 'lock_ConnectorLockFailedUnlock':
        error_lock_ConnectorLockFailedUnlock(mod, raise);
        break;
      case 'lock_MREC1ConnectorLockFailure':
        error_lock_MREC1ConnectorLockFailure(mod, raise);
        break;
      case 'lock_VendorError':
        error_lock_VendorError(mod, raise);
        break;
      default:
        evlog.error('Unknown error raised via MQTT');
    }
  });
}).then((mod) => {
  mod.pubCnt = 0;
  clearData(mod);
  setInterval(simulation_loop, 250, mod);
  if (global_info.telemetry_enabled) {
    setInterval(telemetry_slow, 15000, mod);
    setInterval(telemetry_fast, 1000, mod);
  }
});
