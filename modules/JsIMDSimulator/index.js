// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');
const { inherits } = require('util');

let config_status;
let config_duration;

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  config_status = config.impl.main.status;
  config_duration = config.impl.main.duration;

  // register commands
  setup.provides.main.register.startIsolationTest((mod, args) => {
    evlog.debug(`Start simulated isolation test with ${config_duration}s delay`);
    setTimeout((mod) => {
      evlog.debug('Simulated isolation test finished');
      mod.provides.main.publish.IsolationStatus(config_status);
    }, config_duration * 1000, mod);
  });
}).then((mod) => {

});
