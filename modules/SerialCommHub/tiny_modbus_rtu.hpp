// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is a tiny and fast modbus RTU implementation
*/
#ifndef TINY_MODBUS_RTU
#define TINY_MODBUS_RTU

#include <stdint.h>
#include <termios.h>

#include <everest/logging.hpp>
#include <gpio.hpp>

namespace tiny_modbus {

constexpr int DEVICE_ADDRESS_POS = 0x00;
constexpr int FUNCTION_CODE_POS = 0x01;

constexpr int REQ_TX_FIRST_REGISTER_ADDR_POS = 0x02;
constexpr int REQ_TX_QUANTITY_POS = 0x04;

constexpr int REQ_TX_MULTIPLE_REG_BYTE_COUNT_POS = 0x06;

constexpr int RES_RX_LEN_POS = 0x02;
constexpr int RES_RX_START_OF_PAYLOAD = 0x03;
constexpr int RES_TX_START_OF_PAYLOAD = 0x02;

constexpr int MODBUS_MAX_REPLY_SIZE = 255 + 6;
constexpr int MODBUS_MIN_REPLY_SIZE = 5;
constexpr int MODBUS_BASE_PAYLOAD_SIZE = 8;

constexpr int MODBUS_RX_INITIAL_TIMEOUT_MS = 500;
constexpr int MODBUS_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

enum FunctionCode : uint8_t {
    READ_COILS = 0x01,
    READ_DISCRETE_INPUTS = 0x02,
    READ_MULTIPLE_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    WRITE_SINGLE_COIL = 0x05,
    WRITE_SINGLE_HOLDING_REGISTER = 0x06,
    WRITE_MULTIPLE_COILS = 0x0F,
    WRITE_MULTIPLE_HOLDING_REGISTERS = 0x10,
};

class TinyModbusRTU {

public:
    ~TinyModbusRTU();

    bool open_device(const std::string& device, int baud, bool ignore_echo,
                     const Everest::GpioSettings& rxtx_gpio_settings);
    std::vector<uint16_t> txrx(uint8_t device_address, FunctionCode function, uint16_t first_register_address,
                               uint16_t register_quantity, bool wait_for_reply = true,
                               std::vector<uint16_t> request = std::vector<uint16_t>());

private:
    // Serial interface
    int fd{0};
    bool ignore_echo{false};
    int read_reply(uint8_t* rxbuf, int rxbuf_len);

    Everest::Gpio rxtx_gpio;
};

} // namespace tiny_modbus
#endif
