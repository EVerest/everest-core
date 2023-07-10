// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_SUNSPEC_BASE_HPP
#define POWERMETER_BSM_SUNSPEC_BASE_HPP

// see ./sunspec_data_types.text for further information about sunspec types.

#include "protocol_related_types.hpp"

#include <array>
#include <cstdint>
#include <endian.h>
#include <exception>
#include <initializer_list>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

using namespace std::string_literals;

enum struct PointType {
    acc32,
    bitfield32,
    enum16,
    int16,
    pad,
    sunssf,
    uint16,
    uint32,
    string,
    blob
};

namespace invalid_point_value {

const std::uint32_t acc32{0};
const std::uint32_t bitfield32{0x80000000};
const std::uint16_t enum16{0xffff};
const std::int16_t int16{static_cast<std::int16_t>(0x8000)};
const std::uint16_t pad{0x8000};
const std::uint16_t sunssf{0x8000};
const std::uint16_t uint16{0xffff};
const std::uint32_t uint32{0xffffffff};

inline bool valid_string(std::string s) {
    return not ( s.empty() || s.at(0) == 0 ); // string starts with a endmarker
}

}; // namespace invalid_point_value

// clang-format off
const std::map<PointType,std::string> point_to_string_map {

    { PointType::acc32 , "acc32" },
    { PointType::bitfield32 , "bitfield32" },
    { PointType::enum16 , "enum16" },
    { PointType::int16 , "int16" },
    { PointType::pad , "pad" },
    { PointType::sunssf , "sunssf" },
    { PointType::uint16 , "uint16" },
    { PointType::uint32 , "uint32" },
    { PointType::string , "string" },
    { PointType::blob   , "blob" },

};
// clang-format on

static std::string point_type_to_string(PointType pointType) {

    auto it = point_to_string_map.find(pointType);

    return it == point_to_string_map.end() ? "unknown type" : it->second;
}

constexpr std::uint16_t point_length_in_bytes(PointType pointType) {

    switch (pointType) {
    case PointType::acc32:
    case PointType::bitfield32:
    case PointType::uint32:
        return 4;

    case PointType::enum16:
    case PointType::int16:
    case PointType::sunssf:
    case PointType::uint16:
    case PointType::blob: // length information must be obained from model data
        return 2;

    case PointType::string: // this is a bit hackish... a point_length_in_bytes means that the initial point list has to
                            // provide a length value (see calc_offset for this).
    case PointType::pad:
        return 0;
    }
}

struct Point {
    const char* id;
    PointType pointType;
    std::uint16_t length_in_bytes = 0;
    std::uint16_t offset = 0;

    std::string to_string() const {
        std::stringstream ss;
        ss << "id: [" << std::setw(12) << id << " ]"
           << " type: [" << std::setw(12) << point_type_to_string(pointType) << " ]"
           << " length: [" << std::setw(3) << length_in_bytes << " ]"
           << " offset: [" << std::setw(3) << offset << " ]";
        return ss.str();
    }
};

namespace sunspec_decoder {

inline std::uint16_t uint16_at(const transport::DataVector& data, transport::DataVector::size_type offset) {
    return be16toh((*reinterpret_cast<const std::uint16_t*>(std::addressof(data.data()[offset]))));
}

inline std::int16_t int16_at(const transport::DataVector& data, transport::DataVector::size_type offset) {
    return be16toh((*reinterpret_cast<const std::uint16_t*>(std::addressof(data.data()[offset]))));
}

inline std::uint32_t uint32_at(const transport::DataVector& data, transport::DataVector::size_type offset) {
    return be32toh((*reinterpret_cast<const std::uint32_t*>(std::addressof(data.data()[offset]))));
}

inline std::string string_at_with_length(const transport::DataVector& data, transport::DataVector::size_type offset,
                                         transport::DataVector::size_type length) {
    // if dirty, then be completely dirty..
    return std::string(reinterpret_cast<const char*>(std::addressof(data.data()[offset])), length).c_str();
}

inline std::string string_at_with_length_from_binary_data(const transport::DataVector& data,
                                                          transport::DataVector::size_type offset,
                                                          transport::DataVector::size_type length) {

    std::stringstream ss;

    for (std::size_t index = 0; index < length; ++index)
        ss << std::hex << std::setfill('0') << std::setw(2) << int(data[index + offset]);

    return ss.str();
}

} // namespace sunspec_decoder

inline std::string point_value_to_string(const transport::DataVector& data, const Point& point) {

    switch (point.pointType) {
    case PointType::acc32:
    case PointType::bitfield32:
    case PointType::uint32:
        return std::to_string(sunspec_decoder::uint32_at(data, point.offset));
    case PointType::enum16:
    case PointType::pad:
    case PointType::sunssf:
    case PointType::uint16:
        return std::to_string(sunspec_decoder::uint16_at(data, point.offset));
    case PointType::int16:
        return std::to_string(sunspec_decoder::int16_at(data, point.offset));
    case PointType::string:
        return sunspec_decoder::string_at_with_length(data, point.offset, point.length_in_bytes);
    case PointType::blob:
        return std::string("not yet implemented!");
    }

    return std::string("Unknown point type!");
}

using PointInitializerList = std::initializer_list<Point>;

template <std::size_t N> using PointArray = std::array<Point, N>;

template <std::size_t N> constexpr std::size_t calc_model_size_in_bytes(const PointArray<N>& model_data) {

    std::size_t model_length = 0;
    for (const auto& point : model_data)
        model_length += point.length_in_bytes;

    return model_length;
}

template <std::size_t N> constexpr std::size_t calc_model_size_in_register(const PointArray<N>& model_data) {

    return calc_model_size_in_bytes(model_data) / 2;
}

template <std::size_t N> constexpr PointArray<N> calc_offset(const PointInitializerList& initList) {

    PointArray<N> result{};
    std::uint16_t offset = 0;
    auto resultIterator = result.begin();
    for (auto initItem : initList) {
        *resultIterator = initItem;
        auto length = point_length_in_bytes(initItem.pointType);
        (*resultIterator).length_in_bytes = length == 0
                                                ? initItem.length_in_bytes * 2
                                                : // this is string / pad length in registers * 2 --> length in bytes
                                                length;
        if ((*resultIterator).length_in_bytes == 0)
            throw std::logic_error("init list length error.");
        (*resultIterator).offset = offset;
        offset += (*resultIterator).length_in_bytes;
        resultIterator++;
    }
    return result;
}

// clang-format off
namespace sunspec {

using acc32      = std::uint32_t;
using bitfield32 = std::uint32_t;
using enum16     = std::uint16_t;
using int16      = std::int16_t;
using pad        = std::int16_t;
using sunssf     = std::int16_t;
using uint16     = std::uint16_t;
using uint32     = std::uint32_t;
using string     = std::string;

}
// clang-format on

template <std::size_t N, const PointInitializerList& PI> struct SunspecModelBase {

    static constexpr PointArray<N> Model{calc_offset<PI.size()>(PI)};

    const std::size_t model_size_in_bytes{calc_model_size_in_bytes(Model)};
    const std::size_t model_size_in_register{calc_model_size_in_register(Model)};

    const transport::DataVector m_data;

    std::uint16_t uint16_at(transport::DataVector::size_type offset) const {
        return sunspec_decoder::uint16_at(m_data, offset);
    }

    std::int16_t int16_at(transport::DataVector::size_type offset) const {
        return sunspec_decoder::int16_at(m_data, offset);
    }

    std::uint32_t uint32_at(transport::DataVector::size_type offset) const {
        return sunspec_decoder::uint32_at(m_data, offset);
    }

    std::string string_at_with_length(transport::DataVector::size_type offset,
                                      transport::DataVector::size_type length) const {
        return sunspec_decoder::string_at_with_length(m_data, offset, length);
    }

    std::string string_at_with_length_from_binary_data(transport::DataVector::size_type offset,
                                                       transport::DataVector::size_type length) const {
        return sunspec_decoder::string_at_with_length_from_binary_data(m_data, offset, length);
    }

    sunspec::acc32 acc32_at(transport::DataVector::size_type offset) const {
        return uint32_at(offset);
    }
    sunspec::bitfield32 bitfield32_at(transport::DataVector::size_type offset) const {
        return uint32_at(offset);
    }

    sunspec::enum16 enum16_at(transport::DataVector::size_type offset) const {
        return uint16_at(offset);
    }
    sunspec::pad pad_at(transport::DataVector::size_type offset) const {
        return uint16_at(offset);
    }
    sunspec::sunssf sunssf_at(transport::DataVector::size_type offset) const {
        return uint16_at(offset);
    }

    SunspecModelBase() = delete;

    explicit SunspecModelBase<N, PI>(const transport::DataVector& data) : m_data(data) {
        if (data.size() < model_size_in_bytes)
            throw std::runtime_error(""s + __PRETTY_FUNCTION__ + " Data container size (" +
                                     std::to_string(data.size()) + ") is smaller than model size (" +
                                     std::to_string(model_size_in_bytes) + ") !");
    }
};

template <typename MODEL>
std::ostream& model_to_stream(std::ostream& out, const MODEL& model, std::initializer_list<std::size_t> index_list) {

    for (std::size_t model_index : index_list) {
        out << "Id : " << MODEL::Model[model_index].id << " offset " << MODEL::Model[model_index].offset << " "
            << " value " << point_value_to_string(model.m_data, MODEL::Model[model_index]) << "\n";
    }

    return out;
}

#endif // POWERMETER_BSM_SUNSPEC_BASE_HPP
