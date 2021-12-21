/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 */
const { evlog, boot_module } = require('everestjs');

boot_module().then((mod) => {
  setInterval(() => {
    const data = {
      token: mod.config.impl.main.token,
      type: mod.config.impl.main.type,
      timeout: mod.config.impl.main.timeout,
    };
    evlog.info('Publishing new dummy token: ', data);
    mod.provides.main.publish.token(data);
  }, mod.config.impl.main.sleep * 1000);
});
