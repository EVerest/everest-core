// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_MATCHING_HANDLE_SLAC_HPP
#define EVSE_SLAC_STATES_MATCHING_HANDLE_SLAC_HPP

#include <everest/slac/fsm/evse/states/matching.hpp>

namespace slac::fsm::evse {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

void session_log(Context& ctx, MatchingSession& session, const LogLevel level, const std::string& text);

} // namespace slac::fsm::evse

#endif // EVSE_SLAC_STATES_MATCHING_HANDLE_SLAC_HPP
