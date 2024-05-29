// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { boot_module } = require('everestjs');
const { setInterval } = require('timers');

let config_min_voltage;
let config_max_voltage;
let config_min_current;
let config_max_current;
let config_bidirectional;
let config_max_power;

let settings_connector_export_voltage = 0;
let settings_connector_import_voltage = 0;
let settings_connector_max_export_current = 0;
let settings_connector_max_import_current = 0;

let connector_voltage = 0.0;
let connector_current = 0.0;
let mode;
let energy_import_total = 0.0;
let energy_export_total = 0.0;

let voltage;
let current;


function power_meter_external() {
  const date = new Date();
  return ({
    timestamp: date.toISOString(),
    meter_id: 'DC_POWERMETER',
    energy_Wh_import: {
      total: energy_import_total,
    },
    energy_Wh_export: {
      total: energy_export_total,
    },
    power_W: {
      total: connector_voltage * connector_current,
    },
    voltage_V: {
      DC: connector_voltage,
    },
    current_A: {
      DC: connector_current,
    }
  });
}

boot_module(async ({
  setup, config,
}) => {
  config_max_voltage = config.impl.main.max_voltage;
  config_min_voltage = config.impl.main.min_voltage;
  config_max_current = config.impl.main.max_current;
  config_min_current = config.impl.main.min_current;
  config_bidirectional = config.impl.main.bidirectional;
  config_max_power = config.impl.main.max_power;

  connector_voltage = 0.0;
  connector_current = 0.0;
  mode = 'Off';
  voltage = 0.0;
  current = 0.0;

  setup.provides.main.register.setMode((mod, args) => {
    if (args.value === 'Off') {
      mode = 'Off'; connector_voltage = 0.0;
      connector_current = 0.0;
    } else if (args.value === 'Export') {
      mode = 'Export'; connector_voltage = settings_connector_export_voltage;
      connector_current = settings_connector_max_export_current;
    } else if (args.value === 'Import') {
      mode = 'Import'; connector_voltage = settings_connector_import_voltage;
      connector_current = -settings_connector_max_import_current;
    } else if (args.value === 'Fault') {
      mode = 'Fault'; connector_voltage = 0.0;
      connector_current = 0.0;
    }
    mod.provides.main.publish.mode(args.value);
  });

  setup.provides.main.register.setExportVoltageCurrent((mod, args) => {
    voltage = args.voltage;
    current = args.current;

    if (voltage < config_min_voltage) voltage = config_min_voltage;
    if (current < config_min_current) current = config_min_current;
    if (voltage > config_max_voltage) voltage = config_max_voltage;
    if (current > config_max_current) current = config_max_current;

    settings_connector_max_export_current = current;
    settings_connector_export_voltage = voltage;

    if (mode === 'Export') {
      connector_voltage = settings_connector_export_voltage;
      connector_current = settings_connector_max_export_current;
    }
  });

  setup.provides.main.register.setImportVoltageCurrent((mod, args) => {
    voltage = args.voltage;
    current = args.current;

    if (voltage < config_min_voltage) voltage = config_min_voltage;
    if (current < config_min_current) current = config_min_current;
    if (voltage > config_max_voltage) voltage = config_max_voltage;
    if (current > config_max_current) current = config_max_current;

    settings_connector_import_voltage = voltage;
    settings_connector_max_import_current = current;

    if (mode === 'Import') {
      connector_voltage = settings_connector_import_voltage;
      connector_current = -settings_connector_max_import_current;
    }
  });

  setup.provides.powermeter.register.stop_transaction((mod, args) => ({
    status: 'NOT_SUPPORTED',
    error: 'DcSupplySimulator does not support stop transaction request.',
  }));

  setup.provides.powermeter.register.start_transaction((mod, args) => ({
    status: 'OK',
  }));
}).then((mod) => {

  const Capabilities = {
    bidirectional: config_bidirectional,
    max_export_voltage_V: config_max_voltage,
    min_export_voltage_V: config_min_voltage,
    max_export_current_A: config_max_current,
    min_export_current_A: config_min_current,
    max_import_voltage_V: config_max_voltage,
    min_import_voltage_V: config_min_voltage,
    max_import_current_A: config_max_current,
    min_import_current_A: config_min_current,
    max_export_power_W: config_max_power,
    max_import_power_W: config_max_power,
    current_regulation_tolerance_A: 2,
    peak_current_ripple_A: 2,
    conversion_efficiency_export: 0.9,
    conversion_efficiency_import: 0.85,
  };

  setInterval(() => {
    mod.provides.main.publish.voltage_current({
      voltage_V: connector_voltage,
      current_A: connector_current,
    });

    if (connector_current > 0) energy_import_total += (connector_voltage * connector_current * 0.5) / 3600;
    if (connector_current < 0) energy_export_total += (connector_voltage * -connector_current * 0.5) / 3600;

    mod.provides.powermeter.publish.powermeter(power_meter_external());
    mod.provides.main.publish.capabilities(Capabilities);
  }, 500, mod);
});





