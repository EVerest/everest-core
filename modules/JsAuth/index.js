// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');

// FIXME (aw): we need some context that we can share from boot to main module
let acceptedToken = null;

boot_module(async ({ setup }) => {
  // subscribe to all token providers and try to validate all new incoming tokens
  // priority of token providers and validators is configured via connection order of this dependency in config.json

  setup.uses_list.tokenProvider.forEach((provider, token_priority) => {
    provider.subscribe.token(async (mod, { token, type, timeout }) => {
      mod.uses_list.tokenValidator.forEach((validator, validator_priority) => {
        if (type == 'ocpp_authorized') {
          evlog.error("already validated");
          const token_data = {
            timestamp: Math.floor(Date.now() / 1000),
            type,
            token,
            token_priority,
            validator_priority,
            result: 'Accepted',
            reason: 'Already authorized',
            timeout,
          };
          acceptedToken = token_data;
          mod.provides.main.publish.authorization_available(true);
          return;
        }
        const { result, reason } = validator.call.validate_token({ token });
        const token_data = {
          timestamp: Math.floor(Date.now() / 1000),
          type,
          token,
          token_priority,
          validator_priority,
          result,
          reason,
          timeout,
        };

        if (result === 'Accepted') {
          evlog.info(`Accepted auth token of type '${type}': '${token}'`);
          acceptedToken = token_data;
          evlog.debug('Validated incoming auth token: ', acceptedToken);
          mod.provides.main.publish.authorization_available(true);

          setTimeout(() => {
            if (acceptedToken === token_data) {
              acceptedToken = null;
              mod.provides.main.publish.authorization_available(false);
            }
          }, timeout * 1000);
        } else {
          evlog.info(`Rejected auth token of type '${type}' with '${result}' (reason: '${reason}'): '${token}'`);
          evlog.warning('Could not validate incoming auth token: ', token_data);
        }
      });
    });
  });

  // API cmd to use by all other modules (auth framework entry point)
  setup.provides.main.register.get_authorization((mod) => {
    if (acceptedToken === null) {
      evlog.warning("Returning 'null' because no validated token could be found");
      return null;
    }

    evlog.info(`Returning validated token: '${acceptedToken.token}'...`);
    const t = acceptedToken.token;
    acceptedToken = null;
    mod.provides.main.publish.authorization_available(false);
    return t;
  });
});
