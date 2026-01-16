// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "Api.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

RPCDataTypes::HelloResObj Api::hello() {
    RPCDataTypes::HelloResObj res{};
    // check if data is valid
    if (!m_dataobj.chargerinfo.get_data().has_value()) {
        throw std::runtime_error("Data is not valid");
    }
    res.charger_info = m_dataobj.chargerinfo.get_data().value();
    res.authentication_required = is_authentication_required();
    res.api_version = get_api_version();
    res.everest_version = m_dataobj.everest_version;
    if (m_authenticated.has_value()) {
        res.authenticated = m_authenticated.value();
    }

    return res;
}

void Api::set_authentication_required(bool required) {
    m_authentication_required = required;
}

bool Api::is_authentication_required() const {
    return m_authentication_required;
}

void Api::set_api_version(const std::string& version) {
    m_api_version = version;
}

const std::string& Api::get_api_version() const {
    return m_api_version;
}

void Api::set_authenticated(bool authenticated) {
    m_authenticated = authenticated;
}

bool Api::is_authenticated() const {
    if (m_authenticated.has_value()) {
        return m_authenticated.value();
    }
    return false;
}

} // namespace methods
