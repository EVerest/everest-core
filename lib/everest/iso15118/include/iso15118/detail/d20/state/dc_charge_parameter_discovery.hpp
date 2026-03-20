// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/d20/config.hpp>
#include <iso15118/d20/limits.hpp>
#include <iso15118/d20/session.hpp>
#include <iso15118/message/dc_charge_parameter_discovery.hpp>

namespace iso15118::d20::state {

bool handle_compatibility_check(const d20::DcTransferLimits& evse_dc_limits,
                                const std::variant<message_20::datatypes::DC_CPDReqEnergyTransferMode,
                                                   message_20::datatypes::BPT_DC_CPDReqEnergyTransferMode>& ev_limits,
                                d20::DcTransferLimits& out_limits);

message_20::DC_ChargeParameterDiscoveryResponse
handle_request(const message_20::DC_ChargeParameterDiscoveryRequest& req, const d20::Session& session,
               const d20::DcTransferLimits& dc_limits);

} // namespace iso15118::d20::state
