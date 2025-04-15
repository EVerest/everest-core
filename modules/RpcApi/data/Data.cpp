// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Data.hpp"

namespace data {

// ConnectorInfoStore
std::optional<nlohmann::json> ConnectorInfoStore::get_data() const {
    if (this->data_is_valid) {
        nlohmann::json j;
        to_json(j, connectorinfoobj);
        return j;
    } else {
        return std::nullopt;
    }
}
void ConnectorInfoStore::set_data(const nlohmann::json& in) {
    return;
}
void ConnectorInfoStore::init_data() {
    // do something to explicitly initialize our data structures
}

} // namespace data
