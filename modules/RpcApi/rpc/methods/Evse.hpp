// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef EVSE_HPP
#define EVSE_HPP

#include <optional>
#include <vector>

#include "../../data/DataStore.hpp"
#include "../../rpc/RequestHandlerInterface.hpp"

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_EVSE_GET_INFO = "EVSE.GetInfo";
static const std::string METHOD_EVSE_GET_STATUS = "EVSE.GetStatus";
static const std::string METHOD_EVSE_GET_HARDWARE_CAPABILITIES = "EVSE.GetHardwareCapabilities";
static const std::string METHOD_EVSE_SET_CHARGING_ALLOWED = "EVSE.SetChargingAllowed";
static const std::string METHOD_EVSE_SET_AC_CHARGING = "EVSE.SetACCharging";

/// This class includes all methods of the EVSE namespace.
/// It contains the data object and the methods to access it.
class Evse {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    Evse() = delete;
    Evse(data::DataStoreCharger& dataobj, std::unique_ptr<request_interface::RequestHandlerInterface> req_handler) :
    m_dataobj(dataobj), m_request_handler_ptr(std::move(req_handler)) {}

    ~Evse() = default;

    // Methods
    data::DataStoreEvse* getEVSEStore(const std::string& evse_id) {
        if (m_dataobj.evses.empty()) {
            return nullptr;
        }

        for (const auto& evse : m_dataobj.evses) {
            if (not evse->evseinfo.get_data().has_value()) {
                continue;
            }

            const auto data = evse->evseinfo.get_data().value();
            if (data.id == evse_id) {
                return evse.get();
            }
        }
        return nullptr;
    };

    RPCDataTypes::EVSEGetInfoResObj getInfo(const std::string& evse_id) {
        RPCDataTypes::EVSEGetInfoResObj res {};

        auto evse = getEVSEStore(evse_id);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID;
            return res;
        }

        if (!evse->evseinfo.get_data().has_value()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }

        res.info = evse->evseinfo.get_data().value();
        res.error = RPCDataTypes::ResponseErrorEnum::NoError;
        return res;
    };

    RPCDataTypes::EVSEGetStatusResObj getStatus(const std::string& evse_id) {
        RPCDataTypes::EVSEGetStatusResObj res {};

        auto evse = getEVSEStore(evse_id);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID;
            return res;
        }

        if (!evse->evsestatus.get_data().has_value()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }

        res.status = evse->evsestatus.get_data().value();
        res.error = RPCDataTypes::ResponseErrorEnum::NoError;
        return res;
    };

    RPCDataTypes::EVSEGetHardwareCapabilitiesResObj getHardwareCapabilities(const std::string& evse_id) {
        RPCDataTypes::EVSEGetHardwareCapabilitiesResObj res {};

        auto evse = getEVSEStore(evse_id);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID;
            return res;
        }

        if (evse->connectors.empty() || !evse->connectors[0]->hardwarecapabilities.get_data().has_value()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }

        res.hardware_capabilities = evse->connectors[0]->hardwarecapabilities.get_data().value();
        return res;
    };

    RPCDataTypes::ErrorResObj setChargingAllowed(const std::string& evse_id, bool charging_allowed) {
        RPCDataTypes::ErrorResObj res {};
        auto evse = getEVSEStore(evse_id);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID;
            return res;
        }
        auto status = evse->evsestatus.get_data();
        status.value().charging_allowed = charging_allowed;
        evse->evsestatus.set_data(status.value());

        res.error = RPCDataTypes::ResponseErrorEnum::NoError;
        return res;
    };

    RPCDataTypes::ErrorResObj setACCharging(const std::string& evse_id, bool charging_allowed, float max_current, std::optional<int> phase_count) {
        RPCDataTypes::ErrorResObj res {};
        return res;
    };

private:
    // Reference to the DataStoreCharger object that holds EVSE data
    data::DataStoreCharger& m_dataobj;
    // Reference to the RequestHandlerInterface object for handling requests
    std::unique_ptr<request_interface::RequestHandlerInterface> m_request_handler_ptr;
};

} // namespace methods

#endif // EVSE_HPP
