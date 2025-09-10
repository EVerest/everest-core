// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "power_stack_mock/util.hpp"

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace user_acceptance_tests {
namespace test_utils {

static void print(std::ostream& stream, const char* fmt, const char* prefix, va_list args) {
    std::stringstream ss;
    ss << prefix;
    ss << ": ";

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    ss << buffer;
    ss << "\n";

    std::string result = ss.str();

    stream << result;
}

void psu_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print(std::cout, fmt, "PSU", args);
    va_end(args);
}

void vfail_printf(const char* fmt, va_list args) {
    print(std::cout, fmt, "FAIL", args);
}

void fail_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print(std::cout, fmt, "FAIL", args);
    va_end(args);
}

void tester_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print(std::cout, fmt, "TESTER", args);
    va_end(args);
}

void fdispenser_printf(std::ostream& stream, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print(stream, fmt, "DISPENSER", args);
    va_end(args);
}

float uint16_vec_to_float(std::vector<uint16_t> vec) {
    uint8_t v0[4] = {
        (uint8_t)(vec[1] & 0xFF),
        (uint8_t)(vec[1] >> 8),
        (uint8_t)(vec[0] & 0xFF),
        (uint8_t)(vec[0] >> 8),
    };

    auto f = *((float*)v0);

    return f;
}

std::vector<uint16_t> float_to_uint16_vec(float value) {
    uint8_t* v = (uint8_t*)&value;
    uint8_t v0[4] = {v[3], v[2], v[1], v[0]};

    std::vector<uint16_t> v1;
    v1.push_back((uint16_t)(v0[0] << 8 | v0[1]));
    v1.push_back((uint16_t)(v0[2] << 8 | v0[3]));

    return v1;
}

double uint16_vec_to_double(std::vector<uint16_t> vec) {
    uint8_t v0[8] = {
        (uint8_t)(vec[3] & 0xFF), (uint8_t)(vec[3] >> 8), (uint8_t)(vec[2] & 0xFF), (uint8_t)(vec[2] >> 8),
        (uint8_t)(vec[1] & 0xFF), (uint8_t)(vec[1] >> 8), (uint8_t)(vec[0] & 0xFF), (uint8_t)(vec[0] >> 8),
    };

    return *((double*)v0);
}

uint32_t uint16_vec_to_uint32(std::vector<uint16_t> vec) {
    uint16_t v0[2] = {
        (uint16_t)(vec[1]),
        (uint16_t)(vec[0]),
    };

    return *((uint32_t*)v0);
}

void dispenser_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    print(std::cout, fmt, "DISPENSER", args);
    va_end(args);
}

} // namespace test_utils

} // namespace user_acceptance_tests
