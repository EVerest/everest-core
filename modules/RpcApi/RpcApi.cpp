// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <utils/date.hpp>

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);

    for (const auto& evse_manager : r_evse_manager) {
        // create one DataStore object per EVSE
        auto& evse_data = this->data.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
        // create one connector per EVSE
        auto& connector = evse_data->connectors.emplace_back(std::make_unique<data::DataStoreConnector>());
        // initialize connector id, counting from 0
        connector->connectorinfo.set_id(0);

        // subscribe to evse_manager interface variables
        this->subscribe_evse_manager(evse_manager, *evse_data);

        // no info available until we get the new charger_information interface, see
        // https://github.com/EVerest/everest-core/pull/1109
        this->data.chargerinfo.set_unknown();
    }

    // Check if all evse_energy_sink(s) have a mapping to an EVSE
    check_evse_energy_sink_mapping();

    // Create the request handler
    m_request_handler = std::make_unique<RpcApiRequestHandler>(r_evse_manager, r_error_history, r_evse_energy_sink);

    m_websocket_server = std::make_unique<server::WebSocketServer>(config.websocket_tls_enabled, config.websocket_port);
    // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
    std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
    transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
    m_rpc_server = std::make_unique<rpc::RpcHandler>(std::move(transport_interfaces), data, std::move(m_request_handler));
}

void RpcApi::ready() {
    // Start server instances
    m_rpc_server->start_server();

    invoke_ready(*p_main);
}

void RpcApi::subscribe_evse_manager(const std::unique_ptr<evse_managerIntf>& evse_manager,
                                    data::DataStoreEvse& evse_data) {
    evse_manager->subscribe_powermeter([this, &evse_data](const types::powermeter::Powermeter& powermeter) {
        this->meter_interface_to_datastore(powermeter, evse_data.meterdata);
    });
    evse_manager->subscribe_hw_capabilities(
        [this, &evse_data](const types::evse_board_support::HardwareCapabilities& hwcaps) {
            // there is only one connector supported currently
            this->hwcaps_interface_to_datastore(hwcaps, evse_data.connectors[0]->hardwarecapabilities);
        });
    evse_manager->subscribe_evse_id(
        [this, &evse_data](const std::string& evse_id) {
            // set the EVSE id in the data store
            evse_data.evseinfo.set_id(evse_id);
        });
}

void RpcApi::meter_interface_to_datastore(const types::powermeter::Powermeter& powermeter,
                                          data::MeterDataStore& meter_data) {
    types::json_rpc_api::MeterDataObj meter_data_new; // default initialized
    if (meter_data.get_data().has_value()) {
        // initialize with existing values
        meter_data_new = meter_data.get_data().value();
    }

    // mandatory objects from the EVerest powermeter interface variable
    // timestamp
    const std::chrono::time_point<date::utc_clock> ts = Everest::Date::from_rfc3339(powermeter.timestamp);
    // const std::chrono::milliseconds ts_millis = std::chrono::duration_cast<std::chrono::milliseconds>(ts);
    const std::chrono::nanoseconds ts_nanos = ts.time_since_epoch();
    // FIXME this is only a hack, as it only accepts nanos
    meter_data_new.timestamp = ts_nanos.count() * 1000000000.f; // nanoseconds integer  precision back to float seconds
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

void RpcApi::hwcaps_interface_to_datastore(const types::evse_board_support::HardwareCapabilities& hwcaps,
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
    // FIXME: the interface variable's connector_type is covered elsewhere?

    // submit changes
    hw_caps_data.set_data(hw_caps_data_new);
}

bool RpcApi::check_evse_energy_sink_mapping() {
    // Interate over all configured EVSE sinks and configure the data store accordingly
    if (r_evse_energy_sink.empty()) {
        EVLOG_error << "No EVSE energy sinks configured. Unable to extract EVSE index and connector id for the "
                        "connected r_evse_energy_sink. Please check your configuration file.";
        return false;
    }
    else if (r_evse_energy_sink.size() != this->data.evses.size()) {
        EVLOG_error << "The number of configured r_evse_energy_sink does not match the number of configured EVSEs. "
                        "Please check your configuration file.";
        return false;
    }
    else {
        // Iterate over all configured EVSE sinks and configure the data store accordingly
        uint32_t idx = 0;
        for (const auto& evse_energy_sink : r_evse_energy_sink) {
            auto& evse_data = this->data.evses[idx++];
            // create one DataStore object per EVSE sink
            if (evse_energy_sink->get_mapping().has_value()) {
                // Write EVSE index and connector id to the datastore
                evse_data->evseinfo.set_index(evse_energy_sink->get_mapping().value().evse);
                if (evse_energy_sink->get_mapping().value().connector.has_value()) {
                    // Initialize connector id
                    auto& connector = evse_data->connectors[0];
                    connector->connectorinfo.set_id(evse_energy_sink->get_mapping().value().connector.value());
                }
            }
            else {
                EVLOG_error << "Please configure an evse mapping your configuration file for the connected "
                                  "r_evse_energy_sink with module_id: " << evse_energy_sink->module_id;
                return false;
            }
        }
    }
    return true;
}
} // namespace module
