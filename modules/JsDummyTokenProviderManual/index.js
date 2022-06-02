// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

boot_module(async ({ setup, info, config, mqtt }) => {
  mqtt.subscribe('everest_api/dummy_token_provider/cmd/provide', (mod, token) => {

    const data = {
      token: token,
      type: mod.config.impl.main.type,
      timeout: mod.config.impl.main.timeout,
    };
    evlog.info('Publishing new dummy token: ', data);
    mod.provides.main.publish.token(data);
  });
});
