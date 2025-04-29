// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef EVSE_HPP
#define EVSE_HPP

#include <optional>
#include <vector>

#include "../../data/DataStore.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_EVSE_GET_EVSE_INFO = "EVSE.GetEVSEInfos";

/// This class includes all methods of the EVSE namespace.
/// It contains the data object and the methods to access it.
class Evse {
public:
    // Constructor and Destructor
    Evse() = delete;
    Evse(DataStoreCharger& dataobj) : m_dataobj(dataobj){};

    ~Evse() = default;

    // Methods
    RPCDataTypes::EVSEGetInfoResObj getEVSEInfos(std::string evse_id) {
        RPCDataTypes::EVSEGetInfoResObj res;

        // Check if evse info is available
        if (m_dataobj.evses.empty()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }

        // Iterate over all EVSEs and add the EVSEInfo objects to the response
        for (const auto& evse : m_dataobj.evses) {
            if (not evse->evseinfo.get_data().has_value()) {
                res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
                return res;
            }

            const auto& data = evse->evseinfo.get_data().value();
            if (data.id == evse_id) {
                res.info = data;
                res.error = RPCDataTypes::ResponseErrorEnum::NoError;
                return res;
            }
        }
        // No EVSE found with the given ID, return an error
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID;
        return res;
    };

private:
    DataStoreCharger& m_dataobj;
};

} // namespace methods

#endif // EVSE_HPP
