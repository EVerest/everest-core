// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "others.hpp"

#include <cstring>

#include <fmt/core.h>

#include "matching.hpp"

static auto create_cm_set_key_req(uint8_t const* session_nmk) {
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

static auto create_cm_reset_req() {
    slac::messages::cm_reset_req reset_req{0x00, 0xB0, 0x52};
    return reset_req;
}

void ResetState::enter() {
    ctx.log_info("Entered Reset state");
}

FSMSimpleState::HandleEventReturnType ResetState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return sa.create_simple<IdleState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetState::callback() {
    const auto& cfg = ctx.slac_config;

    if (cfg.soft_reset_chip && chip_reset_has_been_send == false) {
        ctx.log_info(fmt::format("Resetting chip and wait for {}ms", cfg.soft_reset_wait_time_ms));
        auto reset_req = create_cm_reset_req();
        ctx.send_slac_message(cfg.plc_peer_mac, reset_req, 0);
        chip_reset_has_been_send = true;

        return cfg.soft_reset_wait_time_ms;
    }

    if (cfg.soft_reset_chip && chip_reset_has_been_send && !chip_reset_completed) {
        // did we receive confirmation from chip or did we timeout?
        chip_reset_completed = true;
        if (chip_reset_has_been_confirmed) {
            ctx.log_info(fmt::format("Reset confirmed, wait for {}ms", cfg.soft_reset_wait_time_ms));
            return cfg.soft_reset_wait_time_ms;
        } else {
            ctx.log_error("CM_RESET_REQ timeout - chip did not confirm reset command");
        }
    }

    if (nmk_has_been_set == false) {
        auto set_key_req = create_cm_set_key_req(cfg.session_nmk);
        ctx.log_info("Sending key set req");
        ctx.send_slac_message(cfg.plc_peer_mac, set_key_req);

        nmk_has_been_set = true;

        return cfg.set_key_timeout_ms;
    } else {
        ctx.log_error("CM_SET_KEY_REQ timeout - failed to setup NMK key");
        return {};
    }
}

bool ResetState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype == (slac::defs::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_CNF)) {
        ctx.log_info("Received CM_RESET_DEVICE_CNF");
        chip_reset_has_been_confirmed = true;
        return false;

    } else if (mmtype == (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        ctx.log_info("Received CM_SET_KEY_CNF");
        return true;
    } else {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        ctx.log_info(fmt::format("Received non-expected SLAC message of type {:#06x}", mmtype));
        return false;
    }
}

void IdleState::enter() {
    ctx.signal_state("UNMATCHED");
    ctx.log_info("Entered Idle state");
}

FSMSimpleState::HandleEventReturnType IdleState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::ENTER_BCD) {
        return sa.create_simple<MatchingState>(ctx);
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void MatchedState::enter() {
    ctx.signal_state("MATCHED");
    ctx.signal_dlink_ready(true);
    ctx.log_info("Entered Matched state");
}

FSMSimpleState::HandleEventReturnType MatchedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void MatchedState::leave() {
    // FIXME (aw): do we want to generate the NMK here?
    ctx.slac_config.generate_nmk();
    ctx.signal_dlink_ready(false);
}

void FailedState::enter() {
    ctx.signal_error_routine_request();
    ctx.log_info("Entered Failed state");
}

FSMSimpleState::HandleEventReturnType FailedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}
