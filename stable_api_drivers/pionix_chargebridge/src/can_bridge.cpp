// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/can_bridge.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <chrono>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/netlink/vcan_netlink_manager.hpp>
#include <iostream>
#include <memory>
#include <protocol/cb_can_message.h>

namespace charge_bridge {
using namespace std::chrono_literals;

namespace {

void msg_cb_to_host(cb_can_message const& src, everest::lib::io::can::socket_can::ClientPayloadT& tar) {
    tar.set_can_id_with_flags(src.can_id, src.can_flags & CanFlags_EFF, src.can_flags & CanFlags_RTR,
                              src.can_flags & CanFlags_ERR);
    tar.len8_dlc = 0;
    tar.payload.resize(src.dlc);
    std::memcpy(tar.payload.data(), src.data, src.dlc);
}

void msg_host_to_cb(everest::lib::io::can::socket_can::ClientPayloadT const& src, cb_can_message& tar) {
    tar = cb_can_message_set_zero;
    tar.can_id = src.get_can_id();
    tar.can_flags = 0;
    if (src.eff_flag()) {
        tar.can_flags |= CanFlags_EFF;
    }
    if (src.rtr_flag()) {
        tar.can_flags |= CanFlags_RTR;
    }
    if (src.err_flag()) {
        tar.can_flags |= CanFlags_ERR;
    }
    tar.dlc = std::min<uint8_t>(src.payload.size(), sizeof(tar.data));
    std::memcpy(tar.data, src.payload.data(), src.payload.size());
}

bool is_data_msg([[maybe_unused]] cb_can_message const& msg) {
    return true;
}

} // namespace

can_bridge::can_bridge(can_bridge_config const& config) :
    m_udp(config.cb_remote, config.cb_port), m_can_device(config.can_device) {

    auto& manager = everest::lib::io::netlink::vcan_netlink_manager::Instance();
    auto success = manager.create(config.can_device) && manager.bring_up(config.can_device);
    if (success) {
        m_can = std::make_unique<everest::lib::io::can::socket_can>(config.can_device);
    } else {
        manager.destroy(config.can_device);
        throw std::runtime_error("Failed to setup virtual CAN device: " + config.can_device);
    }

    m_can->set_rx_handler([this](auto const& data, auto&) {
        everest::lib::io::udp::udp_client::ClientPayloadT pl;
        cb_can_message msg;
        msg_host_to_cb(data, msg);
        send_can_to_udp(msg);
    });

    m_udp.set_rx_handler([this](auto const& data, auto&) {
        everest::lib::io::can::socket_can::ClientPayloadT pl;
        cb_can_message msg;
        std::memcpy(&msg, data.buffer.data(), sizeof(cb_can_message));
        // std::cout << "bitrate: " << (int)msg.bitrate << ", error_state: " << (int)msg.error_state << std::endl;
        // std::cout << " stats tx: " << msg.statistics.frames_tx << ":" << msg.statistics.event_tx_buf_full << "\n"
        //           << " stats rx: " << msg.statistics.frames_rx << ":" << msg.statistics.event_tx_buf_full << std::endl;

        msg_cb_to_host(msg, pl);
        if (is_data_msg(msg)) {
            m_can->tx(pl);
        }
    });

    identifier = config.cb + "/" + config.item;
    m_can->set_error_handler([this](auto id, auto const& msg) {
        utilities::print_error(identifier, "CAN/HW", id) << msg << std::endl;
        if (id not_eq 0) {
            // This is a smart pointer!! Using .reset() would delete the obj!
            m_can->reset();
        }
    });

    m_udp.set_error_handler([this](auto id, auto const& msg) {
        utilities::print_error(identifier, "CAN/UDP", id) << msg << std::endl;
        if (id not_eq 0) {
            m_udp.reset();
        }
    });
}

can_bridge::~can_bridge() {
    auto& manager = everest::lib::io::netlink::vcan_netlink_manager::Instance();
    if (m_can) {
        m_can.reset();
        manager.destroy(m_can_device);
    }
}

bool can_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(m_can.get()) &&
        handler.register_event_handler(&m_udp);
    // clang-format on
}

bool can_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(m_can.get()) &&
        handler.unregister_event_handler(&m_udp);
    // clang-format on
}

void can_bridge::send_can_to_udp(cb_can_message const& msg) {
    everest::lib::io::udp::udp_client::ClientPayloadT udp_pl;
    udp_pl.buffer.resize(sizeof(cb_can_message));
    std::memcpy(udp_pl.buffer.data(), &msg, sizeof(cb_can_message));
    m_udp.tx(udp_pl);
}

} // namespace charge_bridge
