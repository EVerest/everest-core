// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/display_message/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/display_message.hpp"
#include "generated/types/text_message.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::display_message {

using MessagePriorityEnum_Internal = ::types::display_message::MessagePriorityEnum;
using MessagePriorityEnum_External = MessagePriorityEnum;

MessagePriorityEnum_Internal toInternalApi(MessagePriorityEnum_External const& val);
MessagePriorityEnum_External toExternalApi(MessagePriorityEnum_Internal const& val);

using MessageStateEnum_Internal = ::types::display_message::MessageStateEnum;
using MessageStateEnum_External = MessageStateEnum;

MessageStateEnum_Internal toInternalApi(MessageStateEnum_External const& val);
MessageStateEnum_External toExternalApi(MessageStateEnum_Internal const& val);

using DisplayMessageStatusEnum_Internal = ::types::display_message::DisplayMessageStatusEnum;
using DisplayMessageStatusEnum_External = DisplayMessageStatusEnum;

DisplayMessageStatusEnum_Internal toInternalApi(DisplayMessageStatusEnum_External const& val);
DisplayMessageStatusEnum_External toExternalApi(DisplayMessageStatusEnum_Internal const& val);

using ClearMessageResponseEnum_Internal = ::types::display_message::ClearMessageResponseEnum;
using ClearMessageResponseEnum_External = ClearMessageResponseEnum;

ClearMessageResponseEnum_Internal toInternalApi(ClearMessageResponseEnum_External const& val);
ClearMessageResponseEnum_External toExternalApi(ClearMessageResponseEnum_Internal const& val);

using MessageFormat_Internal = ::types::text_message::MessageFormat;
using MessageFormat_External = MessageFormat;

MessageFormat_Internal toInternalApi(MessageFormat_External const& val);
MessageFormat_External toExternalApi(MessageFormat_Internal const& val);

using Identifier_type_Internal = ::types::display_message::IdentifierType;
using Identifier_type_External = Identifier_type;

Identifier_type_Internal toInternalApi(Identifier_type_External const& val);
Identifier_type_External toExternalApi(Identifier_type_Internal const& val);

using MessageContent_Internal = ::types::text_message::MessageContent;
using MessageContent_External = MessageContent;

MessageContent_Internal toInternalApi(MessageContent_External const& val);
MessageContent_External toExternalApi(MessageContent_Internal const& val);

using DisplayMessage_Internal = ::types::display_message::DisplayMessage;
using DisplayMessage_External = DisplayMessage;

DisplayMessage_Internal toInternalApi(DisplayMessage_External const& val);
DisplayMessage_External toExternalApi(DisplayMessage_Internal const& val);

using SetDisplayMessageResponse_Internal = ::types::display_message::SetDisplayMessageResponse;
using SetDisplayMessageResponse_External = SetDisplayMessageResponse;

SetDisplayMessageResponse_Internal toInternalApi(SetDisplayMessageResponse_External const& val);
SetDisplayMessageResponse_External toExternalApi(SetDisplayMessageResponse_Internal const& val);

using GetDisplayMessageRequest_Internal = ::types::display_message::GetDisplayMessageRequest;
using GetDisplayMessageRequest_External = GetDisplayMessageRequest;

GetDisplayMessageRequest_Internal toInternalApi(GetDisplayMessageRequest_External const& val);
GetDisplayMessageRequest_External toExternalApi(GetDisplayMessageRequest_Internal const& val);

using GetDisplayMessageResponse_Internal = ::types::display_message::GetDisplayMessageResponse;
using GetDisplayMessageResponse_External = GetDisplayMessageResponse;

GetDisplayMessageResponse_Internal toInternalApi(GetDisplayMessageResponse_External const& val);
GetDisplayMessageResponse_External toExternalApi(GetDisplayMessageResponse_Internal const& val);

using ClearDisplayMessageRequest_Internal = ::types::display_message::ClearDisplayMessageRequest;
using ClearDisplayMessageRequest_External = ClearDisplayMessageRequest;

ClearDisplayMessageRequest_Internal toInternalApi(ClearDisplayMessageRequest_External const& val);
ClearDisplayMessageRequest_External toExternalApi(ClearDisplayMessageRequest_Internal const& val);

using ClearDisplayMessageResponse_Internal = ::types::display_message::ClearDisplayMessageResponse;
using ClearDisplayMessageResponse_External = ClearDisplayMessageResponse;

ClearDisplayMessageResponse_Internal toInternalApi(ClearDisplayMessageResponse_External const& val);
ClearDisplayMessageResponse_External toExternalApi(ClearDisplayMessageResponse_Internal const& val);

} // namespace everest::lib::API::V1_0::types::display_message
