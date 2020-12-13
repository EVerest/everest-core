const { evlog, boot_module } = require('everestjs');

function wait(time) {
  return new Promise((resolve) => {
    setTimeout(resolve, time);
  });
}

boot_module(async ({ setup, config }) => {
  setup.provides.main.register.validate_token(async (mod, { token }) => {
    evlog.info(`Got validation request for token '${token}'...`);
    await wait(config.impl.main.sleep * 1000);
    const retval = {
      result: config.impl.main.validation_result,
      reason: config.impl.main.validation_reason,
    };
    evlog.info(`Returning validation result for token '${token}': `, retval);
    return retval;
  });
});
