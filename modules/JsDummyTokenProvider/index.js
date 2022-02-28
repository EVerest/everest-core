// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

boot_module(async ({ setup }) => {
  setup.uses.evse.subscribe.session_events((mod, sessionEvents) => {
    if (!(sessionEvents.event === undefined) && sessionEvents.event === 'AuthRequired') {
      const data = {
        token: mod.config.impl.main.token,
        type: mod.config.impl.main.type,
        timeout: mod.config.impl.main.timeout,
      };
      evlog.info('Publishing new dummy token: ', data);
      mod.provides.main.publish.token(data);
    }
  });
});
