// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "PhyVersoBSP.hpp"
#include <filesystem>

namespace module {

void PhyVersoBSP::init() {
    // transform everest config into evConfig accessible to evSerial
    everest_config_to_verso_config();

    // initialize serial driver
    if (!serial.open_device(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_error << "Could not open serial port " << config.serial_port << " with baud rate " << config.baud_rate;
        return;
    }

    invoke_init(*p_connector_1);
    invoke_init(*p_connector_2);
    invoke_init(*p_rcd_1);
    invoke_init(*p_rcd_2);
    invoke_init(*p_connector_lock_1);
    invoke_init(*p_connector_lock_2);
    invoke_init(*p_phyverso_mcu_temperature);
    invoke_init(*p_system_specific_data_1);
    invoke_init(*p_system_specific_data_2);

    // pass PhyVersoBSP module config values to evConfig (everest/standalone json config bridge to evSerial that is
    // talking to MCU)
    everest_config_to_verso_config();

    serial.signal_config_request.connect([&]() {
        serial.send_config();
        EVLOG_info << "Sent config packet to MCU";
    });
}

void PhyVersoBSP::ready() {
    serial.run();

    invoke_ready(*p_connector_1);
    invoke_ready(*p_connector_2);
    invoke_ready(*p_rcd_1);
    invoke_ready(*p_rcd_2);
    invoke_ready(*p_connector_lock_1);
    invoke_ready(*p_connector_lock_2);
    invoke_ready(*p_phyverso_mcu_temperature);
    invoke_ready(*p_system_specific_data_1);
    invoke_ready(*p_system_specific_data_2);

    if (not serial.is_open()) {
        auto err = p_connector_1->error_factory->create_error("evse_board_support/CommunicationFault", "",
                                                              "Could not open serial port.");
        p_connector_1->raise_error(err);
        err = p_connector_2->error_factory->create_error("evse_board_support/CommunicationFault", "",
                                                         "Could not open serial port.");
        p_connector_2->raise_error(err);
    }
}

// fills evConfig bridge with config values from manifest/everest config
void PhyVersoBSP::everest_config_to_verso_config() {
    // TODO: fill evConfig Conf struct with this modules conf structure values
    verso_config.conf.serial_port = this->config.serial_port;
    verso_config.conf.baud_rate = this->config.baud_rate;
    verso_config.conf.reset_gpio_bank = this->config.reset_gpio_bank;
    verso_config.conf.reset_gpio_pin = this->config.reset_gpio_pin;
    verso_config.conf.conn1_motor_lock_type = this->config.conn1_motor_lock_type;
    verso_config.conf.conn2_motor_lock_type = this->config.conn2_motor_lock_type;
}

} // namespace module
