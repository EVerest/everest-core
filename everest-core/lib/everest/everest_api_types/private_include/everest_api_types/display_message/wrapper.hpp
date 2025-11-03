// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/display_message/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/display_message.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::display_message {

using MessagePriorityEnum_Internal = ::types::display_message::MessagePriorityEnum;
using MessagePriorityEnum_External = MessagePriorityEnum;

MessagePriorityEnum_Internal to_internal_api(MessagePriorityEnum_External const& val);
MessagePriorityEnum_External to_external_api(MessagePriorityEnum_Internal const& val);

using MessageStateEnum_Internal = ::types::display_message::MessageStateEnum;
using MessageStateEnum_External = MessageStateEnum;

MessageStateEnum_Internal to_internal_api(MessageStateEnum_External const& val);
MessageStateEnum_External to_external_api(MessageStateEnum_Internal const& val);

using DisplayMessageStatusEnum_Internal = ::types::display_message::DisplayMessageStatusEnum;
using DisplayMessageStatusEnum_External = DisplayMessageStatusEnum;

DisplayMessageStatusEnum_Internal to_internal_api(DisplayMessageStatusEnum_External const& val);
DisplayMessageStatusEnum_External to_external_api(DisplayMessageStatusEnum_Internal const& val);

using ClearMessageResponseEnum_Internal = ::types::display_message::ClearMessageResponseEnum;
using ClearMessageResponseEnum_External = ClearMessageResponseEnum;

ClearMessageResponseEnum_Internal to_internal_api(ClearMessageResponseEnum_External const& val);
ClearMessageResponseEnum_External to_external_api(ClearMessageResponseEnum_Internal const& val);

using Identifier_type_Internal = ::types::display_message::IdentifierType;
using Identifier_type_External = Identifier_type;

Identifier_type_Internal to_internal_api(Identifier_type_External const& val);
Identifier_type_External to_external_api(Identifier_type_Internal const& val);

using DisplayMessage_Internal = ::types::display_message::DisplayMessage;
using DisplayMessage_External = DisplayMessage;

DisplayMessage_Internal to_internal_api(DisplayMessage_External const& val);
DisplayMessage_External to_external_api(DisplayMessage_Internal const& val);

using SetDisplayMessageResponse_Internal = ::types::display_message::SetDisplayMessageResponse;
using SetDisplayMessageResponse_External = SetDisplayMessageResponse;

SetDisplayMessageResponse_Internal to_internal_api(SetDisplayMessageResponse_External const& val);
SetDisplayMessageResponse_External to_external_api(SetDisplayMessageResponse_Internal const& val);

using GetDisplayMessageRequest_Internal = ::types::display_message::GetDisplayMessageRequest;
using GetDisplayMessageRequest_External = GetDisplayMessageRequest;

GetDisplayMessageRequest_Internal to_internal_api(GetDisplayMessageRequest_External const& val);
GetDisplayMessageRequest_External to_external_api(GetDisplayMessageRequest_Internal const& val);

using GetDisplayMessageResponse_Internal = ::types::display_message::GetDisplayMessageResponse;
using GetDisplayMessageResponse_External = GetDisplayMessageResponse;

GetDisplayMessageResponse_Internal to_internal_api(GetDisplayMessageResponse_External const& val);
GetDisplayMessageResponse_External to_external_api(GetDisplayMessageResponse_Internal const& val);

using ClearDisplayMessageRequest_Internal = ::types::display_message::ClearDisplayMessageRequest;
using ClearDisplayMessageRequest_External = ClearDisplayMessageRequest;

ClearDisplayMessageRequest_Internal to_internal_api(ClearDisplayMessageRequest_External const& val);
ClearDisplayMessageRequest_External to_external_api(ClearDisplayMessageRequest_Internal const& val);

using ClearDisplayMessageResponse_Internal = ::types::display_message::ClearDisplayMessageResponse;
using ClearDisplayMessageResponse_External = ClearDisplayMessageResponse;

ClearDisplayMessageResponse_Internal to_internal_api(ClearDisplayMessageResponse_External const& val);
ClearDisplayMessageResponse_External to_external_api(ClearDisplayMessageResponse_Internal const& val);

} // namespace everest::lib::API::V1_0::types::display_message
