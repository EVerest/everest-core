// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/detail/helper.hpp>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#ifdef LIBISO_OPENSSL
#include <openssl/err.h>
#else
#include <mbedtls/error.h>
#endif

static int add_error_str(const char* str, std::size_t len, void* u) {
    assert(u != nullptr);
    auto* list = reinterpret_cast<std::string*>(u);
    *list += '\n' + std::string(str, len);
    return 0;
}

namespace iso15118 {

std::string adding_err_msg(const std::string& msg) {
    return (msg + " (reason: " + strerror(errno) + ")");
}

void log_and_throw(const char* msg) {
    throw std::runtime_error(std::string(msg) + " (reason: " + strerror(errno) + ")");
}

static void log_and_raise(const std::string& error_msg) {
    throw std::runtime_error(error_msg);
}

#ifdef LIBISO_OPENSSL
void log_and_raise_openssl_error(const std::string& error_msg) {
    std::string error_message = {error_msg};
    ERR_print_errors_cb(&add_error_str, &error_message);
    log_and_raise(error_message);
}
#else
void log_and_raise_mbed_error(const std::string& error_msg, int error_code) {
    char mbed_error_msg[256];
    mbedtls_strerror(error_code, mbed_error_msg, sizeof(mbed_error_msg));
    log_and_raise(error_msg + " (" + mbed_error_msg + ")");
}
#endif

} // namespace iso15118
