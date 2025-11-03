// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <ctime>
#include <goose/frame.hpp>

namespace fusion_charger {
namespace goose {
namespace utils {

::goose::frame::ber::BEREntry make_u16(uint16_t value) {
    ::goose::frame::ber::BEREntry entry;
    entry.tag = 0x86;
    entry.value = ::goose::frame::ber::encode_be(value);
    return entry;
}

::goose::frame::ber::BEREntry make_f32(float value) {
    ::goose::frame::ber::BEREntry entry;
    entry.tag = 0x87;
    entry.value = ::goose::frame::ber::encode_be(*(uint32_t*)&value);
    return entry;
}

uint16_t expect_u16(const ::goose::frame::ber::BEREntry& entry) {
    if (entry.tag != 0x86) {
        throw std::runtime_error("Expected tag 0x86, got " + std::to_string(entry.tag));
    }
    return ::goose::frame::ber::decode_be<uint16_t>(entry.value);
}

float expect_f32(const ::goose::frame::ber::BEREntry& entry) {
    if (entry.tag != 0x87) {
        throw std::runtime_error("Expected tag 0x87, got " + std::to_string(entry.tag));
    }
    // todo: verify
    uint32_t val_u32 = ::goose::frame::ber::decode_be<uint32_t>(entry.value);
    return *(float*)&val_u32;
}

}; // namespace utils
}; // namespace goose
}; // namespace fusion_charger
