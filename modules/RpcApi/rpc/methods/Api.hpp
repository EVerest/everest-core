// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef API_HPP
#define API_HPP

#include <vector>
#include <optional>

#include "../../data/DataStore.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_API_HELLO = "API.Hello";

/// This is the API class for the RPC methods.
/// It contains the data object and the methods to access it.
class Api {
    public:
    // Constructor and Destructor
    Api() = delete;
    Api(DataStoreCharger &dataobj) : m_dataobj(dataobj) {
    };

    ~Api() = default;

    // Methods
    RPCDataTypes::HelloResObj hello() {
        RPCDataTypes::HelloResObj res;
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
    };

    void set_authentication_required(bool required) {
        m_authentication_required = required;
    };

    bool is_authentication_required() const {
        return m_authentication_required;
    };
    void set_api_version(const std::string &version) {
        m_api_version = version;
    };

    std::string get_api_version() const {
        return m_api_version;
    };

    void set_authenticated(bool authenticated) {
        m_authenticated = authenticated;
    };

    bool is_authenticated() const {
        if (m_authenticated.has_value()) {
            return m_authenticated.value();
        }
        return false;
    };

private:
    DataStoreCharger &m_dataobj;
    bool m_authentication_required;
    // optional
    std::optional<bool> m_authenticated;
    std::string m_api_version;
};

} // namespace rpc_methods

#endif // API_HPP
