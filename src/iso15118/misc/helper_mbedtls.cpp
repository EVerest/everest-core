// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/detail/io/helper_mbedtls.hpp>

#include <stdexcept>

#include <mbedtls/error.h>

namespace iso15118::io {

static void log_and_raise(const std::string& error_msg) {
    throw std::runtime_error(error_msg);
}

void log_and_raise_mbed_error(const std::string& error_msg, int error_code) {
    char mbed_error_msg[256];
    mbedtls_strerror(error_code, mbed_error_msg, sizeof(mbed_error_msg));
    log_and_raise(error_msg + " (" + mbed_error_msg + ")");
}

} // namespace iso15118::io
