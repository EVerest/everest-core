// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <goose/ber.hpp>

namespace goose {
namespace frame {
namespace ber {

BEREntry::BEREntry(std::vector<uint8_t>* input) {
    if (input == nullptr) {
        throw std::runtime_error("BEREntry: input is nullptr");
    }

    if (input->size() < 2) {
        throw std::runtime_error("BEREntry: input has no tag or length");
    }

    tag = (*input)[0];
    uint8_t length_octets;
    size_t length;

    if ((*input)[1] & 0x80) {
        length_octets = (*input)[1] & 0x7F;
        length = 0;
        if (length_octets > input->size() - 2) {
            throw std::runtime_error("BEREntry: input too short, length octets missing");
        }

        for (size_t i = 0; i < length_octets; i++) {
            length = (length << 8) | (*input)[2 + i];
        }
    } else {
        length_octets = 0;
        length = (*input)[1];
    }

    // Remove tag and length bytes
    input->erase(input->begin(), input->begin() + 2 + length_octets);

    // Copy and remove value bytes
    if (length > input->size()) {
        throw std::runtime_error("BEREntry: input too short, payload missing");
    }
    value.insert(value.end(), input->begin(), input->begin() + length);
    input->erase(input->begin(), input->begin() + length);
}

void BEREntry::add(const BEREntry& entry) {
    auto encoded = entry.encode();
    value.insert(value.end(), encoded.begin(), encoded.end());
}

std::vector<uint8_t> BEREntry::encode() const {
    std::vector<uint8_t> result;
    result.push_back(tag);

    size_t length = value.size();

    if (length <= 127) {
        result.push_back(length & 0x7F);
    } else {
        size_t required_bytes = 0;
        if (length & 0xFF000000) {
            required_bytes = 4;
        } else if (length & 0x00FF0000) {
            required_bytes = 3;
        } else if (length & 0x0000FF00) {
            required_bytes = 2;
        } else {
            required_bytes = 1;
        }

        result.push_back(0x80 | required_bytes);
        for (size_t i = 0; i < required_bytes; i++) {
            result.push_back((length >> (8 * (required_bytes - i - 1))) & 0xFF);
        }
    }

    result.insert(result.end(), value.begin(), value.end());
    return result;
}

}; // namespace ber
}; // namespace frame
}; // namespace goose
