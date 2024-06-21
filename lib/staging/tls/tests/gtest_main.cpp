// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include <cstdlib>
#include <iostream>
#include <linux/limits.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include <openssl_util.hpp>

namespace {

void log_handler(openssl::log_level_t level, const std::string& str) {
    switch (level) {
    case openssl::log_level_t::debug:
        std::cout << "DEBUG:   " << str << std::endl;
        break;
    case openssl::log_level_t::warning:
        std::cout << "WARN:    " << str << std::endl;
        break;
    case openssl::log_level_t::error:
        std::cerr << "ERROR:   " << str << std::endl;
        break;
    default:
        std::cerr << "Unknown: " << str << std::endl;
        break;
    }
}

} // namespace

int main(int argc, char** argv) {
    // create test certificates and keys
    openssl::set_log_handler(log_handler);
    if (std::system("./pki.sh") != 0) {
        std::cerr << "Problem creating test certificates and keys" << std::endl;
        char buf[PATH_MAX];
        if (getcwd(&buf[0], sizeof(buf)) != nullptr) {
            std::cerr << "./pki.sh not found in " << buf << std::endl;
        }
        return 1;
    }
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
