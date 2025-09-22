// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_DEVICE_HPP
#define CAN_DEVICE_HPP

#include "CanPackets.hpp"
#include <atomic>
#include <linux/can.h>
#include <mutex>
#include <thread>

class CanDevice {
public:
    CanDevice();
    virtual ~CanDevice();
    void open_device(const std::string& dev);
    bool close_device();
    bool is_running() {
        return is_open;
    };

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload);
    virtual void connection_established();
    bool _tx(uint32_t can_id, const std::vector<uint8_t>& payload);

private:
    bool try_open_device_internal();
    void close_device_internal();
    bool is_open{false};
    std::string can_device;
    int can_fd;
    std::atomic_bool exit_rx_thread;
    std::thread rx_thread_handle;
    void rx_thread();
};

#endif // CAN_DEVICE_HPP
