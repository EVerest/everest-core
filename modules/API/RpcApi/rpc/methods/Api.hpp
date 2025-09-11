// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef METHODS_API_HPP
#define METHODS_API_HPP

#include <optional>
#include <string>

#include "../../data/DataStore.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_API_HELLO = "API.Hello";

/// This class includes all methods of the API namespace.
/// It contains the data object and the methods to access it.
class Api {
public:
    // Constructor and Destructor
    Api() = delete;
    explicit Api(DataStoreCharger& dataobj) : m_dataobj(dataobj), m_authentication_required(false){};

    ~Api() = default;

    // Methods
    RPCDataTypes::HelloResObj hello();
    void set_authentication_required(bool required);
    bool is_authentication_required() const;
    void set_api_version(const std::string& version);
    std::string get_api_version() const;
    void set_authenticated(bool authenticated);
    bool is_authenticated() const;

private:
    DataStoreCharger& m_dataobj;
    bool m_authentication_required;
    // optional
    std::optional<bool> m_authenticated;
    std::string m_api_version;
};

} // namespace methods

#endif // METHODS_API_HPP
