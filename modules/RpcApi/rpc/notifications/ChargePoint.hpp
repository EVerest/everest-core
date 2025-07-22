// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef NOTIFICATIONS_CHARGEPOINT_HPP
#define NOTIFICATIONS_CHARGEPOINT_HPP

#include "../../data/DataStore.hpp"
#include <jsonrpccxx/client.hpp>

namespace RPCDataTypes = types::json_rpc_api;

// forward declaration
namespace rpc {
class JsonRpc2ServerWithClient;
}

namespace notifications {

class ChargePoint {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    ChargePoint() = delete;
    // This needs to take a copy of rpc_server for reference counting, not a reference to it
    ChargePoint(std::shared_ptr<rpc::JsonRpc2ServerWithClient> rpc_server, data::DataStoreCharger& dataobj);
    ~ChargePoint() = default;

    // Notifications
    void sendActiveErrorsChanged(const std::vector<types::json_rpc_api::ErrorObj>& active_errors);

private:
    // Reference to the DataStoreCharger object that holds EVSE data
    data::DataStoreCharger& m_dataobj;
    std::shared_ptr<rpc::JsonRpc2ServerWithClient> m_rpc_server;
};

} // namespace notifications

#endif // NOTIFICATIONS_CHARGEPOINT_HPP
