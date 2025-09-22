/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <endian.h>

// Helper template to ensure type safety for conversion operations
template <typename T> struct is_conversion_safe {
    static constexpr bool value =
        std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T> && !std::is_pointer_v<T>;
};

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint8_t) && is_conversion_safe<T>::value, T>
from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    T ret;
    memcpy(&ret, &raw[idx], 1);
    return ret;
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint16_t) && is_conversion_safe<T>::value, T>
from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint16_t tmp;
    memcpy(&tmp, raw.data() + idx, sizeof(uint16_t)); // Safe copy from buffer
    tmp = be16toh(tmp);                               // Convert endianness
    T ret;
    memcpy(&ret, &tmp, sizeof(T));
    return ret;
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint32_t) && is_conversion_safe<T>::value, T>
from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint32_t tmp;
    memcpy(&tmp, raw.data() + idx, sizeof(uint32_t)); // Safe copy from buffer
    tmp = be32toh(tmp);                               // Convert endianness
    T ret;
    memcpy(&ret, &tmp, sizeof(T));
    return ret;
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value && is_conversion_safe<T>::value && (sizeof(T) == 4), T>
from_raw(const std::vector<uint8_t>& raw, std::size_t idx) {
    constexpr std::size_t N = 4;
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    if (idx + N > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }

    uint32_t tmp;
    std::memcpy(&tmp, raw.data() + idx, sizeof(tmp));
    tmp = be32toh(tmp);

    float f;
    std::memcpy(&f, &tmp, sizeof(f));
    return static_cast<T>(f);
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint64_t) && is_conversion_safe<T>::value, T>
from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint64_t tmp;
    memcpy(&tmp, raw.data() + idx, sizeof(uint64_t)); // Safe copy from buffer
    tmp = be64toh(tmp);                               // Convert endianness
    T ret;
    memcpy(&ret, &tmp, sizeof(T));
    return ret;
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint8_t) && is_conversion_safe<T>::value>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint8_t tmp;
    memcpy(&tmp, &src, sizeof(T));
    dest.push_back(tmp);
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint16_t) && is_conversion_safe<T>::value>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint16_t tmp;
    memcpy(&tmp, &src, sizeof(T));
    tmp = htobe16(tmp);

    // Use array for better alignment guarantees
    alignas(uint16_t) uint8_t ret[sizeof(uint16_t)];
    memcpy(ret, &tmp, sizeof(uint16_t));
    dest.insert(dest.end(), {ret[0], ret[1]});
}

template <class T>
typename std::enable_if_t<std::is_integral<T>::value && is_conversion_safe<T>::value && (sizeof(T) == 4)>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint32_t tmp;
    memcpy(&tmp, &src, sizeof(T));
    tmp = htobe32(tmp);

    // Use array for better alignment guarantees
    alignas(uint32_t) uint8_t ret[sizeof(uint32_t)];
    memcpy(ret, &tmp, sizeof(uint32_t));
    dest.insert(dest.end(), {ret[0], ret[1], ret[2], ret[3]});
}

template <typename T>
typename std::enable_if_t<std::is_floating_point<T>::value && is_conversion_safe<T>::value && (sizeof(T) == 4)>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint32_t tmp = src;
    memcpy(&tmp, &src, sizeof(T));
    tmp = htobe32(static_cast<uint32_t>(tmp));

    // Use array for better alignment guarantees
    alignas(float) uint8_t ret[sizeof(float)];
    memcpy(ret, &tmp, sizeof(float));
    dest.insert(dest.end(), {ret[0], ret[1], ret[2], ret[3]});
}

template <class T>
typename std::enable_if_t<sizeof(T) == sizeof(uint64_t) && is_conversion_safe<T>::value>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint64_t tmp;
    memcpy(&tmp, &src, sizeof(T));
    tmp = htobe64(tmp);

    // Use array for better alignment guarantees
    alignas(uint64_t) uint8_t ret[sizeof(uint64_t)];
    memcpy(ret, &tmp, sizeof(uint64_t));
    dest.insert(dest.end(), {ret[0], ret[1], ret[2], ret[3], ret[4], ret[5], ret[6], ret[7]});
}

#endif // CONVERSIONS_HPP
