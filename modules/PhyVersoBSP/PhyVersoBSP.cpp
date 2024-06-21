// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "PhyVersoBSP.hpp"
#include <filesystem>

namespace module {

void PhyVersoBSP::init() {
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

    std::filesystem::path mcu_config_file = config.mcu_config_file;

    if (mcu_config_file.is_relative()) {
        // Add everest config folder prefix if relative path is given
        mcu_config_file = info.paths.etc / mcu_config_file;
    }

    if (!verso_config.open_file(mcu_config_file.string())) {
        EVLOG_error << "Could not open config file " << mcu_config_file;
    }

    serial.signal_config_request.connect([&]() {
        serial.send_config(verso_config);
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

} // namespace module
