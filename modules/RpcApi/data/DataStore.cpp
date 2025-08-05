// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "DataStore.hpp"
#include "GenericInfoStore.hpp"
#include "SessionInfo.hpp"
#include <everest/logging.hpp>

namespace data {

// we currently don't get this info from the system yet, so allow setting to unknown
void ChargerInfoStore::set_unknown() {
    this->dataobj.vendor = "unknown";
    this->dataobj.model = "unknown";
    this->dataobj.serial = "unknown";
    this->dataobj.firmware_version = "unknown";
    // pretend we got something
    this->data_is_valid = true;
}

void ChargerErrorsStore::add_error(const types::json_rpc_api::ErrorObj& error) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // Check if the error already exists in the vector
    for (const auto& existing_error : this->dataobj) {
        if (existing_error.uuid == error.uuid) {
            // Error already exists, no need to add it again
            throw std::runtime_error("Error with UUID " + error.uuid + " already exists in the store.");
        }
    }
    this->dataobj.push_back(error);
    this->data_is_valid = true; // set the data as valid, since we have a valid error now
    data_lock.unlock();
    // Notify that data has changed
    this->notify_data_changed();
}

void ChargerErrorsStore::clear_error(const types::json_rpc_api::ErrorObj& error) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // Find and remove the error from the vector
    for (auto it = this->dataobj.begin(); it != this->dataobj.end(); ++it) {
        // String comparison for uuid
        if (it->uuid == error.uuid) {
            this->dataobj.erase(it);
            data_lock.unlock();
            // Notify that data has changed
            this->notify_data_changed();
            return; // Exit after removing the first matching error
        }
    }
}

void EVSEInfoStore::set_bidi_charging(bool enabled) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    this->dataobj.bidi_charging = enabled;
}
void EVSEInfoStore::set_index(int32_t index) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    this->dataobj.index = index;
}
void EVSEInfoStore::set_id(const std::string& id) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    this->dataobj.id = id;
    this->data_is_valid = true; // set the data as valid, since we have a valid id now
}
void EVSEInfoStore::set_available_connectors(const std::vector<RPCDataTypes::ConnectorInfoObj>& connectors) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    this->dataobj.available_connectors = connectors;
}

void EVSEInfoStore::set_available_connector(types::json_rpc_api::ConnectorInfoObj& available_connector) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // Interate through the vector and set the connector with the given id
    for (auto& connector : this->dataobj.available_connectors) {
        if (connector.id == available_connector.id) {
            connector = available_connector; // Update the existing connector
            return;
        }
    }
    // If the connector with the given id is not found, add it to the vector
    this->dataobj.available_connectors.push_back(available_connector);
}

int32_t EVSEInfoStore::get_index() {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    return this->dataobj.index;
}
std::vector<types::json_rpc_api::ConnectorInfoObj> EVSEInfoStore::get_available_connectors() {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    return this->dataobj.available_connectors;
}

void EVSEStatusStore::update_data_is_valid() {
    if (this->data_is_valid) {
        return; // No need to update if data is already valid
    }

    // Check if all fields are set
    for (const auto& [field, is_set] : field_status) {
        if (!is_set) {
            this->data_is_valid = false;
            EVLOG_debug << "EVSEStatusStore: Field " << static_cast<int>(field) << " is not set, data is invalid.";
            return;
        }
    }
    this->data_is_valid = true;
}

EVSEStatusStore::EVSEStatusStore() {
    // Initialize field_status map with all fields set to false
    field_status = {
        {EVSEStatusField::ActiveConnectorId, false},
        {EVSEStatusField::ChargingAllowed, false},
        {EVSEStatusField::State, false},
        {EVSEStatusField::ErrorPresent, false},
        {EVSEStatusField::ChargeProtocol, false},
        {EVSEStatusField::ChargingDurationS, false},
        {EVSEStatusField::ChargedEnergyWh, false},
        {EVSEStatusField::DischargedEnergyWh, false},
        {EVSEStatusField::Available, false},
    };

    // Initialize data store with default values
    set_charging_duration_s(0);
    set_charged_energy_wh(0.0f);
    set_discharged_energy_wh(0.0f);
    set_error_present(false);
    set_charging_allowed(true);
}

// Example set method using the enum
void EVSEStatusStore::set_active_connector_id(int32_t active_connector_id) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex); // check if data has changed
    field_status[EVSEStatusField::ActiveConnectorId] = true;  // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.active_connector_id != active_connector_id) {
        this->dataobj.active_connector_id = active_connector_id;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the charging allowed flag
void EVSEStatusStore::set_charging_allowed(bool charging_allowed) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::ChargingAllowed] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.charging_allowed != charging_allowed) {
        this->dataobj.charging_allowed = charging_allowed;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the EVSE state
void EVSEStatusStore::set_state(types::json_rpc_api::EVSEStateEnum state) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::State] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.state != state) {
        this->dataobj.state = state;
        data_lock.unlock();
        this->notify_data_changed();
    }
}

// set EVSE errors
void EVSEStatusStore::set_error_present(const bool error_present) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::ErrorPresent] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.error_present != error_present) {
        this->dataobj.error_present = error_present;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the charge protocol
void EVSEStatusStore::set_charge_protocol(types::json_rpc_api::ChargeProtocolEnum charge_protocol) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::ChargeProtocol] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.charge_protocol != charge_protocol) {
        this->dataobj.charge_protocol = charge_protocol;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the charging duration in seconds
void EVSEStatusStore::set_charging_duration_s(int32_t charging_duration_s) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::ChargingDurationS] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.charging_duration_s != charging_duration_s) {
        this->dataobj.charging_duration_s = charging_duration_s;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the charged energy in Wh
void EVSEStatusStore::set_charged_energy_wh(float charged_energy_wh) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::ChargedEnergyWh] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.charged_energy_wh != charged_energy_wh) {
        this->dataobj.charged_energy_wh = charged_energy_wh;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the discharged energy in Wh
void EVSEStatusStore::set_discharged_energy_wh(float discharged_energy_wh) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::DischargedEnergyWh] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.discharged_energy_wh != discharged_energy_wh) {
        this->dataobj.discharged_energy_wh = discharged_energy_wh;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the available flag
void EVSEStatusStore::set_available(bool available) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    field_status[EVSEStatusField::Available] = true; // Mark field as set
    update_data_is_valid();
    // check if data has changed
    if (this->dataobj.available != available) {
        this->dataobj.available = available;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the AC charge parameters
void EVSEStatusStore::set_ac_charge_param(const std::optional<RPCDataTypes::ACChargeParametersObj>& ac_charge_param) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // check if data has changed
    if (this->dataobj.ac_charge_param != ac_charge_param) {
        this->dataobj.ac_charge_param = ac_charge_param;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the DC charge parameters
void EVSEStatusStore::set_dc_charge_param(const std::optional<RPCDataTypes::DCChargeParametersObj>& dc_charge_param) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // check if data has changed
    if (this->dataobj.dc_charge_param != dc_charge_param) {
        this->dataobj.dc_charge_param = dc_charge_param;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the AC charge status
void EVSEStatusStore::set_ac_charge_status(const std::optional<RPCDataTypes::ACChargeStatusObj>& ac_charge_status) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // check if data has changed
    if (this->dataobj.ac_charge_status != ac_charge_status) {
        this->dataobj.ac_charge_status = ac_charge_status;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the DC charge status
void EVSEStatusStore::set_dc_charge_status(const std::optional<RPCDataTypes::DCChargeStatusObj>& dc_charge_status) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // check if data has changed
    if (this->dataobj.dc_charge_status != dc_charge_status) {
        this->dataobj.dc_charge_status = dc_charge_status;
        data_lock.unlock();
        this->notify_data_changed();
    }
}
// set the display parameters
void EVSEStatusStore::set_display_parameters(
    const std::optional<RPCDataTypes::DisplayParametersObj>& display_parameters) {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    // check if data has changed
    if (this->dataobj.display_parameters != display_parameters) {
        this->dataobj.display_parameters = display_parameters;
        data_lock.unlock();
        this->notify_data_changed();
    }
}

types::json_rpc_api::EVSEStateEnum EVSEStatusStore::get_state() const {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    return this->dataobj.state;
}

std::optional<RPCDataTypes::ACChargeParametersObj>& EVSEStatusStore::get_ac_charge_param() {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    return this->dataobj.ac_charge_param;
}

std::optional<RPCDataTypes::DCChargeParametersObj>& EVSEStatusStore::get_dc_charge_param() {
    std::unique_lock<std::mutex> data_lock(this->data_mutex);
    return this->dataobj.dc_charge_param;
}

} // namespace data
