// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_FSM_HPP
#define EVSE_FSM_HPP

#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <string>

#include <fsm/sync.hpp>
#include <fsm/utils/Identifiable.hpp>

#include "slac_io.hpp"

// FIXME (aw): could we separate out the internal events, somehow?
enum class EventID {
    Reset,
    ResetDone,
    EnterBCD,
    LeaveBCD,
    SlacMessage,
    InitTimout,
    StartMatching,
    StartSounding,
    FinishSounding,
    AttenCharRspReceived,
    LinkDetected,
    MatchingFailed,
};

using EventTypeFactory = fsm::utils::IdentifiableTypeFactory<EventID>;

using EventReset = EventTypeFactory::Derived<EventID::Reset>;
using EventEnterBCD = EventTypeFactory::Derived<EventID::EnterBCD>;
using EventLeaveBCD = EventTypeFactory::Derived<EventID::LeaveBCD>;
using EventSlacMessage = EventTypeFactory::Derived<EventID::SlacMessage, slac::messages::HomeplugMessage&>;
using EventLinkDetected = EventTypeFactory::Derived<EventID::LinkDetected>;

enum class State {
    Reset,
    Idle,
    WaitForMatchingStart,
    Matching,
    Sounding,
    DoAttenChar,
    WaitForSlacMatch,
    ReceivedSlacMatch,
    Matched,
    SignalError,
    NoSlacPerformed,
    MatchingFailed,
};

struct StateIDType {
    const State id;
    const std::string name;
};

// FIXME (aw): could be probably forward declared
struct MatchingSessionContext {
    MatchingSessionContext();

    // FIXME (aw): might return more information on what mismatched
    bool conforms(const uint8_t ev_mac[], const uint8_t run_id[]) const;

    uint8_t ev_mac[ETH_ALEN];
    uint8_t run_id[slac::defs::RUN_ID_LEN];

    int captured_sounds{0};
    int captured_aags[slac::defs::AAG_LIST_LEN];
    bool received_mnbc_sound{false};
    std::chrono::time_point<date::utc_clock> tp_sound_start;

    slac::messages::cm_atten_char_ind calculate_avg() const;
};

using EventBufferType = std::aligned_union_t<0, EventSlacMessage>;
using EventBaseType = EventTypeFactory::Base;

class EvseFSM {
public:
    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::sync::StateHandle<EventInfoType, StateIDType>;
    using TransitionType = StateHandleType::TransitionWrapperType;
    using FSMInitContextType = StateHandleType::FSMInitContextType;
    using FSMContextType = StateHandleType::FSMContextType;

    // declare state descriptors
    StateHandleType& get_initial_state() {
        return sd_reset;
    }

    void set_five_percent_mode(bool value);
    void generate_nmk();

    explicit EvseFSM(SlacIO& slac_io, int _set_key_timeout);

private:
    using SlacMsgType = slac::messages::HomeplugMessage;

    StateHandleType sd_reset{{State::Reset, "Reset"}};

    StateHandleType sd_idle{{State::Idle, "Idle"}};

    StateHandleType sd_wait_for_matching_start{{State::WaitForMatchingStart, "WaitForMatchingStart"}};

    StateHandleType sd_matching{{State::Matching, "Matching"}};
    StateHandleType sd_sounding{{State::Sounding, "Sounding"}};

    struct DoAttenCharStateType : public StateHandleType {
        using StateHandleType::StateHandleType;
        int ind_msg_count{0};
    } sd_do_atten_char{{State::DoAttenChar, "DoAttenChar"}};

    // FIXME (aw): this state should also handle the CM_VALIDATE.REQ
    StateHandleType sd_wait_for_slac_match{{State::WaitForSlacMatch, "WaitForSlacMatch"}};

    StateHandleType sd_received_slac_match{{State::ReceivedSlacMatch, "ReceivedSlacMatch"}};

    StateHandleType sd_matched{{State::Matched, "Matched"}};

    StateHandleType sd_signal_error{{State::SignalError, "SignalError"}};
    StateHandleType sd_no_slac_performed{{State::NoSlacPerformed, "NoSlacPerformed"}};
    StateHandleType sd_matching_failed{{State::MatchingFailed, "MatchingFailed"}};

    // declare transitions
    // StateHandleType& t_init_timeout();

    void default_transition(const EventBaseType& ev, TransitionType& trans);

    bool check_valid_slac_parm_req();
    void sd_wait_for_matching_hsm(FSMContextType& ctx, const EventSlacMessage& ev);
    void sd_matching_hsm(FSMContextType& ctx, const EventSlacMessage& ev);
    void sd_sounding_hsm(FSMContextType& ctx, const EventSlacMessage& ev);
    void sd_wait_for_atten_char_rsp_hsm(FSMContextType& ctx, const EventSlacMessage& ev);
    void sd_wait_for_slac_match_hsm(TransitionType& trans, const EventSlacMessage& ev);
    void sd_reset_hsm(FSMContextType& ctx, const EventSlacMessage& ev);

    MatchingSessionContext matching_ctx;
    SlacMsgType msg_out;

    uint8_t session_nmk[slac::defs::NMK_LEN];
    uint8_t plc_peer_mac[ETH_ALEN] = {0x00, 0xB0, 0x52, 0x00, 0x00, 0x01};

    bool ac_mode_five_percent{true};
    const int MAX_INIT_RETRIES = 3; // this is C_sequ_retry
    int init_retry_count = 0;

    SlacIO& slac_io;
    int set_key_timeout;
};

#endif // EVSE_FSM_HPP
