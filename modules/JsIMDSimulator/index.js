// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');

let config_resistance_N_Ohm;
let config_resistance_P_Ohm;
let config_interval;
let intervalID;
let interval_running = false;

boot_module(async ({
  setup, config,
}) => {
  config_resistance_N_Ohm = config.impl.main.resistance_N_Ohm;
  config_resistance_P_Ohm = config.impl.main.resistance_P_Ohm;
  config_interval = config.impl.main.interval;

  // register commands
  setup.provides.main.register.start((mod) => {
    if (interval_running) return;
    evlog.debug(`Started simulated isolation monitoring with ${config_interval}ms interval`);

    intervalID = setInterval(() => {
      evlog.debug('Simulated isolation test finished');
      mod.provides.main.publish.IsolationMeasurement({
        resistance_P_Ohm: config_resistance_P_Ohm,
        resistance_N_Ohm: config_resistance_N_Ohm,
      });
    }, config_interval, mod);
    interval_running = true;
  });

  // register commands
  setup.provides.main.register.stop(() => {
    if (interval_running) {
      evlog.debug('Stopped simulated isolation monitoring.');
      clearInterval(intervalID);
      interval_running = false;
    }
  });
}).then(() => { });
