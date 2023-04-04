// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is a tiny and fast modbus RTU implementation
*/
#ifndef TINY_MODBUS_RTU
#define TINY_MODBUS_RTU

#include <everest/logging.hpp>
#include <stdint.h>
#include <termios.h>

namespace tinymod {

constexpr int device_address_pos = 0x00;
constexpr int function_code_pos = 0x01;

constexpr int req_tx_first_register_addr_pos = 0x02;
constexpr int req_tx_quantity_pos = 0x04;

constexpr int req_tx_multiple_reg_byte_count_pos = 0x06;

constexpr int res_rx_len_pos = 0x02;
constexpr int res_rx_start_of_payload = 0x03;
constexpr int res_tx_start_of_payload = 0x02;

enum function_code : uint8_t {
    read_coils = 0x01,
    read_discrete_inputs = 0x02,
    read_multiple_holding_registers = 0x03,
    read_input_registers = 0x04,
    write_single_coil = 0x05,
    write_single_holding_register = 0x06,
    write_mulitple_coils = 0x0F,
    write_multiple_holding_registers = 0x10,
};

class TinyModbusRTU {

public:
    ~TinyModbusRTU();

    bool openDevice(const std::string& device, int baud);
    std::vector<uint16_t> txrx(uint8_t device_address, function_code function, uint16_t first_register_address,
                               uint16_t register_quantity, bool wait_for_reply = true,
                               std::vector<uint16_t> request = std::vector<uint16_t>());

private:
    // Serial interface
    int fd{0};
};

} // namespace tinymod
#endif
