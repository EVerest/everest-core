// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for custom serial communications
*/
#ifndef SERIAL_DEVICE
#define SERIAL_DEVICE

#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace serial_device {

constexpr int SERIAL_RX_INITIAL_TIMEOUT_MS = 500;
constexpr int SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

class SerialDevice {

public:
    SerialDevice() = default;
    SerialDevice(const SerialDevice&) = delete;
    SerialDevice(SerialDevice&&) = delete;
    SerialDevice& operator=(const SerialDevice&) = delete;
    SerialDevice& operator=(SerialDevice&&) = delete;
    ~SerialDevice();

    bool open_device(const std::string& device, int baud, bool ignore_echo);
    void tx(const std::vector<std::uint8_t>& request);
    int rx(std::vector<std::uint8_t>& rxbuf, std::optional<int> initial_timeout_ms,
           std::optional<int> in_msg_timeout_ms);

private:
    // Serial interface
    int fd{0};
    bool ignore_echo{false};

    std::mutex serial_mutex;
};

} // namespace serial_device
#endif // SERIAL_DEVICE
