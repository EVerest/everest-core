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
    // same structure as in PhyVersoBSP, gets filled either by json parsing config file for phyverso_cli
    // or gets overwritten with PhyVersoBSP everest module config
    // changes in manifest have to be fixed here manually
    struct Conf {
        std::string serial_port = "/dev/ttyUSB0";
        int baud_rate = 115200;
        int reset_gpio = -1;
        int conn1_max_current_A_import = 16;
        int conn1_min_current_A_import = 6;
        int conn1_min_phase_count_import = 3;
        int conn1_max_phase_count_import = 3;
        int conn1_min_current_A_export = 0;
        int conn1_max_current_A_export = 0;
        int conn1_min_phase_count_export = 3;
        int conn1_max_phase_count_export = 3;
        bool conn1_has_socket = false;
        bool conn1_dc = false;
        int conn2_max_current_A_import = 16;
        int conn2_min_current_A_import = 6;
        int conn2_min_phase_count_import = 3;
        int conn2_max_phase_count_import = 3;
        int conn2_min_current_A_export = 0;
        int conn2_max_current_A_export = 0;
        int conn2_min_phase_count_export = 3;
        int conn2_max_phase_count_export = 3;
        bool conn2_has_socket = false;
        bool conn2_dc = false;
        int reset_gpio_bank = 1;
        int reset_gpio_pin = 23;
        int conn1_motor_lock_type = 2;
        int conn2_motor_lock_type = 2;
        bool conn1_gpio_stop_button_enabled = false;
        int conn1_gpio_stop_button_bank = 1;
        int conn1_gpio_stop_button_pin = 36;
        bool conn1_gpio_stop_button_invert = false;
        bool conn2_gpio_stop_button_enabled = false;
        int conn2_gpio_stop_button_bank = 1;
        int conn2_gpio_stop_button_pin = 37;
        bool conn2_gpio_stop_button_invert = false;
    } conf;

    evConfig();
    ~evConfig();

    bool open_file(std::string path);
    EverestToMcu get_config_packet();
    void json_conf_to_evConfig();

private:
    bool check_validity();
    bool read_hw_eeprom(ConfigHardwareRevision& hw_rev);
    void fill_config_packet();

    json config_file;
    EverestToMcu config_packet = EverestToMcu_init_default;
};

#endif // PHYVERSO_CONFIG_EV_CONFIG_HPP
