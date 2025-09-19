// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::money {

std::string serialize(CurrencyCode val) noexcept;
std::string serialize(Currency val) noexcept;
std::string serialize(MoneyAmount val) noexcept;
std::string serialize(Price val) noexcept;

std::ostream& operator<<(std::ostream& os, CurrencyCode const& val);
std::ostream& operator<<(std::ostream& os, Currency const& val);
std::ostream& operator<<(std::ostream& os, MoneyAmount const& val);
std::ostream& operator<<(std::ostream& os, Price const& val);

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

} // namespace everest::lib::API::V1_0::types::money
