// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::generic {

enum class ErrorEnum {
    CommunicationFault,
    VendorError,
    VendorWarning
};

struct Error {
    ErrorEnum type;
    std::optional<std::string> sub_type;
    std::optional<std::string> message;
};

struct RequestReply {
    std::string replyTo;
    std::string payload;
};

} // namespace everest::lib::API::V1_0::types::generic
