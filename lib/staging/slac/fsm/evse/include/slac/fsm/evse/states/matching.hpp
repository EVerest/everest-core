// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_MATCHING_HPP
#define EVSE_SLAC_STATES_MATCHING_HPP

#include <chrono>
#include <memory>

#include <slac/slac.hpp>

#include "../fsm.hpp"

namespace slac::fsm::evse {

enum class MatchingSubState {
    WAIT_FOR_START_ATTEN_CHAR,
    SOUNDING,
    FINALIZE_SOUNDING,
    WAIT_FOR_ATTEN_CHAR_RSP,
    WAIT_FOR_SLAC_MATCH,
    RECEIVED_SLAC_MATCH,
    MATCH_COMPLETE,
    FAILED,
};

constexpr auto FINALIZE_SOUNDING_DELAY_MS = 45;

using MatchingTimepoint = std::chrono::time_point<std::chrono::steady_clock>;

struct MatchingSession {
    MatchingSession(const uint8_t* ev_mac, const uint8_t* run_id);

    // common session related
    MatchingSubState state{MatchingSubState::WAIT_FOR_START_ATTEN_CHAR};
    uint8_t ev_mac[ETH_ALEN];
    uint8_t run_id[slac::defs::RUN_ID_LEN];

    // timeout related
    MatchingTimepoint next_timeout;
    bool timeout_active{false};

    // sounding related
    int captured_sounds{0};
    int captured_aags[slac::defs::AAG_LIST_LEN];
    bool received_mnbc_sound{false};

    int num_retries{0};

    // helper functions
    void set_next_timeout(int delay_ms);
    void ack_timeout();
    bool is_identified_by(const uint8_t* ev_mac, const uint8_t* run_id) const;
    slac::messages::cm_atten_char_ind calculate_avg() const;
};

struct MatchingState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    std::vector<MatchingSession> sessions;

    // FIXME (aw): this should be const ref, but some of the member functions of HomeplugMessage are not const'd
    void handle_slac_message(slac::messages::HomeplugMessage&);

    void handle_cm_slac_parm_req(const slac::messages::cm_slac_parm_req&);
    void handle_cm_start_atten_char_ind(const slac::messages::cm_start_atten_char_ind&);
    void handle_cm_mnbc_sound_ind(const slac::messages::cm_mnbc_sound_ind&);
    void handle_cm_atten_profile_ind(const slac::messages::cm_atten_profile_ind&);
    void handle_cm_atten_char_rsp(const slac::messages::cm_atten_char_rsp&);
    void handle_cm_validate_req(const slac::messages::cm_validate_req&);
    void handle_cm_slac_match_req(const slac::messages::cm_slac_match_req&);

    void finalize_sounding(MatchingSession& session);

    // FIXME (aw): this should be wrapped somewhere else
    const uint8_t* tmp_ev_mac;

    MatchingTimepoint timeout_slac_parm_req;

    bool seen_slac_parm_req{false};
    int num_retries{0};

    std::unique_ptr<slac::messages::cm_slac_match_cnf> match_cnf_message;
};

} // namespace slac::fsm::evse

#endif // EVSE_SLAC_STATES_MATCHING_HPP
