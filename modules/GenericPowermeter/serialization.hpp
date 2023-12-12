// SPDX-License-Identifier: Apache-2.0
// Copyright Qwello GmbH and Contributors to EVerest

/// @brief Serialization and deserialization for the modbus interface.
///
/// The format has a major quirk: Even though our exchanged data is four bytes
/// wide, the modbus implementation only uses the first two bytes, effectively
/// casting every element in the exchanged data from uint32_t to uint16_t. If
/// this ever gets changed, the code below must be equally adjusted.

#include <chrono>
#include <everest/logging.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
// TODO(ddo) We don't need the utils types but otherwise our compilation fails
// since the generated files aren't self contained (they need json to be
// externally included)...
#include <utils/types.hpp>

#include <generated/types/serial_comm_hub_requests.hpp>

namespace internal {

using Result = types::serial_comm_hub_requests::Result;

inline void check_message(const Result& message) {
    if (!message.value.has_value())
        throw std::invalid_argument("The `value` must be set");
}

/// @brief Deserialization for small integral types (less or equal to two bytes).
/// @param message The message read.
/// @param result The result which will contain the deserialized data.
template <typename Output, std::enable_if_t<std::is_integral<Output>::value && (sizeof(Output) <= 2), bool> = true>
inline void deserialize(const Result& message, Output& result) {
    check_message(message);

    const auto& value = message.value.value();
    if (value.size() != 1)
        throw std::runtime_error("Result must have 1 entry");

    result = static_cast<Output>(value.front());
}

/// @brief Deserialization for large integral types (more than two bytes).
template <typename Output, std::enable_if_t<std::is_integral<Output>::value && (sizeof(Output) > 2), bool> = true>
inline void deserialize(const Result& message, Output& result) {
    check_message(message);

    const auto& value = message.value.value();
    const auto size = value.size();
    if (size != 1 && size != 2)
        throw std::runtime_error("Result must have 1 or 2 entries");

    auto out = 0;
    // Given the values [0xdead, 0xbeef], we want to move the first entry by
    // 16 bits to get the final value of 0xdeadbeef:
    // 0x0000beef +
    // 0xdead0000 =
    // 0xdeadbeef
    for (size_t ii = 0; ii != size; ++ii)
        out += (value.at(size - ii - 1) << 16 * ii);

    result = out;
}

/// @brief Deserialization for float.
/// @param message The message read.
/// @param result The result which will contain the deserialized data.
template <typename Output, std::enable_if_t<std::is_floating_point<Output>::value, bool> = true>
inline void deserialize(const Result& message, Output& result) {
    uint32_t out;
    deserialize<uint32_t>(message, out);
    result = static_cast<Output>(out);
}

/// @brief Deserialization for string.
/// @param message The message read.
/// @param result The result which will contain the deserialized data.
inline void deserialize(const Result& message, std::string& result) {
    check_message(message);

    const auto& value = message.value.value();
    // The serialized data holds data in the first 2 bytes of every element. We
    // iterate over both, naming ii after the input and oo after the output.
    const auto old_size = result.size();
    result.resize(old_size + value.size() * 2);
    for (size_t ii = 0, oo = old_size; ii != value.size(); ++ii, ++oo) {
        result.at(oo) = value.at(ii) >> 8;
        result.at(++oo) = value.at(ii);
    }
}

} // namespace internal

template <typename Output> inline Output deserialize(const types::serial_comm_hub_requests::Result& message) {
    Output out;
    internal::deserialize(message, out);
    return out;
}

/// @brief Serialization for integral types.
template <typename Input, std::enable_if_t<std::is_integral<Input>::value && (sizeof(Input) <= 2), bool> = true>
inline types::serial_comm_hub_requests::VectorUint16 serialize(const Input& data) {
    return {std::vector<int32_t>{static_cast<int32_t>(data)}};
}

/// @brief Serialization for integral types.
template <typename Input, std::enable_if_t<std::is_integral<Input>::value && (sizeof(Input) > 2), bool> = true>
inline types::serial_comm_hub_requests::VectorUint16 serialize(const Input& data) {
    // The input 0xdeadbeef which is four bytes long, should be splitted into to
    // elements [0xdead, 0xbeef], each holding only two bytes of data.
    constexpr size_t factor = sizeof(Input) / sizeof(int16_t);
    std::vector<int32_t> out(factor);
    for (size_t ii = 0; ii != out.size(); ++ii)
        out.at(ii) = static_cast<uint16_t>(data >> ((factor - 1 - ii) * 16));

    return {out};
}

/// @brief Serialization for vectors for signed and unsigned chars.
template <typename Input, std::enable_if_t<std::is_integral<Input>::value && (sizeof(Input) == 1), bool> = true>
types::serial_comm_hub_requests::VectorUint16 serialize(const std::vector<Input>& data) {
    std::vector<int32_t> out(static_cast<size_t>(!data.empty()) * (data.size() / 2 + 1), 0);

    // Drop the last bit - we handle here only bytes which can be "compacted" to
    // to int16_t.
    const auto size = (data.size() >> 1) << 1;
    for (size_t ii = 0, oo = 0; ii != size; ++ii, ++oo) {
        out.at(oo) = data.at(ii++) << 8;
        out.at(oo) += data.at(ii);
    }

    // Handle the last byte - if any.
    if (data.size() % 2 == 1) {
        out.back() = data.back();
    }
    return {out};
}

/// @brief Serialization for vectors for signed and unsigned words.
template <typename Input, std::enable_if_t<std::is_integral<Input>::value && (sizeof(Input) == 2), bool> = true>
inline types::serial_comm_hub_requests::VectorUint16 serialize(const std::vector<Input>& data) {
    std::vector<int32_t> out(data.size());
    std::copy(data.begin(), data.end(), out.begin());
    return {out};
}

/// @brief Serialization for vectors of integral types.
template <typename Input, std::enable_if_t<std::is_integral<Input>::value && (sizeof(Input) > 2), bool> = true>
types::serial_comm_hub_requests::VectorUint16 serialize(const std::vector<Input>& data) {
    constexpr size_t factor = sizeof(Input) / sizeof(int16_t);
    std::vector<int32_t> out(data.size() * factor);
    auto begin = out.begin();
    for (const auto& ii : data) {
        const auto ii_out = serialize(ii);
        std::move(ii_out.data.begin(), ii_out.data.end(), begin);
        begin += ii_out.data.size();
    }

    return {out};
}

inline types::serial_comm_hub_requests::VectorUint16 serialize(const std::string& data) {
    std::vector<char> tmp(data.begin(), data.end());
    return serialize(tmp);
}

template <typename Clock>
types::serial_comm_hub_requests::VectorUint16 serialize(const std::chrono::time_point<Clock>& time_point) {
    // TODO(ddo) this is narrowing from uint to int.
    const auto seconds =
        static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch()).count());
    return serialize(seconds);
}
