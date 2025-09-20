// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/text_message/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/text_message.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::text_message {

using MessageFormat_Internal = ::types::text_message::MessageFormat;
using MessageFormat_External = MessageFormat;

MessageFormat_Internal to_internal_api(MessageFormat_External const& val);
MessageFormat_External to_external_api(MessageFormat_Internal const& val);

using MessageContent_Internal = ::types::text_message::MessageContent;
using MessageContent_External = MessageContent;

MessageContent_Internal to_internal_api(MessageContent_External const& val);
MessageContent_External to_external_api(MessageContent_Internal const& val);

} // namespace everest::lib::API::V1_0::types::text_message
