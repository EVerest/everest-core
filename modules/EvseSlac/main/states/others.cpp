// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "others.hpp"

#include <cstring>

#include <fmt/core.h>

#include "matching.hpp"

static auto create_cm_set_key_req(uint8_t* session_nmk) {
    slac::messages::cm_set_key_req set_key_req;

    set_key_req.key_type = slac::defs::CM_SET_KEY_REQ_KEY_TYPE_NMK;
    set_key_req.my_nonce = 0xAAAAAAAA;
    set_key_req.your_nonce = 0x00000000;
    set_key_req.pid = slac::defs::CM_SET_KEY_REQ_PID_HLE;
    set_key_req.prn = htole16(slac::defs::CM_SET_KEY_REQ_PRN_UNUSED);
    set_key_req.pmn = slac::defs::CM_SET_KEY_REQ_PMN_UNUSED;
    set_key_req.cco_capability = slac::defs::CM_SET_KEY_REQ_CCO_CAP_NONE;
    slac::utils::generate_nid_from_nmk(set_key_req.nid, session_nmk);
    set_key_req.new_eks = slac::defs::CM_SET_KEY_REQ_PEKS_NMK_KNOWN_TO_STA;
    memcpy(set_key_req.new_key, session_nmk, sizeof(set_key_req.new_key));

    return set_key_req;
}

int ResetState::enter() {
    ctx.log_info("Entered Reset state");

    auto set_key_req = create_cm_set_key_req(ctx.session_nmk);

    ctx.send_slac_message(ctx.plc_peer_mac, set_key_req);

    return ctx.set_key_timeout_ms;
}

FSM::NextStateType ResetState::handle_event(Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return create_with_context<IdleState>();
        } else {
            return fsm::NextStateOption::PASS_ON;
        }
    } else if (ev == Event::RESET) {
        return create_with_context<ResetState>();
    } else {
        return fsm::NextStateOption::PASS_ON;
    }
}

FSM::CallbackResultType ResetState::callback() {
    ctx.log_info("CM_SET_KEY_REQ timeout - failed to setup NMK key");
    return fsm::DO_NOT_CALL_ME_AGAIN;
}

bool ResetState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        ctx.log_info(fmt::format("Received non-expected SLAC message of type {:#06x}", mmtype));
        return false;
    } else {
        ctx.log_info("Received CM_SET_KEY_CNF");
        return true;
    }
}

int IdleState::enter() {
    ctx.signal_state("UNMATCHED");
    ctx.log_info("Entered Idle state");
    return fsm::DO_NOT_CALL_ME_AGAIN;
}

FSM::NextStateType IdleState::handle_event(Event ev) {
    if (ev == Event::ENTER_BCD) {
        return create_with_context<MatchingState>();
    } else if (ev == Event::RESET) {
        return create_with_context<ResetState>();
    } else {
        return fsm::NextStateOption::PASS_ON;
    }
}

int MatchedState::enter() {
    ctx.signal_state("MATCHED");
    ctx.signal_dlink_ready(true);
    ctx.log_info("Entered Matched state");
    return fsm::DO_NOT_CALL_ME_AGAIN;
}

FSM::NextStateType MatchedState::handle_event(Event ev) {
    if (ev == Event::RESET) {
        return create_with_context<ResetState>();
    } else {
        return fsm::NextStateOption::PASS_ON;
    }
}

void MatchedState::leave() {
    // FIXME (aw): do we want to generate the NMK here?
    ctx.generate_nmk();
    ctx.signal_dlink_ready(false);
}

int FailedState::enter() {
    ctx.signal_error_routine_request();
    ctx.log_info("Entered Failed state");
    return fsm::DO_NOT_CALL_ME_AGAIN;
}

FSM::NextStateType FailedState::handle_event(Event ev) {
    if (ev == Event::RESET) {
        return create_with_context<ResetState>();
    } else {
        return fsm::NextStateOption::PASS_ON;
    }
}
