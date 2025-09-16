// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_BUS_HPP
#define CAN_BUS_HPP

#include "CanPackets.hpp"
#include <atomic>
#include <condition_variable>
#include <linux/can.h>
#include <mutex>
#include <thread>

#include <everest/io/can/socket_can.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>

using namespace everest::lib::io;

class CanBus {
public:
    CanBus();
    virtual ~CanBus();
    bool open_device(const std::string& dev);
    bool close_device();

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) = 0;
    virtual void poll_status_handler() = 0;
    bool _tx(uint32_t can_id, const std::vector<uint8_t>& payload);

private:
    std::unique_ptr<can::socket_can> can_bus;
    std::atomic_bool on_error{false};
    event::fd_event_handler ev_handler;
    event::timer_fd recovery_timer;
    event::timer_fd poll_status_timer;
    std::atomic_bool exit_rx_thread;
    std::thread rx_thread_handle;
    void rx_thread();
};

#endif // CAN_BUS_HPP
