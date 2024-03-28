// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
const { boot_module } = require('everestjs');
const { setInterval } = require('timers');

let state_evse;
let state_ev;
let cntmatching;
const STATE_UNMATCHED = 0;
const STATE_MATCHING = 1;
const STATE_MATCHED = 2;

function state_to_string(s) {
  switch (s) {
    case STATE_UNMATCHED:
      return 'UNMATCHED';
    case STATE_MATCHING:
      return 'MATCHING';
    case STATE_MATCHED:
      return 'MATCHED';
    default:
      return '';
  }
}

function set_unmatched_evse(mod) {
  if (state_evse !== STATE_UNMATCHED) {
    state_evse = STATE_UNMATCHED;
    mod.provides.evse.publish.state(state_to_string(state_evse));
    mod.provides.evse.publish.dlink_ready(false);
  }
}
function set_unmatched_ev(mod) {
  if (state_ev !== STATE_UNMATCHED) {
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
  cntmatching += 1;
  if (state_ev === STATE_MATCHING && state_evse === STATE_MATCHING && cntmatching > 2 * 4) {
    set_matched_ev(mod);
    set_matched_evse(mod);
  }
}

boot_module(async ({
  setup,
}) => {
  state_evse = STATE_UNMATCHED;
  state_ev = STATE_UNMATCHED;
  cntmatching = 0;

  // register commands for EVSE side
  setup.provides.evse.register.reset((mod) => {
    set_unmatched_evse(mod);
  });

  setup.provides.evse.register.enter_bcd((mod) => {
    set_matching_evse(mod);
    return true;
  });

  setup.provides.evse.register.leave_bcd((mod) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_terminate((mod) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_error((mod) => {
    set_unmatched_evse(mod);
    return true;
  });

  setup.provides.evse.register.dlink_pause(() => true);

  // register commands for EV side
  setup.provides.ev.register.reset((mod) => {
    set_unmatched_ev(mod);
  });

  setup.provides.ev.register.trigger_matching((mod) => {
    set_matching_ev(mod);
    return true;
  });
}).then((mod) => {
  mod.provides.ev.publish.state(state_to_string(state_ev));
  mod.provides.evse.publish.state(state_to_string(state_evse));
  setInterval(simulation_loop, 250, mod);
});
