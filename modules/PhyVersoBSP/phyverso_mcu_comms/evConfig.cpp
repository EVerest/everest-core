// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evConfig.h"
#include "phyverso.pb.h"
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

// todo: needs to be refactored a lot
bool evConfig::check_validity() {
    std::cout << config_file.dump() << "\n\n";

    if (!config_file.contains("motor_lock_type")) {
        std::cout << "Missing 'motor_lock_type' config parameter" << std::endl;
        return false;
    }

    return true;
}

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
    config_packet.payload.config_response.lock_1.type = config_file["motor_lock_type"];
    config_packet.payload.config_response.has_lock_1 = true;

    // TODO: implement handling of two different lock types on MCU, for now only send motor_lock_type
    // and use config for both MCU charging ports
}

EverestToMcu evConfig::get_config_packet() {
    fill_config_packet();
    return config_packet;
}
