// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <cstring>
#include <goose-ethernet/frame.hpp>
#include <goose/ber.hpp>
#include <vector>

namespace goose {
namespace frame {

const std::uint16_t GOOSE_ETHERTYPE = 0x88B8;

struct GooseTimestamp {
    std::uint32_t seconds;
    // only 24 lower bits are used
    std::uint32_t fraction;
    std::uint8_t quality_of_time;

    GooseTimestamp() = default;

    /**
     * @param seconds number of seconds since epoch
     * @param fraction 24-bit fraction of a second (0x1000000 = 1 second). Only
     * lower 24 bits are used
     * @param quality_of_time quality of time, see IEC 61850-8-1
     */
    GooseTimestamp(std::uint32_t seconds, std::uint32_t fraction, std::uint8_t quality_of_time) :
        seconds(seconds), fraction(fraction), quality_of_time(quality_of_time) {
    }

    GooseTimestamp(const std::vector<std::uint8_t>& raw);

    std::vector<std::uint8_t> encode() const;
    float to_ms();
    bool operator==(const GooseTimestamp& other) const;

    static GooseTimestamp from_ms(std::uint64_t ms);
    static GooseTimestamp now();
};

struct GoosePDU {
    // todo: check length
    char go_cb_ref[65]; // 64 bytes + null terminator

    std::uint32_t time_allowed_to_live; // seconds

    // todo: check length
    char dat_set[65]; // 64 bytes + null terminator

    // todo: check length
    char go_id[65]; // 64 bytes + null terminator

    // already parsed
    GooseTimestamp timestamp; // milliseconds

    std::uint32_t st_num;
    std::uint32_t sq_num;

    bool simulation;
    std::uint32_t conf_rev; // configuration revision
    std::uint8_t ndsCom;

    std::vector<ber::BEREntry> apdu_entries;

    GoosePDU() = default;
    GoosePDU(const std::vector<std::uint8_t>& pdu);

    std::vector<std::uint8_t> serialize() const;
};

struct GooseFrameIntf {
    std::uint8_t source_mac_address[6];
    std::uint8_t destination_mac_address[6];

    std::uint8_t appid[2];

    GoosePDU pdu;
    std::uint8_t priority;
    std::uint16_t vlan_id;

public:
    GooseFrameIntf() = default;
    GooseFrameIntf(const goose_ethernet::EthernetFrame& ethernet_frame);
    virtual ~GooseFrameIntf() = default;
};

// Generic Goose Frame; without IEC 62351-6 security
struct GooseFrame : public GooseFrameIntf {
public:
    GooseFrame() = default;

    /**
     * @brief GooseFrame constructor
     */
    GooseFrame(const goose_ethernet::EthernetFrame& ethernet_frame);
    GooseFrame(const GooseFrame& other) = default;

    goose_ethernet::EthernetFrame serialize() const;
};

struct SecureGooseFrame : public GooseFrameIntf {
public:
    SecureGooseFrame() = default;

    /**
     * @brief SecureGooseFrame constructor, validating the HMAC if hmac_key is
     * provided
     */
    SecureGooseFrame(const goose_ethernet::EthernetFrame& ethernet_frame,
                     std::optional<std::vector<std::uint8_t>> hmac_key);

    /**
     * @brief SecureGooseFrame constructor, not validating the HMAC
     */
    SecureGooseFrame(const goose_ethernet::EthernetFrame& ethernet_frame) :
        SecureGooseFrame(ethernet_frame, std::nullopt) {
    }

    goose_ethernet::EthernetFrame serialize(std::vector<std::uint8_t> hmac_key) const;
};

} // namespace frame
} // namespace goose
