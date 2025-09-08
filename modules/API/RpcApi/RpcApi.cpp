// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"
#include "helpers/Conversions.hpp"
#include "helpers/ErrorHandler.hpp"

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);

    for (const auto& evse_manager : r_evse_manager) {
        // create one DataStore object per EVSE
        auto& evse_data = this->data.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
        // subscribe to evse_manager interface variables
        this->subscribe_evse_manager(evse_manager, *evse_data);
    }

    if (r_evse_energy_sink.empty()) {
        EVLOG_warning << "No EVSE energy sinks configured. Configuration of EVSE external limits will not be possible.";
    }

    // Check if all evse_energy_sink(s) have a mapping to an EVSE
    check_evse_mapping();

    // Subscribe to all EVSE energy interfaces
    for (std::size_t idx = 0; idx < r_energy_listener.size(); idx++) {
        const auto& evse_energy = r_energy_listener.at(idx);
        if (evse_energy->get_mapping().has_value()) {
            // Write EVSE index and connector id to the datastore
            const auto evse_index = evse_energy->get_mapping().value().evse;
            auto evse_store = this->data.get_evse_store(evse_index);
            if (evse_store == nullptr) {
                EVLOG_error << "EVSE index " << evse_index
                            << " not found in data store, cannot subscribe to energy interface";
                continue;
            }
            this->subscribe_evse_energy(evse_energy, *evse_store);
        } else {
            std::size_t evse_index = idx;
            EVLOG_warning << "Energy interface with module_id \"" << evse_energy->module_id
                          << "\" does not have an EVSE mapping, guessing the EVSE index: " << idx;
            if (this->data.evses.size() < r_energy_listener.size()) {
                EVLOG_warning << "Number of EVSEs " << this->data.evses.size()
                              << " does not match with number of energy interfaces " << r_energy_listener.size()
                              << ", using first EVSE index 0";
                evse_index = 0;
            }
            this->subscribe_evse_energy(evse_energy, *this->data.evses.at(evse_index));
        }
    }

    // Create the request handler
    m_request_handler = std::make_unique<RpcApiRequestHandler>(data, r_evse_manager, r_evse_energy_sink);

    std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;

    if (config.websocket_enabled) {
        m_websocket_server = std::make_unique<server::WebSocketServer>(
            config.websocket_tls_enabled, config.websocket_port, config.websocket_interface);
        transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
    }

    if (transport_interfaces.empty()) {
        throw std::runtime_error("No other transports currently available, please enable websocket transport");
    }

    m_rpc_handler = std::make_unique<rpc::RpcHandler>(std::move(transport_interfaces), data,
                                                      std::move(m_request_handler), config.max_decimal_places_other);

    subscribe_global_errors();
}

void RpcApi::ready() {
    // get charger information (cmd not available during init())
    if (r_charger_information.size() > 0) {
        types::json_rpc_api::ChargerInfoObj charger_info;
        auto info = r_charger_information[0]->call_get_charger_information();
        // mandatory members
        charger_info.vendor = info.vendor;
        charger_info.model = info.model;
        charger_info.serial = info.chargepoint_serial.value_or("unknown");
        charger_info.firmware_version = info.firmware_version.value_or("unknown");
        // optional members
        if (info.friendly_name.has_value()) {
            charger_info.friendly_name = info.friendly_name.value();
        }
        if (info.manufacturer.has_value()) {
            charger_info.manufacturer = info.manufacturer.value();
        }
        if (info.manufacturer_url.has_value()) {
            charger_info.manufacturer_url = info.manufacturer_url.value();
        }
        if (info.model_number.has_value()) {
            charger_info.model_no = info.model_number.value();
        }
        if (info.model_revision.has_value()) {
            charger_info.revision = info.model_revision.value();
        }
        if (info.board_revision.has_value()) {
            charger_info.board_revision = info.board_revision.value();
        }
        this->data.chargerinfo.set_data(charger_info);
    } else {
        this->data.chargerinfo.set_unknown();
    }

    // Start server instances
    m_rpc_handler->start_server();

    invoke_ready(*p_main);
}

void RpcApi::check_evse_session_event(data::DataStoreEvse& evse_data,
                                      const types::evse_manager::SessionEvent& session_event) {
    // store the session info in the data store
    types::json_rpc_api::EVSEStateEnum evse_state =
        types::json_rpc_api::evse_manager_session_event_to_evse_state(session_event);
    evse_data.evsestatus.set_state(evse_state);
    evse_data.sessioninfo.update_state(session_event);

    if (evse_state == types::json_rpc_api::EVSEStateEnum::Charging) {
        evse_data.evsestatus.set_charging_allowed(true);
    }

    if (session_event.source.has_value()) {
        const auto source = session_event.source.value();
        evse_data.sessioninfo.set_enable_disable_source(
            types::evse_manager::enable_source_to_string(source.enable_source),
            types::evse_manager::enable_state_to_string(source.enable_state), source.enable_priority);
        if (source.enable_state == types::evse_manager::Enable_state::Disable) {
            evse_data.evsestatus.set_available(false);
        } else if (source.enable_state == types::evse_manager::Enable_state::Enable) {
            evse_data.evsestatus.set_available(true);
        }
    }

    if (session_event.event == types::evse_manager::SessionEventEnum::TransactionStarted) {
        if (session_event.transaction_started.has_value()) {
            auto transaction_started = session_event.transaction_started.value();
            auto energy_Wh_import = transaction_started.meter_value.energy_Wh_import.total;
            evse_data.sessioninfo.set_start_energy_import_wh(energy_Wh_import);

            if (transaction_started.meter_value.energy_Wh_export.has_value()) {
                auto energy_Wh_export = transaction_started.meter_value.energy_Wh_export.value().total;
                evse_data.sessioninfo.set_start_energy_export_wh(energy_Wh_export);
            } else {
                evse_data.sessioninfo.start_energy_export_wh_was_set = false;
            }
        }
        evse_data.evsestatus.set_charging_allowed(true);
    } else if (session_event.event == types::evse_manager::SessionEventEnum::TransactionFinished) {
        if (session_event.transaction_finished.has_value()) {
            auto transaction_finished = session_event.transaction_finished.value();
            auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
            evse_data.sessioninfo.set_end_energy_import_wh(energy_Wh_import);

            if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
                auto energy_Wh_export = transaction_finished.meter_value.energy_Wh_export.value().total;
                evse_data.sessioninfo.set_end_energy_export_wh(energy_Wh_export);
            } else {
                evse_data.sessioninfo.end_energy_export_wh_was_set = false;
            }
        }
        evse_data.evsestatus.set_charged_energy_wh(evse_data.sessioninfo.get_charged_energy_wh());
        evse_data.evsestatus.set_discharged_energy_wh(evse_data.sessioninfo.get_discharged_energy_wh());
        evse_data.evsestatus.set_charging_duration_s(evse_data.sessioninfo.get_charging_duration_s().count());
    }
}

void RpcApi::subscribe_evse_manager(const std::unique_ptr<evse_managerIntf>& evse_manager,
                                    data::DataStoreEvse& evse_data) {
    evse_manager->subscribe_powermeter([this, &evse_data](const types::powermeter::Powermeter& powermeter) {
        this->meterdata_var_to_datastore(powermeter, evse_data.meterdata);

        evse_data.sessioninfo.set_latest_energy_import_wh(powermeter.energy_Wh_import.total);
        if (powermeter.energy_Wh_export.has_value()) {
            evse_data.sessioninfo.set_latest_energy_export_wh(powermeter.energy_Wh_export.value().total);
        }
        if (powermeter.power_W.has_value()) {
            evse_data.sessioninfo.set_latest_total_w(powermeter.power_W.value().total);
        }
        // update duration and energy values in the EVSE status store
        evse_data.evsestatus.set_charging_duration_s(evse_data.sessioninfo.get_charging_duration_s().count());
        evse_data.evsestatus.set_charged_energy_wh(evse_data.sessioninfo.get_charged_energy_wh());
        evse_data.evsestatus.set_discharged_energy_wh(evse_data.sessioninfo.get_discharged_energy_wh());
    });
    evse_manager->subscribe_hw_capabilities(
        [this, &evse_data](const types::evse_board_support::HardwareCapabilities& hwcaps) {
            // there is only one connector supported currently
            this->hwcaps_var_to_datastore(hwcaps, evse_data.hardwarecapabilities);
        });
    evse_manager->subscribe_evse_id([this, &evse_data](const std::string& evse_id) {
        // set the EVSE id in the data store
        evse_data.evseinfo.set_id(evse_id);
    });

    evse_manager->subscribe_session_event([this, &evse_data](types::evse_manager::SessionEvent session_event) {
        check_evse_session_event(evse_data, session_event);
    });
    evse_manager->subscribe_selected_protocol([this, &evse_data](const std::string& selected_protocol) {
        auto var_selected_protocol = types::json_rpc_api::evse_manager_protocol_to_charge_protocol(selected_protocol);
        evse_data.evsestatus.set_charge_protocol(var_selected_protocol);
    });

    evse_manager->subscribe_limits([this, &evse_data](const types::evse_manager::Limits& limits) {
        // set the external limits in the data store
        if (evse_data.evseinfo.get_is_ac_transfer_mode()) {
            auto tmp = evse_data.evsestatus.get_ac_charge_param();
            if (!tmp.has_value()) {
                tmp.emplace();
            }
            tmp->evse_max_current = limits.max_current;
            tmp->evse_max_phase_count = limits.nr_of_phases_available;
            evse_data.evsestatus.set_ac_charge_param(tmp);
        } else {
            evse_data.evsestatus.set_ac_charge_param(std::nullopt);
        }
    });

    evse_manager->subscribe_supported_energy_transfer_modes(
        [this, &evse_data](const std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes) {
            // convert to rpc type
            bool is_ac_transfer_mode = false;
            auto rpc_supported_energy_transfer_modes =
                types::json_rpc_api::iso15118_energy_transfer_modes_to_json_rpc_api(supported_energy_transfer_modes,
                                                                                    is_ac_transfer_mode);
            evse_data.evseinfo.set_supported_energy_transfer_modes(rpc_supported_energy_transfer_modes);
            evse_data.evseinfo.set_is_ac_transfer_mode(is_ac_transfer_mode);
        });
}

void RpcApi::subscribe_evse_energy(const std::unique_ptr<energyIntf>& evse_energy, data::DataStoreEvse& evse_data) {
    evse_energy->subscribe_energy_flow_request([this, &evse_data](const types::energy::EnergyFlowRequest& request) {
        if (evse_data.evseinfo.get_is_ac_transfer_mode() &&
            request.schedule_import.at(0).limits_to_root.ac_number_of_active_phases.has_value() &&
            request.schedule_import.at(0).limits_to_root.ac_number_of_active_phases.value() != 0) {
            RPCDataTypes::ACChargeStatusObj ac_charge_status;
            ac_charge_status.evse_active_phase_count =
                request.schedule_import.at(0).limits_to_root.ac_number_of_active_phases.value();
            evse_data.evsestatus.set_ac_charge_status(ac_charge_status);
        } else {
            evse_data.evsestatus.set_ac_charge_status(std::nullopt);
        }
    });
}

void RpcApi::subscribe_global_errors() {
    // Subscribe to global error events
    const auto error_handler = [this](const Everest::error::Error& error) {
        auto tmp_error = types::json_rpc_api::everest_error_to_rpc_error(error);
        helpers::handle_error_raised(this->data, tmp_error);
    };
    const auto error_cleared_handler = [this](const Everest::error::Error& error) {
        auto tmp_error = types::json_rpc_api::everest_error_to_rpc_error(error);
        helpers::handle_error_cleared(this->data, tmp_error);
    };
    subscribe_global_all_errors(error_handler, error_cleared_handler);
}

void RpcApi::meterdata_var_to_datastore(const types::powermeter::Powermeter& powermeter,
                                        data::MeterDataStore& meter_data) {
    types::json_rpc_api::MeterDataObj meter_data_new; // default initialized
    if (meter_data.get_data().has_value()) {
        // initialize with existing values
        meter_data_new = meter_data.get_data().value();
    }

    // mandatory objects from the EVerest powermeter interface variable
    // timestamp
    meter_data_new.timestamp = powermeter.timestamp;

    // energy_Wh_import
    if (powermeter.energy_Wh_import.L1.has_value()) {
        meter_data_new.energy_Wh_import.L1 = powermeter.energy_Wh_import.L1.value();
    }
    if (powermeter.energy_Wh_import.L2.has_value()) {
        meter_data_new.energy_Wh_import.L2 = powermeter.energy_Wh_import.L2.value();
    }
    if (powermeter.energy_Wh_import.L3.has_value()) {
        meter_data_new.energy_Wh_import.L3 = powermeter.energy_Wh_import.L3.value();
    }
    meter_data_new.energy_Wh_import.total = powermeter.energy_Wh_import.total;

    // optional objects from the EVerest powermeter interface
    if (powermeter.current_A.has_value()) {
        meter_data_new.current_A.emplace();
        const auto& inobj = powermeter.current_A.value();
        if (inobj.L1.has_value()) {
            meter_data_new.current_A.value().L1 = inobj.L1.value();
        }
    }
    if (powermeter.energy_Wh_export.has_value()) {
        // a shortcut reference to the input data sub-object
        const auto& inobj = powermeter.energy_Wh_export.value();
        // a shortcut reference to the output data sub-object optional
        auto& export_opt = meter_data_new.energy_Wh_export;
        // keep original (copied) optional value, or emplace empty if non exist
        auto& newobj = export_opt.emplace(export_opt.value_or(types::json_rpc_api::Energy_Wh_export{}));
        if (inobj.L1.has_value()) {
            newobj.L1 = inobj.L1.value();
        }
        if (inobj.L2.has_value()) {
            newobj.L2 = inobj.L2.value();
        }
        if (inobj.L3.has_value()) {
            newobj.L3 = inobj.L3.value();
        }
        newobj.total = inobj.total;
    }
    if (powermeter.frequency_Hz.has_value()) {
        // a shortcut reference to the input data sub-object
        const auto& inobj = powermeter.frequency_Hz.value();
        // a shortcut reference to the output data sub-object optional
        auto& frequency_optional = meter_data_new.frequency_Hz;
        // keep original (copied) optional value, or emplace empty if non exist
        auto& newobj = frequency_optional.emplace(frequency_optional.value_or(types::json_rpc_api::Frequency_Hz{}));
        newobj.L1 = inobj.L1;
        if (inobj.L2.has_value()) {
            newobj.L2 = inobj.L2.value();
        }
        if (inobj.L3.has_value()) {
            newobj.L3 = inobj.L3.value();
        }
    }
    if (powermeter.meter_id.has_value()) {
        meter_data_new.meter_id = powermeter.meter_id.value();
    }
    // serial_number  is not yet available
    if (powermeter.phase_seq_error.has_value()) {
        meter_data_new.phase_seq_error = powermeter.phase_seq_error.value();
    }
    if (powermeter.power_W.has_value()) {
        // a shortcut reference to the input data sub-object
        const auto& inobj = powermeter.power_W.value();
        // a shortcut reference to the output data sub-object optional
        auto& export_opt = meter_data_new.power_W;
        // keep original (copied) optional value, or emplace empty if non exist
        auto& newobj = export_opt.emplace(export_opt.value_or(types::json_rpc_api::Power_W{}));
        if (inobj.L1.has_value()) {
            newobj.L1 = inobj.L1.value();
        }
        if (inobj.L2.has_value()) {
            newobj.L2 = inobj.L2.value();
        }
        if (inobj.L3.has_value()) {
            newobj.L3 = inobj.L3.value();
        }
        newobj.total = inobj.total;
    }
    if (powermeter.voltage_V.has_value()) {
        // a shortcut reference to the input data sub-object
        const auto& inobj = powermeter.voltage_V.value();
        // a shortcut reference to the output data sub-object optional
        auto& export_opt = meter_data_new.voltage_V;
        // keep original (copied) optional value, or emplace empty if non exist
        auto& newobj = export_opt.emplace(export_opt.value_or(types::json_rpc_api::Voltage_V{}));
        if (inobj.L1.has_value()) {
            newobj.L1 = inobj.L1.value();
        }
        if (inobj.L2.has_value()) {
            newobj.L2 = inobj.L2.value();
        }
        if (inobj.L3.has_value()) {
            newobj.L3 = inobj.L3.value();
        }
    }

    // submit changes
    // Note: timestamp will skew this, as it will always change, and therefore always trigger a notification for the
    // complete dataset
    meter_data.set_data(meter_data_new);
}

void RpcApi::hwcaps_var_to_datastore(const types::evse_board_support::HardwareCapabilities& hwcaps,
                                     data::HardwareCapabilitiesStore& hw_caps_data) {
    types::json_rpc_api::HardwareCapabilitiesObj hw_caps_data_new; // default initialized
    if (hw_caps_data.get_data().has_value()) {
        // initialize with existing values
        hw_caps_data_new = hw_caps_data.get_data().value();
    }

    // mandatory objects from the EVerest hw_capabilites interface variable
    hw_caps_data_new.max_current_A_export = hwcaps.max_current_A_import;
    hw_caps_data_new.max_current_A_import = hwcaps.max_current_A_import;
    hw_caps_data_new.max_phase_count_export = hwcaps.max_phase_count_export;
    hw_caps_data_new.max_phase_count_import = hwcaps.max_phase_count_import;
    hw_caps_data_new.min_current_A_export = hwcaps.min_current_A_export;
    hw_caps_data_new.min_current_A_import = hwcaps.min_current_A_import;
    hw_caps_data_new.min_phase_count_export = hwcaps.min_phase_count_export;
    hw_caps_data_new.min_phase_count_import = hwcaps.min_phase_count_import;
    hw_caps_data_new.phase_switch_during_charging = hwcaps.supports_changing_phases_during_charging;

    // submit changes
    hw_caps_data.set_data(hw_caps_data_new);
}

bool RpcApi::check_evse_mapping() {
    // Interate over the configured EVSE mapping and configure the data store accordingly
    if (r_evse_manager.size() != this->data.evses.size()) {
        throw std::runtime_error("The number of EVSE managers does not match the number of EVSE data stores.");
    }
    // As long as the EvseManager only supports one statically configured connector, we extract the
    // connector id from the mapping. Only the connector type is retrieved from the EvseManager.
    // Iterate over all over the mapping of the EVSE's and configure the data store accordingly
    for (std::size_t idx = 0; idx < r_evse_manager.size(); idx++) {
        const auto& evse_manager = r_evse_manager[idx];
        const auto& evse_data = this->data.evses[idx];
        // Initialize connector index for the case of no mapping information
        types::json_rpc_api::ConnectorInfoObj connector;
        connector.index = 1;                                              // default connector id
        connector.type = types::json_rpc_api::ConnectorTypeEnum::Unknown; // default type
        evse_data->evseinfo.set_available_connector(connector);
        evse_data->evsestatus.set_active_connector_index(connector.index); // TODO: support multiple connectors
        // create one DataStore object per EVSE sink
        if (evse_manager->get_mapping().has_value()) {
            // Write EVSE index and connector index to the datastore
            evse_data->evseinfo.set_index(evse_manager->get_mapping().value().evse);
            if (evse_manager->get_mapping().value().connector.has_value()) {
                // Initialize connector index
                connector.index = evse_manager->get_mapping().value().connector.value();
                types::evse_manager::Evse evse = evse_manager->call_get_evse();
                if (!evse.connectors.empty() && evse.connectors[0].type.has_value()) {
                    try {
                        connector.type = types::json_rpc_api::string_to_connector_type_enum(
                            types::evse_manager::connector_type_enum_to_string(
                                evse.connectors[0]
                                    .type.value())); // evse.connectors[0].type; // use the first connector type
                    } catch (const std::out_of_range& e) {
                        EVLOG_debug << "Unknown connector type for connector index " << connector.index;
                    }
                } else {
                    EVLOG_debug << "No connector type determined for connector index " << connector.index;
                }
                evse_data->evseinfo.set_available_connector(connector);
                evse_data->evsestatus.set_active_connector_index(connector.index); // TODO: support multiple connectors
            } else {
                EVLOG_debug << "No connector index configured in the EVSE mapping, using default connector index "
                            << connector.index;
            }
        } else {
            // FIXME no mapping may be incorrect
            // begin with index 1, as 0 should be reserved for the complete charger
            const auto evse_index = idx + 1;
            EVLOG_warning << "No mapping found for EVSE manager with module_id \"" << evse_manager->module_id
                          << "\", assigning index " << evse_index << " and connector index " << connector.index;
            evse_data->evseinfo.set_index(evse_index);
        }
    }
    return true;
}
} // namespace module
