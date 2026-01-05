// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <ctime>
#include <goose/frame.hpp>

namespace fusion_charger {
namespace goose {
namespace utils {

::goose::frame::ber::BEREntry make_u16(std::uint16_t value);

::goose::frame::ber::BEREntry make_f32(float value);

std::uint16_t expect_u16(const ::goose::frame::ber::BEREntry& entry);

float expect_f32(const ::goose::frame::ber::BEREntry& entry);

}; // namespace utils
}; // namespace goose
}; // namespace fusion_charger
