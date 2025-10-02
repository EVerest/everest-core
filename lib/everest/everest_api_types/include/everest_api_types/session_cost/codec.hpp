// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::session_cost {

std::string serialize(TariffMessage val) noexcept;
std::string serialize(IdlePrice val) noexcept;
std::string serialize(CostCategory val) noexcept;
std::string serialize(ChargingPriceComponent val) noexcept;
std::string serialize(NextPeriodPrice val) noexcept;
std::string serialize(SessionCostChunk val) noexcept;
std::string serialize(SessionStatus val) noexcept;
std::string serialize(SessionCost val) noexcept;

std::ostream& operator<<(std::ostream& os, TariffMessage const& val);
std::ostream& operator<<(std::ostream& os, IdlePrice const& val);
std::ostream& operator<<(std::ostream& os, CostCategory const& val);
std::ostream& operator<<(std::ostream& os, ChargingPriceComponent const& val);
std::ostream& operator<<(std::ostream& os, NextPeriodPrice const& val);
std::ostream& operator<<(std::ostream& os, SessionCostChunk const& val);
std::ostream& operator<<(std::ostream& os, SessionStatus const& val);
std::ostream& operator<<(std::ostream& os, SessionCost const& val);

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

} // namespace everest::lib::API::V1_0::types::session_cost
