// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <slac/fsm/evse/states/others.hpp>

#include <cstring>

#include <slac/fsm/evse/states/matching.hpp>

#include "../misc.hpp"

namespace slac::fsm::evse {

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

void ResetState::enter() {
    ctx.log_info("Entered Reset state");
}

FSMSimpleState::HandleEventReturnType ResetState::handle_event(AllocatorType& sa, Event ev) {
    const auto& cfg = ctx.slac_config;
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            if (cfg.chip_reset.do_chip_reset) {
                // If chip reset is enabled in config, go to ResetChipState and from there to IdleState
                return sa.create_simple<ResetChipState>(ctx);
            } else {
                // If chip reset is disabled, go to IdleState directly
                return sa.create_simple<IdleState>(ctx);
            }
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
    if (setup_has_been_send == false) {
        auto set_key_req = create_cm_set_key_req(cfg.session_nmk);

        ctx.log_info("New NMK key: " + format_nmk(cfg.session_nmk));

        ctx.send_slac_message(cfg.plc_peer_mac, set_key_req);

        setup_has_been_send = true;

        return cfg.set_key_timeout_ms;
    } else {
        ctx.log_info("CM_SET_KEY_REQ timeout - failed to setup NMK key");
        return {};
    }
}

bool ResetState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    } else {
        ctx.log_info("Received CM_SET_KEY_CNF");
        return true;
    }
}

void ResetChipState::enter() {
    ctx.log_info("Entered HW Chip Reset state");
}

FSMSimpleState::HandleEventReturnType ResetChipState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return sa.create_simple<IdleState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetChipState::callback() {
    const auto& cfg = ctx.slac_config;
    if (sub_state == ChipResetState::DELAY) {
        sub_state = ChipResetState::SEND_RESET;
        return cfg.chip_reset.delay_ms;
    } else if (sub_state == ChipResetState::SEND_RESET) {
        slac::messages::cm_reset_device_req set_reset_req;

        ctx.log_info("Resetting HW Chip using RS_DEV.REQ");

        // RS_DEV.REQ is on protocol version 0
        ctx.send_slac_message(cfg.plc_peer_mac, set_reset_req);

        sub_state = ChipResetState::DONE;

        return cfg.chip_reset.timeout_ms;
    } else {
        ctx.log_info("RS_DEV.REQ timeout, no response received - failed to reset the chip");
        return {};
    }
}

bool ResetChipState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    } else {
        ctx.log_info("Received RS_DEV.CNF");
        return true;
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

void WaitForLinkState::enter() {
    ctx.log_info("Waiting for Link to be ready...");
    start_time = std::chrono::steady_clock::now();
}

FSMSimpleState::HandleEventReturnType WaitForLinkState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return sa.create_simple<MatchedState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::RETRY_MATCHING) {
        ctx.log_info("Link could not be established, resetting...");
        // Notify higher layers to on CP signal
        return sa.create_simple<FailedState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType WaitForLinkState::callback() {
    const auto& cfg = ctx.slac_config;
    if (not link_status_req_sent) {
        slac::messages::link_status_req link_status_req;

        // LINK_STATUS.REQ is on protocol version 0
        ctx.send_slac_message(cfg.plc_peer_mac, link_status_req);

        link_status_req_sent = true;
        return cfg.link_status.retry_ms;
    } else {
        // Did we timeout?
        if (std::chrono::steady_clock::now() - start_time > std::chrono::milliseconds(cfg.link_status.timeout_ms)) {
            return Event::RETRY_MATCHING;
        }
        // Link is confirmed not up yet, query again
        link_status_req_sent = false;
        return cfg.link_status.retry_ms;
    }
}

bool WaitForLinkState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_LINK_STATUS | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    } else {
        if (message.get_payload<slac::messages::link_status_cnf>().link_status == 0x01) {
            return true;
        } else {
            return false;
        }
    }
}

} // namespace slac::fsm::evse
