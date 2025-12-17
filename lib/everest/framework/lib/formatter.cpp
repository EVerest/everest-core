// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <utils/formatter.hpp>

#include <fmt/format.h>

namespace everest::formatting {
void throw_format_error(const char* message) {
    throw fmt::format_error(message);
}
} // namespace everest::formatting
