// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');
const { inherits } = require('util');

var config_min_current;
var config_min_voltage;
var config_max_current;
var config_min_current;
var config_bidirectional;

var settings_connector_export_voltage;
var settings_connector_import_voltage;
var settings_connector_max_export_current;
var settings_connector_max_import_current;

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  config_max_voltage = config.impl.main.max_voltage;
  config_min_voltage = config.impl.main.min_voltage;
  config_max_current = config.impl.main.max_current;
  config_min_current = config.impl.main.min_current;
  config_bidirectional = config.impl.main.bidirectional;
  config_max_power = config.impl.main.max_power;

  connector_voltage = 0.0;

  // register commands
  setup.provides.main.register.getCapabilities((mod, args) => {
    //evlog.debug(`Start simulated isolation test with ${config_duration}s delay`);
    return {
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
      peak_current_ripple_A: 2
    };
  });

  setup.provides.main.register.setMode((mod, args) => {
    if (args.value == "Off") { connector_voltage = 0.0; }
    else if (args.value == "Export") { connector_voltage = settings_connector_export_voltage; }
    else if (args.value == "Import") { connector_voltage = settings_connector_import_voltage; }
    else if (args.value == "Fault") { connector_voltage = 0.0; }
    mod.provides.main.publish.mode(args.value);
  });

  setup.provides.main.register.setExportVoltageCurrent((mod, args) => {

    if (args.voltage < config_min_voltage) args.voltage = config_min_voltage;
    if (args.current < config_min_current) args.current = config_min_current;

    if (args.voltage > config_max_voltage) args.voltage = config_max_voltage;
    if (args.current > config_max_current) args.current = config_max_current;

    settings_connector_max_export_current = args.value;
    settings_connector_export_voltage = args.value;
  });

  setup.provides.main.register.setImportVoltageCurrent((mod, args) => {

    if (args.voltage < config_min_voltage) args.voltage = config_min_voltage;
    if (args.current < config_min_current) args.current = config_min_current;

    if (args.voltage > config_max_voltage) args.voltage = config_max_voltage;
    if (args.current > config_max_current) args.current = config_max_current;

    settings_connector_import_voltage = args.value;
    settings_connector_max_import_current = args.value;
  });

}).then((mod) => {
  setInterval((mod) => {
    mod.provides.main.publish.voltage_current({
      voltage_V: connector_voltage,
      current_A: 0.1
    });
  }, 500, mod);
});
