// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class evConfig {
public:
    evConfig();
    ~evConfig();

    bool open_file(const char* path);
private:
    json config_file;
    bool config_valid = false;
};