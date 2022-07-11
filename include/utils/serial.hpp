// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_SERIAL_HPP
#define UTILS_SERIAL_HPP

#include <cstdio>
#include <stdint.h>
#include <termios.h>

namespace Everest {
class Serial {

public:
    Serial();
    ~Serial();

    bool openDevice(const char* device, int baud);

    virtual void run() = 0;

protected:
    int fd;
    int baud;

private:
    // Serial interface
    bool setSerialAttributes();

    // COBS de-/encoder
    void cobsDecodeReset();
    void handlePacket(uint8_t* buf, int len);
    void cobsDecode(uint8_t* buf, int len);
    void cobsDecodeByte(uint8_t byte);
    size_t cobsEncode(const void* data, size_t length, uint8_t* buffer);
    uint8_t msg[2048];
    uint8_t code;
    uint8_t block;
    uint8_t* decode;
    uint32_t crc32(uint8_t* buf, int len);
};
} // namespace Everest

#endif // UTILS_CONFIG_HPP
