// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <cstring>
#include <goose-ethernet/frame.hpp>

using namespace goose_ethernet;

EthernetFrame::EthernetFrame(const uint8_t* data, size_t size) {
    // minimum size of a normal ethernet frame is 64 bytes, without crc it is 60
    if (size < 60) {
        throw DeserializeError("Ethernet frame too short (size < 60)");
    }

    memcpy(destination, data, 6);
    memcpy(source, data + 6, 6);
    ethertype = (data[12] << 8) | data[13];
    if (ethertype == 0x8100) {
        eth_802_1q_tag = (data[14] << 8) | data[15];
        ethertype = (data[16] << 8) | data[17];
        payload = std::vector<uint8_t>(data + 18, data + size);
    } else {
        eth_802_1q_tag = std::nullopt;
        payload = std::vector<uint8_t>(data + 14, data + size);
    }

    // todo: check if redundant
    if (payload.size() < 42) {
        throw DeserializeError("Ethernet frame payload too short (payload size < 42)");
    }
}

EthernetFrame::EthernetFrame(const std::vector<uint8_t>& data) : EthernetFrame(data.data(), data.size()) {
}

std::vector<uint8_t> EthernetFrame::serialize() const {
    if (payload.size() > 1500) {
        throw SerializeError("Ethernet frame too long (payload size > 1500)");
    }

    size_t package_size = 14 + payload.size() + (eth_802_1q_tag.has_value() ? 4 : 0);

    // todo: maybe not throw but append zeros
    if (package_size < 60) {
        throw SerializeError("Ethernet frame too short (size < 60)");
    }

    std::vector<uint8_t> data;
    data.reserve(package_size);

    data.insert(data.end(), destination, destination + 6);
    data.insert(data.end(), source, source + 6);

    if (eth_802_1q_tag.has_value()) {
        data.push_back(0x81);
        data.push_back(0x00);
        data.push_back(eth_802_1q_tag.value() >> 8);
        data.push_back(eth_802_1q_tag.value() & 0xff);
    }

    data.push_back(ethertype >> 8);
    data.push_back(ethertype & 0xff);

    data.insert(data.end(), payload.begin(), payload.end());

    return data;
}

bool EthernetFrame::ethertype_is_length() {
    return ethertype <= 1500;
}
