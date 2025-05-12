// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef EVSE_HPP
#define EVSE_HPP

#include <optional>
#include <vector>

#include "../../data/DataStore.hpp"

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_EVSE_GET_INFO = "EVSE.GetInfo";
static const std::string METHOD_EVSE_GET_STATUS = "EVSE.GetStatus";
static const std::string METHOD_EVSE_GET_HARDWARE_CAPABILITIES = "EVSE.GetHardwareCapabilities";

/// This class includes all methods of the EVSE namespace.
/// It contains the data object and the methods to access it.
class Evse {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    Evse() = delete;
    Evse(data::DataStoreCharger& dataobj) : m_dataobj(dataobj){};

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

private:
    // Reference to the DataStoreCharger object that holds EVSE data
    data::DataStoreCharger& m_dataobj;
};

} // namespace methods

#endif // EVSE_HPP
