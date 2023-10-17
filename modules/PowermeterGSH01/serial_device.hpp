// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for custom serial communications
*/
#ifndef SERIAL_DEVICE
#define SERIAL_DEVICE

#include <everest/logging.hpp>
#include <stdint.h>
#include <termios.h>
#include <mutex>
#include <optional>

namespace serial_device {

constexpr int SERIAL_RX_INITIAL_TIMEOUT_MS = 500;
constexpr int SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

class SerialDevice {

public:
    SerialDevice() = default;
    ~SerialDevice();

    bool open_device(const std::string& device, int baud, bool ignore_echo);
    void tx(std::vector<uint8_t>& request);
    int rx(std::vector<uint8_t>& rxbuf,
           std::optional<int> initial_timeout_ms, 
           std::optional<int> in_msg_timeout_ms);

private:
    // Serial interface
    int fd{0};
    bool ignore_echo{false};

    std::mutex serial_mutex;
};

} // namespace serial_device
#endif // SERIAL_DEVICE
