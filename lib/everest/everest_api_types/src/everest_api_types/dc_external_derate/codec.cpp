// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "dc_external_derate/codec.hpp"
#include "dc_external_derate/API.hpp"
#include "dc_external_derate/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::dc_external_derate {

std::string serialize(ExternalDerating const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, ExternalDerating const& val) {
    os << serialize(val);
    return os;
}

template <> ExternalDerating deserialize(std::string const& val) {
    auto data = json::parse(val);
    ExternalDerating obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::dc_external_derate
