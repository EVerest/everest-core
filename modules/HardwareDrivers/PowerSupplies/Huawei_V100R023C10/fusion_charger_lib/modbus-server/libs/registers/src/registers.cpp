// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-registers/registers.hpp>

using namespace modbus::registers::complex_registers;

ComplexRegisterUntyped::ComplexRegisterUntyped(uint16_t start_address, uint16_t size) :
    start_address(start_address), size(size) {
}

uint16_t ComplexRegisterUntyped::get_start_address() const {
    return start_address;
}

uint16_t ComplexRegisterUntyped::get_size() const {
    return size;
}
