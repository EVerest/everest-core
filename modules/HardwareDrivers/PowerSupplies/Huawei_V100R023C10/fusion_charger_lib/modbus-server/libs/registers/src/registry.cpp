// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-registers/registry.hpp>

using namespace modbus::registers::complex_registers;
using namespace modbus::registers::registry;

ComplexRegisterSubregistry::ComplexRegisterSubregistry() : registers() {
}

void ComplexRegisterSubregistry::add(ComplexRegisterUntyped* reg) {
    registers.push_back(std::unique_ptr<ComplexRegisterUntyped>(reg));
}

void ComplexRegisterSubregistry::verify_internal_overlap() {
    std::vector<uint16_t> used_addresses;
    for (auto& reg : registers) {
        for (uint16_t i = 0; i < reg->get_size(); ++i) {
            uint16_t address = reg->get_start_address() + i;
            if (std::find(used_addresses.begin(), used_addresses.end(), address) != used_addresses.end()) {
                throw std::runtime_error("Overlapping register addresses");
            }
            used_addresses.push_back(address);
        }
    }
}

std::vector<ComplexRegisterUntyped*> ComplexRegisterSubregistry::get_registers() {
    std::vector<complex_registers::ComplexRegisterUntyped*> result;
    for (auto& reg : registers) {
        result.push_back(reg.get());
    }
    return result;
}

void ComplexRegisterRegistry::verify_overlap() {
    std::vector<uint16_t> used_addresses;

    for (auto& reg : get_all_registers()) {
        for (uint16_t i = 0; i < reg->get_size(); ++i) {
            uint16_t address = reg->get_start_address() + i;
            if (std::find(used_addresses.begin(), used_addresses.end(), address) != used_addresses.end()) {
                throw std::runtime_error("Overlapping register addresses");
            }
            used_addresses.push_back(address);
        }
    }
}

void ComplexRegisterRegistry::on_write(uint16_t address, const std::vector<uint8_t>& data) {
    if (data.size() % 2 != 0) {
        throw std::runtime_error("Data size must be a multiple of 2 bytes");
    }

    for (auto& reg : get_all_registers()) {
        // unit: 2b
        uint16_t start_address = reg->get_start_address();
        // unit: 2b
        uint16_t end_address = start_address + reg->get_size() - 1;
        // unit: 2b
        uint16_t data_size = data.size() / 2;

        // check overlap with write region
        if (address + data_size <= start_address || address > end_address) { // todo: check this condition
            continue;
        }

        // unit 2b
        int32_t vector_offset = address - start_address;
        if (vector_offset < 0) {
            auto data_offset = std::vector<uint8_t>(data.begin() - vector_offset * 2, data.end());
            reg->on_write(0, data_offset);
        } else {
            reg->on_write(vector_offset, data);
        }
    }
}

std::vector<uint8_t> ComplexRegisterRegistry::on_read(uint16_t address, uint16_t size) {
    if (size == 0) {
        return {};
    }

    std::vector<uint8_t> result(size * 2, 0);
    std::vector<bool> is_empty(size * 2, true);

    uint32_t read_start_address_b = address * 2;
    uint32_t read_size_b = size * 2;

    for (auto& reg : get_all_registers()) {
        // unit: 2b
        uint16_t start_address = reg->get_start_address();
        // unit: 2b
        uint16_t end_address = start_address + reg->get_size() - 1;

        // check overlap with read region
        if (address + size <= start_address || address > end_address) { // todo: check this condition
            continue;
        }

        uint32_t data_start_address_b = start_address * 2;
        uint32_t data_end_address_b = end_address * 2;

        std::vector<uint8_t> data = reg->on_read();

        // delete data before read_start_address_b
        if (read_start_address_b > data_start_address_b) {
            data.erase(data.begin(), data.begin() + (read_start_address_b - data_start_address_b));
            data_start_address_b = read_start_address_b;
        }

        // delete data after read_end_address_b
        if (read_start_address_b + read_size_b < data_end_address_b) {
            data.erase(data.begin() + (read_start_address_b + read_size_b - data_start_address_b), data.end());
            data_end_address_b = read_start_address_b + read_size_b;
        }

        // copy data to result
        int32_t result_offset = data_start_address_b - read_start_address_b;
        for (uint32_t i = 0; i < data.size(); ++i) {
            if (is_empty[i + result_offset]) {
                result[i + result_offset] = data[i];
                is_empty[i + result_offset] = false;
            }
        }
    }

    // todo: check if some registers are not written (is_empty) and fail?

    return result;
}

void ComplexRegisterRegistry::add(std::shared_ptr<ComplexRegisterSubregistry> registry) {
    this->registries.push_back(std::move(registry));
}

std::vector<ComplexRegisterUntyped*> ComplexRegisterRegistry::get_all_registers() {
    std::vector<ComplexRegisterUntyped*> result;
    for (auto& reg : registries) {
        for (auto& subreg : reg->get_registers()) {
            result.push_back(subreg);
        }
    }
    return result;
}
