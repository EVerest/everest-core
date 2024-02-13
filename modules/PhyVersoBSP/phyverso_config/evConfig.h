// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <fstream>
#include <nlohmann/json.hpp>
#include "phyverso.pb.h"

using json = nlohmann::json;

class evConfig {
public:
    evConfig();
    ~evConfig();

    bool open_file(const char* path);
    EverestToMcu get_config_packet();
private:
    bool check_validity();
    bool read_hw_eeprom(ConfigHardwareRevision& hw_rev);
    void fill_config_packet();

    json config_file;
    bool config_valid = false;

    EverestToMcu config_packet = EverestToMcu_init_default;
};