// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <slac/fsm/evse/states/matching.hpp>

#include <cstring>
#include <optional>

#include "matching_handle_slac.hpp"
#include <slac/fsm/evse/states/others.hpp>

namespace slac::fsm::evse {

//
// Helper functions
//
static inline auto remaining_milliseconds(const MatchingTimepoint& timeout, const MatchingTimepoint& now) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(timeout - now).count();
}

MatchingSession::MatchingSession(const uint8_t* ev_mac, const uint8_t* run_id) {
    memcpy(this->ev_mac, ev_mac, sizeof(this->ev_mac));
    memcpy(this->run_id, run_id, sizeof(this->run_id));
    memset(captured_aags, 0, sizeof(captured_aags));
}

void MatchingSession::set_next_timeout(int delay_ms) {
    next_timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms);
    timeout_active = true;
}

void MatchingSession::ack_timeout() {
    timeout_active = false;
}

bool MatchingSession::is_identified_by(const uint8_t* ev_mac, const uint8_t* run_id) const {
    if (0 != memcmp(run_id, this->run_id, sizeof(this->run_id))) {
        return false;
    }

    if (0 != memcmp(ev_mac, this->ev_mac, sizeof(this->ev_mac))) {
        return false;
    }

    return true;
}

bool all_sessions_failed(const std::vector<MatchingSession>& sessions) {
    for (const auto& session : sessions) {
        if (session.state != MatchingSubState::FAILED) {
            return false;
        }
    }

    return true;
}

//
// Matching state related
//
void MatchingState::enter() {
    ctx.signal_state("MATCHING");
    ctx.log_info("Entered Matching state, waiting for CM_SLAC_PARM_REQ");
    // timeout for getting CM_SLAC_PARM_REQ
    timeout_slac_parm_req =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(slac::defs::TT_EVSE_SLAC_INIT_MS);
}

FSMSimpleState::CallbackReturnType MatchingState::callback() {
    // check timeouts
    auto now_tp = std::chrono::steady_clock::now();
    std::optional<FSMReturnType> call_back_ms;

    if (!seen_slac_parm_req) {
        if (now_tp >= timeout_slac_parm_req) {
            return Event::RETRY_MATCHING;
        }

        call_back_ms = remaining_milliseconds(timeout_slac_parm_req, now_tp);
    }

    // fallthrough: CM_SLAC_PARM_REQ has been seen, check individual sessions

    for (auto& session : sessions) {
        // there should always be an active timeout, right?
        // FIXME (aw)
        while (session.timeout_active) {
            // FIXME (aw): this way we only take the first one
            if (session.state == MatchingSubState::MATCH_COMPLETE) {
                return Event::MATCH_COMPLETE;
            }

            auto remaining_ms = remaining_milliseconds(session.next_timeout, now_tp);
            if (remaining_ms > 0) {
                if (call_back_ms.has_value() == false || *call_back_ms > remaining_ms) {
                    call_back_ms = remaining_ms;
                }
                break;
            }

            // fall-through, timeout should be handled now
            session.ack_timeout();

            if (session.state == MatchingSubState::WAIT_FOR_START_ATTEN_CHAR) {
                session_log(ctx, session, "Waiting for CM_START_ATTEN_CHAR_IND timeouted -> failed");
                session.state = MatchingSubState::FAILED;
            } else if (session.state == MatchingSubState::SOUNDING) {
                session_log(ctx, session,
                            "Sounding not yet complete but timeouted, going to sub-state FINALIZE_SOUNDING");
                session.state = MatchingSubState::FINALIZE_SOUNDING;
                session.set_next_timeout(FINALIZE_SOUNDING_DELAY_MS);
            } else if (session.state == MatchingSubState::FINALIZE_SOUNDING) {
                finalize_sounding(session);
            } else if (session.state == MatchingSubState::WAIT_FOR_ATTEN_CHAR_RSP) {
                session_log(ctx, session, "Waiting for CM_ATTEN_CHAR_RSP timeouted -> failed");
                session.state = MatchingSubState::FAILED;
            } else if (session.state == MatchingSubState::WAIT_FOR_SLAC_MATCH) {
                session_log(ctx, session, "Wating for CM_SLAC_MATCH_REQ timeouted -> failed");
                session.state = MatchingSubState::FAILED;
            }
        }

        if (all_sessions_failed(sessions)) {
            return Event::FAILED;
        }
    }

    if (call_back_ms.has_value() == false) {
        // FIXME (aw): this should not happen, should we assert here or something similar?
    }
    return *call_back_ms;
}

FSMSimpleState::HandleEventReturnType MatchingState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        handle_slac_message(ctx.slac_message_payload);
        return sa.HANDLED_INTERNALLY;
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else if (ev == Event::MATCH_COMPLETE) {
        // Wait for link up to be confirmed before going to MATCHED state if enabled in config
        if (ctx.slac_config.link_status.do_detect) {
            return sa.create_simple<WaitForLinkState>(ctx);
        } else {
            return sa.create_simple<MatchedState>(ctx);
        }
    } else if (ev == Event::RETRY_MATCHING) {
        num_retries++;
        if (num_retries == slac::defs::C_EV_MATCH_RETRY) {
            ctx.log_info("Reached retry limit for matching");
            return sa.create_simple<FailedState>(ctx);
        }

        // otherwise, reset timeout
        timeout_slac_parm_req =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(slac::defs::TT_EVSE_SLAC_INIT_MS);
        return sa.HANDLED_INTERNALLY;
    } else if (ev == Event::FAILED) {
        return sa.create_simple<FailedState>(ctx);
    }

    return sa.PASS_ON;
}

} // namespace slac::fsm::evse
