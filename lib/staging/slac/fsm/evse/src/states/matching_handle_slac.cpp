// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "matching_handle_slac.hpp"

#include <cstring>

#include "../misc.hpp"

namespace slac::fsm::evse {

void session_log(Context& ctx, MatchingSession& session, const std::string& text) {
    const auto run_id = format_run_id(session.run_id);
    const auto mac = format_mac_addr(session.ev_mac);
    ctx.log_info("Session (run_id=" + run_id + ", ev_mac=" + mac + "): " + text);
}

//
// MatchingSession related
//
static MatchingSession* find_session(std::vector<MatchingSession>& sessions, const uint8_t* ev_mac,
                                     const uint8_t* run_id) {
    for (auto& session : sessions) {
        if (session.is_identified_by(ev_mac, run_id)) {
            return &session;
        }
    }

    return nullptr;
}

static auto create_cm_slac_parm_cnf(const MatchingSession& session) {
    slac::messages::cm_slac_parm_cnf param_confirm;

    memcpy(param_confirm.m_sound_target, slac::defs::BROADCAST_MAC_ADDRESS, sizeof(slac::defs::BROADCAST_MAC_ADDRESS));
    param_confirm.num_sounds = slac::defs::CM_SLAC_PARM_CNF_NUM_SOUNDS;
    param_confirm.timeout = slac::defs::CM_SLAC_PARM_CNF_TIMEOUT;
    param_confirm.resp_type = slac::defs::CM_SLAC_PARM_CNF_RESP_TYPE;
    memcpy(param_confirm.forwarding_sta, session.ev_mac, sizeof(param_confirm.forwarding_sta));
    param_confirm.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    param_confirm.security_type = slac::defs::COMMON_SECURITY_TYPE;
    memcpy(param_confirm.run_id, session.run_id, sizeof(param_confirm.run_id));

    return param_confirm;
}

static auto create_cm_atten_char_ind(const MatchingSession& session, int atten_offset = 0) {
    slac::messages::cm_atten_char_ind atten_char_ind;

    atten_char_ind.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    atten_char_ind.security_type = slac::defs::COMMON_SECURITY_TYPE;
    memcpy(atten_char_ind.source_address, session.ev_mac, sizeof(atten_char_ind.source_address));
    memcpy(atten_char_ind.run_id, session.run_id, sizeof(atten_char_ind.run_id));
    // memcpy(atten_char_ind.source_id, session_ev_id, sizeof(atten_char_ind.source_id));
    memset(atten_char_ind.source_id, 0, sizeof(atten_char_ind.source_id));
    // memcpy(atten_char_ind.resp_id, sample_evse_vin, sizeof(atten_char_ind.resp_id));
    memset(atten_char_ind.resp_id, 0, sizeof(atten_char_ind.resp_id));
    atten_char_ind.num_sounds = session.captured_sounds;
    atten_char_ind.attenuation_profile.num_groups = slac::defs::AAG_LIST_LEN;
    if (session.captured_sounds != 0) {
        for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
            atten_char_ind.attenuation_profile.aag[i] =
                session.captured_aags[i] / session.captured_sounds + atten_offset;
        }
    } else {
        // FIXME (aw): what to do here, if we didn't receive any sounds?
        memset(atten_char_ind.attenuation_profile.aag, 0x01, sizeof(atten_char_ind.attenuation_profile.aag));
    }

    return atten_char_ind;
}

static auto create_cm_slac_match_cnf(const MatchingSession& session, const slac::messages::cm_slac_match_req& match_req,
                                     const uint8_t* session_nmk) {
    slac::messages::cm_slac_match_cnf match_cnf;

    match_cnf.application_type = slac::defs::COMMON_APPLICATION_TYPE;
    match_cnf.security_type = slac::defs::COMMON_SECURITY_TYPE;
    match_cnf.mvf_length = htole16(slac::defs::CM_SLAC_MATCH_CNF_MVF_LENGTH);
    memcpy(match_cnf.pev_id, match_req.pev_id, sizeof(match_cnf.pev_id));
    memcpy(match_cnf.pev_mac, match_req.pev_mac, sizeof(match_cnf.pev_mac));
    memcpy(match_cnf.evse_id, match_req.evse_id, sizeof(match_cnf.evse_id));
    memcpy(match_cnf.evse_mac, match_req.evse_mac, sizeof(match_cnf.evse_mac));
    memcpy(match_cnf.run_id, match_req.run_id, sizeof(match_cnf.run_id));
    memset(match_cnf._rerserved, 0, 8);
    match_cnf._reserved2 = 0;
    slac::utils::generate_nid_from_nmk(match_cnf.nid, session_nmk);
    memcpy(match_cnf.nmk, session_nmk, sizeof(match_cnf.nmk));

    return match_cnf;
}

void MatchingState::handle_slac_message(slac::messages::HomeplugMessage& msg) {
    const auto mmtype = msg.get_mmtype();
    tmp_ev_mac = msg.get_src_mac();

    switch (mmtype) {
    case (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_REQ):
        handle_cm_slac_parm_req(msg.get_payload<slac::messages::cm_slac_parm_req>());
        break;
    case (slac::defs::MMTYPE_CM_START_ATTEN_CHAR | slac::defs::MMTYPE_MODE_IND):
        handle_cm_start_atten_char_ind(msg.get_payload<slac::messages::cm_start_atten_char_ind>());
        break;
    case (slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND):
        handle_cm_mnbc_sound_ind(msg.get_payload<slac::messages::cm_mnbc_sound_ind>());
        break;
    case (slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND):
        handle_cm_atten_profile_ind(msg.get_payload<slac::messages::cm_atten_profile_ind>());
        break;
    case (slac::defs::MMTYPE_CM_ATTEN_CHAR | slac::defs::MMTYPE_MODE_RSP):
        handle_cm_atten_char_rsp(msg.get_payload<slac::messages::cm_atten_char_rsp>());
        break;
    case (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_REQ):
        handle_cm_slac_match_req(msg.get_payload<slac::messages::cm_slac_match_req>());
        break;
    case (slac::defs::MMTYPE_CM_VALIDATE | slac::defs::MMTYPE_MODE_REQ):
        handle_cm_validate_req(msg.get_payload<slac::messages::cm_validate_req>());
        break;
    default:
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
    }
}

void MatchingState::handle_cm_slac_parm_req(const slac::messages::cm_slac_parm_req& msg) {
    // set this flag to true, to disable the retry timeout
    seen_slac_parm_req = true;

    auto session = find_session(sessions, tmp_ev_mac, msg.run_id);
    if (session) {
        // the matching session existed already, according to [V2G3-A09-16] we should restart
        *session = MatchingSession(tmp_ev_mac, msg.run_id);
    } else {
        // the session didn't exist, lets create it
        sessions.emplace_back(MatchingSession{tmp_ev_mac, msg.run_id});
        session = &sessions.back();
    }

    session_log(ctx, *session, "initialized, waiting for CM_START_ATTEN_CHAR_IND");

    // timeout until we need to get cm_start_atten_char_ind
    session->set_next_timeout(slac::defs::TT_MATCH_SEQUENCE_MS);

    auto param_confirm = create_cm_slac_parm_cnf(*session);

    ctx.send_slac_message(param_confirm.forwarding_sta, param_confirm);
    ctx.signal_cm_slac_parm_req(tmp_ev_mac);
}

void MatchingState::handle_cm_start_atten_char_ind(const slac::messages::cm_start_atten_char_ind& msg) {
    auto session = find_session(sessions, tmp_ev_mac, msg.run_id);

    if (!session) {
        ctx.log_info("No session found for CM_START_ATTEN_CHAR_IND");
        return;
    }

    if (session->state != MatchingSubState::WAIT_FOR_START_ATTEN_CHAR) {
        session_log(ctx, *session, "needs to be in state WAIT_FOR_START_ATTEN_CHAR for CM_START_ATTEN_CHAR_IND");
        return;
    }

    // go to sounding
    session_log(ctx, *session, "received CM_START_ATTEN_CHAR_IND, going to substate SOUNDING");
    session->state = MatchingSubState::SOUNDING;
    session->set_next_timeout(slac::defs::TT_EVSE_MATCH_MNBC_MS);
}

void MatchingState::handle_cm_mnbc_sound_ind(const slac::messages::cm_mnbc_sound_ind& msg) {
    auto session = find_session(sessions, tmp_ev_mac, msg.run_id);

    if (!session) {
        ctx.log_info("No session found for CM_MNNC_SOUND_IND");
        return;
    }

    if (session->state != MatchingSubState::SOUNDING) {
        session_log(ctx, *session, "needs to be in state SOUNDING for CM_MNNC_SOUND_IND");
        return;
    }

    session_log(ctx, *session, "received CM_MNNC_SOUND_IND");

    session->received_mnbc_sound = true;
}

void MatchingState::handle_cm_atten_profile_ind(const slac::messages::cm_atten_profile_ind& msg) {
    // cm_atten_profile_ind does not carry a run_id, so we can't exactly identify the session
    // FIXME (aw): for now, we only take the first one found
    MatchingSession* session = nullptr;

    for (auto& session_i : sessions) {
        if (memcmp(msg.pev_mac, session_i.ev_mac, sizeof(msg.pev_mac)) == 0) {
            session = &session_i;
        }
    }

    if (!session) {
        ctx.log_info("No session found for CM_ATTEN_PROFILE_IND");
        return;
    }

    if (session->state != MatchingSubState::SOUNDING) {
        session_log(ctx, *session, "needs to be in state SOUNDING for CM_ATTEN_PROFILE_IND");
        return;
    }

    if (msg.num_groups != slac::defs::AAG_LIST_LEN) {
        session_log(ctx, *session, "mismatch in number of AAG groups");
        return;
    }

    session_log(ctx, *session, "received CM_ATTEN_PROFILE_IND");

    for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
        session->captured_aags[i] += msg.aag[i];
    }

    session->captured_sounds++;

    if (session->captured_sounds < slac::defs::CM_SLAC_PARM_CNF_NUM_SOUNDS) {
        return;
    }

    // fall-through: all sounds captured
    session_log(ctx, *session, "received all sounds, going to substate FINALIZE_SOUNDING");
    session->state = MatchingSubState::FINALIZE_SOUNDING;
    session->set_next_timeout(FINALIZE_SOUNDING_DELAY_MS);
}

void MatchingState::handle_cm_atten_char_rsp(const slac::messages::cm_atten_char_rsp& msg) {
    auto session = find_session(sessions, tmp_ev_mac, msg.run_id);

    if (!session) {
        ctx.log_info("No session found for CM_ATTEN_CHAR_RSP");
        return;
    }

    if (session->state != MatchingSubState::WAIT_FOR_ATTEN_CHAR_RSP) {
        session_log(ctx, *session, "needs to be in state WAIT_FOR_ATTEN_CHAR_RSP for CM_ATTEN_CHAR_RSP");
        return;
    }

    session_log(ctx, *session, "received CM_ATTEN_CHAR_RSP, going to substate WAIT_FOR_SLAC_MATCH");
    session->state = MatchingSubState::WAIT_FOR_SLAC_MATCH;

    // FIXME (aw): referring to the standard, it is not clear here, if we should offset from TT_EVSE_MATCH_MNBC
    session->set_next_timeout(slac::defs::TT_EVSE_MATCH_SESSION_MS);
}

void MatchingState::handle_cm_validate_req(const slac::messages::cm_validate_req& msg) {
    // NOTE: CM_VALIDATE.REQ does not specify its session
    ctx.log_info("Received CM_VALIDATE.REQ / not implemented - will return failure code");

    slac::messages::cm_validate_cnf validate_cnf;
    validate_cnf.signal_type = slac::defs::CM_VALIDATE_REQ_SIGNAL_TYPE;
    validate_cnf.toggle_num = 0;
    validate_cnf.result = slac::defs::CM_VALIDATE_REQ_RESULT_FAILURE;

    ctx.send_slac_message(tmp_ev_mac, validate_cnf);
}

void MatchingState::handle_cm_slac_match_req(const slac::messages::cm_slac_match_req& msg) {
    auto session = find_session(sessions, tmp_ev_mac, msg.run_id);

    if (!session) {
        ctx.log_info("No session found for CM_SLAC_MATCH_REQ");
        return;
    }

    if (session->state != MatchingSubState::WAIT_FOR_SLAC_MATCH) {
        session_log(ctx, *session, "needs to be in state WAIT_FOR_SLAC_MATCH for CM_SLAC_MATCH_REQ");
        return;
    }

    session_log(ctx, *session, "Received CM_SLAC_MATCH_REQ, sending CM_SLAC_MATCH_CNF -> session complete");

    if (not ctx.slac_config.link_status.debug_simulate_failed_matching) {
        auto match_confirm = create_cm_slac_match_cnf(*session, msg, ctx.slac_config.session_nmk);
        ctx.send_slac_message(tmp_ev_mac, match_confirm);
    } else {
        ctx.log_info("Sending wrong NMK to EV to simulate a failed link setup after match request");
        uint8_t wrong_session_nmk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                         0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
        auto match_confirm = create_cm_slac_match_cnf(*session, msg, wrong_session_nmk);
        ctx.send_slac_message(tmp_ev_mac, match_confirm);
    }

    session->state = MatchingSubState::MATCH_COMPLETE;

    // call this immediately again in MatchedState::callback to handle things
    session->set_next_timeout(0);
    ctx.signal_cm_slac_match_cnf(tmp_ev_mac);
}

void MatchingState::finalize_sounding(MatchingSession& session) {
    session_log(ctx, session, "Finalize sounding, sending CM_ATTEN_CHAR_IND");
    session.state = MatchingSubState::WAIT_FOR_ATTEN_CHAR_RSP;

    auto atten_char = create_cm_atten_char_ind(session, ctx.slac_config.sounding_atten_adjustment);

    ctx.send_slac_message(session.ev_mac, atten_char);

    session.set_next_timeout(slac::defs::TT_MATCH_RESPONSE_MS);
}

} // namespace slac::fsm::evse
