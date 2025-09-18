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
        return true;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Exception of unknown type!" << std::endl;
    }
    return false;
}

// unused for now
bool evConfig::read_hw_eeprom(ConfigHardwareRevision& hw_rev) {
    // TODO: read eeprom on new phyVERSO hw revisions,
    // for now return hardcoded value
    hw_rev = ConfigHardwareRevision_HW_REV_A;
    return true;
}

void evConfig::fill_config_packet() {
    config_packet.which_payload = EverestToMcu_config_response_tag;
    config_packet.connector = 0;
    read_hw_eeprom(config_packet.payload.config_response.hw_rev);

    /* fill port 1 config */
    {
        auto& chargeport_config = config_packet.payload.config_response.chargeport_config[0];

        chargeport_config.has_lock = true;
        chargeport_config.lock.type = static_cast<MotorLockType>(conf.conn1_motor_lock_type);
        chargeport_config.feedback_active_low = conf.conn1_feedback_active_low;
        chargeport_config.feedback_pull = static_cast<GpioPull>(conf.conn1_feedback_pull);
        chargeport_config.has_socket = conf.conn1_has_socket;

        if (conf.conn1_disable_port) {
            chargeport_config.type = ChargePortType_DISABLED;
        } else if (conf.conn1_dc) {
            chargeport_config.type = ChargePortType_DC;
        } else {
            chargeport_config.type = ChargePortType_AC;
        }
    }

    /* fill port 2 config */
    {
        auto& chargeport_config = config_packet.payload.config_response.chargeport_config[1];

        chargeport_config.has_lock = true;
        chargeport_config.lock.type = static_cast<MotorLockType>(conf.conn2_motor_lock_type);
        chargeport_config.feedback_active_low = conf.conn2_feedback_active_low;
        chargeport_config.feedback_pull = static_cast<GpioPull>(conf.conn2_feedback_pull);
        chargeport_config.has_socket = conf.conn2_has_socket;

        if (conf.conn2_disable_port) {
            chargeport_config.type = ChargePortType_DISABLED;
        } else if (conf.conn2_dc) {
            chargeport_config.type = ChargePortType_DC;
        } else {
            chargeport_config.type = ChargePortType_AC;
        }
    }
}

EverestToMcu evConfig::get_config_packet() {
    fill_config_packet();
    return config_packet;
}

// keep in mind, json config is only used for testing via phyverso_cli
void evConfig::json_conf_to_evConfig() {
    // try and get value from json file or keep default values as is
    conf.conn1_motor_lock_type = config_file.value("conn1_motor_lock_type", conf.conn1_motor_lock_type);
    conf.conn2_motor_lock_type = config_file.value("conn2_motor_lock_type", conf.conn2_motor_lock_type);
    conf.reset_gpio_bank = config_file.value("reset_gpio_bank", conf.reset_gpio_bank);
    conf.reset_gpio_pin = config_file.value("reset_gpio_pin", conf.reset_gpio_pin);
    conf.conn1_disable_port = config_file.value("conn1_disable_port", conf.conn1_disable_port);
    conf.conn2_disable_port = config_file.value("conn2_disable_port", conf.conn2_disable_port);
    conf.conn1_feedback_active_low = config_file.value("conn1_feedback_active_low", conf.conn1_feedback_active_low);
    conf.conn2_feedback_active_low = config_file.value("conn2_feedback_active_low", conf.conn2_feedback_active_low);
    conf.conn1_feedback_pull = config_file.value("conn1_feedback_pull", conf.conn1_feedback_pull);
    conf.conn2_feedback_pull = config_file.value("conn2_feedback_pull", conf.conn2_feedback_pull);
    conf.conn1_dc = config_file.value("conn1_dc", conf.conn1_dc);
    conf.conn2_dc = config_file.value("conn2_dc", conf.conn2_dc);
    conf.conn1_disable_port = config_file.value("conn1_disable_port", conf.conn1_disable_port);
    conf.conn2_disable_port = config_file.value("conn2_disable_port", conf.conn2_disable_port);
    conf.conn1_has_socket = config_file.value("conn1_has_socket", conf.conn1_has_socket);
    conf.conn2_has_socket = config_file.value("conn2_has_socket", conf.conn2_has_socket);
}