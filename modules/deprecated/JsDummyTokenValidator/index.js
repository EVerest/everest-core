// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

function wait(time) {
  return new Promise((resolve) => {
    setTimeout(resolve, time);
  });
}

boot_module(async ({ setup, config }) => {
  setup.provides.main.register.validate_token(async (mod, { provided_token } ) => {
    evlog.info(`Got validation request for token '${provided_token.id_token.value}'...`);
    await wait(config.impl.main.sleep * 1000);
    const retval = {
      authorization_status: config.impl.main.validation_result,
      reason: config.impl.main.validation_reason,
    };
    evlog.info(`Returning validation result for token '${provided_token.id_token.value}': `, retval);
    return retval;
  });
});
