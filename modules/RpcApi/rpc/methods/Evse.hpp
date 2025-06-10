// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef METHODS_EVSE_HPP
#define METHODS_EVSE_HPP

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
    m_dataobj(dataobj), m_request_handler_ptr(std::move(req_handler)) {}

    ~Evse() = default;

    // Methods
    RPCDataTypes::EVSEGetInfoResObj get_info(const int32_t evse_index) {
        RPCDataTypes::EVSEGetInfoResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
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

    RPCDataTypes::EVSEGetStatusResObj get_status(const int32_t evse_index) {
        RPCDataTypes::EVSEGetStatusResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
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

    RPCDataTypes::EVSEGetHardwareCapabilitiesResObj get_hardware_capabilities(const int32_t evse_index) {
        RPCDataTypes::EVSEGetHardwareCapabilitiesResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }

        if (!evse->hardwarecapabilities.get_data().has_value()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }

        res.hardware_capabilities = evse->hardwarecapabilities.get_data().value();
        return res;
    };

    RPCDataTypes::ErrorResObj set_charging_allowed(const int32_t evse_index, bool charging_allowed) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_charging_allowed(evse_index, charging_allowed);
    };

    RPCDataTypes::EVSEGetMeterDataResObj get_meter_data(const int32_t evse_index) {
        RPCDataTypes::EVSEGetMeterDataResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }

        if (!evse->meterdata.get_data().has_value()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable;
            return res;
        }
        res.meter_data = evse->meterdata.get_data().value();
        return res;
    };

    RPCDataTypes::ErrorResObj set_ac_charging(const int32_t evse_index, bool charging_allowed, float max_current, std::optional<int> phase_count) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_ac_charging(evse_index, charging_allowed, max_current, phase_count);
    };

    RPCDataTypes::ErrorResObj set_ac_charging_current(const int32_t evse_index, float max_current) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_ac_charging_current(evse_index, max_current);
    };

    RPCDataTypes::ErrorResObj set_ac_charging_phase_count(const int32_t evse_index, int phase_count) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_ac_charging_phase_count(evse_index, phase_count);

    };

    RPCDataTypes::ErrorResObj set_dc_charging(const int32_t evse_index, bool charging_allowed, float max_power) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_dc_charging(evse_index, charging_allowed, max_power);
    };

    RPCDataTypes::ErrorResObj set_dc_charging_power(const int32_t evse_index, float max_power) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        return m_request_handler_ptr->set_dc_charging_power(evse_index, max_power);
    };

    RPCDataTypes::ErrorResObj enable_connector(const int32_t evse_index, int connector_id, bool enable, int priority) {
        RPCDataTypes::ErrorResObj res {};

        auto evse = data::DataStoreCharger::get_evse_store(m_dataobj, evse_index);
        if (!evse) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
            return res;
        }
        // Iterate through the connectors to find the one with the given ID
        auto connectors = evse->evseinfo.get_available_connectors();
        auto it = std::find_if(connectors.begin(), connectors.end(),
            [connector_id](const auto& connector) { return connector.id == connector_id; });
        // If not found, return an error
        if (it == connectors.end()) {
            res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidConnectorID;
            return res;
        }
        return m_request_handler_ptr->enable_connector(evse_index, connector_id, enable, priority);
    };

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
