// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SERIAL_COMMUNICATION_HUB_IMPL_HPP
#define MAIN_SERIAL_COMMUNICATION_HUB_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/serial_communication_hub/Implementation.hpp>

#include "../SerialCommHub.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "tiny_modbus_rtu.hpp"
#include <chrono>
#include <termios.h>
#include <utils/thread.hpp>
#include <vector>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string serial_port;
    int baudrate;
    int parity;
    int rs485_direction_gpio;
    bool ignore_echo;
    std::string rxtx_gpio_chip;
    int rxtx_gpio_line;
    bool rxtx_gpio_tx_high;
};

class serial_communication_hubImpl : public serial_communication_hubImplBase {
public:
    serial_communication_hubImpl() = delete;
    serial_communication_hubImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<SerialCommHub>& mod,
                                 Conf& config) :
        serial_communication_hubImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::serial_comm_hub_requests::Result
    handle_modbus_read_holding_registers(int& target_device_id, int& first_register_address,
                                         int& num_registers_to_read) override;
    virtual types::serial_comm_hub_requests::Result
    handle_modbus_read_input_registers(int& target_device_id, int& first_register_address,
                                       int& num_registers_to_read) override;
    virtual types::serial_comm_hub_requests::StatusCodeEnum
    handle_modbus_write_multiple_registers(int& target_device_id, int& first_register_address,
                                           types::serial_comm_hub_requests::VectorUint16& data_raw) override;
    virtual void handle_nonstd_write(int& target_device_id, int& first_register_address,
                                     int& num_registers_to_read) override;
    virtual types::serial_comm_hub_requests::Result
    handle_nonstd_read(int& target_device_id, int& first_register_address, int& num_registers_to_read) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<SerialCommHub>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    static constexpr uint8_t num_resends_on_error{3};
    tiny_modbus::TinyModbusRTU modbus;

    std::mutex serial_mutex;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SERIAL_COMMUNICATION_HUB_IMPL_HPP
