// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <string>

#include "log_levels.hpp"

namespace iso15118::io {
void set_logging_callback(const std::function<void(LogLevel, std::string)>&);

} // namespace iso15118::io
