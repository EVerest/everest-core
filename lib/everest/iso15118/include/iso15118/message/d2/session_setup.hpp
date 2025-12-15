// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <array>
#include <optional>
#include <string>

namespace iso15118::d2::msg {

namespace data_types {
constexpr auto EVCC_ID_LENGTH = 6;
using EVCCID = std::array<uint8_t, EVCC_ID_LENGTH>; // hexBinary, max length 6
} // namespace data_types

struct SessionSetupRequest {
    Header header;
    data_types::EVCCID evcc_id;
};

struct SessionSetupResponse {
    Header header;
    data_types::ResponseCode response_code;
    std::string evse_id;
    std::optional<int64_t> timestamp;
};

} // namespace iso15118::d2::msg
