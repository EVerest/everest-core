// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include "IECStateMachine.hpp"
#include "SessionLog.hpp"
#include "Timeout.hpp"
#include "scoped_lock_timeout.hpp"
using namespace std::literals::chrono_literals;

namespace module {

inline static void trim_colons_from_string(std::string& text) {
    text.erase(remove(text.begin(), text.end(), ':'), text.end());
}

inline static types::authorization::ProvidedIdToken create_autocharge_token(std::string token, int connector_id) {
    types::authorization::ProvidedIdToken autocharge_token;
    autocharge_token.authorization_type = types::authorization::AuthorizationType::Autocharge;
    autocharge_token.id_token_type = types::authorization::IdTokenType::MacAddress;
    trim_colons_from_string(token);
    autocharge_token.id_token = "VID:" + token;
    autocharge_token.connectors.emplace(connector_id, 1);
    return autocharge_token;
}

void EvseManager::init() {
    local_three_phases = config.three_phases;

    random_delay_enabled = config.uk_smartcharging_random_delay_enable;
    random_delay_max_duration = std::chrono::seconds(config.uk_smartcharging_random_delay_max_duration);
    if (random_delay_enabled) {
        EVLOG_info << "UK Smart Charging regulations: enabled random delay with a default of "
                   << random_delay_max_duration.load().count() << "s.";
    }

    session_log.setPath(config.session_logging_path);
    session_log.setMqtt([this](json data) {
        std::string hlc_log_topic = "everest_api/" + this->info.id + "/var/hlc_log";
        mqtt.publish(hlc_log_topic, data.dump());
    });
    if (config.session_logging) {
        session_log.enable();
    }
    session_log.xmlOutput(config.session_logging_xml);

    invoke_init(*p_evse);
    invoke_init(*p_energy_grid);
    invoke_init(*p_token_provider);
    invoke_init(*p_random_delay);

    // check if a slac module is connected to the optional requirement
    slac_enabled = not r_slac.empty();

    // if hlc is disabled in config, disable slac even if requirement is connected
    if (not(config.ac_hlc_enabled or config.ac_with_soc or config.charge_mode == "DC")) {
        slac_enabled = false;
    }

    // Use SLAC MAC address for Autocharge if configured.
    if (config.autocharge_use_slac_instead_of_hlc and slac_enabled) {
        r_slac[0]->subscribe_ev_mac_address([this](const std::string& token) {
            p_token_provider->publish_provided_token(create_autocharge_token(token, config.connector_id));
        });
    }

    hlc_enabled = not r_hlc.empty();
    if (not slac_enabled)
        hlc_enabled = false;

    if (config.charge_mode == "DC" and (not hlc_enabled or not slac_enabled or r_powersupply_DC.empty())) {
        EVLOG_error << "DC mode requires slac, HLC and powersupply DCDC to be connected";
        exit(255);
    }

    if (config.charge_mode == "DC" and r_imd.empty()) {
        EVLOG_warning << "DC mode without isolation monitoring configured, please check your national regulations.";
    }

    reserved = false;
    reservation_id = 0;

    hlc_waiting_for_auth_eim = false;
    hlc_waiting_for_auth_pnc = false;

    latest_target_voltage = 0;
    latest_target_current = 0;
}

void EvseManager::ready() {
    bsp = std::unique_ptr<IECStateMachine>(new IECStateMachine(r_bsp));
    error_handling =
        std::unique_ptr<ErrorHandling>(new ErrorHandling(r_bsp, r_hlc, r_connector_lock, r_ac_rcd, p_evse));

    hw_capabilities = r_bsp->call_get_hw_capabilities();

    charger = std::unique_ptr<Charger>(new Charger(bsp, error_handling, hw_capabilities.connector_type));

    if (r_connector_lock.size() > 0) {
        bsp->signal_lock.connect([this]() { r_connector_lock[0]->call_lock(); });
        bsp->signal_unlock.connect([this]() { r_connector_lock[0]->call_unlock(); });
    }

    if (get_hlc_enabled()) {

        // Set up EVSE ID
        types::iso15118_charger::EVSEID evseid = {config.evse_id, config.evse_id_din};

        // Set up auth options for HLC
        std::vector<types::iso15118_charger::PaymentOption> payment_options;

        if (config.payment_enable_eim) {
            payment_options.push_back(types::iso15118_charger::PaymentOption::ExternalPayment);
        }
        if (config.payment_enable_contract) {
            payment_options.push_back(types::iso15118_charger::PaymentOption::Contract);
        }
        r_hlc[0]->call_session_setup(payment_options, config.payment_enable_contract);

        r_hlc[0]->subscribe_dlink_error([this] {
            session_log.evse(true, "D-LINK_ERROR.req");
            // Inform charger
            charger->dlink_error();
            // Inform SLAC layer, it will leave the logical network
            r_slac[0]->call_dlink_error();
        });

        r_hlc[0]->subscribe_dlink_pause([this] {
            // tell charger (it will disable PWM)
            session_log.evse(true, "D-LINK_PAUSE.req");
            charger->dlink_pause();
            r_slac[0]->call_dlink_pause();
        });

        r_hlc[0]->subscribe_dlink_terminate([this] {
            session_log.evse(true, "D-LINK_TERMINATE.req");
            charger->dlink_terminate();
            r_slac[0]->call_dlink_terminate();
        });

        r_hlc[0]->subscribe_V2G_Setup_Finished([this] { charger->set_hlc_charging_active(); });

        r_hlc[0]->subscribe_AC_Close_Contactor([this] {
            session_log.car(true, "AC HLC Close contactor");
            charger->set_hlc_allow_close_contactor(true);
        });

        r_hlc[0]->subscribe_AC_Open_Contactor([this] {
            session_log.car(true, "AC HLC Open contactor");
            charger->set_hlc_allow_close_contactor(false);
        });

        // Trigger SLAC restart
        charger->signal_slac_start.connect([this] { r_slac[0]->call_enter_bcd(); });
        // Trigger SLAC reset
        charger->signal_slac_reset.connect([this] { r_slac[0]->call_reset(false); });

        // Ask HLC to stop charging session
        charger->signal_hlc_stop_charging.connect([this] { r_hlc[0]->call_stop_charging(true); });

        auto sae_mode = types::iso15118_charger::SAE_J2847_Bidi_Mode::None;

        types::iso15118_charger::SetupPhysicalValues setup_physical_values;

        // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
        std::vector<types::iso15118_charger::EnergyTransferMode> transfer_modes;
        if (config.charge_mode == "AC") {
            setup_physical_values.ac_nominal_voltage = config.ac_nominal_voltage;

            transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::AC_single_phase_core);
            if (config.three_phases) {
                transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::AC_three_phase_core);
            }

        } else if (config.charge_mode == "DC") {
            // transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_core);
            transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_extended);

            powersupply_capabilities = r_powersupply_DC[0]->call_getCapabilities();

            updateLocalMaxWattLimit(powersupply_capabilities.max_export_power_W);

            setup_physical_values.dc_current_regulation_tolerance =
                powersupply_capabilities.current_regulation_tolerance_A;
            setup_physical_values.dc_peak_current_ripple = powersupply_capabilities.peak_current_ripple_A;

            types::iso15118_charger::DC_EVSEPresentVoltage_Current present_values;
            present_values.EVSEPresentVoltage = 0;
            present_values.EVSEPresentCurrent = 0;
            r_hlc[0]->call_update_dc_present_values(present_values);

            setup_physical_values.dc_energy_to_be_delivered = 10000;

            types::iso15118_charger::DC_EVSEMaximumLimits evseMaxLimits;
            evseMaxLimits.EVSEMaximumCurrentLimit = powersupply_capabilities.max_export_current_A;
            evseMaxLimits.EVSEMaximumPowerLimit = powersupply_capabilities.max_export_power_W;
            evseMaxLimits.EVSEMaximumVoltageLimit = powersupply_capabilities.max_export_voltage_V;
            setup_physical_values.dc_maximum_limits = evseMaxLimits;
            charger->inform_new_evse_max_hlc_limits(evseMaxLimits);

            types::iso15118_charger::DC_EVSEMinimumLimits evseMinLimits;
            evseMinLimits.EVSEMinimumCurrentLimit = powersupply_capabilities.min_export_current_A;
            evseMinLimits.EVSEMinimumVoltageLimit = powersupply_capabilities.min_export_voltage_V;
            setup_physical_values.dc_minimum_limits = evseMinLimits;

            // Cable check for DC charging
            r_hlc[0]->subscribe_Start_CableCheck([this] { cable_check(); });

            // Notification that current demand has started
            r_hlc[0]->subscribe_currentDemand_Started([this] {
                charger->notify_currentdemand_started();
                current_demand_active = true;
            });

            r_hlc[0]->subscribe_currentDemand_Finished([this] {
                current_demand_active = false;
                sae_bidi_active = false;
            });

            // Isolation monitoring for DC charging handler
            if (not r_imd.empty()) {

                imd_stop();

                r_imd[0]->subscribe_IsolationMeasurement([this](types::isolation_monitor::IsolationMeasurement m) {
                    // new DC isolation monitoring measurement received
                    session_log.evse(false, fmt::format("Isolation measurement R_F {}.", m.resistance_F_Ohm));
                    isolation_measurement = m;
                });
            }

            // Get voltage/current from DC power supply
            if (not r_powersupply_DC.empty()) {
                r_powersupply_DC[0]->subscribe_voltage_current([this](types::power_supply_DC::VoltageCurrent m) {
                    powersupply_measurement = m;
                    types::iso15118_charger::DC_EVSEPresentVoltage_Current present_values;
                    present_values.EVSEPresentVoltage = (m.voltage_V > 0 ? m.voltage_V : 0.0);
                    if (config.sae_j2847_2_bpt_enabled) {
                        present_values.EVSEPresentCurrent = m.current_A;
                    } else {
                        present_values.EVSEPresentCurrent = (m.current_A > 0 ? m.current_A : 0.0);
                    }

                    if (config.hack_present_current_offset > 0) {
                        present_values.EVSEPresentCurrent =
                            present_values.EVSEPresentCurrent.value() + config.hack_present_current_offset;
                    }

                    if (config.hack_pause_imd_during_precharge and m.voltage_V * m.current_A > 1000) {
                        // Start IMD again as it was stopped after CableCheck
                        imd_start();
                        EVLOG_info << "Hack: Restarting Isolation Measurement at " << m.voltage_V << " " << m.current_A;
                    }

                    r_hlc[0]->call_update_dc_present_values(present_values);

                    {
                        // dont publish ev_info here, it will be published when other values change.
                        // otherwise we will create too much traffic on mqtt
                        Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_set_ev_info);
                        ev_info.present_voltage = present_values.EVSEPresentVoltage;
                        ev_info.present_current = present_values.EVSEPresentCurrent;
                        // p_evse->publish_ev_info(ev_info);
                    }
                });
            }

            // Car requests a target voltage and current limit
            r_hlc[0]->subscribe_DC_EVTargetVoltageCurrent([this](types::iso15118_charger::DC_EVTargetValues v) {
                bool target_changed = false;

                // Hack for Skoda Enyaq that should be fixed in a different way
                if (config.hack_skoda_enyaq and (v.DC_EVTargetVoltage < 300 or v.DC_EVTargetCurrent < 0))
                    return;

                if (v.DC_EVTargetVoltage not_eq latest_target_voltage or
                    v.DC_EVTargetCurrent not_eq latest_target_current) {
                    latest_target_voltage = v.DC_EVTargetVoltage;
                    latest_target_current = v.DC_EVTargetCurrent;
                    target_changed = true;
                }

                if (target_changed) {
                    apply_new_target_voltage_current();
                    if (not contactor_open) {
                        powersupply_DC_on();
                    }

                    {
                        Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                          Everest::MutexDescription::EVSE_publish_ev_info);
                        ev_info.target_voltage = latest_target_voltage;
                        ev_info.target_current = latest_target_current;
                        p_evse->publish_ev_info(ev_info);
                    }
                }
            });

            // Car requests DC contactor open. We don't actually open but switch off DC supply.
            // opening will be done by Charger on C->B CP event.
            r_hlc[0]->subscribe_DC_Open_Contactor([this] {
                powersupply_DC_off();
                imd_stop();
            });

            // Back up switch off - charger signalled that it needs to switch off now.
            // During normal operation this should be done earlier before switching off relais by HLC protocol.
            charger->signal_dc_supply_off.connect([this] {
                powersupply_DC_off();
                imd_stop();
            });

            // Current demand has finished - switch off DC supply
            r_hlc[0]->subscribe_currentDemand_Finished([this] { powersupply_DC_off(); });

            r_hlc[0]->subscribe_DC_EVMaximumLimits([this](types::iso15118_charger::DC_EVMaximumLimits l) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_DC_EVMaximumLimits);
                ev_info.maximum_current_limit = l.DC_EVMaximumCurrentLimit;
                ev_info.maximum_power_limit = l.DC_EVMaximumPowerLimit;
                ev_info.maximum_voltage_limit = l.DC_EVMaximumVoltageLimit;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DepartureTime([this](const std::string& t) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_DepartureTime);
                ev_info.departure_time = t;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_AC_EAmount([this](double e) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_AC_EAmount);
                ev_info.remaining_energy_needed = e;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_AC_EVMaxVoltage([this](double v) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_AC_EVMaxVoltage);
                ev_info.maximum_voltage_limit = v;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_AC_EVMaxCurrent([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_AC_EVMaxCurrent);
                ev_info.maximum_current_limit = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_AC_EVMinCurrent([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_AC_EVMinCurrent);
                ev_info.minimum_current_limit = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_EVEnergyCapacity([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_DC_EVEnergyCapacity);
                ev_info.battery_capacity = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_EVEnergyRequest([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_DC_EVEnergyRequest);
                ev_info.remaining_energy_needed = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_FullSOC([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_DC_FullSOC);
                ev_info.battery_full_soc = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_BulkSOC([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_DC_BulkSOC);
                ev_info.battery_bulk_soc = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_EVRemainingTime([this](types::iso15118_charger::DC_EVRemainingTime t) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_DC_EVRemainingTime);
                ev_info.estimated_time_full = t.EV_RemainingTimeToFullSoC;
                ev_info.estimated_time_bulk = t.EV_RemainingTimeToBulkSoC;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_DC_EVStatus([this](types::iso15118_charger::DC_EVStatusType s) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_DC_EVStatus);
                ev_info.soc = s.DC_EVRESSSOC;
                p_evse->publish_ev_info(ev_info);
            });

            // SAE J2847/2 Bidi
            if (config.sae_j2847_2_bpt_enabled == true) {

                sae_mode = types::iso15118_charger::string_to_sae_j2847_bidi_mode(config.sae_j2847_2_bpt_mode);

                r_hlc[0]->subscribe_sae_bidi_mode_active([this] {
                    sae_bidi_active = true;

                    if (config.sae_j2847_2_bpt_mode == "V2H") {
                        setup_v2h_mode();
                        session_log.evse(true, "SAE J2847 V2H mode is active");
                    } else if (config.sae_j2847_2_bpt_mode == "V2G") {
                        session_log.evse(true, "SAE J2847 V2G mode is active");
                    } else {
                        EVLOG_error << "Unknown mode discovered. Please select V2G or V2H!";
                    }
                });
            }

            // unused vars of HLC for now:

            // AC_Close_Contactor
            // AC_Open_Contactor

            // SelectedPaymentOption
            // RequestedEnergyTransferMode

            // EV_ChargingSession
            // DC_BulkChargingComplete
            // DC_ChargingComplete

        } else {
            EVLOG_error << "Unsupported charging mode.";
            exit(255);
        }

        r_hlc[0]->call_receipt_is_required(config.ev_receipt_required);

        r_hlc[0]->call_setup(evseid, transfer_modes, sae_mode, config.session_logging, setup_physical_values);

        // reset error flags
        r_hlc[0]->call_reset_error();

        // implement Auth handlers
        r_hlc[0]->subscribe_Require_Auth_EIM([this]() {
            //  Do we have auth already (i.e. delayed HLC after charging already running)?
            if ((config.dbg_hlc_auth_after_tstep and charger->get_authorized_eim_ready_for_hlc()) or
                (not config.dbg_hlc_auth_after_tstep and charger->get_authorized_eim())) {
                {
                    Everest::scoped_lock_timeout lock(hlc_mutex,
                                                      Everest::MutexDescription::EVSE_subscribe_Require_Auth_EIM);
                    hlc_waiting_for_auth_eim = false;
                    hlc_waiting_for_auth_pnc = false;
                }
                r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                                      types::authorization::CertificateStatus::NoCertificateAvailable);
            } else {
                p_token_provider->publish_provided_token(autocharge_token);
                Everest::scoped_lock_timeout lock(hlc_mutex, Everest::MutexDescription::EVSE_publish_provided_token);
                hlc_waiting_for_auth_eim = true;
                hlc_waiting_for_auth_pnc = false;
            }
        });

        if (not config.autocharge_use_slac_instead_of_hlc) {
            r_hlc[0]->subscribe_EVCCIDD([this](const std::string& token) {
                autocharge_token = create_autocharge_token(token, config.connector_id);
                car_manufacturer = get_manufacturer_from_mac(token);
                p_evse->publish_car_manufacturer(car_manufacturer);

                {
                    Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_EVCCIDD);
                    ev_info.evcc_id = token;
                    p_evse->publish_ev_info(ev_info);
                }
            });
        }

        r_hlc[0]->subscribe_Require_Auth_PnC([this](types::authorization::ProvidedIdToken _token) {
            // Do we have auth already (i.e. delayed HLC after charging already running)?

            std::vector<int> referenced_connectors = {this->config.connector_id};
            _token.connectors.emplace(referenced_connectors);
            p_token_provider->publish_provided_token(_token);
            if (charger->get_authorized_pnc()) {
                {
                    Everest::scoped_lock_timeout lock(hlc_mutex,
                                                      Everest::MutexDescription::EVSE_subscribe_Require_Auth_PnC);
                    hlc_waiting_for_auth_eim = false;
                    hlc_waiting_for_auth_pnc = false;
                }
            } else {
                Everest::scoped_lock_timeout lock(hlc_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_Require_Auth_PnC2);
                hlc_waiting_for_auth_eim = false;
                hlc_waiting_for_auth_pnc = true;
            }
        });

        // Install debug V2G Messages handler if session logging is enabled
        if (config.session_logging) {
            r_hlc[0]->subscribe_V2G_Messages([this](types::iso15118_charger::V2G_Messages v2g_messages) {
                json v2g = v2g_messages;
                log_v2g_message(v2g);
            });

            r_hlc[0]->subscribe_Selected_Protocol(
                [this](std::string selected_protocol) { this->selected_protocol = selected_protocol; });
        }
        // switch to DC mode for first session for AC with SoC
        if (config.ac_with_soc) {

            bsp->signal_event.connect([this](const CPEvent event) {
                if (event == CPEvent::CarUnplugged) {
                    // configure for DC again for next session. Will reset to AC when SoC is received
                    switch_DC_mode();
                }
            });

            charger->signal_ac_with_soc_timeout.connect([this]() { switch_DC_mode(); });

            r_hlc[0]->subscribe_DC_EVStatus([this](types::iso15118_charger::DC_EVStatusType status) {
                EVLOG_info << fmt::format("SoC received: {}.", status.DC_EVRESSSOC);
                switch_AC_mode();
            });
        }

        r_hlc[0]->subscribe_Certificate_Request([this](types::iso15118_charger::Request_Exi_Stream_Schema request) {
            p_evse->publish_iso15118_certificate_request(request);
        });
    }

    // Maybe override with user setting for this EVSE
    if (config.max_current_import_A < hw_capabilities.max_current_A_import) {
        hw_capabilities.max_current_A_import = config.max_current_import_A;
    }
    if (config.max_current_export_A < hw_capabilities.max_current_A_export) {
        hw_capabilities.max_current_A_export = config.max_current_export_A;
    }

    if (config.charge_mode == "AC") {
        // by default we import energy
        updateLocalMaxCurrentLimit(hw_capabilities.max_current_A_import);
    }

    // Maybe limit to single phase by user setting if possible with HW
    if (not config.three_phases and hw_capabilities.min_phase_count_import == 1) {
        hw_capabilities.max_phase_count_import = 1;
        local_three_phases = false;
    } else if (hw_capabilities.max_phase_count_import == 3) {
        local_three_phases = true; // other configonfigurations currently not supported by HW
    }

    p_evse->publish_hw_capabilities(hw_capabilities);

    if (config.charge_mode == "AC") {
        EVLOG_info << fmt::format("Max AC hardware capabilities: {}A/{}ph", hw_capabilities.max_current_A_import,
                                  hw_capabilities.max_phase_count_import);
    }

    bsp->signal_event.connect([this](const CPEvent event) {
        // Forward events from BSP to SLAC module before we process the events in the charger
        if (slac_enabled) {
            if (event == CPEvent::EFtoBCD) {
                // this means entering BCD from E|F
                r_slac[0]->call_enter_bcd();
            } else if (event == CPEvent::BCDtoEF) {
                r_slac[0]->call_leave_bcd();
            } else if (event == CPEvent::CarPluggedIn) {
                // CC: right now we dont support energy saving mode, so no need to reset slac here.
                // It is more important to start slac as early as possible to avoid unneccesary retries
                // e.g. by Tesla cars which send the first SLAC_PARM_REQ directly after plugin.
                // If we start slac too late, Tesla will do a B->C->DF->B sequence for each retry which
                // may confuse the PWM state machine in some implementations.
                // r_slac[0]->call_reset(true);
                // This is entering BCD from state A
                car_manufacturer = types::evse_manager::CarManufacturer::Unknown;
                r_slac[0]->call_enter_bcd();
            } else if (event == CPEvent::CarUnplugged) {
                r_slac[0]->call_leave_bcd();
                r_slac[0]->call_reset(false);
            }
        }

        charger->process_event(event);

        // Forward some events to HLC
        if (get_hlc_enabled()) {
            // Reset HLC auth waiting flags on new session
            if (event == CPEvent::CarPluggedIn) {
                r_hlc[0]->call_reset_error();
                r_hlc[0]->call_ac_contactor_closed(false);
                r_hlc[0]->call_stop_charging(false);
                latest_target_voltage = 0;
                latest_target_current = 0;
                {
                    Everest::scoped_lock_timeout lock(hlc_mutex, Everest::MutexDescription::EVSE_signal_event);
                    hlc_waiting_for_auth_eim = false;
                    hlc_waiting_for_auth_pnc = false;
                }
            }

            if (event == CPEvent::PowerOn) {
                contactor_open = false;
                r_hlc[0]->call_ac_contactor_closed(true);
            }

            if (event == CPEvent::PowerOff) {
                contactor_open = true;
                latest_target_voltage = 0;
                latest_target_current = 0;
                r_hlc[0]->call_ac_contactor_closed(false);
            }
        }
    });

    r_bsp->subscribe_ac_nr_of_phases_available([this](int n) { signalNrOfPhasesAvailable(n); });

    if (r_powermeter_billing().size() > 0) {
        r_powermeter_billing()[0]->subscribe_powermeter([this](types::powermeter::Powermeter p) {
            // Inform charger about current charging current. This is used for slow OC detection.
            if (p.current_A and p.current_A.value().L1 and p.current_A.value().L2 and p.current_A.value().L3) {
                charger->set_current_drawn_by_vehicle(p.current_A.value().L1.value(), p.current_A.value().L2.value(),
                                                      p.current_A.value().L3.value());
            }

            // Inform HLC about the power meter data
            if (get_hlc_enabled()) {
                r_hlc[0]->call_update_meter_info(p);
            }

            // Store local cache
            {
                Everest::scoped_lock_timeout lock(power_mutex, Everest::MutexDescription::EVSE_subscribe_powermeter);
                latest_powermeter_data_billing = p;
            }

            // External Nodered interface
            if (p.phase_seq_error) {
                mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/phaseSeqError", config.connector_id),
                             p.phase_seq_error.value());
            }

            mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/time_stamp", config.connector_id),
                         p.timestamp);

            if (p.power_W) {
                mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/totalKw", config.connector_id),
                             p.power_W.value().total / 1000., 1);
            }

            mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/totalKWattHr", config.connector_id),
                         p.energy_Wh_import.total / 1000.);
            json j;
            to_json(j, p);
            mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter_json", config.connector_id), j.dump());
            // /External Nodered interface
        });
    }

    if (slac_enabled) {
        r_slac[0]->subscribe_state([this](const std::string& s) {
            session_log.evse(true, fmt::format("SLAC {}", s));
            // Notify charger whether matching was started (or is done) or not
            if (s == "UNMATCHED") {
                charger->set_matching_started(false);
            } else {
                charger->set_matching_started(true);
            }
        });

        r_slac[0]->subscribe_request_error_routine([this]() {
            EVLOG_info << "Received request error routine from SLAC in evsemanager\n";
            charger->request_error_sequence();
        });

        r_slac[0]->subscribe_dlink_ready([this](const bool value) {
            session_log.evse(true, fmt::format("D-LINK_READY ({})", value));
            if (hlc_enabled) {
                r_hlc[0]->call_dlink_ready(value);
            }
        });
    }

    charger->signal_max_current.connect([this](float ampere) {
        // The charger changed the max current setting. Forward to HLC
        if (get_hlc_enabled()) {
            r_hlc[0]->call_update_ac_max_current(ampere);
        }
    });

    charger->signal_simple_event.connect([this](types::evse_manager::SessionEventEnum s) {
        // Cancel reservations if charger is disabled or faulted
        if (s == types::evse_manager::SessionEventEnum::Disabled or
            s == types::evse_manager::SessionEventEnum::PermanentFault) {
            cancel_reservation(true);
        }
        if (s == types::evse_manager::SessionEventEnum::SessionFinished) {
            // Reset EV information on Session start and end
            ev_info = types::evse_manager::EVInfo();
            p_evse->publish_ev_info(ev_info);
        }

        std::vector<types::iso15118_charger::PaymentOption> payment_options;

        if (get_hlc_enabled() and s == types::evse_manager::SessionEventEnum::SessionFinished) {
            if (config.payment_enable_eim) {
                payment_options.push_back(types::iso15118_charger::PaymentOption::ExternalPayment);
            }
            if (config.payment_enable_contract) {
                payment_options.push_back(types::iso15118_charger::PaymentOption::Contract);
            }
            r_hlc[0]->call_session_setup(payment_options, config.payment_enable_contract);
        }
    });

    charger->signal_session_started_event.connect([this](types::evse_manager::StartSessionReason start_reason) {
        // Reset EV information on Session start and end
        ev_info = types::evse_manager::EVInfo();
        p_evse->publish_ev_info(ev_info);

        std::vector<types::iso15118_charger::PaymentOption> payment_options;

        if (get_hlc_enabled() and start_reason == types::evse_manager::StartSessionReason::Authorized) {
            payment_options.push_back(types::iso15118_charger::PaymentOption::ExternalPayment);
            r_hlc[0]->call_session_setup(payment_options, false);
        }
    });

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);
    invoke_ready(*p_token_provider);
    invoke_ready(*p_random_delay);

    if (config.ac_with_soc) {
        setup_fake_DC_mode();
    } else {
        charger->setup(local_three_phases, config.has_ventilation, config.country_code,
                       (config.charge_mode == "DC" ? Charger::ChargeMode::DC : Charger::ChargeMode::AC), hlc_enabled,
                       config.ac_hlc_use_5percent, config.ac_enforce_hlc, false,
                       config.soft_over_current_tolerance_percent, config.soft_over_current_measurement_noise_A);
    }

    telemetryThreadHandle = std::thread([this]() {
        while (not telemetryThreadHandle.shouldExit()) {
            sleep(10);
            auto p = get_latest_powermeter_data_billing();
            Everest::TelemetryMap telemetry_data{{"timestamp", p.timestamp},
                                                 {"type", "power_meter"},
                                                 {"meter_id", p.meter_id.value_or("N/A")},
                                                 {"energy_import_total_Wh", p.energy_Wh_import.total}};

            if (p.energy_Wh_import.L1) {
                telemetry_data["energy_import_L1_Wh"] = p.energy_Wh_import.L1.value();
            }
            if (p.energy_Wh_import.L2) {
                telemetry_data["energy_import_L2_Wh"] = p.energy_Wh_import.L2.value();
            }
            if (p.energy_Wh_import.L3) {
                telemetry_data["energy_import_L3_Wh"] = p.energy_Wh_import.L3.value();
            }

            if (p.energy_Wh_export) {
                telemetry_data["energy_export_total_Wh"] = p.energy_Wh_export.value().total;
            }
            if (p.energy_Wh_export and p.energy_Wh_export.value().L1) {
                telemetry_data["energy_export_L1_Wh"] = p.energy_Wh_export.value().L1.value();
            }
            if (p.energy_Wh_export and p.energy_Wh_export.value().L2) {
                telemetry_data["energy_export_L2_Wh"] = p.energy_Wh_export.value().L2.value();
            }
            if (p.energy_Wh_export and p.energy_Wh_export.value().L3) {
                telemetry_data["energy_export_L3_Wh"] = p.energy_Wh_export.value().L3.value();
            }

            if (p.power_W) {
                telemetry_data["power_total_W"] = p.power_W.value().total;
            }
            if (p.power_W and p.power_W.value().L1) {
                telemetry_data["power_L1_W"] = p.power_W.value().L1.value();
            }
            if (p.power_W and p.power_W.value().L2) {
                telemetry_data["power_L3_W"] = p.power_W.value().L2.value();
            }
            if (p.power_W and p.power_W.value().L3) {
                telemetry_data["power_L3_W"] = p.power_W.value().L3.value();
            }

            if (p.VAR) {
                telemetry_data["var_total"] = p.VAR.value().total;
            }
            if (p.VAR and p.VAR.value().L1) {
                telemetry_data["var_L1"] = p.VAR.value().L1.value();
            }
            if (p.VAR and p.VAR.value().L2) {
                telemetry_data["var_L1"] = p.VAR.value().L2.value();
            }
            if (p.VAR and p.VAR.value().L3) {
                telemetry_data["var_L1"] = p.VAR.value().L3.value();
            }

            if (p.voltage_V and p.voltage_V.value().L1) {
                telemetry_data["voltage_L1_V"] = p.voltage_V.value().L1.value();
            }
            if (p.voltage_V and p.voltage_V.value().L2) {
                telemetry_data["voltage_L2_V"] = p.voltage_V.value().L2.value();
            }
            if (p.voltage_V and p.voltage_V.value().L3) {
                telemetry_data["voltage_L3_V"] = p.voltage_V.value().L3.value();
            }
            if (p.voltage_V and p.voltage_V.value().DC) {
                telemetry_data["voltage_DC_V"] = p.voltage_V.value().DC.value();
            }

            if (p.current_A and p.current_A.value().L1) {
                telemetry_data["current_L1_A"] = p.current_A.value().L1.value();
            }
            if (p.current_A and p.current_A.value().L2) {
                telemetry_data["current_L2_A"] = p.current_A.value().L2.value();
            }
            if (p.current_A and p.current_A.value().L3) {
                telemetry_data["current_L3_A"] = p.current_A.value().L3.value();
            }
            if (p.current_A and p.current_A.value().DC) {
                telemetry_data["current_DC_A"] = p.current_A.value().DC.value();
            }

            if (p.frequency_Hz) {
                telemetry_data["frequency_L1_Hz"] = p.frequency_Hz.value().L1;
            }
            if (p.frequency_Hz and p.frequency_Hz.value().L2) {
                telemetry_data["frequency_L2_Hz"] = p.frequency_Hz.value().L2.value();
            }
            if (p.frequency_Hz and p.frequency_Hz.value().L3) {
                telemetry_data["frequency_L3_Hz"] = p.frequency_Hz.value().L3.value();
            }

            if (p.phase_seq_error) {
                telemetry_data["phase_seq_error"] = p.phase_seq_error.value();
            }

            // Publish as external telemetry data
            telemetry.publish("livedata", "power_meter", telemetry_data);
        }
    });

    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw
    //  caps.
    charger->set_max_current(0.0F, date::utc_clock::now() + std::chrono::seconds(10));
    this->p_evse->publish_waiting_for_external_ready(config.external_ready_to_start_charging);
    if (not config.external_ready_to_start_charging) {
        // immediately ready, otherwise delay until we get the external signal
        this->ready_to_start_charging();
    }
}

void EvseManager::ready_to_start_charging() {
    timepoint_ready_for_charging = std::chrono::steady_clock::now();
    charger->run();
    charger->enable(0);

    this->p_evse->publish_ready(true);
    EVLOG_info << fmt::format(fmt::emphasis::bold | fg(fmt::terminal_color::green),
                              "🌀🌀🌀 Ready to start charging 🌀🌀🌀");
}

types::powermeter::Powermeter EvseManager::get_latest_powermeter_data_billing() {
    Everest::scoped_lock_timeout lock(power_mutex, Everest::MutexDescription::EVSE_get_latest_powermeter_data_billing);
    return latest_powermeter_data_billing;
}

types::evse_board_support::HardwareCapabilities EvseManager::get_hw_capabilities() {
    return hw_capabilities;
}

int32_t EvseManager::get_reservation_id() {
    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_get_reservation_id);
    return reservation_id;
}

void EvseManager::switch_DC_mode() {
    charger->evse_replug();
    setup_fake_DC_mode();
}

void EvseManager::switch_AC_mode() {
    charger->evse_replug();
    setup_AC_mode();
}

// This sets up a fake DC mode that is just supposed to work until we get the SoC.
// It is only used for AC<>DC<>AC<>DC mode to get AC charging with SoC.
void EvseManager::setup_fake_DC_mode() {
    charger->setup(local_three_phases, config.has_ventilation, config.country_code, Charger::ChargeMode::DC,
                   hlc_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc, false,
                   config.soft_over_current_tolerance_percent, config.soft_over_current_measurement_noise_A);

    types::iso15118_charger::EVSEID evseid = {config.evse_id, config.evse_id_din};

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    std::vector<types::iso15118_charger::EnergyTransferMode> transfer_modes;

    transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_core);
    transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_extended);
    transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_combo_core);
    transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::DC_unique);

    types::iso15118_charger::SetupPhysicalValues setup_physical_values;

    setup_physical_values.dc_current_regulation_tolerance = powersupply_capabilities.current_regulation_tolerance_A;
    setup_physical_values.dc_peak_current_ripple = powersupply_capabilities.peak_current_ripple_A;

    types::iso15118_charger::DC_EVSEPresentVoltage_Current present_values;
    present_values.EVSEPresentVoltage = 400; // FIXME: set a correct values
    present_values.EVSEPresentCurrent = 0;

    r_hlc[0]->call_update_dc_present_values(present_values);

    types::iso15118_charger::DC_EVSEMaximumLimits evseMaxLimits;
    evseMaxLimits.EVSEMaximumCurrentLimit = 400;
    evseMaxLimits.EVSEMaximumPowerLimit = 200000;
    evseMaxLimits.EVSEMaximumVoltageLimit = 1000;

    setup_physical_values.dc_maximum_limits = evseMaxLimits;

    types::iso15118_charger::DC_EVSEMinimumLimits evseMinLimits;
    evseMinLimits.EVSEMinimumCurrentLimit = 0;
    evseMinLimits.EVSEMinimumVoltageLimit = 0;

    setup_physical_values.dc_minimum_limits = evseMinLimits;

    const auto sae_mode = types::iso15118_charger::SAE_J2847_Bidi_Mode::None;

    r_hlc[0]->call_setup(evseid, transfer_modes, sae_mode, config.session_logging, setup_physical_values);
}

void EvseManager::setup_AC_mode() {
    charger->setup(local_three_phases, config.has_ventilation, config.country_code, Charger::ChargeMode::AC,
                   hlc_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc, true,
                   config.soft_over_current_tolerance_percent, config.soft_over_current_measurement_noise_A);

    types::iso15118_charger::EVSEID evseid = {config.evse_id, config.evse_id_din};

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    std::vector<types::iso15118_charger::EnergyTransferMode> transfer_modes;

    transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::AC_single_phase_core);

    if (config.three_phases) {
        transfer_modes.push_back(types::iso15118_charger::EnergyTransferMode::AC_three_phase_core);
    }

    types::iso15118_charger::SetupPhysicalValues setup_physical_values;

    const auto sae_mode = types::iso15118_charger::SAE_J2847_Bidi_Mode::None;

    if (get_hlc_enabled()) {
        r_hlc[0]->call_setup(evseid, transfer_modes, sae_mode, config.session_logging, setup_physical_values);
    }
}

void EvseManager::setup_v2h_mode() {
    types::iso15118_charger::DC_EVSEMaximumLimits evseMaxLimits;
    types::iso15118_charger::DC_EVSEMinimumLimits evseMinLimits;

    if (powersupply_capabilities.max_import_current_A.has_value() and
        powersupply_capabilities.max_import_power_W.has_value() and
        powersupply_capabilities.max_import_voltage_V.has_value()) {
        evseMaxLimits.EVSEMaximumCurrentLimit = -powersupply_capabilities.max_import_current_A.value();
        evseMaxLimits.EVSEMaximumPowerLimit = -powersupply_capabilities.max_import_power_W.value();
        evseMaxLimits.EVSEMaximumVoltageLimit = powersupply_capabilities.max_import_voltage_V.value();
        r_hlc[0]->call_update_dc_maximum_limits(evseMaxLimits);
        charger->inform_new_evse_max_hlc_limits(evseMaxLimits);
    } else {
        EVLOG_error << "No Import Current, Power or Voltage is available!!!";
        return;
    }

    if (powersupply_capabilities.min_import_current_A.has_value() and
        powersupply_capabilities.min_import_voltage_V.has_value()) {
        evseMinLimits.EVSEMinimumCurrentLimit = -powersupply_capabilities.min_import_current_A.value();
        evseMinLimits.EVSEMinimumVoltageLimit = powersupply_capabilities.min_import_voltage_V.value();
        r_hlc[0]->call_update_dc_minimum_limits(evseMinLimits);
    } else {
        EVLOG_error << "No Import Current, Power or Voltage is available!!!";
        return;
    }

    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;
    target_entry.limits_to_leaves.total_power_W = powersupply_capabilities.max_import_power_W.value();

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = 0;

    external_limits.schedule_export.emplace(std::vector<types::energy::ScheduleReqEntry>(1, target_entry));
    external_limits.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>(1, zero_entry));

    updateLocalEnergyLimit(external_limits);
}

bool EvseManager::updateLocalEnergyLimit(types::energy::ExternalLimits l) {

    // received empty limits, fall back to hardware limits
    if (not l.schedule_import.has_value() and not l.schedule_export.has_value()) {
        EVLOG_info << "External limits are empty, defaulting to hardware limits";
        if (config.charge_mode == "AC") {
            // by default we import energy
            updateLocalMaxCurrentLimit(hw_capabilities.max_current_A_import);
        } else {
            updateLocalMaxWattLimit(powersupply_capabilities.max_export_power_W);
        }
    } else {
        // apply external limits if they are lower
        local_energy_limits = l;
    }

    // wait for EnergyManager to assign optimized current on next opimizer run

    return true;
}

// Note: deprecated, use updateLocalEnergyLimit. Only kept for node red compat.
// This overwrites all other schedules set before.
bool EvseManager::updateLocalMaxWattLimit(float max_watt) {
    types::energy::ScheduleReqEntry e;
    e.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    if (max_watt >= 0) {
        e.limits_to_leaves.total_power_W = max_watt;
        local_energy_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);
        e.limits_to_leaves.total_power_W = 0;
        local_energy_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);
    } else {
        e.limits_to_leaves.total_power_W = -max_watt;
        local_energy_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);
        e.limits_to_leaves.total_power_W = 0;
        local_energy_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);
    }
    return true;
}

// Note: deprecated, use updateLocalEnergyLimit. Only kept for node red compat.
// This overwrites all other schedules set before.
bool EvseManager::updateLocalMaxCurrentLimit(float max_current) {
    if (config.charge_mode == "DC") {
        return false;
    }

    types::energy::ScheduleReqEntry e;
    e.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    if (max_current >= 0) {
        e.limits_to_leaves.ac_max_current_A = max_current;
        local_energy_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);
        e.limits_to_leaves.ac_max_current_A = 0;
        local_energy_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);
    } else {
        e.limits_to_leaves.ac_max_current_A = -max_current;
        local_energy_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);
        e.limits_to_leaves.ac_max_current_A = 0;
        local_energy_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);
    }

    return true;
}

bool EvseManager::reserve(int32_t id) {

    // is the evse Unavailable?
    if (charger->get_current_state() == Charger::EvseState::Disabled) {
        return false;
    }

    // is the evse faulted?
    if (charger->errors_prevent_charging()) {
        return false;
    }

    // is the connector currently ready to accept a new car?
    if (charger->get_current_state() not_eq Charger::EvseState::Idle) {
        return false;
    }

    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_reserve);

    if (not reserved) {
        reserved = true;
        reservation_id = id;

        // publish event to other modules
        types::evse_manager::SessionEvent se;
        se.event = types::evse_manager::SessionEventEnum::ReservationStart;

        signalReservationEvent(se);
        return true;
    }

    return false;
}

void EvseManager::cancel_reservation(bool signal_event) {

    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_cancel_reservation);
    if (reserved) {
        reserved = false;
        reservation_id = 0;

        // publish event to other modules
        if (signal_event) {
            types::evse_manager::SessionEvent se;
            se.event = types::evse_manager::SessionEventEnum::ReservationEnd;
            signalReservationEvent(se);
        }
    }
}

bool EvseManager::is_reserved() {
    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_is_reserved);
    return reserved;
}

bool EvseManager::getLocalThreePhases() {
    return local_three_phases;
}

bool EvseManager::get_hlc_enabled() {
    Everest::scoped_lock_timeout lock(hlc_mutex, Everest::MutexDescription::EVSE_get_hlc_enabled);
    return hlc_enabled;
}

bool EvseManager::get_hlc_waiting_for_auth_pnc() {
    Everest::scoped_lock_timeout lock(hlc_mutex, Everest::MutexDescription::EVSE_get_hlc_waiting_for_auth_pnc);
    return hlc_waiting_for_auth_pnc;
}

void EvseManager::log_v2g_message(Object m) {
    std::string msg = m["V2G_Message_ID"];

    std::string xml = "";
    std::string json_str = "";
    if (m["V2G_Message_XML"].is_null() and m["V2G_Message_JSON"].is_string()) {
        json_str = m["V2G_Message_JSON"];
    } else if (m["V2G_Message_XML"].is_string()) {
        xml = m["V2G_Message_XML"];
    }

    // All messages from EVSE contain Req and all originating from Car contain Res
    if (msg.find("Res") == std::string::npos) {
        session_log.car(true, fmt::format("V2G {}", msg), xml, m["V2G_Message_EXI_Hex"], m["V2G_Message_EXI_Base64"],
                        json_str);
    } else {
        session_log.evse(true, fmt::format("V2G {}", msg), xml, m["V2G_Message_EXI_Hex"], m["V2G_Message_EXI_Base64"],
                         json_str);
    }
}

void EvseManager::charger_was_authorized() {

    Everest::scoped_lock_timeout lock(hlc_mutex, Everest::MutexDescription::EVSE_charger_was_authorized);
    if (hlc_waiting_for_auth_pnc and charger->get_authorized_pnc()) {
        r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                              types::authorization::CertificateStatus::Accepted);
        hlc_waiting_for_auth_eim = false;
        hlc_waiting_for_auth_pnc = false;
    }

    if (hlc_waiting_for_auth_eim and charger->get_authorized_eim()) {
        r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                              types::authorization::CertificateStatus::NoCertificateAvailable);
        hlc_waiting_for_auth_eim = false;
        hlc_waiting_for_auth_pnc = false;
    }
}

void EvseManager::cable_check() {

    if (r_imd.empty()) {
        // If no IMD is connected, we skip isolation checking.
        EVLOG_info << "No IMD: skippint cable check.";
        r_hlc[0]->call_update_isolation_status(types::iso15118_charger::IsolationStatus::No_IMD);
        r_hlc[0]->call_cable_check_finished(true);
        return;
    }
    // start cable check in a seperate thread.
    std::thread t([this]() {
        session_log.evse(true, "Start cable check...");
        bool ok = false;

        // normally contactors should be closed before entering cable check routine.
        // On some hardware implementation it may take some time until the confirmation arrives though,
        // so we wait with a timeout here until the contactors are confirmed to be closed.
        // Allow closing from HLC perspective, it will wait for CP state C in Charger IEC state machine as well.
        session_log.car(true, "DC HLC Close contactor (in CableCheck)");
        charger->set_hlc_allow_close_contactor(true);

        Timeout timeout;
        timeout.start(CABLECHECK_CONTACTORS_CLOSE_TIMEOUT);

        while (not timeout.reached()) {
            if (not contactor_open) {
                break;
            }
            std::this_thread::sleep_for(100ms);
        }

        // verify the relais are really switched on and set 500V output
        if (not contactor_open) {
            if (powersupply_DC_set(config.dc_isolation_voltage_V, 2)) {
                powersupply_DC_on();
                imd_start();

                // wait until the voltage has rised to the target value
                if (not wait_powersupply_DC_voltage_reached(config.dc_isolation_voltage_V)) {
                    EVLOG_info << "Voltage did not rise to 500V within timeout";
                    powersupply_DC_off();
                    fail_session();
                    ok = false;
                    imd_stop();
                } else {
                    // read out one new isolation resistance
                    isolation_measurement.clear();
                    types::isolation_monitor::IsolationMeasurement m;
                    if (not isolation_measurement.wait_for(m, 10s)) {
                        EVLOG_info << "Did not receive isolation measurement from IMD within 10 seconds.";
                        powersupply_DC_off();
                        ok = false;
                        fail_session();
                    } else {
                        // wait until the voltage is back to safe level
                        float minvoltage = (config.switch_to_minimum_voltage_after_cable_check
                                                ? powersupply_capabilities.min_export_voltage_V
                                                : config.dc_isolation_voltage_V);

                        // We do not want to shut down power supply
                        if (minvoltage < 60) {
                            minvoltage = 60;
                        }
                        powersupply_DC_set(minvoltage, 2);

                        if (not wait_powersupply_DC_below_voltage(minvoltage + 20)) {
                            EVLOG_info << "Voltage did not go back to minimal voltage within timeout.";
                            ok = false;
                            fail_session();
                        } else {
                            // verify it is within ranges. Warning level is <500 Ohm/V_max_output_rating, Fault
                            // is <100
                            const double min_resistance_ok = 500. * powersupply_capabilities.max_export_voltage_V;
                            const double min_resistance_warning = 100. * powersupply_capabilities.max_export_voltage_V;

                            if (m.resistance_F_Ohm < min_resistance_warning) {
                                session_log.evse(
                                    false, fmt::format("Isolation measurement FAULT R_F {}.", m.resistance_F_Ohm));
                                ok = true; // this just means that we are finished measuring, not that we are ok with
                                           // the result
                                r_hlc[0]->call_update_isolation_status(types::iso15118_charger::IsolationStatus::Fault);
                                imd_stop();
                                fail_session();
                            } else if (m.resistance_F_Ohm < min_resistance_ok) {
                                session_log.evse(
                                    false, fmt::format("Isolation measurement WARNING R_F {}.", m.resistance_F_Ohm));
                                ok = true;
                                r_hlc[0]->call_update_isolation_status(
                                    types::iso15118_charger::IsolationStatus::Warning);
                            } else {
                                session_log.evse(false,
                                                 fmt::format("Isolation measurement Ok R_F {}.", m.resistance_F_Ohm));
                                ok = true;
                                r_hlc[0]->call_update_isolation_status(types::iso15118_charger::IsolationStatus::Valid);
                            }
                        }
                    }
                }
            } else {
                EVLOG_error << fmt::format("CableCheck Thread: Could not set DC power supply voltage and current.");
                fail_session();
            }
        } else {
            EVLOG_error << fmt::format("CableCheck Thread: Contactors are still open after timeout, giving up.");
            fail_session();
        }

        if (config.hack_pause_imd_during_precharge)
            imd_stop();

        // Sleep before submitting result to spend more time in cable check. This is needed for some solar inverters
        // used as DC chargers for them to warm up.
        sleep(config.hack_sleep_in_cable_check);
        if (car_manufacturer == types::evse_manager::CarManufacturer::VolkswagenGroup) {
            sleep(config.hack_sleep_in_cable_check_volkswagen);
        }

        // submit result to HLC
        r_hlc[0]->call_cable_check_finished(ok);
    });
    // Detach thread and exit command handler right away
    t.detach();
}

void EvseManager::powersupply_DC_on() {
    if (not powersupply_dc_is_on) {
        session_log.evse(false, "DC power supply: switch ON called");
        r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Export);
        powersupply_dc_is_on = true;
    }
}

// input voltage/current is what the evse/car would like to set.
// if it is more then what the energymanager gave us, we can limit it here.
bool EvseManager::powersupply_DC_set(double _voltage, double _current) {
    double voltage = _voltage;
    double current = _current;
    static bool last_is_actually_exporting_to_grid{false};

    // Some cars always request integer ampere values, so if we offer 14.34A they will request 14.0A.
    // On low power DC charging this makes quite a difference
    // this option will deliver the offered ampere value in those cases

    if (config.hack_fix_hlc_integer_current_requests) {
        auto hlc_limits = charger->get_evse_max_hlc_limits();
        if (hlc_limits.EVSEMaximumCurrentLimit - (int)current < 1.)
            current = hlc_limits.EVSEMaximumCurrentLimit;
    }

    if (config.sae_j2847_2_bpt_enabled) {
        current = std::abs(current);
    }

    if ((config.hack_allow_bpt_with_iso2 or config.sae_j2847_2_bpt_enabled) and current_demand_active and
        is_actually_exporting_to_grid) {
        if (not last_is_actually_exporting_to_grid) {
            // switching from import from grid to export to grid
            session_log.evse(false, "DC power supply: switch ON in import mode");
            r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Import);
        }
        last_is_actually_exporting_to_grid = is_actually_exporting_to_grid;
        // Hack: we are exporting to grid but are in ISO-2 mode
        // check limits of supply
        if (powersupply_capabilities.min_import_voltage_V.has_value() and
            voltage >= powersupply_capabilities.min_import_voltage_V.value() and
            voltage <= powersupply_capabilities.max_import_voltage_V.value()) {

            if (powersupply_capabilities.max_import_current_A.has_value() and
                current > powersupply_capabilities.max_import_current_A.value())
                current = powersupply_capabilities.max_import_current_A.value();

            if (powersupply_capabilities.min_import_current_A.has_value() and
                current < powersupply_capabilities.min_import_current_A.value())
                current = powersupply_capabilities.min_import_current_A.value();

            // Now it is within limits of DC power supply.
            // now also limit with the limits given by the energymanager.
            // FIXME: dont do this for now, see if the car reduces if we supply new limits.

            session_log.evse(false, fmt::format("BPT HACK: DC power supply set: {}V/{}A, requested was {}V/{}A.",
                                                voltage, current, _voltage, _current));

            // set the new limits for the DC output
            r_powersupply_DC[0]->call_setImportVoltageCurrent(voltage, current);
            return true;
        }
        EVLOG_critical << fmt::format("DC voltage/current out of limits requested: Voltage {} Current {}.", voltage,
                                      current);
        return false;

    } else {

        if ((config.hack_allow_bpt_with_iso2 or config.sae_j2847_2_bpt_enabled) and current_demand_active and
            last_is_actually_exporting_to_grid) {
            // switching from export to grid to import from grid
            session_log.evse(false, "DC power supply: switch ON in export mode");
            r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Export);
            last_is_actually_exporting_to_grid = is_actually_exporting_to_grid;
        }

        // check limits of supply
        if (voltage >= powersupply_capabilities.min_export_voltage_V and
            voltage <= powersupply_capabilities.max_export_voltage_V) {

            if (current > powersupply_capabilities.max_export_current_A)
                current = powersupply_capabilities.max_export_current_A;

            if (current < powersupply_capabilities.min_export_current_A)
                current = powersupply_capabilities.min_export_current_A;

            // Now it is within limits of DC power supply.
            // now also limit with the limits given by the energymanager.
            // FIXME: dont do this for now, see if the car reduces if we supply new limits.

            session_log.evse(false, fmt::format("DC power supply set: {}V/{}A, requested was {}V/{}A.", voltage,
                                                current, _voltage, _current));

            // set the new limits for the DC output
            r_powersupply_DC[0]->call_setExportVoltageCurrent(voltage, current);
            return true;
        }
        EVLOG_critical << fmt::format("DC voltage/current out of limits requested: Voltage {} Current {}.", voltage,
                                      current);
        return false;
    }
}

void EvseManager::powersupply_DC_off() {
    if (powersupply_dc_is_on) {
        session_log.evse(false, "DC power supply OFF");
        r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Off);
        powersupply_dc_is_on = false;
    }
}

bool EvseManager::wait_powersupply_DC_voltage_reached(double target_voltage) {
    // wait until the voltage has rised to the target value
    Timeout timeout;
    timeout.start(30s);
    bool voltage_ok = false;
    while (not timeout.reached()) {
        types::power_supply_DC::VoltageCurrent m;
        if (powersupply_measurement.wait_for(m, 2000ms)) {
            if (fabs(m.voltage_V - target_voltage) < 10) {
                voltage_ok = true;
                break;
            }
        } else {
            EVLOG_info << "Did not receive voltage measurement from power supply within 2 seconds.";
            powersupply_DC_off();
            break;
        }
    }
    return voltage_ok;
}

bool EvseManager::wait_powersupply_DC_below_voltage(double target_voltage) {
    // wait until the voltage is below the target voltage
    Timeout timeout;
    timeout.start(30s);
    bool voltage_ok = false;
    while (not timeout.reached()) {
        types::power_supply_DC::VoltageCurrent m;
        if (powersupply_measurement.wait_for(m, 2000ms)) {
            if (m.voltage_V < target_voltage) {
                voltage_ok = true;
                break;
            }
        } else {
            EVLOG_info << "Did not receive voltage measurement from power supply within 2 seconds.";
            powersupply_DC_off();
            break;
        }
    }
    return voltage_ok;
}

const std::vector<std::unique_ptr<powermeterIntf>>& EvseManager::r_powermeter_billing() {
    if (r_powermeter_car_side.size() > 0) {
        return r_powermeter_car_side;
    } else {
        return r_powermeter_grid_side;
    }
}

void EvseManager::imd_stop() {
    if (not r_imd.empty()) {
        r_imd[0]->call_stop();
    }
}

void EvseManager::imd_start() {
    if (not r_imd.empty()) {
        r_imd[0]->call_start();
    }
}

types::energy::ExternalLimits EvseManager::getLocalEnergyLimits() {
    return local_energy_limits;
}

void EvseManager::fail_session() {
    r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_EmergencyShutdown);
    if (config.charge_mode == "DC") {
        powersupply_DC_off();
    }
    charger->set_hlc_error();
}

types::evse_manager::EVInfo EvseManager::get_ev_info() {
    Everest::scoped_lock_timeout l(ev_info_mutex, Everest::MutexDescription::EVSE_get_ev_info);
    return ev_info;
}

void EvseManager::apply_new_target_voltage_current() {
    if (latest_target_voltage > 0) {
        powersupply_DC_set(latest_target_voltage, latest_target_current);
    }
}

} // namespace module
