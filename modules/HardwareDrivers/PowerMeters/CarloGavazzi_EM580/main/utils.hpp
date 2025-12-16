// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_UTILS_HPP
#define POWERMETER_UTILS_HPP

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "base64.hpp"
#include "transport.hpp"

/**
 * @brief Utility functions for converting Modbus data to various types
 * 
 * These functions handle byte order conversion from Modbus (big-endian) format
 * to native integer types. Modbus transmits data in big-endian format where
 * the most significant byte comes first.
 */

namespace modbus_utils {

// Strong type wrappers to prevent parameter swapping
struct ByteOffset {
    explicit ByteOffset(transport::DataVector::size_type v) : value(v) {}
    operator transport::DataVector::size_type() const { return value; }

private:
    transport::DataVector::size_type value;
};

struct ByteLength {
    explicit ByteLength(transport::DataVector::size_type v) : value(v) {}
    operator transport::DataVector::size_type() const { return value; }

private:
    transport::DataVector::size_type value;
};

/**
 * @brief Convert 4 bytes from Modbus data to int32_t
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 32-bit signed integer
 * @note According to EM580 Modbus spec: byte order within word is MSB->LSB,
 *       but for INT32/UINT32/UINT64, word order is LSW->MSW.
 *       So bytes are arranged as: [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
 *       which becomes: MSW_MSB MSW_LSB LSW_MSB LSW_LSB
 */
inline int32_t to_int32(const transport::DataVector& data, ByteOffset offset) {
    // Original byte order: [byte0, byte1, byte2, byte3] = [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
    // Convert to: MSW_MSB MSW_LSB LSW_MSB LSW_LSB = byte2<<24 | byte3<<16 | byte0<<8 | byte1
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    return static_cast<int32_t>(data[off + 2] << 24 | data[off + 3] << 16 | data[off] << 8 | data[off + 1]);
}

/**
 * @brief Convert 2 bytes from Modbus data to uint16_t (big-endian)
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 16-bit unsigned integer
 */
inline uint16_t to_uint16(const transport::DataVector& data, ByteOffset offset) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    return static_cast<uint16_t>(data[off] << 8 | data[off + 1]);
}

/**
 * @brief Convert 2 bytes from Modbus data to int16_t (big-endian)
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 16-bit signed integer
 */
inline int16_t to_int16(const transport::DataVector& data, ByteOffset offset) {
    uint16_t raw = to_uint16(data, offset);
    return static_cast<int16_t>(raw);
}

/**
 * @brief Convert a range of bytes to a hexadecimal string representation
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @param length Number of bytes to convert
 * @return Hexadecimal string (uppercase, no separators)
 */
inline std::string to_hex_string(const transport::DataVector& data, ByteOffset offset, ByteLength length) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    const auto len = static_cast<transport::DataVector::size_type>(length);
    std::stringstream ss;
    for (std::size_t index = 0; index < len; ++index) {
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[index + off]);
    }
    return ss.str();
}

/**
 * @brief Convert a range of bytes to a Base64-encoded string
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @param length Number of bytes to convert
 * @return Base64-encoded string
 */
inline std::string to_base64_string(const transport::DataVector& data, ByteOffset offset, ByteLength length) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    const auto len = static_cast<transport::DataVector::size_type>(length);
    auto begin = std::begin(data) + off;
    auto end = begin + len;
    return base64::encode_into<std::string>(begin, end);
}

} // namespace modbus_utils

#endif // POWERMETER_UTILS_HPP

