// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATASTORE_HPP
#define DATASTORE_HPP

#include "GenericInfoStore.hpp"
#include "SessionInfo.hpp"
#include <atomic>
#include <everest/logging.hpp>

namespace data {

enum class EVSEStatusField {
    ActiveConnectorIndex,
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
    void set_unknown();
};

class ChargerErrorsStore : public GenericInfoStore<std::vector<types::json_rpc_api::ErrorObj>> {
public:
    void add_error(const types::json_rpc_api::ErrorObj& error);
    void clear_error(const types::json_rpc_api::ErrorObj& error);
};

class EVSEInfoStore : public GenericInfoStore<RPCDataTypes::EVSEInfoObj> {
public:
    void set_supported_energy_transfer_modes(
        const std::vector<types::json_rpc_api::EnergyTransferModeEnum>& supported_energy_transfer_modes);
    void set_index(int32_t index);
    void set_id(const std::string& id);
    void set_available_connectors(const std::vector<RPCDataTypes::ConnectorInfoObj>& connectors);
    void set_available_connector(types::json_rpc_api::ConnectorInfoObj& available_connector);
    void set_is_ac_transfer_mode(bool is_ac);
    bool get_is_ac_transfer_mode() const;
    int32_t get_index();
    std::vector<types::json_rpc_api::ConnectorInfoObj> get_available_connectors();

private:
    std::atomic<bool> is_ac_transfer_mode;
};

class EVSEStatusStore : public GenericInfoStore<RPCDataTypes::EVSEStatusObj> {
private:
    std::map<EVSEStatusField, bool> field_status; // Tracks whether each field has been set

    void update_data_is_valid();

public:
    EVSEStatusStore();

    // Set the active connector index
    void set_active_connector_index(int32_t active_connector_index);
    // set the charging allowed flag
    void set_charging_allowed(bool charging_allowed);
    // set the EVSE state
    void set_state(types::json_rpc_api::EVSEStateEnum state);

    // set EVSE errors
    void set_error_present(const bool error_present);
    // set the charge protocol
    void set_charge_protocol(types::json_rpc_api::ChargeProtocolEnum charge_protocol);
    // set the charging duration in seconds
    void set_charging_duration_s(int32_t charging_duration_s);
    // set the charged energy in Wh
    void set_charged_energy_wh(float charged_energy_wh);
    // set the discharged energy in Wh
    void set_discharged_energy_wh(float discharged_energy_wh);
    // set the available flag
    void set_available(bool available);
    // set the AC charge parameters
    void set_ac_charge_param(const std::optional<RPCDataTypes::ACChargeParametersObj>& ac_charge_param);
    // set the DC charge parameters
    void set_dc_charge_param(const std::optional<RPCDataTypes::DCChargeParametersObj>& dc_charge_param);
    // set the AC charge status
    void set_ac_charge_status(const std::optional<RPCDataTypes::ACChargeStatusObj>& ac_charge_status);
    // set the DC charge status
    void set_dc_charge_status(const std::optional<RPCDataTypes::DCChargeStatusObj>& dc_charge_status);
    // set the display parameters
    void set_display_parameters(const std::optional<RPCDataTypes::DisplayParametersObj>& display_parameters);

    types::json_rpc_api::EVSEStateEnum get_state() const;
    std::optional<RPCDataTypes::ACChargeParametersObj>& get_ac_charge_param();
    std::optional<RPCDataTypes::DCChargeParametersObj>& get_dc_charge_param();
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
    data::DataStoreEvse* get_evse_store(const int32_t evse_index) {
        if (evses.empty()) {
            EVLOG_error << "No EVSEs found in the data store.";
            return nullptr;
        }

        for (const auto& evse : evses) {
            const auto tmp_index = evse->evseinfo.get_index();
            if (tmp_index == evse_index) {
                return evse.get();
            }
        }
        EVLOG_error << "EVSE index " << evse_index << " not found in data store.";
        return nullptr;
    }
};

} // namespace data

#endif // DATASTORE_HPP
