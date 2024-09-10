// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

namespace iso15118::io {

void log_and_raise_mbed_error(const std::string& error_msg, int error_code);

} // namespace iso15118::io
