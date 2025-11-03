const { evlog, boot_module } = require('everestjs');

// FIXME (aw): we need some context that we can share from boot to main module
let acceptedToken = null;

boot_module(async ({ setup }) => {
  // subscribe to all token providers and try to validate all new incoming tokens
  // priority of token providers and validators is configured via connection order of this dependency in config.json

  // FIXME: this should be a list of *all* token providers once we have 1-n requirements
  [setup.uses.tokenProvider].forEach((provider, token_priority) => {
    provider.subscribe.token(async (mod, { token, type, timeout }) => {
      // FIXME: this should be a list of *all* token providers once we have 1-n requirements
      [mod.uses.tokenValidator].forEach((validator, validator_priority) => {
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
          mod.provides.main.publish.authorized(token);

          setTimeout(() => {
            if (acceptedToken === token_data) {
              acceptedToken = null;
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
  setup.provides.main.register.authorize(async () => {
    if (acceptedToken === null) {
      evlog.warning("Returning 'null' because no validated token could be found");
      return null;
    }

    evlog.info(`Returning validated token: '${acceptedToken.token}'...`);
    return acceptedToken.token;
  });
});
