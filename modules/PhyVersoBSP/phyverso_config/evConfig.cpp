// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include "evConfig.h"
#include <iostream>

// for convenience
using json = nlohmann::json;
using namespace std;

evConfig::evConfig()
{
}

evConfig::~evConfig()
{
}

bool evConfig::open_file(const char* path)
{
    try {
        std::ifstream f(path);
        config_file = json::parse(f);
        // check validity first
        // TODO
        config_valid = true;
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
    config_valid = false;
    return false;
}