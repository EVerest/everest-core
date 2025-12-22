// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef METHODS_EVSE_HPP
#define METHODS_EVSE_HPP

#include <optional>

#include "../../data/DataStore.hpp"
#include "../../rpc/RequestHandlerInterface.hpp"

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
namespace methods {

static const std::string METHOD_EVSE_GET_INFO = "EVSE.GetInfo";
static const std::string METHOD_EVSE_GET_STATUS = "EVSE.GetStatus";
static const std::string METHOD_EVSE_GET_HARDWARE_CAPABILITIES = "EVSE.GetHardwareCapabilities";
static const std::string METHOD_EVSE_SET_CHARGING_ALLOWED = "EVSE.SetChargingAllowed";
static const std::string METHOD_EVSE_GET_METER_DATA = "EVSE.GetMeterData";
static const std::string METHOD_EVSE_SET_AC_CHARGING = "EVSE.SetACCharging";
static const std::string METHOD_EVSE_SET_AC_CHARGING_CURRENT = "EVSE.SetACChargingCurrent";
static const std::string METHOD_EVSE_SET_AC_CHARGING_PHASE_COUNT = "EVSE.SetACChargingPhaseCount";
static const std::string METHOD_EVSE_SET_DC_CHARGING = "EVSE.SetDCCharging";
static const std::string METHOD_EVSE_SET_DC_CHARGING_POWER = "EVSE.SetDCChargingPower";
static const std::string METHOD_EVSE_ENABLE_CONNECTOR = "EVSE.EnableConnector";

/// This class includes all methods of the EVSE namespace.
/// It contains the data object and the methods to access it.
class Evse {
public:
    // Constructor and Destructor
    // Deleting the default constructor to ensure the class is always initialized with a DataStoreCharger object
    Evse() = delete;
    Evse(data::DataStoreCharger& dataobj, std::unique_ptr<request_interface::RequestHandlerInterface> req_handler) :
        m_dataobj(dataobj), m_request_handler_ptr(std::move(req_handler)) {
    }

    ~Evse() = default;

    // Methods
    RPCDataTypes::EVSEGetInfoResObj get_info(const int32_t evse_index);
    RPCDataTypes::EVSEGetStatusResObj get_status(const int32_t evse_index);
    RPCDataTypes::EVSEGetHardwareCapabilitiesResObj get_hardware_capabilities(const int32_t evse_index);
    RPCDataTypes::ErrorResObj set_charging_allowed(const int32_t evse_index, bool charging_allowed);
    RPCDataTypes::EVSEGetMeterDataResObj get_meter_data(const int32_t evse_index);
    RPCDataTypes::ErrorResObj set_ac_charging(const int32_t evse_index, bool charging_allowed, float max_current,
                                              std::optional<int> phase_count);
    RPCDataTypes::ErrorResObj set_ac_charging_current(const int32_t evse_index, float max_current);
    RPCDataTypes::ErrorResObj set_ac_charging_phase_count(const int32_t evse_index, int phase_count);
    RPCDataTypes::ErrorResObj set_dc_charging(const int32_t evse_index, bool charging_allowed, float max_power);
    RPCDataTypes::ErrorResObj set_dc_charging_power(const int32_t evse_index, float max_power);
    RPCDataTypes::ErrorResObj enable_connector(const int32_t evse_index, int connector_id, bool enable, int priority);

private:
    // Reference to the DataStoreCharger object that holds and manages EVSE-related data.
    // This object is used to retrieve and update information about EVSEs, such as their status,
    // hardware capabilities, and meter data, ensuring consistent access to the underlying data store.
    data::DataStoreCharger& m_dataobj;
    // Reference to the RequestHandlerInterface object for handling requests
    std::unique_ptr<request_interface::RequestHandlerInterface> m_request_handler_ptr;
};

} // namespace methods

#endif // METHODS_EVSE_HPP
