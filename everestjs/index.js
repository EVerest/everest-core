// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
    main_dir: process.env.EV_MAIN_DIR,
    schemas_dir: process.env.EV_SCHEMAS_DIR,
    modules_dir: process.env.EV_MODULES_DIR,
    interfaces_dir: process.env.EV_INTERFACES_DIR,
    config_file: process.env.EV_CONF_FILE,
    log_config_file: process.env.EV_LOG_CONF_FILE,
    mqtt_server_address: process.env.MQTT_SERVER_ADDRESS,
    mqtt_server_port: process.env.MQTT_SERVER_PORT,
  };

  const settings = { ...env_settings, ...user_settings };

  if (!settings.module) {
    throw new Error('parameter "module" is missing');
  }

  const main_dir = helpers.get_default(settings, 'main_dir', './');
  const config = {
    module: settings.module,
    main_dir,
    schemas_dir: helpers.get_default(settings, 'schemas_dir', `${main_dir}/schemas`),
    modules_dir: helpers.get_default(settings, 'modules_dir', `${main_dir}/modules`),
    interfaces_dir: helpers.get_default(settings, 'interfaces_dir', `${main_dir}/interfaces`),
    config_file: helpers.get_default(settings, 'config_file', `${main_dir}/conf/config.json`),
    log_config_file: helpers.get_default(settings, 'log_config_file', `${main_dir}/conf/logging.ini`),
    validate_schema: helpers.get_default(settings, 'validate_schema', true),
    mqtt_server_address: helpers.get_default(settings, 'mqtt_server_address', 'localhost'),
    mqtt_server_port: helpers.get_default(settings, 'mqtt_server_port', '1883'),
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
  };

  if (this.mqtt === undefined) {
    const missing_mqtt_getter = {
      get() { throw new Error('External mqtt not available - missing enable_external_mqtt in manifest?'); },
    };
    Object.defineProperty(module_setup, 'mqtt', missing_mqtt_getter);
    Object.defineProperty(this, 'mqtt', missing_mqtt_getter);
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
