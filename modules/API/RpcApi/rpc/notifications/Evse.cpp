// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#include "Evse.hpp"
#include "../RpcHandler.hpp"

namespace notifications {

static const std::string NOTIFICATION_EVSE_HWCAPS_CHANGED = "EVSE.HardwareCapabilitiesChanged";
static const std::string NOTIFICATION_EVSE_STATUS_CHANGED = "EVSE.StatusChanged";
static const std::string NOTIFICATION_EVSE_METER_DATA_CHANGED = "EVSE.MeterDataChanged";

Evse::Evse(std::shared_ptr<rpc::JsonRpc2ServerWithClient> rpc_server, data::DataStoreCharger& dataobj, int precision) :
    m_dataobj(dataobj), m_rpc_server(std::move(rpc_server)), m_precision(precision) {
    for (const auto& evse : m_dataobj.evses) {
        const int32_t index = evse->evseinfo.get_index();
        evse->hardwarecapabilities.register_notification_callback(
            [this, index](const RPCDataTypes::HardwareCapabilitiesObj& data) {
                this->sendHardwareCapabilitiesChanged(index, data);
            });
        evse->evsestatus.register_notification_callback(
            [this, index](const RPCDataTypes::EVSEStatusObj& data) { this->sendStatusChanged(index, data); });
        evse->meterdata.register_notification_callback(
            [this, index](const RPCDataTypes::MeterDataObj& data) { this->sendMeterDataChanged(index, data); });
    }
};

// Notifications

void Evse::sendHardwareCapabilitiesChanged(int32_t evse_index, const RPCDataTypes::HardwareCapabilitiesObj& hwcap) {
    RPCDataTypes::EVSEHardwareCapabilitiesChangedObj hwcap_changed;
    hwcap_changed.evse_index = evse_index;
    hwcap_changed.hardware_capabilities = hwcap;
    m_rpc_server->CallNotificationWithObject(NOTIFICATION_EVSE_HWCAPS_CHANGED, hwcap_changed, m_precision);
}
void Evse::sendStatusChanged(int32_t evse_index, const RPCDataTypes::EVSEStatusObj& status) {
    RPCDataTypes::EVSEStatusChangedObj status_changed;
    status_changed.evse_index = evse_index;
    status_changed.evse_status = status;
    m_rpc_server->CallNotificationWithObject(NOTIFICATION_EVSE_STATUS_CHANGED, status_changed, m_precision);
}
void Evse::sendMeterDataChanged(int32_t evse_index, const RPCDataTypes::MeterDataObj& meter) {
    RPCDataTypes::EVSEMeterDataChangedObj meter_changed;
    meter_changed.evse_index = evse_index;
    meter_changed.meter_data = meter;
    m_rpc_server->CallNotificationWithObject(NOTIFICATION_EVSE_METER_DATA_CHANGED, meter_changed, m_precision);
}

} // namespace notifications
