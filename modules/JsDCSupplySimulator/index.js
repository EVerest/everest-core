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

let settings_connector_export_voltage;
let settings_connector_import_voltage;
let settings_connector_max_export_current;
let settings_connector_max_import_current;

let connector_voltage;
let mode;

let voltage;
let current;

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
  mode = 'Off';
  voltage = 0.0;
  current = 0.0;

  // register commands
  setup.provides.main.register.getCapabilities((mod, args) => {
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
    };
    return Capabilities;
  });

  setup.provides.main.register.setMode((mod, args) => {
    if (args.value === 'Off') {
      mode = 'Off'; connector_voltage = 0.0;
    } else if (args.value === 'Export') {
      mode = 'Export'; connector_voltage = settings_connector_export_voltage;
    } else if (args.value === 'Import') {
      mode = 'Import'; connector_voltage = settings_connector_import_voltage;
    } else if (args.value === 'Fault') {
      mode = 'Fault'; connector_voltage = 0.0;
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

    if (mode === 'Export') connector_voltage = settings_connector_export_voltage;
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

    if (mode === 'Import') connector_voltage = settings_connector_import_voltage;
  });
}).then((mod) => {
  setInterval(() => {
    mod.provides.main.publish.voltage_current({
      voltage_V: connector_voltage,
      current_A: 0.1,
    });
  }, 500, mod);
});
