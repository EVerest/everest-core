// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <cstdlib>
#include <iostream>
#include <linux/limits.h>
#include <unistd.h>

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    // create test certificates and keys
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
