// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest_api_types/utilities/codec.hpp"
#include "everest_api_types/generic/codec.hpp"

namespace everest::lib::API {

template <> bool deserialize(std::string const& data, bool& obj) {
    return everest::lib::API::V1_0::types::generic::adl_deserialize(data, obj);
}

template <> bool deserialize(std::string const& data, int& obj) {
    return everest::lib::API::V1_0::types::generic::adl_deserialize(data, obj);
}

template <> bool deserialize(std::string const& data, double& obj) {
    return everest::lib::API::V1_0::types::generic::adl_deserialize(data, obj);
}

template <> bool deserialize(std::string const& data, float& obj) {
    return everest::lib::API::V1_0::types::generic::adl_deserialize(data, obj);
}

template <> bool deserialize(std::string const& data, std::string& obj) {
    return everest::lib::API::V1_0::types::generic::adl_deserialize(data, obj);
}

} // namespace everest::lib::API
