// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

boot_module(async ({ setup, info, config, mqtt }) => {
  mqtt.subscribe('everest_api/dummy_token_provider/cmd/provide', (mod, raw_data) => {
    const data = JSON.parse(raw_data);
    evlog.info('Publishing new dummy token: ', data);
    mod.provides.main.publish.provided_token(data);
  });
});
