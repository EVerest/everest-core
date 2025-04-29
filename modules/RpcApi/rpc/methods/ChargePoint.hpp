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

static const std::string METHOD_CHARGEPOINT_GET_EVSE_INFOS = "ChargePoint.GetEVSEInfos";

/// This class includes all methods of the ChargePoint namespace.
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
        // Iterate over all EVSEs and add the EVSEInfo objects to the response
        for (const auto& evse : m_dataobj.evses) {
            if (evse->evseinfo.get_data().has_value()) {
                res.infos.push_back(evse->evseinfo.get_data().value());
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
