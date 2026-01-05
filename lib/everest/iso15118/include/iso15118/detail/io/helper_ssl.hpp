// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

namespace iso15118::io {

std::string log_openssl_error(const std::string& error_msg);
void log_and_raise_openssl_error(const std::string& error_msg);

} // namespace iso15118::io
