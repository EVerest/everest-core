// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evConfig.h"
#include "phyverso.pb.h"
#include <fmt/core.h>
#include <fstream>
#include <iostream>

// for convenience
using json = nlohmann::json;

evConfig::evConfig() {
}

evConfig::~evConfig() {
}

bool evConfig::open_file(std::string path) {
    try {
        std::ifstream f(path);
        config_file = json::parse(f);
        // check validity first
        return check_validity();
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception of unknown type!" << std::endl;
    }
    return false;
}

bool evConfig::check_validity() {
    std::vector<std::string> mandatory_config_keys = {"conn1_motor_lock_type", "conn2_motor_lock_type"};

    for (const std::string& key : mandatory_config_keys) {
        if (!config_file.contains(key)) {
            std::cout << fmt::format("Missing '{}' config parameter", key) << std::endl;
            return false;
        }
    }

    return true;
}

// unused for now
bool evConfig::read_hw_eeprom(ConfigHardwareRevision& hw_rev) {
    // TODO: read eeprom on new phyVERSO hw revisions,
    // for now return hardcoded value
    hw_rev = ConfigHardwareRevision_HW_REV_A;
    return true;
}

// todo: needs to refactored a lot
void evConfig::fill_config_packet() {
    config_packet.which_payload = EverestToMcu_config_response_tag;
    config_packet.connector = 0;
    read_hw_eeprom(config_packet.payload.config_response.hw_rev);

    config_packet.payload.config_response.lock_1.type = static_cast<MotorLockType>(conf.conn1_motor_lock_type);
    config_packet.payload.config_response.has_lock_1 = true;

    config_packet.payload.config_response.lock_2.type = static_cast<MotorLockType>(conf.conn2_motor_lock_type);
    config_packet.payload.config_response.has_lock_2 = true;
}

EverestToMcu evConfig::get_config_packet() {
    fill_config_packet();
    return config_packet;
}

void evConfig::json_conf_to_evConfig() {
    conf.conn1_motor_lock_type = config_file["conn1_motor_lock_type"];
    conf.conn2_motor_lock_type = config_file["conn2_motor_lock_type"];

    // set GPIO related settings for evSerial if available
    try {
        conf.reset_gpio_bank = config_file["reset_gpio_bank"];
    } catch (...) {
    }
    try {
        conf.reset_gpio_pin = config_file["reset_gpio_pin"];
    } catch (...) {
    }
}