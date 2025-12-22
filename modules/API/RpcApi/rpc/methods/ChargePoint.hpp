// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef METHODS_CHARGEPOINT_HPP
#define METHODS_CHARGEPOINT_HPP

#include "../../data/DataStore.hpp"

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
using namespace data;

namespace methods {

static const std::string METHOD_CHARGEPOINT_GET_EVSE_INFOS = "ChargePoint.GetEVSEInfos";
static const std::string METHOD_CHARGEPOINT_GET_ACTIVE_ERRORS = "ChargePoint.GetActiveErrors";

/// This class includes all methods of the ChargePoint namespace.
/// It contains the data object and the methods to access it.
class ChargePoint {
public:
    // Constructor and Destructor
    ChargePoint() = delete;
    explicit ChargePoint(DataStoreCharger& dataobj) : m_dataobj(dataobj){};

    ~ChargePoint() = default;

    // Methods
    RPCDataTypes::ChargePointGetEVSEInfosResObj getEVSEInfos();
    RPCDataTypes::ChargePointGetActiveErrorsResObj getActiveErrors();

private:
    DataStoreCharger& m_dataobj;
};

} // namespace methods

#endif // METHODS_CHARGEPOINT_HPP
