// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef EXTENSIONS_HELPERS_
#define EXTENSIONS_HELPERS_

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

#ifdef UNIT_TEST
#include <iostream>
#endif

#include <openssl_util.hpp>

namespace tls {

using openssl::log_warning;

/**
 * \brief update position and remaining by amount
 * \param[inout] ptr the pointer to increment
 * \param[inout] remaining the value to decrement
 */
constexpr void update_position(const std::uint8_t*& ptr, std::int32_t& remaining, std::size_t amount) {
    ptr += amount;
    remaining -= amount;
}

/**
 * \brief update position and remaining by amount
 * \param[inout] ptr the pointer to increment
 * \param[inout] remaining the value to decrement
 */
constexpr void update_position(std::uint8_t*& ptr, std::int32_t& remaining, std::size_t amount) {
    ptr += amount;
    remaining -= amount;
}

/**
 * \brief copy structure from data pointer
 * \param[out] dest the destination structure
 * \param[inout] ptr the pointer the start of the data, updated to point to the
 *               next byte (ptr += sizeof(dest))
 * \param[inout] remaining updated with the remaining number of bytes
 *               (remaining -= sizeof(dest))
 * \param[in] err_message to log on error
 * \return true when structure populated from data
 */
template <typename T>
constexpr bool struct_copy(T& dest, const std::uint8_t*& ptr, std::int32_t& remaining, const std::string& err_message) {
    bool bResult{false};
    if (remaining < sizeof(T)) {
        log_warning(err_message);
    } else {
        std::memcpy(&dest, ptr, sizeof(T));
        update_position(ptr, remaining, sizeof(T));
        bResult = true;
    }
    return bResult;
}

/**
 * \brief copy structure to data pointer
 * \param[out] ptr the destination pointer, updated to point to the
 *             next byte (ptr += sizeof(src))
 * \param[inout] src the source structure
 * \param[inout] remaining updated with the remaining number of bytes
 *               (remaining -= sizeof(src))
 * \param[in] err_message to log on error
 * \return true when ptr populated from structure
 */
template <typename T>
constexpr bool struct_copy(std::uint8_t*& ptr, const T& src, std::int32_t& remaining, const std::string& err_message) {
    bool bResult{false};
    if (remaining < sizeof(T)) {
        log_warning(err_message);
    } else {
        std::memcpy(ptr, &src, sizeof(T));
        update_position(ptr, remaining, sizeof(T));
        bResult = true;
    }
    return bResult;
}

/**
 * \brief copy DER to data pointer
 * \param[out] ptr the destination pointer, updated to point to the
 *             next byte (ptr += src.size())
 * \param[inout] src the DER source object
 * \param[inout] remaining updated with the remaining number of bytes
 *               (remaining -= src.size())
 * \param[in] err_message to log on error
 * \return true when ptr populated from structure
 */
inline bool der_copy(std::uint8_t*& ptr, const openssl::DER& src, std::int32_t& remaining,
                     const std::string& err_message) {
    bool bResult{false};
    if (remaining < src.size()) {
        log_warning(err_message + ' ' + std::to_string(remaining) + '/' + std::to_string(src.size()));
    } else {
        std::memcpy(ptr, src.get(), src.size());
        update_position(ptr, remaining, src.size());
        bResult = true;
    }
    return bResult;
}

/**
 * \brief convert a big endian 3 byte (24 bit) unsigned value to uint32
 * \param[in] ptr the pointer to the most significant byte
 * \return the interpreted value
 */
constexpr std::uint32_t uint24(const std::uint8_t* ptr) {
    return (static_cast<std::uint32_t>(ptr[0]) << 16U) | (static_cast<std::uint32_t>(ptr[1]) << 8U) |
           static_cast<std::uint32_t>(ptr[2]);
}

/**
 * \brief convert a uint32 to big endian 3 byte (24 bit) value
 * \param[in] ptr the pointer to the most significant byte
 * \param[in] value the 24 bit value
 */
constexpr void uint24(std::uint8_t* ptr, std::uint32_t value) {
    ptr[0] = (value >> 16U) & 0xffU;
    ptr[1] = (value >> 8U) & 0xffU;
    ptr[2] = value & 0xffU;
}

/**
 * \brief convert a big endian 2 byte (16 bit) unsigned value to uint16
 * \param[in] ptr the pointer to the most significant byte
 * \return the interpreted value
 */
constexpr std::uint16_t uint16(const std::uint8_t* ptr) {
    return (static_cast<std::uint32_t>(ptr[0]) << 8U) | static_cast<std::uint32_t>(ptr[1]);
}

/**
 * \brief convert a uint16 to big endian 2 byte (16 bit) value
 * \param[in] ptr the pointer to the most significant byte
 * \param[in] value the 16 bit value
 */
constexpr void uint16(std::uint8_t* ptr, std::uint32_t value) {
    ptr[0] = (value >> 8U) & 0xffU;
    ptr[1] = value & 0xffU;
}

template <typename T> std::string to_string(const T& digest) {
    std::stringstream string_stream;
    string_stream << std::hex;
    for (const auto& c : digest) {
        string_stream << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return string_stream.str();
}

} // namespace tls

#ifdef UNIT_TEST
namespace tls::trusted_ca_keys {
struct trusted_ca_keys_t;
}

std::ostream& operator<<(std::ostream& out, const openssl::certificate_ptr& obj);
std::ostream& operator<<(std::ostream& out, const openssl::sha_1_digest_t& obj);
std::ostream& operator<<(std::ostream& out, const tls::trusted_ca_keys::trusted_ca_keys_t& obj);
std::ostream& operator<<(std::ostream& out, const openssl::DER& obj);
#endif

#endif // EXTENSIONS_HELPERS_
