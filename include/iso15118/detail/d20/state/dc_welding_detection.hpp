// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/d20/session.hpp>
#include <iso15118/message/dc_welding_detection.hpp>

namespace iso15118::d20::state {

message_20::DC_WeldingDetectionResponse handle_request(const message_20::DC_WeldingDetectionRequest& req,
                                                       const d20::Session& session, const float present_voltage);

} // namespace iso15118::d20::state
