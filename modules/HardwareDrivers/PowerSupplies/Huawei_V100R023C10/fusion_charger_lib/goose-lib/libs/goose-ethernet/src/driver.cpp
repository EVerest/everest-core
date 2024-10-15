// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <goose-ethernet/driver.hpp>

using namespace goose_ethernet;

void EthernetInterfaceIntf::send_packet(const EthernetFrame& frame) {
    auto serialized = frame.serialize();
    this->send_packet_raw(serialized.data(), serialized.size());
}

std::optional<EthernetFrame> EthernetInterfaceIntf::receive_packet() {
    auto received = this->receive_packet_raw();
    if (!received.has_value()) {
        return std::nullopt;
    }
    if (received.value().size() < 60) {
        return std::nullopt;
    }
    return EthernetFrame(received.value());
}
