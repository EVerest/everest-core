// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "evse_fsm.hpp"

#include <chrono>

#include <fmt/core.h>

// FIXME (aw):
//  - handle evse, ev, sender and source id, probably also the mac
//  - what is the correct PLC peer mac?

void setup_atten_char_ind_message(slac::messages::HomeplugMessage& msg, const MatchingSessionContext& matching_ctx) {
    auto atten_char_ind = matching_ctx.calculate_avg();

    msg.setup_payload(&atten_char_ind, sizeof(atten_char_ind),
                      slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND);
    msg.setup_ethernet_header(matching_ctx.ev_mac);
}

using EventResetDone = EventTypeFactory::Derived<EventID::ResetDone>;
using EventInitTimeout = EventTypeFactory::Derived<EventID::InitTimout>;
using EventStartMatching = EventTypeFactory::Derived<EventID::StartMatching>;
using EventStartSounding = EventTypeFactory::Derived<EventID::StartSounding>;
using EventMatchingFailed = EventTypeFactory::Derived<EventID::MatchingFailed>;
using EventFinishSounding = EventTypeFactory::Derived<EventID::FinishSounding>;
using EventAttenCharRspReceived = EventTypeFactory::Derived<EventID::AttenCharRspReceived>;

EvseFSM::EvseFSM(SlacIO& slac_io) : slac_io(slac_io) {
    // define transitions
    sd_reset.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::SlacMessage:
            sd_reset_hsm(trans.set_internal(), ev);
            return;
        case EventID::ResetDone:
            return trans.set(sd_idle);
        }
    };

    sd_reset.entry = [this](FSMInitContextType& ctx) {
        // send our setup message

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

        msg_out.setup_payload(&set_key_req, sizeof(set_key_req),
                              slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ);

        msg_out.setup_ethernet_header(plc_peer_mac);

        this->slac_io.send(msg_out);

        // FIXME (aw): timemout for correct response
        ctx.set_next_timeout(100);
    };

    sd_reset.handler = [this](FSMContextType& ctx) {
        // FIXME (aw): what to do here? should fail hard with IOError
        fmt::print("Failed to setup NMK key\n");
    };

    sd_idle.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::EnterBCD:
            return trans.set(sd_wait_for_matching_start);
        default:
            return;
        }
    };

    sd_wait_for_matching_start.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::SlacMessage:
            sd_wait_for_matching_hsm(trans.set_internal(), ev);
            return;
        case EventID::StartMatching:
            return trans.set(sd_matching);
        case EventID::InitTimout:
            // FIXME (aw): where to put MAX_INIT_RETRIES?
            if (ac_mode_five_percent) {
                return trans.set(sd_signal_error);
            } else {
                return trans.set(sd_no_slac_performed);
            }
        }
    };

    sd_wait_for_matching_start.entry = [this](FSMInitContextType& ctx) {
        ctx.set_next_timeout(slac::defs::TT_EVSE_SLAC_INIT_MS);
    };

    sd_wait_for_matching_start.handler = [this](FSMContextType& ctx) {
        // we didn't received a CM_SLAC_PARM.REQ
        ctx.submit_event(EventInitTimeout());
        return;
    };

    sd_matching.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::LeaveBCD:
            return trans.set(sd_idle);
        case EventID::SlacMessage:
            sd_matching_hsm(trans.set_internal(), ev);
            return;
        case EventID::StartSounding:
            return trans.set(sd_sounding);
        }
    };

    sd_matching.entry = [this](FSMInitContextType& ctx) { ctx.set_next_timeout(slac::defs::TT_MATCH_SEQUENCE_MS); };

    sd_matching.handler = [this](FSMContextType& ctx) {
        // we didn't receive CM_START_ATTEN_CHAR in time
        ctx.submit_event(EventMatchingFailed());
        return;
    };

    sd_sounding.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::LeaveBCD:
            return trans.set(sd_idle);
        case EventID::SlacMessage:
            sd_sounding_hsm(trans.set_internal(), ev);
            return;
        case EventID::FinishSounding:
            return trans.set(sd_do_atten_char);
        }
    };

    sd_sounding.entry = [this](FSMInitContextType& ctx) {
        matching_ctx.tp_sound_start = std::chrono::system_clock::now();
        ctx.set_next_timeout(slac::defs::TT_EVSE_MATCH_MNBC_MS);
    };

    sd_sounding.handler = [this](FSMContextType& ctx) {
        fmt::print("Timeout in sounding, send results\n");
        ctx.submit_event(EventFinishSounding());
    };

    sd_do_atten_char.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::LeaveBCD:
            return trans.set(sd_idle);
        case EventID::SlacMessage:
            sd_wait_for_atten_char_rsp_hsm(trans.set_internal(), ev);
            return;
        case EventID::AttenCharRspReceived:
            return trans.set(sd_wait_for_slac_match);
        case EventID::MatchingFailed:
            return trans.set(sd_matching_failed);
        }
    };

    sd_do_atten_char.entry = [this](FSMInitContextType& ctx) {
        setup_atten_char_ind_message(msg_out, matching_ctx);
        this->slac_io.send(msg_out);
        sd_do_atten_char.ind_msg_count = 0; // no retries yet
        ctx.set_next_timeout(slac::defs::TT_MATCH_RESPONSE_MS, true);
    };

    sd_do_atten_char.handler = [this](FSMContextType& ctx) {
        // got a time out, retry if possible
        if (sd_do_atten_char.ind_msg_count == slac::defs::C_EV_MATCH_RETRY) {
            fmt::print("No response to CM_ATTEN_CHAR.IND after {} retries\n", slac::defs::C_EV_MATCH_RETRY);
            ctx.submit_event(EventMatchingFailed());
        }

        // resend CM_ATTEN_CHAR.IND
        this->slac_io.send(msg_out);
        sd_do_atten_char.ind_msg_count++;
    };

    sd_wait_for_slac_match.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case EventID::LeaveBCD:
            return trans.set(sd_idle);
        case EventID::SlacMessage:
            sd_wait_for_slac_match_hsm(trans.set_internal(), ev);
            return;
        case EventID::MatchingFailed:
            return trans.set(sd_matching_failed);
        case EventID::LinkDetected:
            if (matching_ctx.received_match_req) {
                return trans.set(sd_matched);
            }
            return;
        }
    };

    sd_wait_for_slac_match.entry = [this](FSMInitContextType& ctx) {
        // setup timeout for slac_match or validate.  Note, that
        // TT_EVSE_MATCH_SESSION_MS refers to the expiration of
        // TT_EVSE_MATCH_MNBC_MS
        auto t_dur =
            matching_ctx.tp_sound_start +
            std::chrono::milliseconds(slac::defs::TT_EVSE_MATCH_MNBC_MS + slac::defs::TT_EVSE_MATCH_SESSION_MS) -
            std::chrono::system_clock::now();

        ctx.set_next_timeout(std::chrono::duration_cast<std::chrono::milliseconds>(t_dur).count());
    };

    sd_wait_for_slac_match.handler = [this](FSMContextType& ctx) {
        if (!matching_ctx.received_match_req) {
            // timeout on waiting for slac_match / validate
            ctx.submit_event(EventMatchingFailed());
        } else {
            // timeout on waiting for logical link establishment
            ctx.submit_event(EventMatchingFailed());
        }
    };

    sd_signal_error.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        // FIXME (aw): what about SLAC messages that we receive while in this mode?
        case EventID::ErrorSequenceDone:
            // FIXME (aw): where/when to reset init_retry_count?
            ++init_retry_count;
            if (init_retry_count == MAX_INIT_RETRIES) {
                // FIXME (aw): we need to signal this to the EvseManager
                // somehow
                return trans.set(sd_no_slac_performed);
            } else {
                return trans.set(sd_wait_for_matching_start);
            }
        default:
            return;
        }
    };

    sd_signal_error.entry = [this](FSMInitContextType& ctx) {
        // FIXME (aw): how long do we want/need to wait for the
        //             EventErrorSequenceDone event?
        //             right now, we're stipulating it by ourself ..
        ctx.set_next_timeout(30);
    };

    sd_signal_error.handler = [this](FSMContextType& ctx) { ctx.submit_event(EventErrorSequenceDone()); };

    sd_no_slac_performed.transitions = [this](const EventBaseType& ev, TransitionType& trans) { return; };
}

void EvseFSM::sd_wait_for_matching_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    if (msg_in.get_mmtype() != (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_REQ)) {
        fmt::print("Received non-expected message of type {:#06x}\n", msg_in.get_mmtype());
        return;
    }

    auto& param_request = msg_in.get_payload<slac::messages::cm_slac_parm_req>();

    // reset matching context
    matching_ctx = MatchingSessionContext();
    memcpy(matching_ctx.ev_mac, msg_in.get_src_mac(), sizeof(matching_ctx.ev_mac));
    memcpy(matching_ctx.run_id, param_request.run_id, sizeof(matching_ctx.run_id));

    slac::messages::cm_slac_parm_cnf param_confirm;

    memcpy(param_confirm.m_sound_target, slac::defs::BROADCAST_MAC_ADDRESS, sizeof(slac::defs::BROADCAST_MAC_ADDRESS));
    param_confirm.num_sounds = slac::defs::CM_SLAC_PARM_CNF_NUM_SOUNDS;
    param_confirm.timeout = slac::defs::CM_SLAC_PARM_CNF_TIMEOUT;
    param_confirm.resp_type = slac::defs::CM_SLAC_PARM_CNF_RESP_TYPE;
    memcpy(param_confirm.forwarding_sta, matching_ctx.ev_mac, sizeof(param_confirm.forwarding_sta));
    param_confirm.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    param_confirm.security_type = slac::defs::COMMON_SECURITY_TYPE;
    memcpy(param_confirm.run_id, matching_ctx.run_id, sizeof(param_confirm.run_id));

    msg_out.setup_ethernet_header(param_confirm.forwarding_sta);
    msg_out.setup_payload(&param_confirm, sizeof(param_confirm),
                          slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_CNF);

    slac_io.send(msg_out);

    ctx.submit_event(EventStartMatching());
}

void EvseFSM::sd_matching_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    // FIXME (aw): we should also deal with the CM_SLAC_PARAM_REQ, right?
    if (msg_in.get_mmtype() != (slac::defs::MMTYPE_CM_START_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND)) {
        fmt::print("Received non-expected message of type {:#06x}\n", msg_in.get_mmtype());
        return;
    }

    auto& start_atten = msg_in.get_payload<slac::messages::cm_start_atten_char_ind>();

    if (!matching_ctx.conforms(msg_in.get_src_mac(), start_atten.run_id)) {
        // invalid message, skip it
        return;
    }

    // everything is fine, go to next state
    ctx.submit_event(EventStartSounding());
}

void EvseFSM::sd_sounding_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    const auto mmtype = msg_in.get_mmtype();

    if (mmtype == (slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND)) {
        auto& mnbc_sound = msg_in.get_payload<slac::messages::cm_mnbc_sound_ind>();
        if (!matching_ctx.conforms(msg_in.get_src_mac(), mnbc_sound.run_id)) {
            // invalid message, skip it
            return;
        }
        matching_ctx.received_mnbc_sound = true;
        // handle CM_MNBC_SOUND.IND
    } else if (mmtype == (slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND)) {
        auto& atten_profile = msg_in.get_payload<slac::messages::cm_atten_profile_ind>();
        if (!matching_ctx.conforms(atten_profile.pev_mac, nullptr)) {
            // invalid message, skip it
            return;
        }

        if (atten_profile.num_groups != slac::defs::AAG_LIST_LEN) {
            fmt::print("Mismatch in number of AAG groups\n");
            return;
        }

        for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
            matching_ctx.captured_aags[i] += atten_profile.aag[i];
        }

        matching_ctx.captured_sounds++;
        // handle CM_ATTEN_PROFILE.IND
    } else if (mmtype == (slac::defs::MMTYPE_CM_START_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND)) {
        // FIXME (aw): could we just skip that?
    } else if (mmtype == (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_IND)) {
        // FIXME (aw): how should this be handled?
    } else {
        fmt::print("Received non-expected message of type {:#06x}\n", msg_in.get_mmtype());
        return;
    }

    if (matching_ctx.captured_sounds == slac::defs::CM_SLAC_PARM_CNF_NUM_SOUNDS) {
        fmt::print("Received all sounds\n");
        ctx.submit_event(EventFinishSounding());
    }
}

void EvseFSM::sd_wait_for_atten_char_rsp_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    const auto mmtype = msg_in.get_mmtype();

    if (mmtype == (slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_RSP)) {
        auto& atten_char_rsp = msg_in.get_payload<slac::messages::cm_atten_char_rsp>();
        if (!matching_ctx.conforms(msg_in.get_src_mac(), atten_char_rsp.run_id)) {
            // invalid message, skip it
            return;
        }

        // FIXME (aw): shall we use the source address from the atten_char_rsp message?

        ctx.submit_event(EventAttenCharRspReceived());
    }
}

void EvseFSM::sd_wait_for_slac_match_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    const auto mmtype = msg_in.get_mmtype();

    if (mmtype != (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_REQ)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        return;
    }

    auto& match_req = msg_in.get_payload<slac::messages::cm_slac_match_req>();
    if (!matching_ctx.conforms(msg_in.get_src_mac(), match_req.run_id)) {
        // invalid message, skip it
        return;
    }

    matching_ctx.received_match_req = true;

    // reply with CM_SLAC_MATCH.CNF

    slac::messages::cm_slac_match_cnf match_cnf;
    match_cnf.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    match_cnf.security_type = slac::defs::COMMON_SECURITY_TYPE;
    match_cnf.mvf_length = htole16(slac::defs::CM_SLAC_MATCH_CNF_MVF_LENGTH);
    memcpy(match_cnf.pev_id, match_req.pev_id, sizeof(match_cnf.pev_id));
    memcpy(match_cnf.pev_mac, match_req.pev_mac, sizeof(match_cnf.pev_mac));
    memcpy(match_cnf.evse_id, match_req.evse_id, sizeof(match_cnf.evse_id));
    memcpy(match_cnf.evse_mac, match_req.evse_mac, sizeof(match_cnf.evse_mac));
    memcpy(match_cnf.run_id, match_req.run_id, sizeof(match_cnf.run_id));
    slac::utils::generate_nid_from_nmk(match_cnf.nid, session_nmk);
    memcpy(match_cnf.nmk, session_nmk, sizeof(match_cnf.nmk));

    msg_out.setup_ethernet_header(matching_ctx.ev_mac);
    msg_out.setup_payload(&match_cnf, sizeof(match_cnf),
                          slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_CNF);

    slac_io.send(msg_out);

    ctx.set_next_timeout(slac::defs::TT_MATCH_JOIN_MS);
}

void EvseFSM::sd_reset_hsm(FSMContextType& ctx, const EventSlacMessage& ev) {
    auto& msg_in = ev.data;

    const auto mmtype = msg_in.get_mmtype();

    if (mmtype != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        return;
    }

        ctx.submit_event(EventResetDone());
}

bool EvseFSM::received_slac_match() {
    return matching_ctx.received_match_req;
}

void EvseFSM::set_five_percent_mode(bool value) {
    ac_mode_five_percent = value;
}

void EvseFSM::set_nmk(const uint8_t* nmk) {
    memcpy(session_nmk, nmk, sizeof(session_nmk));
}

bool MatchingSessionContext::conforms(const uint8_t ev_mac[], const uint8_t run_id[]) const {
    if (run_id && memcmp(run_id, this->run_id, sizeof(this->run_id))) {
        fmt::print("Received START_ATTEN_CHAR.IND with mismatching run_id\n");
        return false;
    }

    if (ev_mac && memcmp(ev_mac, this->ev_mac, sizeof(this->ev_mac))) {
        fmt::print("EV MAC mismatch for current matching session\n");
        return false;
    }

    return true;
}

MatchingSessionContext::MatchingSessionContext() {
    memset(captured_aags, 0, sizeof(captured_aags));
}

slac::messages::cm_atten_char_ind MatchingSessionContext::calculate_avg() const {
    slac::messages::cm_atten_char_ind atten_char_ind;

    atten_char_ind.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    atten_char_ind.security_type = slac::defs::COMMON_SECURITY_TYPE;
    memcpy(atten_char_ind.source_address, ev_mac, sizeof(atten_char_ind.source_address));
    memcpy(atten_char_ind.run_id, run_id, sizeof(atten_char_ind.run_id));
    // memcpy(atten_char_ind.source_id, session_ev_id, sizeof(atten_char_ind.source_id));
    memset(atten_char_ind.source_id, 0, sizeof(atten_char_ind.source_id));
    // memcpy(atten_char_ind.resp_id, sample_evse_vin, sizeof(atten_char_ind.resp_id));
    memset(atten_char_ind.resp_id, 0, sizeof(atten_char_ind.resp_id));
    atten_char_ind.num_sounds = captured_sounds;
    atten_char_ind.attenuation_profile.num_groups = slac::defs::AAG_LIST_LEN;
    if (captured_sounds != 0) {
        for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
            atten_char_ind.attenuation_profile.aag[i] = captured_aags[i] / captured_sounds;
        }
    } else {
        // FIXME (aw): what to do here, if we didn't receive any sounds?
        memset(atten_char_ind.attenuation_profile.aag, 0x01, sizeof(atten_char_ind.attenuation_profile.aag));
    }

    return atten_char_ind;
}
