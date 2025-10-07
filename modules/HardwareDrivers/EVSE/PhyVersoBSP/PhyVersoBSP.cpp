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

    // init user gpios
    if (not gpio.init_gpios()) {
        EVLOG_error << "Could not initialize user GPIOs. Terminating.";
        return;
    }

    serial.flush_buffers();

    serial.signal_config_request.connect([&]() {
        serial.send_config();
        EVLOG_info << "Sent config packet to MCU";
    });

    serial.signal_connection_timeout.connect([this]() {
        auto err = p_connector_1->error_factory->create_error("evse_board_support/CommunicationFault", "McuToEverest",
                                                              "Serial connection to MCU timed out");
        p_connector_1->raise_error(err);
        err = p_connector_2->error_factory->create_error("evse_board_support/CommunicationFault", "McuToEverest",
                                                         "Serial connection to MCU timed out");
        p_connector_2->raise_error(err);
    });

    serial.signal_error_flags.connect([this](int connector, ErrorFlags error_flags) {
        // heartbeat failure from Mcu side (not receiving packets) will be visible in both connector errors
        if (error_flags.heartbeat_timeout != last_heartbeat_error) {
            if (error_flags.heartbeat_timeout) {
                auto err = p_connector_1->error_factory->create_error(
                    "evse_board_support/CommunicationFault", "EverestToMcu", "MCU did not receive Everest heartbeat");
                p_connector_1->raise_error(err);
                err = p_connector_2->error_factory->create_error(
                    "evse_board_support/CommunicationFault", "EverestToMcu", "MCU did not receive Everest heartbeat");
                p_connector_2->raise_error(err);
            } else {
                p_connector_1->clear_error("evse_board_support/CommunicationFault", "EverestToMcu");
                p_connector_2->clear_error("evse_board_support/CommunicationFault", "EverestToMcu");
            }
        }
        last_heartbeat_error = error_flags.heartbeat_timeout;
    });

    serial.signal_keep_alive.connect([this](KeepAlive d) {
        mcu_config_done = d.configuration_done;
        p_connector_1->clear_error("evse_board_support/CommunicationFault", "McuToEverest");
        p_connector_2->clear_error("evse_board_support/CommunicationFault", "McuToEverest");
    });

    serial.reset(1);

    serial.run();
    gpio.run();

    // very sporadically multiple resets needed for MCU to respond -> retrying until we get MCU response in a
    // configured, ready and running state
    mcu_config_done = false;
    uint16_t n_tries = 0;
    while (!mcu_config_done) {
        serial.keep_alive();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        n_tries++;
        if (n_tries > 20) {
            EVLOG_info << "Trying reset again";
            serial.flush_buffers();
            serial.reset(1);
            n_tries = 0;
        }
    }

    invoke_init(*p_connector_1);
    invoke_init(*p_connector_2);
    invoke_init(*p_rcd_1);
    invoke_init(*p_rcd_2);
    invoke_init(*p_connector_lock_1);
    invoke_init(*p_connector_lock_2);
}

void PhyVersoBSP::ready() {
    invoke_ready(*p_connector_1);
    invoke_ready(*p_connector_2);
    invoke_ready(*p_rcd_1);
    invoke_ready(*p_rcd_2);
    invoke_ready(*p_connector_lock_1);
    invoke_ready(*p_connector_lock_2);

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
    // if a port is configured to be AC and has a socket, a motor lock type specification/usage is mandatory
    if ((this->config.conn1_disable_port == false) && (this->config.conn1_dc == false) &&
        (this->config.conn1_has_socket == true) && (this->config.conn1_motor_lock_type < 1)) {
        EVLOG_critical << "Motor lock type for connector 1 has to be specified when using connector 1 as AC charging "
                          "port with a socket/detachable charging cable!";
        throw std::runtime_error("Motor lock type for connector 1 has to be specified when using connector 1 as AC "
                                 "charging port with a socket/detachable charging cable!");
    }
    if ((this->config.conn2_disable_port == false) && (this->config.conn2_dc == false) &&
        (this->config.conn2_has_socket == true) && (this->config.conn2_motor_lock_type < 1)) {
        EVLOG_critical << "Motor lock type for connector 2 has to be specified when using connector 2 as AC charging "
                          "port with a socket/detachable charging cable!";
        throw std::runtime_error("Motor lock type for connector 2 has to be specified when using connector 2 as AC "
                                 "charging port with a socket/detachable charging cable!");
    }

    if ((this->config.conn1_feedback_pull < 0) || (this->config.conn1_feedback_pull > 2)) {
        EVLOG_error << "conn1_feedback_pull out of range! Falling back to default: 2";
        verso_config.conf.conn1_feedback_pull = 2;
    } else {
        verso_config.conf.conn1_feedback_pull = this->config.conn1_feedback_pull;
    }

    if ((this->config.conn2_feedback_pull < 0) || (this->config.conn2_feedback_pull > 2)) {
        EVLOG_error << "conn2_feedback_pull out of range! Falling back to default: 2";
        verso_config.conf.conn2_feedback_pull = 2;
    } else {
        verso_config.conf.conn2_feedback_pull = this->config.conn2_feedback_pull;
    }

    verso_config.conf.serial_port = this->config.serial_port;
    verso_config.conf.baud_rate = this->config.baud_rate;
    verso_config.conf.reset_gpio_bank = this->config.reset_gpio_bank;
    verso_config.conf.reset_gpio_pin = this->config.reset_gpio_pin;
    verso_config.conf.conn1_motor_lock_type = this->config.conn1_motor_lock_type;
    verso_config.conf.conn2_motor_lock_type = this->config.conn2_motor_lock_type;
    verso_config.conf.conn1_gpio_stop_button_enabled = this->config.conn1_gpio_stop_button_enabled;
    verso_config.conf.conn1_gpio_stop_button_bank = this->config.conn1_gpio_stop_button_bank;
    verso_config.conf.conn1_gpio_stop_button_pin = this->config.conn1_gpio_stop_button_pin;
    verso_config.conf.conn1_gpio_stop_button_invert = this->config.conn1_gpio_stop_button_invert;
    verso_config.conf.conn2_gpio_stop_button_enabled = this->config.conn2_gpio_stop_button_enabled;
    verso_config.conf.conn2_gpio_stop_button_bank = this->config.conn2_gpio_stop_button_bank;
    verso_config.conf.conn2_gpio_stop_button_pin = this->config.conn2_gpio_stop_button_pin;
    verso_config.conf.conn2_gpio_stop_button_invert = this->config.conn2_gpio_stop_button_invert;
    verso_config.conf.conn1_disable_port = this->config.conn1_disable_port;
    verso_config.conf.conn2_disable_port = this->config.conn2_disable_port;
    verso_config.conf.conn1_feedback_active_low = this->config.conn1_feedback_active_low;
    verso_config.conf.conn2_feedback_active_low = this->config.conn2_feedback_active_low;
    verso_config.conf.conn1_dc = this->config.conn1_dc;
    verso_config.conf.conn2_dc = this->config.conn2_dc;
}

} // namespace module
