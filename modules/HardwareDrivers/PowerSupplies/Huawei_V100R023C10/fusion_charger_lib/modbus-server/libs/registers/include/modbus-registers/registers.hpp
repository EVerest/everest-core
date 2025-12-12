// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#warning "This code is only tested on little endian systems; see tests"
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "converter.hpp"
#include "data_provider.hpp"

namespace modbus {
namespace registers {
namespace complex_registers {

/**
 * @brief The base class for all complex registers. Used primarily to be
 * storable in a vector.
 *
 */
class ComplexRegisterUntyped {
protected:
    const std::uint16_t start_address;
    const std::uint16_t size;

public:
    ComplexRegisterUntyped(std::uint16_t start_address, std::uint16_t size);
    virtual ~ComplexRegisterUntyped() = default;

    /**
     * @brief Called when the register is (partially) written to, to be used by a
     * modbus server. (Most likely indirectly through a
     * \c registry::ComplexRegisterRegistry )
     *
     * @warning The offset is in 16-bit words, not in bytes!
     * @note Note that the data length can be any size, but the offset is always
     * maximum \c size - 1
     *
     * @param offset The offset in the register, in 16-bit words; (e.g. offset 1 =
     * 2 byte offset)
     * @param data The data to be written to the register, may be derived from a
     * std::uint16_t vector in big-endian order (0x1234 -> {0x12, 0x34}))
     */
    virtual void on_write(std::uint16_t offset, const std::vector<std::uint8_t>& data) = 0;

    /**
     * @brief Called when the register is (partially) read from, to be used by a
     *
     * @return std::vector<std::uint8_t> The complete register data in big-endian order
     * (can be converted to std::uint16_t vector in big-endian order ({0x12, 0x34} ->
     * 0x1234)). The length of the vector should always be double of \c size
     */
    virtual std::vector<std::uint8_t> on_read() = 0;

    /**
     * @brief Get the start address of the register in the modbus address space
     *
     * @return std::uint16_t The start address (0x0000 - 0xFFFF)
     */
    std::uint16_t get_start_address() const;
    /**
     * @brief Get the size of the register in 16-bit words, to be used in
     * conjunction with \c get_start_address to determine if a register is in the
     * range of a (read or write) request
     *
     * @return std::uint16_t The size of the register in 16-bit words
     */
    std::uint16_t get_size() const;

    /**
     * @brief Get the \c DataProviderUntyped associated with the register. This is
     * primarily used for extended functionality like unsolicitated reportings
     * (not standard modbus)
     *
     * @return DataProviderUntyped* The data provider associated with the register
     */
    virtual data_providers::DataProviderUntyped* get_data_provider() const = 0;
};

/**
 * @brief The typed version and implementation of \c ComplexRegisterUntyped.
 * This is typed for the (currently) sole purpose of type indication for the
 * associated \c DataProvider
 *
 * @tparam provider_type The type of the associated \c DataProvider
 * @tparam buffer_size The size of the buffer in bytes (not 16-bit words!). Must
 * be a multiple of 2 (statically asserted!)
 */
template <typename provider_type, size_t buffer_size> class ComplexRegister : public ComplexRegisterUntyped {
    static_assert(buffer_size > 0);
    static_assert(buffer_size % 2 == 0);

protected:
    data_providers::DataProvider<provider_type>& data_provider;
    const converters::Converter& converter;

public:
    ComplexRegister(std::uint16_t start_address, data_providers::DataProvider<provider_type>& data_provider,
                    const converters::Converter& converter) :
        ComplexRegisterUntyped(start_address, buffer_size / 2), data_provider(data_provider), converter(converter) {
    }

    virtual ~ComplexRegister() = default;

    void on_write(std::uint16_t offset, const std::vector<std::uint8_t>& data) override {
        size_t byte_offset = offset * 2;

        std::uint8_t sys_buffer[buffer_size];
        std::uint8_t net_buffer[buffer_size];

        // read the current value and convert it to network byte order
        data_provider.on_read(sys_buffer, buffer_size);
        converter.sys_to_net(sys_buffer, net_buffer, buffer_size);

        // write the new data to the buffer
        memcpy(net_buffer + byte_offset, data.data(), std::min(data.size(), sizeof(net_buffer) - byte_offset));

        // convert the buffer back to system byte order and write it to the provider
        converter.net_to_sys(net_buffer, sys_buffer, buffer_size);
        data_provider.on_write(sys_buffer, buffer_size);
    }

    std::vector<std::uint8_t> on_read() override {
        std::vector<std::uint8_t> net_buffer(buffer_size);

        std::uint8_t sys_buffer[buffer_size];
        data_provider.on_read(sys_buffer, buffer_size);
        converter.sys_to_net(sys_buffer, net_buffer.data(), buffer_size);

        return net_buffer;
    }

    data_providers::DataProviderUntyped* get_data_provider() const override {
        return &data_provider;
    }
};

/**
 * @brief A register for holding (human readable) strings. The string is stored
 * in the associated \c DataProviderString and can be read and written to. A
 * converter can be provided though in most cases the
 * \c converters::ConverterIdentity should be used. (1:1
 * mapping)
 *
 * @note Note that the string is not null terminated when read via modbus, if
 * the provided string is longer or equal to than the register size
 *
 * @tparam string_length The maximum length of the string (without null
 * terminator)
 */
template <size_t string_length> class StringRegister : public ComplexRegister<const char*, string_length> {
public:
    StringRegister(std::uint16_t start_address, data_providers::DataProviderString<string_length>& data_provider,
                   const converters::Converter& converter) :
        ComplexRegister<const char*, string_length>(start_address, data_provider, converter) {
    }

    virtual ~StringRegister() = default;
};

/**
 * @brief A register for holding uint8 arrays. The array is stored
 * in the associated \c DataProviderMemory and can be read and written to. A
 * converter can be provided though in most cases the
 * \c converters::ConverterIdentity should be used. (1:1 mapping)
 *
 * @tparam string_length The size of the array
 */
template <size_t array_size> class MemoryRegister : public ComplexRegister<const std::uint8_t*, array_size> {
public:
    MemoryRegister(std::uint16_t start_address, data_providers::DataProviderMemory<array_size>& data_provider,
                   const converters::Converter& converter) :
        ComplexRegister<const std::uint8_t*, array_size>(start_address, data_provider, converter) {
    }

    virtual ~MemoryRegister() = default;
};

/**
 * @brief A register for holding elementary types (e.g. (u)int{16,32,64}_t,
 * float, double)
 *
 * @tparam T The type of the register value, is used to derive the size of the
 * complex register and the associated \c DataProvider
 */
template <typename T> class ElementaryRegister : public ComplexRegister<T, sizeof(T)> {
    static_assert(std::is_pointer<T>::value == false);

public:
    ElementaryRegister(std::uint16_t start_address, data_providers::DataProvider<T>& data_provider,
                       const converters::Converter& converter) :
        ComplexRegister<T, sizeof(T)>(start_address, data_provider, converter) {
    }

    virtual ~ElementaryRegister() = default;
};

}; // namespace complex_registers
}; // namespace registers
}; // namespace modbus
