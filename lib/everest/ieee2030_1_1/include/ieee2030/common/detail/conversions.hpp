// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <endian.h>

template <class T> typename std::enable_if_t<sizeof(T) == 1, T> from_raw(const std::vector<uint8_t>& raw, int idx) {
    T ret = raw[idx];
    return ret;
}

template <class T> typename std::enable_if_t<sizeof(T) == 2, T> from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint16_t tmp{};
    memcpy(&tmp, raw.data() + idx, sizeof(uint16_t)); // Safe copy from buffer
    tmp = be16toh(tmp);                               // Convert endianness

    T ret;
    memcpy(&ret, &tmp, 2);
    return ret;
}

template <class T> typename std::enable_if_t<sizeof(T) == 4, T> from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint32_t tmp{};
    memcpy(&tmp, raw.data() + idx, sizeof(uint32_t)); // Safe copy from buffer
    tmp = be32toh(tmp);                               // Convert endianness

    T ret;
    memcpy(&ret, &tmp, 4);
    return ret;
}

template <class T> typename std::enable_if_t<sizeof(T) == 8, T> from_raw(const std::vector<uint8_t>& raw, int idx) {
    if (idx + sizeof(T) > raw.size()) {
        throw std::out_of_range("from_raw: buffer access out of bounds");
    }
    uint64_t tmp{};
    memcpy(&tmp, raw.data() + idx, sizeof(uint64_t)); // Safe copy from buffer
    tmp = be64toh(tmp);                               // Convert endianness
    T ret;
    memcpy(&ret, &tmp, 8);
    return ret;
}

template <class T> typename std::enable_if_t<sizeof(T) == 1> to_raw(T src, std::vector<uint8_t>& dest) {
    uint8_t tmp{};
    memcpy(&tmp, &src, 1);
    dest.push_back(tmp);
}

// FIXME (aw): these conversions should be optimized!
template <class T> typename std::enable_if_t<sizeof(T) == 2> to_raw(T src, std::vector<uint8_t>& dest) {
    uint16_t tmp{};
    memcpy(&tmp, &src, 2);
    tmp = htobe16(tmp);
    uint8_t ret[2];
    memcpy(ret, &tmp, 2);
    dest.insert(dest.end(), {ret[0], ret[1]});
}

template <class T> typename std::enable_if_t<sizeof(T) == 4> to_raw(T src, std::vector<uint8_t>& dest) {
    uint32_t tmp{};
    memcpy(&tmp, &src, 4);
    tmp = htobe32(tmp);
    uint8_t ret[4];
    memcpy(ret, &tmp, 4);
    dest.insert(dest.end(), {ret[0], ret[1], ret[2], ret[3]});
}

template <class T> typename std::enable_if_t<sizeof(T) == 8> to_raw(T src, std::vector<uint8_t>& dest) {
    uint64_t tmp{};
    memcpy(&tmp, &src, 8);
    tmp = htobe64(tmp);
    uint8_t ret[8];
    memcpy(ret, &tmp, 8);
    dest.insert(dest.end(), {ret[0], ret[1], ret[2], ret[3], ret[4], ret[5], ret[6], ret[7]});
}
