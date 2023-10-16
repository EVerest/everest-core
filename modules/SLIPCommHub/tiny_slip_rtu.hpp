// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is a tiny and fast modbus RTU implementation
*/
#ifndef TINY_SLIP_RTU
#define TINY_SLIP_RTU

#include <stdint.h>
#include <termios.h>

#include <everest/logging.hpp>
#include <gpio.hpp>

namespace tiny_slip {

constexpr int DEVICE_ADDRESS_POS = 0x00;
constexpr int REGISTER_ADDR_POS = 0x01;
constexpr int REGISTER_ADDR_LOW_BYTE = 0x01;
constexpr int REGISTER_ADDR_HIGH_BYTE = 0x02;
constexpr int FRAME_LEN_POS = 0x03;  
constexpr int FRAME_LEN_LOW_BYTE = 0x03;  
constexpr int FRAME_LEN_HIGH_BYTE = 0x04; 
constexpr int STATUS_CODE = 0x05;

constexpr int START_OF_PAYLOAD = 0x06;
/*
constexpr int RES_RX_LEN_POS = 0x02;
constexpr int RES_RX_START_OF_PAYLOAD = 0x03;
constexpr int RES_TX_START_OF_PAYLOAD = 0x02;
*/
constexpr int SLIP_MAX_REPLY_SIZE = 500 + 10;    //ToDo: maxLength has to be found out, depending on max OCMF frame length
constexpr int SLIP_MIN_REPLY_SIZE = 10; //
constexpr int SLIP_LINK_LAYER_OVERHEAD_SIZE = 8;

constexpr int SLIP_RX_INITIAL_TIMEOUT_MS = 500;
constexpr int SLIP_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

enum class Parity : uint8_t {
    NONE = 0,
    ODD = 1,
    EVEN = 2
};

/*
enum FunctionCode : uint8_t {
    READ_COILS = 0x01,
    READ_DISCRETE_INPUTS = 0x02,
    READ_MULTIPLE_HOLDING_REGISTERS = 0x03,
    READ_INPUT_REGISTERS = 0x04,
    WRITE_SINGLE_COIL = 0x05,
    WRITE_SINGLE_HOLDING_REGISTER = 0x06,
    WRITE_MULTIPLE_COILS = 0x0F,
    WRITE_MULTIPLE_HOLDING_REGISTERS = 0x10,
};*/

class TinySlipRTU {

public:
    ~TinySlipRTU();

    bool open_device(const std::string& device, int baud, bool ignore_echo,
                     const Everest::GpioSettings& rxtx_gpio_settings, const Parity parity);
    std::vector<uint16_t> txrx(uint8_t device_address, uint16_t logical_address,
                               bool wait_for_reply = true, 
                               std::vector<uint8_t> request = std::vector<uint8_t>());

private:
    // Serial interface
    int fd{0};
    bool ignore_echo{false};
    int read_reply(uint8_t* rxbuf, int rxbuf_len);

    Everest::Gpio rxtx_gpio;
};

} // namespace tiny_slip
#endif
