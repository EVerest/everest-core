// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::generic {

std::string trimmed(std::string const& str);
std::optional<std::string> compress_json(std::string data);

} // namespace everest::lib::API::V1_0::types::generic
