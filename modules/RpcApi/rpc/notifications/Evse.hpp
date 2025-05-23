// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef NOTIFICATIONS_EVSE_HPP
#define NOTIFICATIONS_EVSE_HPP

// #include "../RpcHandler.hpp"
#include "../../data/DataStore.hpp"
#include <jsonrpccxx/client.hpp>

namespace RPCDataTypes = types::json_rpc_api;

namespace rpc {
class JsonRpc2ServerWithClient;
}


namespace notifications {

static const std::string NOTIFICATION_EVSE_HWCAPS_CHANGED = "EVSE.HardwareCapabilitiesChanged";
static const std::string NOTIFICATION_EVSE_STATUS_CHANGED = "EVSE.StatusChanged";
static const std::string NOTIFICATION_EVSE_METER_DATA_CHANGED = "EVSE.MeterDataChanged";

class Evse {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    Evse() = delete;
    Evse(std::shared_ptr<rpc::JsonRpc2ServerWithClient> rpc_server, data::DataStoreCharger& dataobj) : m_rpc_server(rpc_server), m_dataobj(dataobj) {
        for (const auto& evse : m_dataobj.evses) {
            const int32_t index = evse->evseinfo.get_index();
            for (const auto& connector : evse->connectors) {
                connector->hardwarecapabilities.register_notification_callback(
                    [this, index](const RPCDataTypes::HardwareCapabilitiesObj& data) {
                        this->sendHardwareCapabilitiesChanged(index, data);
                    });
            }
            evse->evsestatus.register_notification_callback(
                [this, index](const RPCDataTypes::EVSEStatusObj& data) { this->sendStatusChanged(index, data); });
            evse->meterdata.register_notification_callback(
                [this, index](const RPCDataTypes::MeterDataObj& data) { this->sendMeterDataChanged(index, data); });
        }
    };

    ~Evse() = default;

    // Notifications

    void sendHardwareCapabilitiesChanged(int32_t evse_index, const RPCDataTypes::HardwareCapabilitiesObj& hwcap) {
        RPCDataTypes::EVSEHardwareCapabilitiesChangedObj hwcap_changed;
        hwcap_changed.evse_index = evse_index;
        hwcap_changed.hardware_capabilities = hwcap;
        // FIXME: the m_server has a Send() method, which is by inheritance from JsonRpcClient
        // jsonrpccxx::call_notification(NOTIFICATION_EVSE_HWCAPS_CHANGED, hwcap_changed);
    }
    void sendStatusChanged(int32_t evse_index, const RPCDataTypes::EVSEStatusObj& status) {
        RPCDataTypes::EVSEStatusChangedObj status_changed;
        status_changed.evse_index = evse_index;
        status_changed.evse_status = status;
        // FIXME: the m_server has a Send() method, which is by inheritance from JsonRpcClient
        // jsonrpccxx::call_notification(NOTIFICATION_EVSE_STATUS_CHANGED, status_changed);
    }
    void sendMeterDataChanged(int32_t evse_index, const RPCDataTypes::MeterDataObj& meter) {
        RPCDataTypes::EVSEMeterDataChangedObj meter_changed;
        meter_changed.evse_index = evse_index;
        meter_changed.meter_data = meter;
        // FIXME: the m_server has a Send() method, which is by inheritance from JsonRpcClient
        // jsonrpccxx::call_notification(NOTIFICATION_EVSE_METER_DATA_CHANGED, meter_changed);
    }

private:
    // Reference to the DataStoreCharger object that holds EVSE data
    data::DataStoreCharger& m_dataobj;
    std::shared_ptr<rpc::JsonRpc2ServerWithClient> m_rpc_server;
};

} // namespace notifications

#endif // NOTIFICATIONS_EVSE_HPP
