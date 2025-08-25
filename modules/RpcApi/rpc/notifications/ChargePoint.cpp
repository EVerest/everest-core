// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "ChargePoint.hpp"
#include "../RpcHandler.hpp"

namespace RPCDataTypes = types::json_rpc_api;

namespace notifications {

static const std::string NOTIFICATION_CHARGEPOINT_ACTIVE_ERRORS_CHANGED = "ChargePoint.ActiveErrorsChanged";

ChargePoint::ChargePoint(std::shared_ptr<rpc::JsonRpc2ServerWithClient> rpc_server, data::DataStoreCharger& dataobj,
                         int precision) :
    m_rpc_server(std::move(rpc_server)), m_dataobj(dataobj), m_precision(precision) {
    // Register notification callbacks for the charger errors
    m_dataobj.chargererrors.register_notification_callback(
        [this](const std::vector<types::json_rpc_api::ErrorObj>& active_errors) {
            this->sendActiveErrorsChanged(active_errors);
        });
};

// Notifications

void ChargePoint::sendActiveErrorsChanged(const std::vector<types::json_rpc_api::ErrorObj>& active_errors) {
    RPCDataTypes::ChargePointActiveErrorsChangedObj active_errors_changed;
    active_errors_changed.active_errors = active_errors;
    m_rpc_server->CallNotificationWithObject(NOTIFICATION_CHARGEPOINT_ACTIVE_ERRORS_CHANGED, active_errors_changed,
                                             m_precision);
}

} // namespace notifications
