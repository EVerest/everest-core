// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PHYVERSO_CONFIG_EV_CONFIG_HPP
#define PHYVERSO_CONFIG_EV_CONFIG_HPP

#include "phyverso.pb.h"
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class evConfig {
public:
    evConfig();
    ~evConfig();

    bool open_file(std::string path);
    EverestToMcu get_config_packet();

private:
    bool check_validity();
    bool read_hw_eeprom(ConfigHardwareRevision& hw_rev);
    void fill_config_packet();

    json config_file;

    EverestToMcu config_packet = EverestToMcu_init_default;
};

#endif // PHYVERSO_CONFIG_EV_CONFIG_HPP
