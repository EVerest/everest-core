// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is a tiny and fast modbus RTU implementation
*/
#ifndef TINY_MODBUS_RTU
#define TINY_MODBUS_RTU

#include <chrono>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <termios.h>

#include <everest/logging.hpp>
#include <gpio.hpp>

namespace tiny_modbus {

constexpr int DEVICE_ADDRESS_POS = 0x00;
constexpr int FUNCTION_CODE_POS = 0x01;

constexpr int REQ_TX_FIRST_REGISTER_ADDR_POS = 0x02;
constexpr int REQ_TX_QUANTITY_POS = 0x04;
constexpr int REQ_TX_SINGLE_REG_PAYLOAD_POS = 0x04;

constexpr int REQ_TX_MULTIPLE_REG_BYTE_COUNT_POS = 0x06;

constexpr int RES_RX_LEN_POS = 0x02;
constexpr int RES_RX_START_OF_PAYLOAD = 0x03;
constexpr int RES_TX_START_OF_PAYLOAD = 0x02;
constexpr int RES_EXCEPTION_CODE = 0x02;

constexpr int MODBUS_MAX_REPLY_SIZE = 255 + 6;
constexpr int MODBUS_MIN_REPLY_SIZE = 5;
constexpr int MODBUS_BASE_PAYLOAD_SIZE = 8;

enum class Parity : uint8_t {
    NONE = 0,
    ODD = 1,
    EVEN = 2
};

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

std::string FunctionCode_to_string(FunctionCode fc);
std::string FunctionCode_to_string_with_hex(FunctionCode fc);
std::ostream& operator<<(std::ostream& os, const FunctionCode& fc);

class TinyModbusException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
class TimeoutException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class ShortPacketException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class AddressMismatchException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class FunctionCodeMismatchException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class ChecksumErrorException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class IncompletePacketException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class OddByteCountException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};
class ModbusException : public TinyModbusException {
    using TinyModbusException::TinyModbusException;
};

class TinyModbusRTU {

public:
    ~TinyModbusRTU();

    bool open_device(const std::string& device, int baud, bool ignore_echo,
                     const Everest::GpioSettings& rxtx_gpio_settings, const Parity parity, bool rtscts,
                     std::chrono::milliseconds initial_timeout, std::chrono::milliseconds within_message_timeout);

    std::vector<uint16_t> txrx(uint8_t device_address, FunctionCode function, uint16_t first_register_address,
                               uint16_t register_quantity, uint16_t chunk_size, bool wait_for_reply = true,
                               std::vector<uint16_t> request = std::vector<uint16_t>());

private:
    // Serial interface
    int fd{-1};
    bool ignore_echo{false};

    std::vector<uint16_t> txrx_impl(uint8_t device_address, FunctionCode function, uint16_t first_register_address,
                                    uint16_t register_quantity, bool wait_for_reply = true,
                                    std::vector<uint16_t> request = std::vector<uint16_t>());

    int read_reply(uint8_t* rxbuf, int rxbuf_len);

    Everest::Gpio rxtx_gpio;
    std::chrono::milliseconds initial_timeout;
    std::chrono::milliseconds within_message_timeout;
};

} // namespace tiny_modbus
#endif
