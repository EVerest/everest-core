// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef CHARGEPOINT_HPP
#define CHARGEPOINT_HPP

#include <vector>
#include <optional>

#include "../../data/DataStore.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_GET_EVSE_INFOS = "ChargePoint.GetEVSEInfos";

/// This is the ChargePoint class for the RPC methods.
/// It contains the data object and the methods to access it.
class ChargePoint {
    public:
    // Constructor and Destructor
    ChargePoint() = delete;
    ChargePoint(DataStoreCharger &dataobj) : m_dataobj(dataobj) {
    };

    ~ChargePoint() = default;

    // Methods
    RPCDataTypes::ChargePointGetEVSEInfosResObj getEVSEInfos() {
        RPCDataTypes::ChargePointGetEVSEInfosResObj res;
        // check if data is valid
        if (!m_dataobj.chargerinfo.get_data().has_value()) {
            throw std::runtime_error("Data is not valid");
        }
        // Iterate over all EVSEs and add the EVSEInfo objects to the response
        for (const auto &evse : m_dataobj.evses) {
            if (evse.evseinfo.get_data().has_value()) {
                res.infos.push_back(evse.evseinfo.get_data().value());
            }
        }

        // Error handling
        if (res.infos.empty()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
        } else {
            res.error = RPCDataTypes::ResponseErrorEnum::NoError;
        }
        
        return res;
    };

private:
    DataStoreCharger &m_dataobj;
};

} // namespace methods

#endif // CHARGEPOINT_HPP
