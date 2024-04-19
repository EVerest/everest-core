// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <string>

namespace iso15118::io {
void set_logging_callback(const std::function<void(std::string)>&);

} // namespace iso15118::io
