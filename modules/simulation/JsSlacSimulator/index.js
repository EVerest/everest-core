// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { evlog, boot_module } = require('everestjs');
const { setInterval } = require('timers');
const { inherits } = require('util');

let enabled;
let state_evse;
let state_ev;
let cntmatching;
const STATE_UNMATCHED = 0;
const STATE_MATCHING = 1;
const STATE_MATCHED = 2;

boot_module(async ({
  setup, info, config, mqtt,
}) => {
  state_evse = STATE_UNMATCHED;
  state_ev = STATE_UNMATCHED;
  cntmatching = 0;

  // register commands for EVSE side
  setup.provides.evse.register.reset((mod, args) => {
    enabled = args.enable;
    set_unmatched_evse(mod);
  });

  setup.provides.evse.register.enter_bcd((mod, args) => {
    set_matching_evse(mod);
    return true;
  });

  setup.provides.evse.register.leave_bcd((mod, args) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_terminate((mod, args) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_error((mod, args) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_pause((mod, args) => true);

  // register commands for EV side
  setup.provides.ev.register.reset((mod, args) => {
    enabled = args.enable;
    set_unmatched_ev(mod);
  });

  setup.provides.ev.register.enter_bcd((mod, args) => {
    set_matching_ev(mod);
    return true;
  });

  setup.provides.ev.register.leave_bcd((mod, args) => {
    set_unmatched_ev(mod);
    return true;
  });

  setup.provides.ev.register.dlink_terminate((mod, args) => {
    set_unmatched_ev(mod);
    return true;
  });

  setup.provides.ev.register.dlink_error((mod, args) => {
    set_unmatched_ev(mod);
    return true;
  });

  setup.provides.ev.register.dlink_pause((mod, args) => true);
}).then((mod) => {
  mod.provides.ev.publish.state(state_to_string(state_ev));
  mod.provides.evse.publish.state(state_to_string(state_evse));
  setInterval(simulation_loop, 250, mod);
});

function set_unmatched_evse(mod) {
  if (state_evse != STATE_UNMATCHED) {
    state_evse = STATE_UNMATCHED;
    mod.provides.evse.publish.state(state_to_string(state_evse));
    mod.provides.evse.publish.dlink_ready(false);
  }
}
function set_unmatched_ev(mod) {
  if (state_ev != STATE_UNMATCHED) {
    state_ev = STATE_UNMATCHED;
    mod.provides.ev.publish.state(state_to_string(state_ev));
    mod.provides.ev.publish.dlink_ready(false);
  }
}

function set_matching_evse(mod) {
  state_evse = STATE_MATCHING;
  cntmatching = 0;
  mod.provides.evse.publish.state(state_to_string(state_evse));
}
function set_matching_ev(mod) {
  state_ev = STATE_MATCHING;
  cntmatching = 0;
  mod.provides.ev.publish.state(state_to_string(state_ev));
}

function set_matched_evse(mod) {
  state_evse = STATE_MATCHED;
  mod.provides.evse.publish.state(state_to_string(state_evse));
  mod.provides.evse.publish.dlink_ready(true);
}
function set_matched_ev(mod) {
  state_ev = STATE_MATCHED;
  mod.provides.ev.publish.state(state_to_string(state_ev));
  mod.provides.ev.publish.dlink_ready(true);
}

function simulation_loop(mod) {
  // if both are in matching for 2 seconds SLAC matches
  if (state_ev === STATE_MATCHING && state_evse === STATE_MATCHING && cntmatching++ > 2 * 4) {
    set_matched_ev(mod);
    set_matched_evse(mod);
  }
}

function state_to_string(s) {
  switch (s) {
    case STATE_UNMATCHED:
      return 'UNMATCHED';
      break;
    case STATE_MATCHING:
      return 'MATCHING';
      break;
    case STATE_MATCHED:
      return 'MATCHED';
      break;
  }
}
