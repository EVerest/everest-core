// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/common_types.hpp>
#include <iso15118/session/feedback.hpp>

namespace iso15118::d20 {

// Holds information reported by the EV
struct EVSessionInfo {
    session::feedback::EvTransferLimits ev_transfer_limits;
    session::feedback::EvSEControlMode ev_control_mode;
    std::vector<message_20::datatypes::ServiceCategory> ev_energy_services;
};

} // namespace iso15118::d20
