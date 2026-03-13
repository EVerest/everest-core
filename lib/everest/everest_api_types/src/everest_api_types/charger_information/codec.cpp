// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "charger_information/codec.hpp"
#include "charger_information/API.hpp"
#include "charger_information/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::charger_information {

std::string serialize(ChargerInformation val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, ChargerInformation const& val) {
    os << serialize(val);
    return os;
}

template <> ChargerInformation deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ChargerInformation obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::charger_information
