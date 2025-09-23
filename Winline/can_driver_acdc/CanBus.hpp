/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#ifndef CAN_BUS_HPP
#define CAN_BUS_HPP

#include "CanPackets.hpp"
#include <atomic>
#include <condition_variable>
#include <linux/can.h>
#include <mutex>
#include <thread>

#include <basecamp/io/can/socket_can.hpp>
#include <basecamp/io/event/fd_event_handler.hpp>
#include <basecamp/io/event/timer_fd.hpp>

using namespace basecamp::io;

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
    std::condition_variable rx_thread_cv;
    void rx_thread();
};

#endif // CAN_BUS_HPP
