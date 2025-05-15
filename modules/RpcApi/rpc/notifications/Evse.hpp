// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef NOTIFICATIONS_EVSE_HPP
#define NOTIFICATIONS_EVSE_HPP

#include "../../data/DataStore.hpp"
#include <jsonrpccxx/client.hpp>

namespace RPCDataTypes = types::json_rpc_api;

// forward declaration
namespace rpc {
class JsonRpc2ServerWithClient;
}

namespace notifications {

class Evse {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    Evse() = delete;
    // This needs to take a copy of rpc_server for reference counting, not a reference to it
    Evse(std::shared_ptr<rpc::JsonRpc2ServerWithClient> rpc_server, data::DataStoreCharger& dataobj);
    ~Evse() = default;

    // Notifications

    void sendHardwareCapabilitiesChanged(int32_t evse_index, const RPCDataTypes::HardwareCapabilitiesObj& hwcap);
    void sendStatusChanged(int32_t evse_index, const RPCDataTypes::EVSEStatusObj& status);
    void sendMeterDataChanged(int32_t evse_index, const RPCDataTypes::MeterDataObj& meter);

private:
    // Reference to the DataStoreCharger object that holds EVSE data
    data::DataStoreCharger& m_dataobj;
    std::shared_ptr<rpc::JsonRpc2ServerWithClient> m_rpc_server;
};

} // namespace notifications

#endif // NOTIFICATIONS_EVSE_HPP
