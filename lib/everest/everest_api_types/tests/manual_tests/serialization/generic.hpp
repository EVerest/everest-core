// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include "api_types/generic/API.hpp"

template <>
everest::lib::API::V1_0::types::generic::RequestReply
generate<everest::lib::API::V1_0::types::generic::RequestReply>(bool set_optional_fields);

template <>
void verify<everest::lib::API::V1_0::types::generic::RequestReply>(
    everest::lib::API::V1_0::types::generic::RequestReply original,
    everest::lib::API::V1_0::types::generic::RequestReply result);
