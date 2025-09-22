// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <string>

namespace everest::lib::API {

template <class T> bool deserialize(std::string const& json_data, T& obj) {
    return adl_deserialize(json_data, obj);
}

template <> bool deserialize(std::string const& data, bool& obj);
template <> bool deserialize(std::string const& data, int& obj);
template <> bool deserialize(std::string const& data, double& obj);
template <> bool deserialize(std::string const& data, float& obj);
template <> bool deserialize(std::string const& data, std::string& obj);

} // namespace everest::lib::API
