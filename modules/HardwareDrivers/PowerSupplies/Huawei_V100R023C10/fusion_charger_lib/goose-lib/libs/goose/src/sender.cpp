// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <goose/sender.hpp>

using namespace goose::sender;

Sender::Sender(std::chrono::milliseconds t0, std::vector<std::chrono::milliseconds> ts,
               std::shared_ptr<goose_ethernet::EthernetInterfaceIntf> intf, logs::LogIntf log) :
    t0(t0), ts(ts), intf(intf), st_num(0), sq_num(0), current_ts_index(0), current_packet(std::nullopt), log(log) {
}

void Sender::send(SendPacketIntf* packet) {
    send(std::unique_ptr<SendPacketIntf>(packet));
}

void Sender::send(std::unique_ptr<SendPacketIntf> packet) {
    std::lock_guard<std::mutex> lock(current_packet_mutex);
    if (current_packet.has_value() && current_packet.value() == packet) {
        /// Already sending this packet, no need to send it again, it gets resent
        /// anyways
        return;
    }

    current_packet = std::move(packet);
    st_num++;
    sq_num = 0;
    current_ts_index = 0;
    has_new_package = true;
    current_packet_cv.notify_all();
}

void Sender::start() {
    stop_flag = false;
    thread = std::thread([this] {
        for (;;) {
            if (stop_flag) {
                return;
            }
            run();
        }
    });
}

void Sender::stop() {
    if (thread.has_value()) {
        stop_flag = true;
        current_packet_cv.notify_all();

        thread->join();
        thread = std::nullopt;
    }
    stop_flag = false;
}

// Note: ran periodically
void Sender::run() {
    if (stop_flag) {
        return;
    }

    std::unique_lock<std::mutex> lock(current_packet_mutex);
    // No packet to send yet, wait for a packt to be sent using send()
    if (!current_packet.has_value()) {
        log.verbose << "Waiting for first packet...";
        current_packet_cv.wait(lock, [this] { return stop_flag || current_packet.has_value(); });
        log.verbose << "Got first packet!";
        // after wait, we own the lock and send the packet
    } else {
        std::chrono::milliseconds wait_time = t0;
        if (current_ts_index < ts.size()) {
            wait_time = ts[current_ts_index];
            current_ts_index++;
        }
        current_packet_cv.wait_for(lock, wait_time, [this] { return stop_flag || has_new_package; });
        has_new_package = false;
    }

    {
        // Maybe the stop flag was set while waiting
        if (stop_flag) {
            return;
        }
    }

    // Send the packet
    try {
        intf->send_packet(current_packet.value()->build_packet({
            sq_num,
            st_num,
        }));
    } catch (...) {
        log.error << "goose::sender: Failed to send packet";
    }
    sq_num++;

    // In the next run the else case of the if above will do the waiting
}

const std::uint8_t* Sender::get_mac_address() const {
    return intf->get_mac_address();
}
