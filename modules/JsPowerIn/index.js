const { evlog, boot_module } = require('everestjs');

boot_module(() => {
  // nothing to initialize for this module
}).then((mod) => {
  evlog.info(`Providing phase_count: ${mod.config.impl.main.phase_count}`);
  evlog.info(`Providing max_current: ${mod.config.impl.main.max_current}`);
  mod.provides.main.publish.phase_count(mod.config.impl.main.phase_count);
  mod.provides.main.publish.max_current(mod.config.impl.main.max_current);
});
