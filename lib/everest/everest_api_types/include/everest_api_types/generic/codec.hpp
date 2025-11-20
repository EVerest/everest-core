// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::generic {

std::string serialize(bool val) noexcept;
std::string serialize(int val) noexcept;
std::string serialize(size_t val) noexcept;
std::string serialize(double val) noexcept;
std::string serialize(float val) noexcept;
std::string serialize(std::string const& val) noexcept;
std::string serialize(RequestReply const& val);
std::string serialize(ErrorEnum val) noexcept;
std::string serialize(Error const& val) noexcept;

std::ostream& operator<<(std::ostream& os, RequestReply const& val);
std::ostream& operator<<(std::ostream& os, const Error& val);
std::ostream& operator<<(std::ostream& os, const ErrorEnum& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::generic
