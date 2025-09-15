// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <goose-ethernet/driver.hpp>
#include <logs/logs.hpp>
#include <mutex>
#include <thread>
#include <vector>

#include "frame.hpp"

namespace goose {
namespace sender {

class SendPacketIntf {
public:
    struct PerPacketInfo {
        uint16_t sq_num;
        uint16_t st_num;
    };

    virtual ~SendPacketIntf() = default;

    virtual goose_ethernet::EthernetFrame build_packet(const PerPacketInfo& info) = 0;
};

class SendPacketNormal : public SendPacketIntf {
protected:
    goose::frame::GooseFrame frame;

public:
    SendPacketNormal(goose::frame::GooseFrame frame) : frame(frame) {
    }
    goose_ethernet::EthernetFrame build_packet(const PerPacketInfo& info) override {
        frame.pdu.st_num = info.st_num;
        frame.pdu.sq_num = info.sq_num;
        return frame.serialize();
    }
};

class SendPacketSecure : public SendPacketIntf {
protected:
    goose::frame::SecureGooseFrame frame;
    std::vector<uint8_t> hmac_key;

public:
    SendPacketSecure(goose::frame::SecureGooseFrame frame, std::vector<uint8_t> hmac_key) :
        frame(frame), hmac_key(hmac_key) {
    }

    goose_ethernet::EthernetFrame build_packet(const PerPacketInfo& info) override {
        frame.pdu.st_num = info.st_num;
        frame.pdu.sq_num = info.sq_num;
        return frame.serialize(hmac_key);
    }
};

class SenderIntf {
public:
    /**
     * @brief Update the currently sent packet, the heap-allocated variant.
     *
     * @param packet the new packet to send, allocated on the heap via \c new
     * (converted to \c std::unique_ptr )
     */
    virtual void send(SendPacketIntf* packet) = 0;

    /**
     * @brief Update the currently sent packet, the \c std::unique_ptr variant.
     *
     * @param packet the new packet to send
     */
    virtual void send(std::unique_ptr<SendPacketIntf> packet) = 0;

    /**
     * @brief The thread's main function, to be run in a loop without delay
     *
     */
    virtual void run() = 0;

    /**
     * @brief Start the sender thread; runs \c run() repeatedly
     */
    virtual void start() = 0;

    /**
     * @brief If using \c start(), this will stop the sender thread with a
     * maximum delay of \c t0
     *
     * @note only works if \c start() was called before
     */
    virtual void stop() = 0;
};

/**
 * @brief An implementation of a GOOSE sender which retransmits GOOSE frames
 *
 */
class Sender : public SenderIntf {
protected:
    std::chrono::milliseconds t0;
    std::vector<std::chrono::milliseconds> ts;

    size_t current_ts_index = 0;

    std::optional<std::thread> thread;
    bool stop_flag = false;
    std::atomic<bool> has_new_package = false;
    std::shared_ptr<goose_ethernet::EthernetInterfaceIntf> intf;

    std::optional<std::unique_ptr<SendPacketIntf>> current_packet;
    std::mutex current_packet_mutex;
    std::condition_variable current_packet_cv; // Condition variable to notify the sender thread of a
                                               // new packet to send; may also be used to signal a
                                               // stop

    uint16_t st_num;
    uint16_t sq_num;

    logs::LogIntf log;

public:
    /**
     * @brief Create a new sender with T_0 and multiple \f$T_i\f$ (with
     * \f$t\in\N_1^+\f$)
     *
     * @param t0 the maximum delay between two frames; if no frame is sent within
     * this time, the last frame is retransmitted
     * @param ts the delays between the initial retransmits
     * @param intf the interface to send the frames on
     */
    Sender(std::chrono::milliseconds t0, std::vector<std::chrono::milliseconds> ts,
           std::shared_ptr<goose_ethernet::EthernetInterfaceIntf> intf, logs::LogIntf log = logs::log_printf);

    Sender(Sender&& other) : t0(other.t0), ts(other.ts), intf(other.intf), log(other.log) {
    }

    const uint8_t* get_mac_address() const;
    void send(SendPacketIntf* packet) override;
    void send(std::unique_ptr<SendPacketIntf> packet) override;
    void run() override;
    void start() override;
    void stop() override;
};

}; // namespace sender
} // namespace goose
