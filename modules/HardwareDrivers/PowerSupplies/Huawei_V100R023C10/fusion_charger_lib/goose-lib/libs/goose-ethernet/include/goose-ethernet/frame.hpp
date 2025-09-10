// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace goose_ethernet {

class DeserializeError : public std::runtime_error {
public:
    DeserializeError(const std::string& what) : std::runtime_error(what) {
    }
};

class SerializeError : public std::runtime_error {
public:
    SerializeError(const std::string& what) : std::runtime_error(what) {
    }
};

/**
 * @brief Ethernet frame without crc, thus minimum size is 60 bytes, payload
 * size on non-802.1Q frames is 46 bytes, on 802.1Q frames it is 42 bytes
 */
struct EthernetFrame {
    uint8_t destination[6];
    uint8_t source[6];
    std::optional<uint16_t> eth_802_1q_tag; // already in system byte order
    uint16_t ethertype;
    std::vector<uint8_t> payload;

    EthernetFrame() = default;

    /**
     * @brief Deserializing Constructor
     *
     * @param data Ethernet frame data
     * @param size Size of the data
     * @throws DeserializeError if the data is too short/long
     */
    EthernetFrame(const uint8_t* data, size_t size);

    /**
     * @brief Deserializing Constructor
     *
     * @param data Ethernet frame data
     * @throws DeserializeError if the data is too short/long
     */
    EthernetFrame(const std::vector<uint8_t>& data);

    /**
     * @brief Serialize the Ethernet frame with header and payload, without crc
     *
     * @return std::vector<uint8_t> Serialized Ethernet frame
     * @throws SerializeError if the frame is invalid; e.g. payload is too
     * long/short
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Check whether the ethertype filed is a length field, this is the
     * case for 802.3 frames. (Ethertype is present in Ethernet II frames)
     *
     * @return true ethertype is a length field
     * @return false ethertype is a type field
     */
    bool ethertype_is_length();
};

} // namespace goose_ethernet
