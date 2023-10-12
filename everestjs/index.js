// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
const util = require('util');
const addon = require('./everestjs.node');

const helpers = {
  get_default: (obj, key, defaultValue) => ((obj[key] === undefined) ? defaultValue : obj[key]),
  to_string: (...values) => {
    let log_string = '';
    values.forEach((value) => {
      if (typeof value == 'string') {
        log_string += value;
      } else if (typeof value == 'number') {
        log_string += value;
      } else if (typeof value == 'boolean') {
        log_string += value ? 'true' : 'false';
      } else if (typeof value == 'object') {
        log_string += util.inspect(value, false, 6, true);
      } else {
        throw new Error(`The logging function cannot handle input of type ${typeof value}`);
      }
    });
    return log_string;
  },
};

const EverestModule = function EverestModule(handler_setup, user_settings) {
  const env_settings = {
    module: process.env.EV_MODULE,
    prefix: process.env.EV_PREFIX,
    config_file: process.env.EV_CONF_FILE,
    mqtt_server_address: process.env.MQTT_SERVER_ADDRESS,
    mqtt_server_port: process.env.MQTT_SERVER_PORT,
    validate_schema: process.env.EV_VALIDATE_SCHEMA,
  };

  const settings = { ...env_settings, ...user_settings };

  if (!settings.module) {
    throw new Error('parameter "module" is missing');
  }

  const config = {
    module: settings.module,
    prefix: settings.prefix,
    config_file: settings.config_file,
    validate_schema: helpers.get_default(settings, 'validate_schema', false),
  };

  function callbackWrapper(on, request, ...args) {
    const result = request(...args);
    Promise.resolve(result).then(on.fulfill, on.reject);
  }

  const available_handlers = addon.boot_module.call(this, config, callbackWrapper);

  const module_setup = {
    setup: available_handlers,
    info: this.info,
    config: this.config,
    mqtt: this.mqtt,
    telemetry: this.telemetry,
  };

  if (this.mqtt === undefined) {
    const missing_mqtt_getter = {
      get() { throw new Error('External mqtt not available - missing enable_external_mqtt in manifest?'); },
    };
    Object.defineProperty(module_setup, 'mqtt', missing_mqtt_getter);
    Object.defineProperty(this, 'mqtt', missing_mqtt_getter);
  }

  if (this.telemetry === undefined) {
    const missing_telemetry_getter = {
      get() { throw new Error('Telemetry not available - missing enable_telemetry in manifest?'); },
    };
    Object.defineProperty(module_setup, 'telemetry', missing_telemetry_getter);
    Object.defineProperty(this, 'telemetry', missing_telemetry_getter);
  }

  // check, if we need to register cmds
  if (typeof handler_setup === 'undefined') {
    if (Object.keys(available_handlers.provides).length !== 0) {
      throw new Error('handler setup callback is missing - you need to register at least one command');
    }
    return addon.signal_ready.call(this);
  }

  if (typeof handler_setup === 'function') {
    return Promise.resolve(handler_setup.call({}, module_setup)).then(
      () => addon.signal_ready.call(this)
    );
  }

  throw new Error('handler setup callback needs to be of type function');
};

// setup log handlers
exports.evlog = {};
Object.keys(addon.log).forEach((key) => {
  exports.evlog[key] = (...log_args) => addon.log[key](helpers.to_string(...log_args));
});

let boot_module_called = false;

exports.boot_module = (handler_setup, settings) => {
  if (boot_module_called) throw Error('Calling initModule more than once is not supported right now');
  boot_module_called = true;
  return new EverestModule(handler_setup, settings);
};
