// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef CHARGEPOINT_HPP
#define CHARGEPOINT_HPP

#include "../../data/DataStore.hpp"

using namespace data;

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

static const std::string METHOD_CHARGEPOINT_GET_EVSE_INFOS = "ChargePoint.GetEVSEInfos";
static const std::string METHOD_CHARGEPOINT_GET_ACTIVE_ERRORS = "ChargePoint.GetActiveErrors";

/// This class includes all methods of the ChargePoint namespace.
/// It contains the data object and the methods to access it.
class ChargePoint {
public:
    // Constructor and Destructor
    ChargePoint() = delete;
    ChargePoint(DataStoreCharger& dataobj) : m_dataobj(dataobj){};

    ~ChargePoint() = default;

    // Methods
    RPCDataTypes::ChargePointGetEVSEInfosResObj getEVSEInfos();
    RPCDataTypes::ChargePointGetActiveErrorsResObj getActiveErrors();

private:
    DataStoreCharger& m_dataobj;
};

} // namespace methods

#endif // CHARGEPOINT_HPP
