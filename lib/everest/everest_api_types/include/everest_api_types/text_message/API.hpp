// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::text_message {

enum class MessageFormat {
    ASCII,
    HTML,
    URI,
    UTF8,
    QRCODE,
};

struct MessageContent {
    std::string content;
    std::optional<MessageFormat> format;
    std::optional<std::string> language;
};

} // namespace everest::lib::API::V1_0::types::text_message