// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>
#include <vector>

#include "CanBus.hpp"
#include <everest/logging.hpp>

using namespace std::chrono_literals;

namespace {
// Timer configuration constants
constexpr auto CAN_RECOVERY_TIMER_INTERVAL = 1000ms;
constexpr auto CAN_POLL_STATUS_TIMER_INTERVAL = 1000ms;
} // namespace

CanBus::CanBus() : exit_rx_thread{false}, can_bus(nullptr) {
}

CanBus::~CanBus() {
    close_device();
}

bool CanBus::open_device(const std::string& dev) {
    can_bus = std::make_unique<can::socket_can>(dev);
    can_bus->set_rx_handler([&](auto const& pl, auto&) {
        // Use get_can_id_with_flags() to preserve EFF flag for extended frames
        uint32_t can_id = pl.get_can_id_with_flags();
        this->rx_handler(can_id, pl.payload);
    });
    can_bus->set_error_handler([&](auto err, auto msg) {
        if (err != 0) {
            EVLOG_error << "CAN error: " << err << " - " << msg << std::endl;
            on_error.store(true);
        } else {
            EVLOG_info << "CAN error cleared: " << msg << std::endl;
            on_error.store(false);
        }
    });
    ev_handler.register_event_handler(can_bus.get());
    recovery_timer.set_timeout(CAN_RECOVERY_TIMER_INTERVAL);

    ev_handler.register_event_handler(&recovery_timer, [&](event::fd_event_handler::event_list const& events) {
        if (on_error.load()) {
            EVLOG_error << "CAN error detected, attempting recovery";
            can_bus->reset();
        }
    });
    poll_status_timer.set_timeout(CAN_POLL_STATUS_TIMER_INTERVAL);

    ev_handler.register_event_handler(
        &poll_status_timer, [&](event::fd_event_handler::event_list const& events) { poll_status_handler(); });
    rx_thread_handle = std::thread(&CanBus::rx_thread, this);
    return true;
}

bool CanBus::close_device() {
    if (!can_bus) {
        return true; // Already closed
    }

    EVLOG_info << "Closing CAN device";

    // Stop the RX thread first
    exit_rx_thread = true;
    if (rx_thread_handle.joinable()) {
        rx_thread_handle.join();
    }

    // Unregister event handlers (this stops timers and cleans up any pending events)
    ev_handler.unregister_event_handler(&recovery_timer);
    ev_handler.unregister_event_handler(&poll_status_timer);
    ev_handler.unregister_event_handler(can_bus.get());

    // Close CAN socket
    can_bus.reset();

    // Reset error state
    on_error.store(false);

    EVLOG_info << "CAN device closed successfully";
    return true;
}

void CanBus::rx_thread() {
    EVLOG_info << "Starting CAN RX thread" << std::endl;
    while (!exit_rx_thread) {
        ev_handler.poll();
    }
}

static std::string bytes_to_hex(const std::vector<uint8_t>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss << std::setw(2) << static_cast<unsigned>(bytes[i]);
    }
    return ss.str();
}

bool CanBus::_tx(uint32_t can_id, const std::vector<uint8_t>& payload) {
    // Validate payload size for CAN protocol compliance
    if (payload.size() > 8) {
        EVLOG_error << "CAN payload too large (" << payload.size() << " bytes), max 8 bytes allowed";
        return false;
    }

    // InfyPower protocol uses 29-bit extended CAN IDs, so we need to set the extended frame flag
    everest::lib::io::can::can_dataset data;
    data.set_can_id_with_flags(can_id | CAN_EFF_FLAG);
    data.payload = payload;

    if (on_error.load()) {
        EVLOG_error << "CAN error detected, not sending frame";
        return false;
    }
    return can_bus->tx(data);
}
