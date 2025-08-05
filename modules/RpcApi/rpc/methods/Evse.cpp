// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "Evse.hpp"

namespace RPCDataTypes = types::json_rpc_api;

namespace methods {

RPCDataTypes::EVSEGetInfoResObj Evse::get_info(const int32_t evse_index) {
    RPCDataTypes::EVSEGetInfoResObj res {};

    auto evse = m_dataobj.get_evse_store(evse_index);
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
}

RPCDataTypes::EVSEGetStatusResObj Evse::get_status(const int32_t evse_index) {
    RPCDataTypes::EVSEGetStatusResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
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
}

RPCDataTypes::EVSEGetHardwareCapabilitiesResObj Evse::get_hardware_capabilities(const int32_t evse_index) {
    RPCDataTypes::EVSEGetHardwareCapabilitiesResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
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
}

RPCDataTypes::ErrorResObj Evse::set_charging_allowed(const int32_t evse_index, bool charging_allowed) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_charging_allowed(evse_index, charging_allowed);
}

RPCDataTypes::EVSEGetMeterDataResObj Evse::get_meter_data(const int32_t evse_index) {
    RPCDataTypes::EVSEGetMeterDataResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
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
}

RPCDataTypes::ErrorResObj Evse::set_ac_charging(const int32_t evse_index, bool charging_allowed, float max_current, std::optional<int> phase_count) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_ac_charging(evse_index, charging_allowed, max_current, phase_count);
}

RPCDataTypes::ErrorResObj Evse::set_ac_charging_current(const int32_t evse_index, float max_current) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_ac_charging_current(evse_index, max_current);
}

RPCDataTypes::ErrorResObj Evse::set_ac_charging_phase_count(const int32_t evse_index, int phase_count) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_ac_charging_phase_count(evse_index, phase_count);

}

RPCDataTypes::ErrorResObj Evse::set_dc_charging(const int32_t evse_index, bool charging_allowed, float max_power) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_dc_charging(evse_index, charging_allowed, max_power);
}

RPCDataTypes::ErrorResObj Evse::set_dc_charging_power(const int32_t evse_index, float max_power) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    return m_request_handler_ptr->set_dc_charging_power(evse_index, max_power);
}

RPCDataTypes::ErrorResObj Evse::enable_connector(const int32_t evse_index, int connector_id, bool enable, int priority) {
    RPCDataTypes::ErrorResObj res {};

    const auto evse = m_dataobj.get_evse_store(evse_index);
    if (!evse) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return res;
    }
    // Iterate through the connectors to find the one with the given ID
    const auto connectors = evse->evseinfo.get_available_connectors();
    auto it = std::find_if(connectors.begin(), connectors.end(),
        [connector_id](const auto& connector) { return connector.id == connector_id; });
    // If not found, return an error
    if (it == connectors.end()) {
        res.error = RPCDataTypes::ResponseErrorEnum::ErrorInvalidConnectorID;
        return res;
    }
    return m_request_handler_ptr->enable_connector(evse_index, connector_id, enable, priority);
}

} // namespace methods
