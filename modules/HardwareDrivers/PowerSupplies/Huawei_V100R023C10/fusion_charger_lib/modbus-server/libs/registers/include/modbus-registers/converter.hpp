// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <cstring>

namespace modbus {
namespace registers {
namespace converters {

/**
 * @brief Converters to be used by registers to convert between system types
 * (e.g. uint32_t) and
 *
 */
class Converter {
public:
    /**
     * @brief Convert from register mapped endianness to system endianness
     *
     * @param in Input buffer (mapped endianness)
     * @param out Output buffer (system endianness)
     * @param len Length of the buffers
     */
    virtual void net_to_sys(const uint8_t* in, uint8_t* out, size_t len) const = 0;

    /**
     * @brief Convert from system endianness to register mapped endianness
     *
     * @param in Input buffer (system endianness)
     * @param out Output buffer (mapped endianness)
     * @param len Length of the buffers
     */
    virtual void sys_to_net(const uint8_t* in, uint8_t* out, size_t len) const = 0;
};

/**
 * @brief Converts outer and inner Big Endian (ABCD; where A is the most
 * significant byte) to system endianness.
 * @note This is the default for 16-bit modbus registers.
 *
 * @example uint32_t sys_to_net: 0x12345678 -> 0x12, 0x34, 0x56, 0x78;
 * net_to_sys: 0x12, 0x34, 0x56, 0x78 -> 0x12345678
 */
class ConverterABCD : public Converter {
public:
    void net_to_sys(const uint8_t* in, uint8_t* out, size_t len) const override {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        for (size_t i = 0; i < len; i++) {
            out[i] = in[len - i - 1];
        }
#else
        memcpy(out, in, len);
#endif
    }

    void sys_to_net(const uint8_t* in, uint8_t* out, size_t len) const override {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        for (size_t i = 0; i < len; i++) {
            out[i] = in[len - i - 1];
        }
    }
#else
        memcpy(out, in, len);
#endif

    /**
     * @brief Singleton instance, as everything is stateless
     *
     * @return const ConverterABCD& Singleton instance
     */
    static const ConverterABCD& instance() {
        static ConverterABCD instance;
        return instance;
    }
};

/**
 * @brief Converter that does not perform any conversion, useful for string
 * registers.
 * @example uint8_t[] sys_to_net: 0x12, 0x34, 0x56, 0x78 -> 0x12, 0x34, 0x56,
 * 0x78; net_to_sys: 0x12, 0x34, 0x56, 0x78 -> 0x12, 0x34, 0x56, 0x78
 */
class ConverterIdentity : public Converter {
public:
    void net_to_sys(const uint8_t* in, uint8_t* out, size_t len) const override {
        memcpy(out, in, len);
    }

    void sys_to_net(const uint8_t* in, uint8_t* out, size_t len) const override {
        memcpy(out, in, len);
    }

    /**
     * @brief Singleton instance, as everything is stateless
     *
     * @return const ConverterABCD& Singleton instance
     */
    static const ConverterIdentity& instance() {
        static ConverterIdentity instance;
        return instance;
    }
};

}; // namespace converters
}; // namespace registers
}; // namespace modbus
