// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/d20/ac_powers.hpp>
#include <iso15118/d20/config.hpp>
#include <iso15118/d20/session.hpp>
#include <iso15118/message/ac_charge_parameter_discovery.hpp>

namespace iso15118::d20::state {

message_20::AC_ChargeParameterDiscoveryResponse
handle_request(const message_20::AC_ChargeParameterDiscoveryRequest& req, const d20::Session& session,
               const d20::AcTransferLimits& limits, const d20::AcPresentPower& powers);

} // namespace iso15118::d20::state
