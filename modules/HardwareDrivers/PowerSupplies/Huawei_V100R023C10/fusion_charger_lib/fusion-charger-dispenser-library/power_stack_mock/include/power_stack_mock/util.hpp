// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <stdint.h>

#include <iostream>
#include <vector>

namespace user_acceptance_tests {
namespace test_utils {

void psu_printf(const char* fmt, ...);
void tester_printf(const char* fmt, ...);

void fail_printf(const char* fmt, ...);
void vfail_printf(const char* fmt, va_list args);

void dispenser_printf(const char* fmt, ...);

void fdispenser_printf(std::ostream& stream, const char* fmt, ...);

float uint16_vec_to_float(std::vector<uint16_t> vec);
std::vector<uint16_t> float_to_uint16_vec(float value);

double uint16_vec_to_double(std::vector<uint16_t> vec);

uint32_t uint16_vec_to_uint32(std::vector<uint16_t> vec);

} // namespace test_utils

} // namespace user_acceptance_tests
