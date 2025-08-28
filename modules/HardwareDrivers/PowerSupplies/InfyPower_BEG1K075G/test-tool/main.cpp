// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "InfyCanDevice.hpp"
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv) {
    InfyCanDevice can;

    if (!can.open_device("can0")) {
        return 1;
    }

    std::cout << "CAN device started." << std::endl;
    can.switch_on_off(false);
    can.adjust_power_factor(1.0);
    can.set_output_mode(InfyCanDevice::OutputMode::Automatic);

    sleep(2);
    can.set_generic_setting(0x21, 0x20, 420 * 1000);
    can.set_generic_setting(0x21, 0x21, 5 * 1000);
    can.set_generic_setting(0x21, 0x22, 440 * 1000);
    can.set_generic_setting(0x21, 0x23, 5 * 1000);

    can.set_generic_setting(0x21, 0x24, 200 * 1000);
    can.set_generic_setting(0x21, 0x25, 5 * 1000);
    can.set_generic_setting(0x21, 0x26, 190 * 1000);
    can.set_generic_setting(0x21, 0x27, 5 * 1000);

    can.set_generic_setting(0x21, 0x28, 53 * 1000);
    can.set_generic_setting(0x21, 0x29, 5 * 1000);
    can.set_generic_setting(0x21, 0x2A, 54 * 1000);
    can.set_generic_setting(0x21, 0x2B, 5 * 1000);

    can.set_generic_setting(0x21, 0x2C, 47 * 1000);
    can.set_generic_setting(0x21, 0x2D, 5 * 1000);
    can.set_generic_setting(0x21, 0x2E, 46 * 1000);
    can.set_generic_setting(0x21, 0x2F, 5 * 1000);
    std::cout << "Protection settings applied" << std::endl;

    can.set_inverter_mode(false);
    can.set_voltage_current(200, 10);
    can.set_walkin_enabled(false);

    can.switch_on_off(true);
    can.set_voltage_current(200, 10);

    can.adjust_power_factor(1.0);

    while (true) {
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x03, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x04, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x05, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x06, 0x00));

        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x30, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x31, 0x00));

        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x32, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x11, 0x33, 0x00));
        for (uint8_t i = 0x01; i < 0x14; i++) {
            can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x21, i, 0x00));
        }

        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x41, 0x01, 0x00));
        can.request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::GenericSetting(0x41, 0x02, 0x00));

        usleep(50000);

        std::cout << can.telemetry;
    }

    can.close_device();
    return 0;
}
