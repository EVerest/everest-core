// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace goose {
namespace frame {
namespace ber {

template <typename T> std::vector<uint8_t> encode_be(T value) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < sizeof(T); i++) {
        result.push_back((value >> (8 * (sizeof(T) - i - 1))) & 0xFF);
    }
    return result;
}

template <typename T> T decode_be(const std::vector<uint8_t>& input) {
    T result = 0;
    for (size_t i = 0; i < sizeof(T) && i < input.size(); i++) {
        result = (result << 8) | input[i];
    }
    return result;
}

struct BEREntry {
    uint8_t tag;
    std::vector<uint8_t> value;

    BEREntry() = default;
    BEREntry(uint8_t tag, std::vector<uint8_t> value) : tag(tag), value(value) {
    }

    /**
     * @brief Input-modifying decoding constructor; removes read bytes from input
     *
     * @warning This constructor modifies the input vector by removing the read
     * bytes
     *
     * @param input
     */
    BEREntry(std::vector<uint8_t>* input);

    // Encode the entry into a vector of bytes and append it to the payload
    void add(const BEREntry& entry);

    std::vector<uint8_t> encode() const;
};

template <typename T> struct PrimitiveBEREntry {
    T data;
    uint8_t tag;

    PrimitiveBEREntry() = default;
    PrimitiveBEREntry(T data, uint8_t tag) : data(data), tag(tag) {
    }
    /**
     * @brief Input-modifying decoding constructor; removes read bytes from input
     *
     * @warning This constructor modifies the input vector by removing the read
     * bytes
     *
     * @param input BER encoded data
     */
    PrimitiveBEREntry(std::vector<uint8_t>& input, std::optional<uint8_t> expected_tag = std::nullopt) {
        BEREntry entry(&input); // Note: this constructor modifies the input vector

        if (expected_tag.has_value()) {
            if (entry.tag != expected_tag.value()) {
                throw std::runtime_error("PrimitiveBEREntry: tag mismatch");
            }
        }

        if (entry.value.size() > sizeof(T)) {
            throw std::runtime_error("PrimitiveBEREntry: value size too big mismatch");
        }

        data = decode_be<T>(entry.value);
        tag = entry.tag;
    }

    std::vector<uint8_t> encode() const {
        return BEREntry{tag, encode_be(data)}.encode();
    }
};

struct StringBEREntry {
    std::string data;
    uint8_t tag;

    StringBEREntry() = default;
    StringBEREntry(const std::string& data, uint8_t tag) : data(data), tag(tag) {
    }
    /**
     * @brief Input-modifying decoding constructor; removes read bytes from input
     *
     * @warning This constructor modifies the input vector by removing the read
     * bytes
     *
     * @param input BER encoded data
     */
    StringBEREntry(std::vector<uint8_t>& input, std::optional<uint8_t> expected_tag,
                   std::optional<size_t> max_length = std::nullopt) {
        BEREntry entry(&input); // Note: this constructor modifies the input vector

        if (expected_tag.has_value()) {
            if (entry.tag != expected_tag.value()) {
                throw std::runtime_error("StringBEREntry: tag mismatch");
            }
        }

        if (max_length.has_value() && entry.value.size() > max_length.value()) {
            throw std::runtime_error("StringBEREntry: value size too big mismatch");
        }

        data = std::string(entry.value.begin(), entry.value.end());
        tag = entry.tag;
    }

    std::vector<uint8_t> encode() const {
        return BEREntry(tag, std::vector<uint8_t>(data.begin(), data.end())).encode();
    }
};

} // namespace ber
} // namespace frame
} // namespace goose
