// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_SERIAL_HPP
#define UTILS_SERIAL_HPP

#include <cstddef>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <termios.h>
#include <vector>

namespace Everest {
class Serial {

public:
    Serial();
    virtual ~Serial();

    bool openDevice(const char* device, int baud);

    virtual void run() = 0;

    virtual std::string hexdump(const std::uint8_t* const msg, int msg_len) const;
    virtual std::string hexdump(const std::vector<std::uint8_t>& msg) const;

protected:
    int fd{-1};
    int baud{0};

private:
    // Serial interface
    bool setSerialAttributes();

    // COBS de-/encoder
    void cobsDecodeReset();
    void handlePacket(uint8_t* buf, int len);
    void cobsDecode(uint8_t* buf, int len);
    void cobsDecodeByte(uint8_t byte);
    std::size_t cobsEncode(const void* data, std::size_t length, uint8_t* buffer);
    uint8_t msg[2048];
    uint8_t code;
    uint8_t block;
    uint8_t* decode;
    uint32_t crc32(uint8_t* buf, int len);
};
} // namespace Everest

#endif // UTILS_CONFIG_HPP
