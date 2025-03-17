// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/session/feedback.hpp>

namespace iso15118::d20 {

// Holds information reported by the EV
struct EVSessionInfo {
    session::feedback::EvTransferLimits ev_transfer_limits;
    session::feedback::EvSEControlMode ev_control_mode;
};

} // namespace iso15118::d20
