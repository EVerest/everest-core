// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
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
typename std::enable_if_t<sizeof(T) == sizeof(uint32_t) && is_conversion_safe<T>::value>
to_raw(T src, std::vector<uint8_t>& dest) {
    uint32_t tmp;
    memcpy(&tmp, &src, sizeof(T));
    tmp = htobe32(tmp);

    // Use array for better alignment guarantees
    alignas(uint32_t) uint8_t ret[sizeof(uint32_t)];
    memcpy(ret, &tmp, sizeof(uint32_t));
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
