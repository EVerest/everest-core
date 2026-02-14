// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include <string>

#include "ChargePoint.hpp"

namespace methods {

RPCDataTypes::ChargePointGetEVSEInfosResObj ChargePoint::getEVSEInfos() {
    RPCDataTypes::ChargePointGetEVSEInfosResObj res{};
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
}

RPCDataTypes::ChargePointGetActiveErrorsResObj ChargePoint::getActiveErrors() {
    RPCDataTypes::ChargePointGetActiveErrorsResObj res{};

    res.active_errors = m_dataobj.chargererrors.get_data().value_or(std::vector<RPCDataTypes::ErrorObj>{});
    res.error = RPCDataTypes::ResponseErrorEnum::NoError;

    return res;
}

} // namespace methods
