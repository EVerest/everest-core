// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <optional>
#include <string>

namespace iso15118::d2::msg {

namespace data_types {} // namespace data_types

struct AuthorizationRequest {
    Header header;
    std::string id;
    std::optional<data_types::GEN_CHALLENGE> gen_challenge;
};

struct AuthorizationResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::EVSEProcessing evse_processing;
};

} // namespace iso15118::d2::msg
