// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

boot_module(async ({ setup }) => {
  setup.uses.evse.subscribe.session_event((mod, sessionEvent) => {
    if (!(sessionEvent.event === undefined) && sessionEvent.event === 'AuthRequired') {
      const data = {
        id_token: mod.config.impl.main.token,
        authorization_type: mod.config.impl.main.type,
      };
      evlog.info('Publishing new dummy token: ', data);
      mod.provides.main.publish.provided_token(data);
    }
  });
});
