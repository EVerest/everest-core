const { evlog, boot_module } = require('everestjs');

let sim_value = 5000;
let update_interval = 1000;
let current_charging_power = 0;

function on_update(mod) {
    mod.provides.main.publish.powermeter(
        {
            timestamp: Date.now()/1000,
            power_W: {
              total: sim_value - current_charging_power
            }
        }        
    );
}

function on_yetipowermeter(mod, val) {
    current_charging_power = val.power_W.total;
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsSimPowermeter..");
    // Load config
    sim_value = config.impl.main.sim_value;
    update_interval = config.impl.main.update_interval;
    setup.uses.yetipowermeter.subscribe.powermeter(on_yetipowermeter);
}).then((mod) => {
    // Start output loop
    setInterval(on_update, update_interval, mod);
});
