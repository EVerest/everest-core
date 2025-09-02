// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/display_message/API.hpp>

namespace everest::lib::API::V1_0::types::display_message {

using json = nlohmann::json;

void to_json(json& j, MessagePriorityEnum const& k) noexcept;
void from_json(const json& j, MessagePriorityEnum& k);

void to_json(json& j, MessageStateEnum const& k) noexcept;
void from_json(const json& j, MessageStateEnum& k);

void to_json(json& j, DisplayMessageStatusEnum const& k) noexcept;
void from_json(const json& j, DisplayMessageStatusEnum& k);

void to_json(json& j, ClearMessageResponseEnum const& k) noexcept;
void from_json(const json& j, ClearMessageResponseEnum& k);

void to_json(json& j, MessageFormat const& k) noexcept;
void from_json(const json& j, MessageFormat& k);

void to_json(json& j, Identifier_type const& k) noexcept;
void from_json(const json& j, Identifier_type& k);

void to_json(json& j, MessageContent const& k) noexcept;
void from_json(const json& j, MessageContent& k);

void to_json(json& j, DisplayMessage const& k) noexcept;
void from_json(const json& j, DisplayMessage& k);

void to_json(json& j, SetDisplayMessageRequest const& k) noexcept;
void from_json(const json& j, SetDisplayMessageRequest& k);

void to_json(json& j, SetDisplayMessageResponse const& k) noexcept;
void from_json(const json& j, SetDisplayMessageResponse& k);

void to_json(json& j, GetDisplayMessageRequest const& k) noexcept;
void from_json(const json& j, GetDisplayMessageRequest& k);

void to_json(json& j, GetDisplayMessageResponse const& k) noexcept;
void from_json(const json& j, GetDisplayMessageResponse& k);

void to_json(json& j, ClearDisplayMessageRequest const& k) noexcept;
void from_json(const json& j, ClearDisplayMessageRequest& k);

void to_json(json& j, ClearDisplayMessageResponse const& k) noexcept;
void from_json(const json& j, ClearDisplayMessageResponse& k);

} // namespace everest::lib::API::V1_0::types::display_message
