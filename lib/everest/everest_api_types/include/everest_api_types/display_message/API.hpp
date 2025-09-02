// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::display_message {

enum class MessagePriorityEnum {
    AlwaysFront,
    InFront,
    NormalCycle,
};

enum class MessageStateEnum {
    Charging,
    Faulted,
    Idle,
    Unavailable,
    Suspending,
    Discharging,
};

enum class DisplayMessageStatusEnum {
    Accepted,
    NotSupportedMessageFormat,
    Rejected,
    NotSupportedPriority,
    NotSupportedState,
    UnknownTransaction,
};

enum class ClearMessageResponseEnum {
    Accepted,
    Unknown,
};

enum class MessageFormat {
    ASCII,
    HTML,
    URI,
    UTF8,
    QRCODE,
};

enum class Identifier_type {
    IdToken,
    SessionId,
    TransactionId,
};

struct MessageContent {
    std::string content;
    std::optional<MessageFormat> format;
    std::optional<std::string> language;
};

struct DisplayMessage {
    MessageContent message;
    std::optional<int32_t> id;
    std::optional<MessagePriorityEnum> priority;
    std::optional<MessageStateEnum> state;
    std::optional<std::string> timestamp_from;
    std::optional<std::string> timestamp_to;
    std::optional<std::string> identifier_id;
    std::optional<Identifier_type> identifier_type;
    std::optional<std::string> qr_code;
};

struct SetDisplayMessageRequest {
    std::vector<DisplayMessage> display_messages;
};

struct SetDisplayMessageResponse {
    DisplayMessageStatusEnum status;
    std::optional<std::string> status_info;
};

struct GetDisplayMessageRequest {
    std::optional<std::vector<int32_t>> id;
    std::optional<MessagePriorityEnum> priority;
    std::optional<MessageStateEnum> state;
};

struct GetDisplayMessageResponse {
    std::optional<std::string> status_info;
    std::optional<std::vector<DisplayMessage>> messages;
};

struct ClearDisplayMessageRequest {
    int32_t id;
};

struct ClearDisplayMessageResponse {
    ClearMessageResponseEnum status;
    std::optional<std::string> status_info;
};

} // namespace everest::lib::API::V1_0::types::display_message
