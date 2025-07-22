// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "GenericInfoStore.hpp"
#include "SessionInfo.hpp"
#include <everest/logging.hpp>

namespace data {

enum class EVSEStatusField {
    ActiveConnectorId,
    ChargingAllowed,
    State,
    ErrorPresent,
    ChargeProtocol,
    ChargingDurationS,
    ChargedEnergyWh,
    DischargedEnergyWh,
    Available,
    ACChargeParam,
    DCChargeParam,
    ACChargeLoop,
    DCChargeLoop,
    DisplayParameters
};

class ChargerInfoStore : public GenericInfoStore<RPCDataTypes::ChargerInfoObj> {
public:
    // we currently don't get this info from the system yet, so allow setting to unknown
    void set_unknown() {
        this->dataobj.vendor = "unknown";
        this->dataobj.model = "unknown";
        this->dataobj.serial = "unknown";
        this->dataobj.firmware_version = "unknown";
        // pretend we got something
        this->data_is_valid = true;
    }
};

class ChargerErrorsStore : public GenericInfoStore<std::vector<types::json_rpc_api::ErrorObj>> {
public:
    void add_error(const types::json_rpc_api::ErrorObj& error) {
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

    void clear_error(const types::json_rpc_api::ErrorObj& error) {
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
};

class EVSEInfoStore : public GenericInfoStore<RPCDataTypes::EVSEInfoObj> {
public:
    void set_bidi_charging(bool enabled) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.bidi_charging = enabled;
    }
    void set_index(int32_t index) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.index = index;
    }
    void set_id(const std::string& id) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.id = id;
        this->data_is_valid = true; // set the data as valid, since we have a valid id now
    }
    void set_available_connectors(const std::vector<RPCDataTypes::ConnectorInfoObj>& connectors) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        this->dataobj.available_connectors = connectors;
    }

    void set_available_connector(types::json_rpc_api::ConnectorInfoObj& available_connector) {
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

    int32_t get_index() {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        return this->dataobj.index;
    }
    std::vector<types::json_rpc_api::ConnectorInfoObj> get_available_connectors() {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        return this->dataobj.available_connectors;
    }
};

 class EVSEStatusStore : public GenericInfoStore<RPCDataTypes::EVSEStatusObj> {
private:
    std::map<EVSEStatusField, bool> field_status; // Tracks whether each field has been set

    void update_data_is_valid() {
        if (this->data_is_valid) {
            return; // No need to update if data is already valid
        }

        // Check if all fields are set
        for (const auto& [field, is_set] : field_status) {
            if (!is_set) {
                this->data_is_valid = false;
                return;
            }
        }
        this->data_is_valid = true;
    }

public:
    EVSEStatusStore() {
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
    }

    // Example set method using the enum
    void set_active_connector_id(int32_t active_connector_id) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);// check if data has changed
        field_status[EVSEStatusField::ActiveConnectorId] = true; // Mark field as set
        update_data_is_valid();
        // check if data has changed
        if (this->dataobj.active_connector_id != active_connector_id) {
            this->dataobj.active_connector_id = active_connector_id;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
    // set the charging allowed flag
    void set_charging_allowed(bool charging_allowed) {
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
    void set_state(types::json_rpc_api::EVSEStateEnum state) {
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
    void set_error_present(const bool error_present) {
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
    void set_charge_protocol(types::json_rpc_api::ChargeProtocolEnum charge_protocol) {
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
    void set_charging_duration_s(int32_t charging_duration_s) {
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
    void set_charged_energy_wh(float charged_energy_wh) {
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
    void set_discharged_energy_wh(float discharged_energy_wh) {
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
    void set_available(bool available) {
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
    void set_ac_charge_param(const std::optional<RPCDataTypes::ACChargeParametersObj>& ac_charge_param) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        // check if data has changed
        if (this->dataobj.ac_charge_param != ac_charge_param) {
            this->dataobj.ac_charge_param = ac_charge_param;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
    // set the DC charge parameters
    void set_dc_charge_param(const std::optional<RPCDataTypes::DCChargeParametersObj>& dc_charge_param) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        // check if data has changed
        if (this->dataobj.dc_charge_param != dc_charge_param) {
            this->dataobj.dc_charge_param = dc_charge_param;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
    // set the AC charge loop
    void set_ac_charge_loop(const std::optional<RPCDataTypes::ACChargeLoopObj>& ac_charge_loop) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        // check if data has changed
        if (this->dataobj.ac_charge_loop != ac_charge_loop) {
            this->dataobj.ac_charge_loop = ac_charge_loop;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
    // set the DC charge loop
    void set_dc_charge_loop(const std::optional<RPCDataTypes::DCChargeLoopObj>& dc_charge_loop) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        // check if data has changed
        if (this->dataobj.dc_charge_loop != dc_charge_loop) {
            this->dataobj.dc_charge_loop = dc_charge_loop;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
    // set the display parameters
    void set_display_parameters(const std::optional<RPCDataTypes::DisplayParametersObj>& display_parameters) {
        std::unique_lock<std::mutex> data_lock(this->data_mutex);
        // check if data has changed
        if (this->dataobj.display_parameters != display_parameters) {
            this->dataobj.display_parameters = display_parameters;
            data_lock.unlock();
            this->notify_data_changed();
        }
    }
};
class HardwareCapabilitiesStore : public GenericInfoStore<RPCDataTypes::HardwareCapabilitiesObj> {};
class MeterDataStore : public GenericInfoStore<RPCDataTypes::MeterDataObj> {};

// This is the data store for a single EVSE. An EVSE can have multiple connectors.
struct DataStoreEvse {
    EVSEInfoStore evseinfo;
    EVSEStatusStore evsestatus;
    MeterDataStore meterdata;
    HardwareCapabilitiesStore hardwarecapabilities;
    SessionInfoStore sessioninfo;
};

// This is the main data store for the charger. A charger can have multiple EVSEs, each with multiple connectors.
// For more information see 3-Tier model definition of OCPP 2.0.
struct DataStoreCharger {
    ChargerInfoStore chargerinfo;
    ChargerErrorsStore chargererrors;
    std::string everest_version;
    std::vector<std::unique_ptr<DataStoreEvse>> evses;

    // get the EVSE data with a specific id
    static data::DataStoreEvse* get_evse_store(DataStoreCharger& dataobj, const int32_t evse_index) {
        if (dataobj.evses.empty()) {
            return nullptr;
        }

        for (const auto& evse : dataobj.evses) {
            if (not evse->evseinfo.get_data().has_value()) {
                continue;
            }

            const auto data = evse->evseinfo.get_data().value();
            if (data.index == evse_index) {
                return evse.get();
            }
        }
        return nullptr;
    };
};

} // namespace data

#endif // DATASTORE_HPP