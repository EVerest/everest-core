const { evlog, boot_module } = require('everestjs');

let pid_controller = {
    p_term: 0,
    i_term: 0,
    d_term: 0,
    error: 0,
    p_weight: 0,
    i_weight: 0,
    d_weight: 0,
    setpoint: 0,
    i_limit: -1,
    min_out: 0,
    max_out: 0
};
let running = false;
let charging_power = 0.0;
let charging_state = "";
let pause_msg = {
    pause_log: "pause"
};
let power_msg = {
    power_log: 0
};

function on_session_events(mod, val) {
    //"enum": ["Enabled", "Disabled", "SessionStarted", "ChargingStarted", "ChargingPausedEV", "ChargingPausedEVSE", "ChargingResumed", "ChargingFinished", "SessionFinished", "Error", "PermanentFault"]
    // Decide between charging / not charging
    if (val.event == "ChargingStarted" || val.event == "ChargingResumed") charging_state = "Charging";
    else charging_state = "NotCharging";
}

function on_grid_powermeter(mod, pm) {

    if (!running) {
        return;
    }
    
    const input_interval = 1;
    let error = pm.power_W.total - pid_controller.setpoint;


    // proportional term
    pid_controller.p_term = error;

    // integral term
    let i_term_old = pid_controller.i_term;
    pid_controller.i_term += error * input_interval;
    if (pid_controller.i_limit > 0) {
        if (pid_controller.i_term > pid_controller.i_limit) {
            pid_controller.i_term = pid_controller.i_limit;
        }
        else if (pid_controller.i_term < -pid_controller.i_limit) {
            pid_controller.i_term = -pid_controller.i_limit;
        }
    }
    if (charging_power < pid_controller.min_out) {
        evlog.debug("PID output is lower than minimum, integral term becomes reset");
        pid_controller.i_term = 0;
    }
    else if (charging_power >= pid_controller.max_out) {
        evlog.debug("PID output is higher than maximum, integral term can't increase");
        if (i_term_old < pid_controller.i_term) {
            pid_controller.i_term = i_term_old;
        }
    }

    // derivative term
    pid_controller.d_term = error - pid_controller.error;
    pid_controller.error = error;

    // publish to data logging
    let msg = {
        pid_p: pid_controller.p_term * pid_controller.p_weight,
        pid_i: pid_controller.i_term * pid_controller.i_weight,
        pid_d: pid_controller.d_term * pid_controller.d_weight,
        pid_error: pid_controller.error
    };
    mod.provides.main.publish.logging(msg);

    // calculate charging power
    charging_power = pid_controller.p_weight * pid_controller.p_term + pid_controller.i_weight * pid_controller.i_term + pid_controller.d_weight * pid_controller.d_term;
    if (charging_power > pid_controller.max_out) {
        charging_power = pid_controller.max_out;
    }
    else if (charging_power < pid_controller.min_out) {
        charging_power = 0.0;
    }
}

function reset_pid_controller(mod) {
    pid_controller.p_term = 0;
    pid_controller.i_term = 0;
    pid_controller.d_term = 0;
    pid_controller.error = 0;
}

function on_set_p(mod, val) {
    evlog.info("Set p_weight to ", val);
    pid_controller.p_weight = val;
    reset_pid_controller();
}

function on_set_i(mod, val) {
    evlog.info("Set i_weight to ", val);
    pid_controller.i_weight = val;
    reset_pid_controller();
}

function on_set_d(mod, val) {
    evlog.info("Set d_weight to ", val);
    pid_controller.d_weight = val;
    reset_pid_controller();
}

function on_set_s(mod, val) {
    evlog.info("Set setpoint to ", val);
    pid_controller.setpoint = val;
    reset_pid_controller();
}

function on_start(mod, val) {
    if (running) {
        evlog.info("PID-Controller is already running..");
        return;
    }
    reset_pid_controller();
    running = true;
    evlog.debug("PID-Controller is starting..");
}

function on_stop(mod, val) {
    if (!running) {
        evlog.info("PID-Controller is not running..");
        return;
    }
    running = false;
    charging_power = 0.0;
    set_charging_power(mod);
    evlog.debug("PID-Controller is stopping..");
}

function on_reset(mod, val) {
    reset_pid_controller(mod);
}

async function set_charging_power(mod) {
    if (!running) {
        return;
    }
    if (charging_power > 0) {
        if (charging_state != "Charging") {
            mod.uses.chargingdriver.call.resume_charging();
            pause_msg.pause_log = "resume";
            mod.provides.main.publish.logging(pause_msg);
        }
        else {
            let msg = {
                max_current: charging_power/230
            };
            mod.uses.chargingdriverenergy.call.set_max_current(msg);
            power_msg.power_log = charging_power;
            mod.provides.main.publish.logging(power_msg);
        }
    }
    else {
        if (charging_state == "Charging") {
            mod.uses.chargingdriver.call.pause_charging();
            pause_msg.pause_log = "pause";
            mod.provides.main.publish.logging(pause_msg);
        }
    }
}

boot_module(async ({ setup, info, config, mqtt }) => {
    evlog.info("Booting JsEnergyManager..");
    // Load config
    pid_controller.p_weight = config.impl.main.pid_p_weight;
    pid_controller.i_weight = config.impl.main.pid_i_weight;
    pid_controller.d_weight = config.impl.main.pid_d_weight;
    pid_controller.setpoint = config.impl.main.pid_setpoint;
    pid_controller.i_limit = config.impl.main.pid_i_limit;
    pid_controller.min_out = config.impl.main.pid_min_output;
    pid_controller.max_out = config.impl.main.pid_max_output;

    // Subscribe to powermeter
    setup.uses.gridpowermeter.subscribe.powermeter(on_grid_powermeter);
    setup.uses.chargingdriver.subscribe.session_events(on_session_events);

    // External subscriptions
    mqtt.subscribe('/external/emgr/set_p_weight', on_set_p);
    mqtt.subscribe('/external/emgr/set_i_weight', on_set_i);
    mqtt.subscribe('/external/emgr/set_d_weight', on_set_d);
    mqtt.subscribe('/external/emgr/set_setpoint', on_set_s);
    mqtt.subscribe('/external/emgr/start', on_start);
    mqtt.subscribe('/external/emgr/stop', on_stop);
    mqtt.subscribe('/external/emgr/reset', on_reset);
}).then((mod) => {
    // Start output loop
    setInterval(set_charging_power, mod.config.impl.main.pid_output_interval, mod);
});
