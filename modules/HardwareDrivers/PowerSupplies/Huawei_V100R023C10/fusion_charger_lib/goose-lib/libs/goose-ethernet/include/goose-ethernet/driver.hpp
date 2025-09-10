// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#ifdef __APPLE__
#include <pcap/pcap.h>
#endif

#include "frame.hpp"

namespace goose_ethernet {

class EthernetInterfaceIntf {
protected:
    virtual void send_packet_raw(const uint8_t* packet, size_t size) = 0;
    virtual std::optional<std::vector<uint8_t>> receive_packet_raw() = 0;

public:
    virtual ~EthernetInterfaceIntf() = default;

    // send frame, throws runtime_error on failure or SerializeError if the
    // frame could not be serialized
    void send_packet(const EthernetFrame& frame);
    // receive frame blocking, throws runtime_error on failure or DeserializeError
    // if the frame could not be deserialized
    std::optional<EthernetFrame> receive_packet();

    virtual const uint8_t* get_mac_address() const = 0;
};

class EthernetInterface : public EthernetInterfaceIntf {
protected:
    uint8_t mac_address[6];
#ifdef __APPLE__
    pcap_t* pcap;
#else
    int fd;
#endif

    // todo: packet abstraction
    void send_packet_raw(const uint8_t* packet, size_t size) override;
    std::optional<std::vector<uint8_t>> receive_packet_raw() override;

public:
    EthernetInterface(const char* interface_name);
    ~EthernetInterface();

    const uint8_t* get_mac_address() const override;
};

}; // namespace goose_ethernet
