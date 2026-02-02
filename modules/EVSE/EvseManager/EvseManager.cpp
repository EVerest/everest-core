// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"

#include <algorithm>
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>

#include "IECStateMachine.hpp"
#include "SessionLog.hpp"
#include "Timeout.hpp"
#include "scoped_lock_timeout.hpp"
#include "utils.hpp"

using namespace std::literals::chrono_literals;

namespace module {

namespace {
const std::vector<std::unique_ptr<powermeterIntf>> EMPTY_POWERMETER_VECTOR;

types::power_supply_DC::Capabilities get_sane_default_power_supply_capabilities() {
    // Init power supply capabilities with safe defaults
    types::power_supply_DC::Capabilities psu_caps;
    psu_caps.bidirectional = false;
    psu_caps.peak_current_ripple_A = 0.5;
    psu_caps.current_regulation_tolerance_A = 0.5;
    psu_caps.max_export_current_A = 0;
    psu_caps.max_export_power_W = 0;
    psu_caps.max_export_voltage_V = 60;
    psu_caps.min_export_voltage_V = 0;
    psu_caps.min_export_current_A = 0;
    return psu_caps;
}

void trim_colons_from_string(std::string& text) {
    text.erase(remove(text.begin(), text.end(), ':'), text.end());
}

types::authorization::ProvidedIdToken create_autocharge_token(std::string token, int connector_id) {
    types::authorization::ProvidedIdToken autocharge_token;
    autocharge_token.authorization_type = types::authorization::AuthorizationType::Autocharge;
    trim_colons_from_string(token);
    autocharge_token.id_token = {"VID:" + token, types::authorization::IdTokenType::MacAddress};
    autocharge_token.connectors.emplace({connector_id});
    return autocharge_token;
}

bool almost_eq(double a, double b) {
    constexpr auto eps = 1e-6;
    return std::fabs(a - b) <= eps;
}

} // namespace

void EvseManager::init() {

    if (!config.connector_type.empty()) {
        try {
            connector_type = types::evse_manager::string_to_connector_type_enum(config.connector_type);
        } catch (const std::out_of_range& e) {
            EVLOG_warning << "Unknown/invalid connector type: " << config.connector_type;
        }
    }

    store = std::unique_ptr<PersistentStore>(new PersistentStore(r_store, info.id));

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
    if (config.autocharge_use_slac_instead_of_hlc and slac_enabled and config.enable_autocharge) {
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

    pnc_enabled = config.payment_enable_contract;
    central_contract_validation_allowed = config.central_contract_validation_allowed;
    contract_certificate_installation_enabled = config.contract_certificate_installation_enabled;

    reserved = false;
    reservation_id = -1;

    hlc_waiting_for_auth_eim = false;
    hlc_waiting_for_auth_pnc = false;

    latest_target_voltage = 0;
    latest_target_current = 0;

    // apply sane defaults capabilities settings once on boot
    powersupply_capabilities = get_sane_default_power_supply_capabilities();

    if (hlc_enabled) {
        if (config.charge_mode == "DC") {
            // subscribe to run time updates for real initial values (and changes e.g. due to de-rating)
            r_powersupply_DC[0]->subscribe_capabilities([this](const auto& caps) {
                update_powersupply_capabilities(caps);

                auto mode = types::iso15118::EnergyTransferMode::DC;
                auto bpt_mode = types::iso15118::EnergyTransferMode::DC_BPT;

                if (connector_type.has_value() and
                    connector_type.value() == types::evse_manager::ConnectorTypeEnum::cMCS) {
                    mode = types::iso15118::EnergyTransferMode::MCS;
                    bpt_mode = types::iso15118::EnergyTransferMode::MCS_BPT;
                }

                const bool dc_was_updated = update_supported_energy_transfers(mode);
                const bool dc_bpt_was_updated =
                    caps.bidirectional ? update_supported_energy_transfers(bpt_mode) : false;

                if (dc_was_updated || dc_bpt_was_updated) {
                    this->p_evse->publish_supported_energy_transfer_modes(supported_energy_transfers);
                }
            });
        }
    }

    r_bsp->subscribe_request_stop_transaction(
        [this](types::evse_manager::StopTransactionRequest r) { charger->cancel_transaction(r); });

    r_bsp->subscribe_capabilities([this](types::evse_board_support::HardwareCapabilities c) {
        {
            std::scoped_lock lock(hw_caps_mutex);
            hw_capabilities = c;
        }

        if (ac_nr_phases_active == 0) {
            ac_nr_phases_active = c.max_phase_count_import;
        }

        if (ac_nr_phases_active > c.max_phase_count_import) {
            ac_nr_phases_active = c.min_phase_count_import;
        }

        signalNrOfPhasesAvailable(ac_nr_phases_active);

        bsp->set_three_phases(c.max_phase_count_import);
        charger->set_connector_type(c.connector_type);
        p_evse->publish_hw_capabilities(c);
        if (config.charge_mode == "AC" and hlc_enabled) {
            EVLOG_debug << fmt::format("Max AC hardware capabilities: {}A/{}ph", hw_capabilities.max_current_A_import,
                                       hw_capabilities.max_phase_count_import);
            const bool ac_1_was_updated =
                update_supported_energy_transfers(types::iso15118::EnergyTransferMode::AC_single_phase_core);
            const bool ac_3_was_updated =
                c.max_phase_count_import == 3
                    ? update_supported_energy_transfers(types::iso15118::EnergyTransferMode::AC_three_phase_core)
                    : false;
            if (ac_1_was_updated || ac_3_was_updated) {
                this->p_evse->publish_supported_energy_transfer_modes(supported_energy_transfers);
            }

            update_hlc_ac_parameters();
        }
    });

    // do not allow to update hardware capabilties until we are ready for it
    hw_caps_mutex.lock();
}

void EvseManager::ready() {
    bsp = std::make_unique<IECStateMachine>(r_bsp, config.lock_connector_in_state_b);

    if (config.hack_simplified_mode_limit_10A) {
        bsp->set_ev_simplified_mode_evse_limit(true);
    }

    // we provide the powermeter interface to the ErrorHandling only if we need to react to powermeter errors
    // otherwise we provide an empty vector of pointers to the powermeter interface
    error_handling = std::unique_ptr<ErrorHandling>(
        new ErrorHandling(r_bsp, r_hlc, r_connector_lock, r_ac_rcd, p_evse, r_imd, r_powersupply_DC,
                          config.fail_on_powermeter_errors ? r_powermeter_billing() : EMPTY_POWERMETER_VECTOR,
                          r_over_voltage_monitor, config.inoperative_error_use_vendor_id));

    internal_over_voltage_monitor = std::make_unique<OverVoltageMonitor>(
        [this](OverVoltageMonitor::FaultType type, const std::string& description) {
            if (this->error_handling) {
                const auto severity = type == OverVoltageMonitor::FaultType::Emergency
                                          ? Everest::error::Severity::High
                                          : Everest::error::Severity::Medium;
                this->error_handling->raise_over_voltage_error(severity, description);
            }
        },
        std::chrono::milliseconds(config.internal_over_voltage_duration_ms));

    if (not config.lock_connector_in_state_b) {
        EVLOG_warning << "Unlock connector in CP state B. This violates IEC61851-1:2019 D.6.5 Table D.9 line 4 and "
                         "should not be used in public environments!";
    }

    charger = std::make_unique<Charger>(bsp, error_handling, r_powermeter_billing(), store,
                                        hw_capabilities.connector_type, config.evse_id);

    // Now incoming hardware capabilties can be processed
    hw_caps_mutex.unlock();

    if (r_connector_lock.size() > 0) {
        bsp->signal_lock.connect([this]() { r_connector_lock[0]->call_lock(); });
        bsp->signal_unlock.connect([this]() { r_connector_lock[0]->call_unlock(); });
    }

    if (hlc_enabled) {

        // Set up EVSE ID
        types::iso15118::EVSEID evseid = {config.evse_id, config.evse_id_din};

        // Set up auth options for HLC
        std::vector<types::iso15118::PaymentOption> payment_options;
        // if pnc is disabled, disable contract installation and central contract validation
        bool _contract_certificate_installation_enabled =
            pnc_enabled ? contract_certificate_installation_enabled.load() : false;
        bool _central_contract_validation_allowed = pnc_enabled ? central_contract_validation_allowed.load() : false;

        if (config.payment_enable_eim) {
            payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
        }
        if (pnc_enabled) {
            payment_options.push_back(types::iso15118::PaymentOption::Contract);
        }
        if (!config.payment_enable_eim and !pnc_enabled) {
            EVLOG_warning << "Both payment options are disabled! ExternalPayment is nevertheless enabled in this case.";
            payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
        }
        r_hlc[0]->call_session_setup(payment_options, _contract_certificate_installation_enabled,
                                     _central_contract_validation_allowed);

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
            selected_d20_energy_service.reset();
            session_log.evse(true, "D-LINK_TERMINATE.req");
            charger->dlink_terminate();
            r_slac[0]->call_dlink_terminate();
        });

        r_hlc[0]->subscribe_v2g_setup_finished([this] { charger->set_hlc_charging_active(); });

        r_hlc[0]->subscribe_ac_close_contactor([this] {
            session_log.car(true, "AC HLC Close contactor");
            charger->set_hlc_allow_close_contactor(true);
        });

        r_hlc[0]->subscribe_ac_open_contactor([this] {
            session_log.car(true, "AC HLC Open contactor");
            charger->set_hlc_allow_close_contactor(false);
        });

        // Trigger SLAC restart
        charger->signal_slac_start.connect([this] { r_slac[0]->call_enter_bcd(); });
        // Trigger SLAC reset
        charger->signal_slac_reset.connect([this] { r_slac[0]->call_reset(false); });

        // Ask HLC to stop charging session
        charger->signal_hlc_stop_charging.connect([this] { r_hlc[0]->call_stop_charging(true); });
        charger->signal_hlc_pause_charging.connect([this] { r_hlc[0]->call_pause_charging(true); });
        charger->signal_hlc_plug_in_timeout.connect([this] {
            r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Unknown,
                                                  types::authorization::CertificateStatus::NoCertificateAvailable);
        });

        // Charger needs to inform ISO stack about emergency stop
        charger->signal_hlc_error.connect([this](types::iso15118::EvseError error) {
            if (r_hlc.empty()) {
                EVLOG_warning << "HLC module not connected, cannot send error!";
                return;
            }
            r_hlc[0]->call_send_error(error);
        });

        // Right now only no energy pause after cable check and pre charge is supported
        // For DIN it is always stop before cablecheck
        charger->signal_hlc_no_energy_available.connect([this] {
            if (config.zero_power_ignore_pause) {
                EVLOG_info << "HLC module wont perform a pause. Even when there is no energy available.";
                return;
            }

            const auto no_energy_pause_mode = config.zero_power_allow_ev_to_ignore_pause
                                                  ? types::iso15118::NoEnergyPauseMode::AllowEvToIgnorePause
                                                  : types::iso15118::NoEnergyPauseMode::PauseAfterPrecharge;
            r_hlc[0]->call_no_energy_pause_charging(no_energy_pause_mode);
        });

        auto sae_mode = types::iso15118::SaeJ2847BidiMode::None;

        // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
        std::vector<types::iso15118::EnergyTransferMode> transfer_modes;
        if (config.charge_mode == "AC") {
            types::iso15118::SetupPhysicalValues setup_physical_values;
            setup_physical_values.ac_nominal_voltage = config.ac_nominal_voltage;
            r_hlc[0]->call_set_charging_parameters(setup_physical_values);

            switch (hw_capabilities.max_phase_count_import) {
            case 3:
                transfer_modes.push_back(types::iso15118::EnergyTransferMode::AC_three_phase_core);
                [[fallthrough]];
            case 2:
                transfer_modes.push_back(types::iso15118::EnergyTransferMode::AC_two_phase);
                [[fallthrough]];
            case 1:
                transfer_modes.push_back(types::iso15118::EnergyTransferMode::AC_single_phase_core);
                break;
            default:
                break;
            }

            update_supported_energy_transfers(types::iso15118::EnergyTransferMode::AC_three_phase_core);

            if (config.supported_iso_ac_bpt and hw_capabilities.max_current_A_export > 0 and
                hw_capabilities.max_phase_count_export >= 1) {
                transfer_modes.push_back({types::iso15118::EnergyTransferMode::AC_BPT});
                update_supported_energy_transfers(types::iso15118::EnergyTransferMode::AC_BPT);
            }

            r_hlc[0]->subscribe_ac_eamount([this](double e) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_ac_eamount);
                ev_info.remaining_energy_needed = e;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_max_voltage([this](double v) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_max_voltage);
                ev_info.maximum_voltage_limit = v;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_max_current([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_max_current);
                ev_info.maximum_current_limit = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_min_current([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_min_current);
                ev_info.minimum_current_limit = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_power_limits([this](types::iso15118::AcEvPowerLimits l) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_power_limits);
                if (l.max_charge_power.has_value()) {
                    ev_info.ac_max_charge_power = l.max_charge_power.value().total;
                }
                if (l.min_charge_power.has_value()) {
                    ev_info.ac_min_charge_power = l.min_charge_power.value().total;
                }
                if (l.max_discharge_power.has_value()) {
                    ev_info.ac_max_discharge_power = l.max_discharge_power.value().total;
                }
                if (l.min_discharge_power.has_value()) {
                    ev_info.ac_min_discharge_power = l.min_discharge_power.value().total;
                }
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_present_powers([this](types::iso15118::AcEvPresentPowerValues values) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_present_powers);
                ev_info.ac_present_active_power = values.present_active_power.total;
                if (values.present_reactive_power.has_value()) {
                    ev_info.ac_present_reactive_power = values.present_reactive_power.value().total;
                }
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_ac_ev_dynamic_control_mode([this](types::iso15118::AcEvDynamicModeValues values) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_ac_ev_dynamic_control_mode);
                ev_info.target_energy_request = values.target_energy_request;
                ev_info.max_energy_request = values.max_energy_request;
                ev_info.min_energy_request = values.min_energy_request;
                if (values.departure_time.has_value()) {
                    // TODO(SL): Convert departure_time to rfc3339
                    // ev_info.departure_time = values.departure_time.value();
                }
                // TODO(SL): Missing max_v2x_energy_request & min_v2x_energy_request
                p_evse->publish_ev_info(ev_info);
            });

        } else if (config.charge_mode == "DC") {
            if (connector_type.has_value() and connector_type.value() == types::evse_manager::ConnectorTypeEnum::cMCS) {
                transfer_modes.push_back(types::iso15118::EnergyTransferMode::MCS);
                update_supported_energy_transfers(types::iso15118::EnergyTransferMode::MCS);
            } else {
                transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_extended);
                update_supported_energy_transfers(types::iso15118::EnergyTransferMode::DC);
            }

            const auto caps = get_powersupply_capabilities();
            update_powersupply_capabilities(caps);

            if (caps.bidirectional) {
                if (connector_type.has_value() and
                    connector_type.value() == types::evse_manager::ConnectorTypeEnum::cMCS) {
                    transfer_modes.push_back(types::iso15118::EnergyTransferMode::MCS_BPT);
                    update_supported_energy_transfers(types::iso15118::EnergyTransferMode::MCS_BPT);
                } else {
                    transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_BPT);
                    update_supported_energy_transfers(types::iso15118::EnergyTransferMode::DC_BPT);
                }
            }

            // Set present measurements on HLC to sane defaults
            types::iso15118::DcEvsePresentVoltageCurrent present_values;
            present_values.evse_present_voltage = 0;
            present_values.evse_present_current = 0;
            r_hlc[0]->call_update_dc_present_values(present_values);

            // Cable check for DC charging
            r_hlc[0]->subscribe_start_cable_check([this] {
                power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::CableCheck;
                cable_check();
            });

            // Cable check for DC charging
            r_hlc[0]->subscribe_start_pre_charge(
                [this] { power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::PreCharge; });

            // Notification that current demand has started
            r_hlc[0]->subscribe_current_demand_started([this] {
                power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Charging;
                current_demand_active = true;
                apply_new_target_voltage_current();
                charger->notify_currentdemand_started();
                if (not r_over_voltage_monitor.empty()) {
                    r_over_voltage_monitor[0]->call_start();
                }
                if (internal_over_voltage_monitor) {
                    internal_over_voltage_monitor->reset();
                    internal_over_voltage_monitor->start_monitor();
                }
            });

            r_hlc[0]->subscribe_current_demand_finished([this] {
                current_demand_active = false;
                sae_bidi_active = false;
                if (not r_over_voltage_monitor.empty()) {
                    r_over_voltage_monitor[0]->call_stop();
                }
                if (internal_over_voltage_monitor) {
                    internal_over_voltage_monitor->stop_monitor();
                }
            });

            // Subscribe to voltage measurements from over_voltage_monitor interface
            // The internal monitor acts as a software watchdog following the hardware OVM values
            if (not r_over_voltage_monitor.empty()) {
                r_over_voltage_monitor[0]->subscribe_voltage_measurement_V([this](float voltage_V) {
                    if (internal_over_voltage_monitor) {
                        internal_over_voltage_monitor->update_voltage(voltage_V);
                    }
                });
            }

            // Isolation monitoring for DC charging handler
            if (not r_imd.empty()) {

                imd_stop();

                r_imd[0]->subscribe_isolation_measurement([this](types::isolation_monitor::IsolationMeasurement m) {
                    // new DC isolation monitoring measurement received

                    // Check for isolation errors
                    if (charger->get_current_state() == Charger::EvseState::Charging and
                        not check_isolation_resistance_in_range(m.resistance_F_Ohm)) {
                        error_handling->raise_isolation_resistance_fault(
                            fmt::format("Isolation resistance too low during charging: {} Ohm", m.resistance_F_Ohm),
                            "Resistance");
                    }
                    // Check L1e and L2e to PE as defined by IEC 61851-23:2023, $6.3.1.112.2
                    if (charger->get_current_state() == Charger::EvseState::Charging) {
                        bool is_in_range = check_voltage_to_protective_earth_in_range(m);
                        auto now = std::chrono::steady_clock::now();

                        if (not is_in_range) {
                            // Voltage to earth is out of range
                            if (voltage_to_earth_failure_count == 0) {
                                // First failure - record the time
                                first_voltage_to_earth_failure_time = now;
                                voltage_to_earth_failure_count = 1;
                                EVLOG_debug << "Voltage to earth out of range (first failure)";
                            } else {
                                // Subsequent failure - increment count
                                voltage_to_earth_failure_count++;
                                auto time_since_first_failure = std::chrono::duration_cast<std::chrono::seconds>(
                                    now - first_voltage_to_earth_failure_time);

                                EVLOG_debug << "Voltage to earth out of range. Failure count: "
                                            << voltage_to_earth_failure_count
                                            << ", time since first: " << time_since_first_failure.count() << "s";

                                // Only raise error if we have at least 2 failures AND at least 2 seconds between first
                                // and last
                                if (voltage_to_earth_failure_count >= REQUIRED_CONSECUTIVE_FAILURES and
                                    time_since_first_failure >= MIN_TIME_BETWEEN_FIRST_AND_LAST_FAILURE) {
                                    error_handling->raise_isolation_resistance_fault(
                                        fmt::format("Voltage to earth too high during charging L1e: {} V, L2e: {} V",
                                                    m.voltage_to_earth_l1e_V.value_or(NAN),
                                                    m.voltage_to_earth_l2e_V.value_or(NAN)),
                                        "VoltageToEarth");
                                }
                            }
                        } else {
                            // Voltage to earth is in range - reset failure tracking
                            if (voltage_to_earth_failure_count > 0) {
                                EVLOG_debug << "Voltage to earth back in range, resetting failure counter";
                                voltage_to_earth_failure_count = 0;
                            }
                        }
                    }
                    isolation_measurement = m;
                });

                r_imd[0]->subscribe_self_test_result([this](bool result) {
                    session_log.evse(false, fmt::format("Isolation monitor self test result: {}", result));
                    selftest_result = result;
                });
            }

            // On start up, stop ovm in case it is still enabled from a previous run
            if (not r_over_voltage_monitor.empty()) {
                r_over_voltage_monitor[0]->call_stop();
            }

            // Get voltage/current from DC power supply
            if (not r_powersupply_DC.empty()) {
                r_powersupply_DC[0]->subscribe_voltage_current([this](types::power_supply_DC::VoltageCurrent m) {
                    powersupply_measurement = m;
                    types::iso15118::DcEvsePresentVoltageCurrent present_values;
                    present_values.evse_present_voltage = (m.voltage_V > 0 ? m.voltage_V : 0.0);
                    present_values.evse_present_current = m.current_A;

                    if (config.hack_present_current_offset > 0) {
                        const auto current_offset = std::fabs(present_values.evse_present_current.value()) +
                                                    static_cast<float>(config.hack_present_current_offset);
                        present_values.evse_present_current = (m.current_A >= 0) ? current_offset : -current_offset;
                    }

                    if (config.hack_pause_imd_during_precharge and m.voltage_V * std::fabs(m.current_A) > 1000) {
                        // Start IMD again as it was stopped after CableCheck
                        imd_start();
                        EVLOG_info << "Hack: Restarting Isolation Measurement at " << m.voltage_V << " " << m.current_A;
                    }

                    r_hlc[0]->call_update_dc_present_values(present_values);

                    {
                        // dont publish ev_info here, it will be published when other values change.
                        // otherwise we will create too much traffic on mqtt
                        Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_set_ev_info);
                        ev_info.present_voltage = present_values.evse_present_voltage;
                        ev_info.present_current = present_values.evse_present_current;
                        // p_evse->publish_ev_info(ev_info);
                    }
                });
            }

            // Car requests a target voltage and current limit
            r_hlc[0]->subscribe_dc_ev_target_voltage_current(
                [this](types::iso15118::DcEvTargetValues v) {
                    bool target_changed = false;

                    // Hack for Skoda Enyaq that should be fixed in a different way
                    if (config.hack_skoda_enyaq and (v.dc_ev_target_voltage < 300 or v.dc_ev_target_current < 0))
                        return;

                    // Limit voltage/current for broken EV implementations
                    const auto ev = get_ev_info();
                    if (ev.maximum_current_limit.has_value() and
                        v.dc_ev_target_current > ev.maximum_current_limit.value()) {
                        v.dc_ev_target_current = ev.maximum_current_limit.value();
                    }

                    if (ev.maximum_voltage_limit.has_value() and
                        v.dc_ev_target_voltage > ev.maximum_voltage_limit.value()) {
                        v.dc_ev_target_voltage = ev.maximum_voltage_limit.value();
                    }

                    bool car_breaks_limit{false};
                    const auto hlc_limits = charger->get_evse_max_hlc_limits();
                    if (v.dc_ev_target_current > hlc_limits.evse_maximum_current_limit) {
                        v.dc_ev_target_current = hlc_limits.evse_maximum_current_limit;
                        car_breaks_limit = true;
                    }

                    const auto actual_voltage =
                        ev_info.present_voltage.has_value() ? ev_info.present_voltage.value() : v.dc_ev_target_voltage;

                    const auto target_power = v.dc_ev_target_current * actual_voltage;
                    if (target_power > hlc_limits.evse_maximum_power_limit) {
                        v.dc_ev_target_current = hlc_limits.evse_maximum_power_limit / actual_voltage;
                        car_breaks_limit = true;
                    }

                    if (v.dc_ev_target_voltage not_eq latest_target_voltage or
                        v.dc_ev_target_current not_eq latest_target_current) {
                        latest_target_voltage = v.dc_ev_target_voltage;
                        latest_target_current = v.dc_ev_target_current;
                        target_changed = true;
                    }

                    if (target_changed) {
                        apply_new_target_voltage_current();
                        if (not contactor_open) {
                            powersupply_DC_on();
                        }
                        if (car_breaks_limit) {
                            EVLOG_warning
                                << "EV ignores new EVSE max limits. Setting target current to new EVSE max limits";
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

            r_hlc[0]->subscribe_d20_dc_dynamic_charge_mode([this](types::iso15118::DcChargeDynamicModeValues values) {
                static bool last_is_actually_exporting_to_grid{false};

                const auto energy_flow_changed = is_actually_exporting_to_grid != last_is_actually_exporting_to_grid;
                last_is_actually_exporting_to_grid = is_actually_exporting_to_grid;

                bool target_changed{false};

                double min_charge_power{0.0};
                double max_charge_power{0.0};
                double max_charge_current{0.0};

                double target_current{0.0};
                double target_voltage{0.0};

                const auto max_hlc_limits = charger->get_evse_max_hlc_limits();
                const auto min_hlc_limits = charger->get_evse_min_hlc_limits();

                // TODO(SL): How to handle 0kW step between switching from uni to bidi
                if (is_actually_exporting_to_grid and current_demand_active) {
                    if (values.max_discharge_power.has_value() and
                        max_hlc_limits.evse_maximum_discharge_power_limit.has_value()) {
                        // fabs every discharge limit
                        const auto ev_max_discharge_power = std::fabs(values.max_discharge_power.value());
                        const auto evse_max_discharge_power =
                            std::fabs(max_hlc_limits.evse_maximum_discharge_power_limit.value());
                        max_charge_power = (ev_max_discharge_power > evse_max_discharge_power)
                                               ? evse_max_discharge_power
                                               : ev_max_discharge_power;
                    }

                    if (values.min_discharge_power.has_value() and
                        min_hlc_limits.evse_minimum_discharge_power_limit.has_value()) {
                        const auto ev_min_discharge_power = std::fabs(values.min_discharge_power.value());
                        const auto evse_min_discharge_power =
                            std::fabs(min_hlc_limits.evse_minimum_discharge_power_limit.value());
                        min_charge_power = (ev_min_discharge_power < evse_min_discharge_power)
                                               ? evse_min_discharge_power
                                               : ev_min_discharge_power;
                    }

                    if (values.max_discharge_current.has_value() and
                        max_hlc_limits.evse_maximum_discharge_current_limit.has_value()) {
                        const auto ev_max_discharge_current = std::fabs(values.max_discharge_current.value());
                        const auto evse_max_discharge_current =
                            std::fabs(max_hlc_limits.evse_maximum_discharge_current_limit.value());
                        max_charge_current = (ev_max_discharge_current > evse_max_discharge_current)
                                                 ? evse_max_discharge_current
                                                 : ev_max_discharge_current;
                    }
                } else {
                    // Charging
                    max_charge_power = (values.max_charge_power > max_hlc_limits.evse_maximum_power_limit)
                                           ? max_hlc_limits.evse_maximum_power_limit
                                           : values.max_charge_power;
                    min_charge_power = (values.min_charge_power > min_hlc_limits.evse_minimum_power_limit)
                                           ? values.min_charge_power
                                           : min_hlc_limits.evse_minimum_power_limit;
                    max_charge_current = (values.max_charge_current > max_hlc_limits.evse_maximum_current_limit)
                                             ? max_hlc_limits.evse_maximum_current_limit
                                             : values.max_charge_current;
                }

                if (min_charge_power > max_charge_power) {
                    EVLOG_error << "Minimum charge power limit is greater then the maximum charge power limit";
                    return;
                }

                // Setting voltage. charging: EvMaxVoltage, discharging: EvMinVoltage
                target_voltage = (is_actually_exporting_to_grid)
                                     ? std::max(values.min_voltage, min_hlc_limits.evse_minimum_voltage_limit)
                                     : values.max_voltage;

                // Setting  power to actual voltage on the powermeter
                const auto actual_voltage = ev_info.present_voltage.value_or(latest_target_voltage);

                target_current = (actual_voltage <= 0.0)
                                     ? max_charge_current
                                     : std::min((max_charge_power / actual_voltage), max_charge_current);
                if (not(almost_eq(target_voltage, latest_target_voltage) and
                        almost_eq(target_current, latest_target_current))) {
                    latest_target_current = target_current;
                    latest_target_voltage = target_voltage;
                    target_changed = true;
                }

                if (target_changed or energy_flow_changed) {
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
            r_hlc[0]->subscribe_dc_open_contactor([this] {
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
            r_hlc[0]->subscribe_current_demand_finished([this] { powersupply_DC_off(); });

            r_hlc[0]->subscribe_dc_ev_maximum_limits([this](types::iso15118::DcEvMaximumLimits l) {
                EVLOG_info << "Received EV maximum limits: " << l;
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_dc_ev_maximum_limits);

                ev_info.maximum_current_limit = l.dc_ev_maximum_current_limit;
                ev_info.maximum_power_limit = l.dc_ev_maximum_power_limit;
                ev_info.maximum_voltage_limit = l.dc_ev_maximum_voltage_limit;
                p_evse->publish_ev_info(ev_info);

                // Update limits for over voltage monitoring
                if (not r_over_voltage_monitor.empty()) {
                    r_over_voltage_monitor[0]->call_set_limits(get_emergency_over_voltage_threshold(),
                                                               get_error_over_voltage_threshold());
                }
                if (internal_over_voltage_monitor) {
                    internal_over_voltage_monitor->set_limits(get_emergency_over_voltage_threshold(),
                                                              get_error_over_voltage_threshold());
                }
            });

            r_hlc[0]->subscribe_departure_time([this](const std::string& t) {
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_departure_time);
                ev_info.departure_time = t;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_ev_energy_capacity([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_dc_ev_energy_capacity);
                ev_info.battery_capacity = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_ev_energy_request([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_dc_ev_energy_request);
                ev_info.remaining_energy_needed = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_full_soc([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_dc_full_soc);
                ev_info.battery_full_soc = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_bulk_soc([this](double c) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_dc_bulk_soc);
                ev_info.battery_bulk_soc = c;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_ev_remaining_time([this](types::iso15118::DcEvRemainingTime t) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_dc_ev_remaining_time);
                ev_info.estimated_time_full = t.ev_remaining_time_to_full_soc;
                ev_info.estimated_time_bulk = t.ev_remaining_time_to_full_bulk_soc;
                p_evse->publish_ev_info(ev_info);
            });

            r_hlc[0]->subscribe_dc_ev_status([this](types::iso15118::DcEvStatus s) {
                // FIXME send only on change / throttle messages
                Everest::scoped_lock_timeout lock(ev_info_mutex,
                                                  Everest::MutexDescription::EVSE_subscribe_dc_ev_status);
                ev_info.soc = s.dc_ev_ress_soc;
                p_evse->publish_ev_info(ev_info);
            });

            // SAE J2847/2 Bidi
            if (config.sae_j2847_2_bpt_enabled == true) {

                sae_mode = types::iso15118::string_to_sae_j2847bidi_mode(config.sae_j2847_2_bpt_mode);

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

            // ac_close_contactor
            // ac_open_contactor

            // selected_payment_option
            // requested_energy_transfer_mode

            // EV_ChargingSession
            // dc_bulk_charging_complete
            // dc_charging_complete
        } else {
            EVLOG_error << "Unsupported charging mode.";
            exit(255);
        }

        r_hlc[0]->subscribe_selected_service_parameters([this](types::iso15118::SelectedServiceParameters parameters) {
            selected_d20_energy_service.emplace(parameters.energy_transfer);

            session_log.car(true, fmt::format("EV selected service: {}",
                                              types::iso15118::service_category_to_string(parameters.energy_transfer)));
        });

        r_hlc[0]->call_receipt_is_required(config.ev_receipt_required);

        r_hlc[0]->call_setup(evseid, sae_mode, config.session_logging);
        r_hlc[0]->call_update_energy_transfer_modes(transfer_modes);

        if ((config.bpt_channel == "Unified" or config.bpt_channel == "Separated") and
            (config.bpt_generator_mode == "GridFollowing" or config.bpt_generator_mode == "GridForming")) {
            types::iso15118::BptSetup bpt_setup_config{};

            bpt_setup_config.bpt_channel = config.bpt_channel == "Unified" ? types::iso15118::BptChannel::Unified
                                                                           : types::iso15118::BptChannel::Separated;
            bpt_setup_config.generator_mode = config.bpt_generator_mode == "GridFollowing"
                                                  ? types::iso15118::GeneratorMode::GridFollowing
                                                  : types::iso15118::GeneratorMode::GridForming;

            if (config.bpt_grid_code_island_method == "Active" or config.bpt_grid_code_island_method == "Passive") {
                bpt_setup_config.grid_code_detection = config.bpt_grid_code_island_method == "Active"
                                                           ? types::iso15118::GridCodeIslandingDetectionMethod::Active
                                                           : types::iso15118::GridCodeIslandingDetectionMethod::Passive;
            }
            r_hlc[0]->call_bpt_setup(bpt_setup_config);
        }
        // reset error flags
        r_hlc[0]->call_reset_error();

        // implement Auth handlers
        r_hlc[0]->subscribe_require_auth_eim([this]() {
            //  Do we have auth already (i.e. delayed HLC after charging already running)?
            if ((config.dbg_hlc_auth_after_tstep and charger->get_authorized_eim_ready_for_hlc()) or
                (not config.dbg_hlc_auth_after_tstep and charger->get_authorized_eim())) {
                hlc_waiting_for_auth_eim = false;
                hlc_waiting_for_auth_pnc = false;
                r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                                      types::authorization::CertificateStatus::NoCertificateAvailable);
                charger->get_stopwatch().mark("Auth EIM Done");
            } else {
                if (config.enable_autocharge) {
                    p_token_provider->publish_provided_token(autocharge_token);
                }
                hlc_waiting_for_auth_eim = true;
                hlc_waiting_for_auth_pnc = false;
            }
        });

        if (not config.autocharge_use_slac_instead_of_hlc) {
            r_hlc[0]->subscribe_evcc_id([this](const std::string& token) {
                autocharge_token = create_autocharge_token(token, config.connector_id);
                car_manufacturer = get_manufacturer_from_mac(token);
                p_evse->publish_car_manufacturer(car_manufacturer);

                {
                    Everest::scoped_lock_timeout lock(ev_info_mutex, Everest::MutexDescription::EVSE_subscribe_evcc_id);
                    ev_info.evcc_id = token;
                    p_evse->publish_ev_info(ev_info);
                }
            });
        }

        r_hlc[0]->subscribe_require_auth_pnc([this](types::authorization::ProvidedIdToken _token) {
            // Do we have auth already (i.e. delayed HLC after charging already running)?

            std::vector<int> referenced_connectors = {this->config.connector_id};
            _token.connectors.emplace(referenced_connectors);

            if (charger->get_authorized_pnc()) {
                hlc_waiting_for_auth_eim = false;
                hlc_waiting_for_auth_pnc = false;
            } else {
                // only publish token if we are not authorized yet. This enables pause/resume with PnC.
                p_token_provider->publish_provided_token(_token);
                hlc_waiting_for_auth_eim = false;
                hlc_waiting_for_auth_pnc = true;
            }
        });

        // Install debug V2G Messages handler if session logging is enabled
        if (config.session_logging) {
            r_hlc[0]->subscribe_v2g_messages(
                [this](types::iso15118::V2gMessages v2g_messages) { log_v2g_message(v2g_messages); });

            r_hlc[0]->subscribe_selected_protocol(
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

            r_hlc[0]->subscribe_dc_ev_status([this](types::iso15118::DcEvStatus status) {
                EVLOG_info << fmt::format("SoC received: {}.", status.dc_ev_ress_soc);
                switch_AC_mode();
            });
        }
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
                // Make a local copy as leave_bcd() will overwrite the slac_unmatched flag
                bool unmatched_on_unplug = not slac_unmatched;
                r_slac[0]->call_leave_bcd();
                if (unmatched_on_unplug) {
                    r_slac[0]->call_reset(false);
                }
                hlc_waiting_for_auth_pnc = false;
                hlc_waiting_for_auth_eim = false;
            }
        }

        if (not r_over_voltage_monitor.empty() and event == CPEvent::CarUnplugged) {
            r_over_voltage_monitor[0]->call_reset_over_voltage_error();
        }
        if (internal_over_voltage_monitor and event == CPEvent::CarUnplugged) {
            internal_over_voltage_monitor->stop_monitor();
            internal_over_voltage_monitor->reset();
        }

        charger->bsp_event_queue.push(event);

        // Forward some events to HLC
        if (hlc_enabled) {
            // Reset HLC auth waiting flags on new session
            if (event == CPEvent::CarPluggedIn) {
                r_hlc[0]->call_reset_error();
                r_hlc[0]->call_ac_contactor_closed(false);
                r_hlc[0]->call_stop_charging(false);
                latest_target_voltage = 0;
                latest_target_current = 0;
                hlc_waiting_for_auth_eim = false;
                hlc_waiting_for_auth_pnc = false;
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
    r_bsp->subscribe_ac_pp_ampacity(
        [this](types::board_support_common::ProximityPilot pp) { bsp->set_pp_ampacity(pp); });

    if (r_powermeter_billing().size() > 0) {
        r_powermeter_billing()[0]->subscribe_powermeter([this](types::powermeter::Powermeter p) {
            // Inform charger about current charging current. This is used for slow OC detection.
            if (p.current_A and p.current_A.value().L1 and p.current_A.value().L2 and p.current_A.value().L3) {
                charger->set_current_drawn_by_vehicle(p.current_A.value().L1.value(), p.current_A.value().L2.value(),
                                                      p.current_A.value().L3.value());
            }

            // Inform HLC about the power meter data
            if (hlc_enabled) {
                r_hlc[0]->call_update_meter_info(p);

                if (p.power_W and selected_d20_energy_service.has_value()) {
                    r_hlc[0]->call_update_ac_present_power(p.power_W.value());
                }
            }

            // Store local cache
            {
                Everest::scoped_lock_timeout lock(power_mutex, Everest::MutexDescription::EVSE_subscribe_powermeter);
                latest_powermeter_data_billing = p;
            }

            {
                std::scoped_lock<std::mutex> lk(powermeter_mutex);
                initial_powermeter_value_received = true;
            }
            powermeter_cv.notify_one();

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

            if (p.energy_Wh_export.has_value()) {
                mqtt.publish(
                    fmt::format("everest_external/nodered/{}/powermeter/totalExportKWattHr", config.connector_id),
                    p.energy_Wh_export.value().total / 1000.);
            }

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
                slac_unmatched = true;
            } else {
                charger->set_matching_started(true);
                slac_unmatched = false;
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
                charger->get_stopwatch().mark("D-LINK_READY");
            }
        });
    }

    charger->signal_max_current.connect([this](float ampere) {
        // The charger changed the max current setting. Forward to HLC
        if (hlc_enabled) {
            if (not selected_d20_energy_service.has_value() and ampere >= 0.0) {
                r_hlc[0]->call_update_ac_max_current(ampere); // ISO-2
                return;
            }

            if (selected_d20_energy_service.value() == types::iso15118::ServiceCategory::AC or
                selected_d20_energy_service.value() == types::iso15118::ServiceCategory::AC_BPT) {

                types::units::Power target_power = {ampere * static_cast<float>(config.ac_nominal_voltage) *
                                                    hw_capabilities.max_phase_count_import};

                // TODO(SL): Adding target frequency
                // TODO(SL): Adding reactive power
                r_hlc[0]->call_update_ac_target_values({target_power});
            }
        }
    });

    // Cancel reservations if charger is faulted
    error_handling->signal_error.connect([this](ErrorHandlingEvents event) {
        if (event == ErrorHandlingEvents::ForceEmergencyShutdown or event == ErrorHandlingEvents::ForceErrorShutdown) {
            cancel_reservation(true);
        }
    });

    charger->signal_simple_event.connect([this](types::evse_manager::SessionEventEnum s) {
        if (s == types::evse_manager::SessionEventEnum::SessionFinished) {
            // Reset EV information on Session start and end
            ev_info = types::evse_manager::EVInfo();
            p_evse->publish_ev_info(ev_info);
        }

        if (not hlc_enabled) {
            return;
        }

        if (s != types::evse_manager::SessionEventEnum::Authorized and
            s != types::evse_manager::SessionEventEnum::SessionFinished) {
            return;
        }

        std::vector<types::iso15118::PaymentOption> payment_options;
        // if pnc is disabled, disable contract installation and central contract validation
        bool _contract_certificate_installation_enabled =
            pnc_enabled ? contract_certificate_installation_enabled.load() : false;
        bool _central_contract_validation_allowed = pnc_enabled ? central_contract_validation_allowed.load() : false;

        if (config.payment_enable_eim) {
            payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
        }
        if (pnc_enabled and s == types::evse_manager::SessionEventEnum::SessionFinished) {
            // PnC is enabled and this is a SessionFinished event -> enable Contract payment option
            payment_options.push_back(types::iso15118::PaymentOption::Contract);
        } else {
            // We dont add contract if this is an Authorized event, as in this case the ISO15118 stack
            // should not offer the contract option and certifiate installation service.
            _contract_certificate_installation_enabled = false;
        }

        if (config.payment_enable_eim == false and pnc_enabled == false) {
            EVLOG_warning << "Both payment options are disabled! ExternalPayment is nevertheless enabled in this case.";
            payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
        }
        r_hlc[0]->call_session_setup(payment_options, _contract_certificate_installation_enabled,
                                     _central_contract_validation_allowed);
    });

    charger->signal_session_started_event.connect(
        [this](types::evse_manager::StartSessionReason start_reason,
               const std::optional<types::authorization::ProvidedIdToken>& provided_id_token) {
            // Reset EV information on Session start and end
            ev_info = types::evse_manager::EVInfo();
            p_evse->publish_ev_info(ev_info);

            if (not hlc_enabled) {
                return;
            }

            std::vector<types::iso15118::PaymentOption> payment_options;
            // if pnc is disabled, disable contract installation and central contract validation
            bool _contract_certificate_installation_enabled =
                pnc_enabled ? contract_certificate_installation_enabled.load() : false;
            bool _central_contract_validation_allowed =
                pnc_enabled ? central_contract_validation_allowed.load() : false;

            if (start_reason == types::evse_manager::StartSessionReason::Authorized) {
                // Session is already authorized, only use ExternalPayment in PaymentOptions
                payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
                _contract_certificate_installation_enabled = false;
                _central_contract_validation_allowed = false;
            } else {
                // Set payment options according to configuration
                if (config.payment_enable_eim) {
                    payment_options.push_back(types::iso15118::PaymentOption::ExternalPayment);
                }
                if (pnc_enabled) {
                    payment_options.push_back(types::iso15118::PaymentOption::Contract);
                }
            }
            r_hlc[0]->call_session_setup(payment_options, _contract_certificate_installation_enabled,
                                         _central_contract_validation_allowed);
        });

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);
    invoke_ready(*p_token_provider);
    invoke_ready(*p_random_delay);

    if (config.ac_with_soc) {
        setup_fake_DC_mode();
    } else {
        charger->setup(config.has_ventilation,
                       (config.charge_mode == "DC" ? Charger::ChargeMode::DC : Charger::ChargeMode::AC), hlc_enabled,
                       config.ac_hlc_use_5percent, config.ac_enforce_hlc, false,
                       config.soft_over_current_tolerance_percent, config.soft_over_current_measurement_noise_A,
                       config.switch_3ph1ph_delay_s, config.switch_3ph1ph_cp_state, config.soft_over_current_timeout_ms,
                       config.state_F_after_fault_ms, config.fail_on_powermeter_errors, config.raise_mrec9,
                       config.sleep_before_enabling_pwm_hlc_mode_ms,
                       utils::get_session_id_type_from_string(config.session_id_type));
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

    {
        // wait for first powermeter value
        std::unique_lock<std::mutex> lk(powermeter_mutex);
        this->powermeter_cv.wait_for(lk, std::chrono::milliseconds(this->config.initial_meter_value_timeout_ms),
                                     [this] { return initial_powermeter_value_received; });
    }

    // Resuming left-over transaction from e.g. powerloss. This information allows other modules like to OCPP to be
    // informed that the EvseManager is aware of previous sessions so that no individual cleanup is required
    const auto session_id = store->get_session();
    if (!session_id.empty()) {
        charger->signal_session_resumed_event(session_id);
    }

    // By default cleanup left-over transaction from e.g. power loss
    // TOOD: Add resume handling
    charger->cleanup_transactions_on_startup();

    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw
    //  caps.
    charger->set_max_current(0.0F, steady_clock::now() + std::chrono::seconds(120));
    this->p_evse->publish_waiting_for_external_ready(config.external_ready_to_start_charging);
    if (not config.external_ready_to_start_charging) {
        // immediately ready, otherwise delay until we get the external signal
        this->ready_to_start_charging();
    }
}

void EvseManager::ready_to_start_charging() {
    Everest::scoped_lock_timeout lock(charger_ready_mutex, Everest::MutexDescription::EVSE_charger_ready);
    if (charger_ready) {
        EVLOG_warning << "Already ready to start charging!";
        return;
    }
    charger_ready = true;

    this->p_evse->publish_supported_energy_transfer_modes(supported_energy_transfers);

    timepoint_ready_for_charging = std::chrono::steady_clock::now();
    charger->run();

    // this will publish a session event Enabled or Disabled that allows other modules the retrieve this state on
    // startup
    charger->enable_disable_initial_state_publish();

    this->p_evse->publish_ready(true);
    EVLOG_info << fmt::format(fmt::emphasis::bold | fg(fmt::terminal_color::green), "🌀🌀🌀 Ready to start charging 🌀🌀🌀");
    if (!initial_powermeter_value_received) {
        EVLOG_warning << "No powermeter value received yet!";
    }
}

types::powermeter::Powermeter EvseManager::get_latest_powermeter_data_billing() {
    Everest::scoped_lock_timeout lock(power_mutex, Everest::MutexDescription::EVSE_get_latest_powermeter_data_billing);
    return latest_powermeter_data_billing;
}

types::evse_board_support::HardwareCapabilities EvseManager::get_hw_capabilities() {
    std::scoped_lock lock(hw_caps_mutex);
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
    charger->setup(config.has_ventilation, Charger::ChargeMode::DC, hlc_enabled, config.ac_hlc_use_5percent,
                   config.ac_enforce_hlc, false, config.soft_over_current_tolerance_percent,
                   config.soft_over_current_measurement_noise_A, config.switch_3ph1ph_delay_s,
                   config.switch_3ph1ph_cp_state, config.soft_over_current_timeout_ms, config.state_F_after_fault_ms,
                   config.fail_on_powermeter_errors, config.raise_mrec9, config.sleep_before_enabling_pwm_hlc_mode_ms,
                   utils::get_session_id_type_from_string(config.session_id_type));

    types::iso15118::EVSEID evseid = {config.evse_id, config.evse_id_din};

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    std::vector<types::iso15118::EnergyTransferMode> transfer_modes;

    transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_core);
    transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_extended);
    transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_combo_core);
    transfer_modes.push_back(types::iso15118::EnergyTransferMode::DC_unique);

    types::iso15118::DcEvsePresentVoltageCurrent present_values;
    present_values.evse_present_voltage = 400; // FIXME: set a correct values
    present_values.evse_present_current = 0;

    r_hlc[0]->call_update_dc_present_values(present_values);

    types::iso15118::DcEvseMaximumLimits evse_max_limits;
    evse_max_limits.evse_maximum_current_limit = 400;
    evse_max_limits.evse_maximum_power_limit = 200000;
    evse_max_limits.evse_maximum_voltage_limit = 1000;
    r_hlc[0]->call_update_dc_maximum_limits(evse_max_limits);

    types::iso15118::DcEvseMinimumLimits evse_min_limits;
    evse_min_limits.evse_minimum_current_limit = 0;
    evse_min_limits.evse_minimum_voltage_limit = 0;
    r_hlc[0]->call_update_dc_minimum_limits(evse_min_limits);

    constexpr auto sae_mode = types::iso15118::SaeJ2847BidiMode::None;

    r_hlc[0]->call_setup(evseid, sae_mode, config.session_logging);
    r_hlc[0]->call_update_energy_transfer_modes(transfer_modes);
}

void EvseManager::setup_AC_mode() {
    charger->setup(config.has_ventilation, Charger::ChargeMode::AC, hlc_enabled, config.ac_hlc_use_5percent,
                   config.ac_enforce_hlc, true, config.soft_over_current_tolerance_percent,
                   config.soft_over_current_measurement_noise_A, config.switch_3ph1ph_delay_s,
                   config.switch_3ph1ph_cp_state, config.soft_over_current_timeout_ms, config.state_F_after_fault_ms,
                   config.fail_on_powermeter_errors, config.raise_mrec9, config.sleep_before_enabling_pwm_hlc_mode_ms,
                   utils::get_session_id_type_from_string(config.session_id_type));

    types::iso15118::EVSEID evseid = {config.evse_id, config.evse_id_din};

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    std::vector<types::iso15118::EnergyTransferMode> transfer_modes;

    transfer_modes.push_back(types::iso15118::EnergyTransferMode::AC_single_phase_core);

    if (get_hw_capabilities().max_phase_count_import == 3) {
        transfer_modes.push_back(types::iso15118::EnergyTransferMode::AC_three_phase_core);
    }

    types::iso15118::SetupPhysicalValues setup_physical_values;

    constexpr auto sae_mode = types::iso15118::SaeJ2847BidiMode::None;

    if (hlc_enabled) {
        r_hlc[0]->call_setup(evseid, sae_mode, config.session_logging);
        r_hlc[0]->call_update_energy_transfer_modes(transfer_modes);
    }
}

void EvseManager::setup_v2h_mode() {

    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;
    target_entry.limits_to_leaves.total_power_W = {powersupply_capabilities.max_import_power_W.value_or(0.),
                                                   info.id + "/setup_v2h_mode"};

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = {0};

    external_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, target_entry);
    external_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, zero_entry);

    update_local_energy_limit(external_limits);
}

bool EvseManager::update_local_energy_limit(types::energy::ExternalLimits l) {
    std::scoped_lock lock(external_local_limits_mutex);
    external_local_energy_limits = l;
    // wait for EnergyManager to assign optimized current on next opimizer run
    return true;
}

// Helper function to set a watt limit in an ExternalLimits type
bool EvseManager::update_max_watt_limit(types::energy::ExternalLimits& limits, float max_watt_export,
                                        std::optional<float> max_watt_import) {
    types::energy::ScheduleReqEntry e;
    e.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    e.limits_to_leaves.total_power_W = {max_watt_export, info.id + " update_max_watt_limit"};
    limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);

    e.limits_to_leaves.total_power_W = {max_watt_import.value_or(0.0f), info.id + " update_max_watt_limit"};
    limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);

    return true;
}

void EvseManager::update_to_zero_discharge_limit(types::energy::ExternalLimits& limits) {
    types::energy::ScheduleReqEntry e;
    e.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    e.limits_to_leaves.ac_max_current_A = {0., info.id + " set_zero_discharge_limit"};
    e.limits_to_leaves.total_power_W = {0., info.id + " set_zero_discharge_limit"};
    limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);
}

// Helper function to set a current limit in an ExternalLimits type
bool EvseManager::update_max_current_limit(types::energy::ExternalLimits& limits, float max_current_import,
                                           float max_current_export) {
    if (config.charge_mode == "DC") {
        return false;
    }

    types::energy::ScheduleReqEntry e;
    e.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    e.limits_to_leaves.ac_max_current_A = {max_current_import, info.id + " update_max_current_limit"};
    limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, e);

    e.limits_to_leaves.ac_max_current_A = {max_current_export, info.id + " update_max_current_limit"};
    limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, e);

    return true;
}

bool EvseManager::reserve(int32_t id, const bool signal_reservation_event) {
    EVLOG_debug << "Reserve called for reservation id " << id
                << ", signal reservation event: " << signal_reservation_event;

    // is the evse Unavailable?
    if (charger->get_current_state() == Charger::EvseState::Disabled) {
        EVLOG_info << "Rejecting reservation because charger is disabled.";
        return false;
    }

    // is the evse faulted?
    if (charger->stop_charging_on_fatal_error()) {
        EVLOG_info << "Rejecting reservation because of a fatal error.";
        return false;
    }

    // is the connector currently ready to accept a new car?
    if (charger->get_current_state() not_eq Charger::EvseState::Idle) {
        EVLOG_info << "Rejecting reservation because evse is not idle";
        return false;
    }

    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_reserve);

    const bool overwrite_reservation = (this->reservation_id == id);

    if (reserved && this->reservation_id != -1) {
        EVLOG_info << "Rejecting reservation because evse is already reserved for reservation id "
                   << this->reservation_id;
    }

    // Check if this evse is not already reserved, or overwrite reservation if it is for the same reservation id.
    if (not reserved || this->reservation_id == -1 || overwrite_reservation) {
        EVLOG_debug << "Make the reservation with id " << id;
        reserved = true;
        if (id >= 0) {
            this->reservation_id = id;
        }

        // When overwriting the reservation, don't signal.
        if ((not overwrite_reservation || this->reservation_id == -1) && signal_reservation_event) {
            // publish event to other modules
            types::evse_manager::SessionEvent se;
            se.event = types::evse_manager::SessionEventEnum::ReservationStart;

            // Normally we should signal for each connector when an evse is reserved, but since in this implementation
            // each evse only has one connector, this is sufficient for now.
            signalReservationEvent(se);
        }

        return true;
    }

    return false;
}

void EvseManager::cancel_reservation(bool signal_event) {

    Everest::scoped_lock_timeout lock(reservation_mutex, Everest::MutexDescription::EVSE_cancel_reservation);
    if (reserved) {
        EVLOG_debug << "Reservation cancelled";
        reserved = false;
        this->reservation_id = -1;

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

bool EvseManager::get_hlc_waiting_for_auth_pnc() {
    return hlc_waiting_for_auth_pnc;
}

void EvseManager::set_pnc_enabled(const bool value) {
    pnc_enabled = value;
}

void EvseManager::set_central_contract_validation_allowed(const bool value) {
    central_contract_validation_allowed = value;
}

void EvseManager::set_contract_certificate_installation_enabled(const bool value) {
    contract_certificate_installation_enabled = value;
}

bool EvseManager::update_supported_energy_transfers(const types::iso15118::EnergyTransferMode& energy_transfer) {
    std::scoped_lock lock(supported_energy_transfers_mutex);
    bool was_updated = supported_energy_transfers.end() ==
                       std::find_if(supported_energy_transfers.begin(), supported_energy_transfers.end(),
                                    [energy_transfer](const auto& supported_energy_transfer) {
                                        return energy_transfer == supported_energy_transfer;
                                    });
    if (was_updated) {
        supported_energy_transfers.push_back(energy_transfer);
    }
    return was_updated;
}

void EvseManager::update_hlc_ac_parameters() {
    std::scoped_lock lock(hlc_ac_parameters_mutex);

    types::iso15118::AcEvseMaximumPower ac_maximum_power;
    const float max_charge_power_per_phase = config.ac_nominal_voltage * hw_capabilities.max_current_A_import;
    const float max_discharge_power_per_phase = config.ac_nominal_voltage * hw_capabilities.max_current_A_export;

    ac_maximum_power.charge_power = {
        max_charge_power_per_phase * hw_capabilities.max_phase_count_import,
        max_charge_power_per_phase,
        hw_capabilities.max_phase_count_import >= 2 ? std::make_optional(max_charge_power_per_phase) : std::nullopt,
        hw_capabilities.max_phase_count_import == 3 ? std::make_optional(max_charge_power_per_phase) : std::nullopt,
    };
    ac_maximum_power.discharge_power.emplace(types::units::Power{
        max_discharge_power_per_phase * hw_capabilities.max_phase_count_export,
        max_discharge_power_per_phase,
        hw_capabilities.max_phase_count_export >= 2 ? std::make_optional(max_discharge_power_per_phase) : std::nullopt,
        hw_capabilities.max_phase_count_export == 3 ? std::make_optional(max_discharge_power_per_phase) : std::nullopt,
    });
    r_hlc[0]->call_update_ac_maximum_limits(ac_maximum_power);

    types::iso15118::AcEvseMinimumPower ac_minimum_power;
    const float min_charge_power_per_phase = config.ac_nominal_voltage * hw_capabilities.min_current_A_import;
    const float min_discharge_power_per_phase = config.ac_nominal_voltage * hw_capabilities.min_current_A_export;

    ac_minimum_power.charge_power = {
        min_charge_power_per_phase * hw_capabilities.max_phase_count_import,
        min_charge_power_per_phase,
        hw_capabilities.max_phase_count_import >= 2 ? std::make_optional(min_charge_power_per_phase) : std::nullopt,
        hw_capabilities.max_phase_count_import == 3 ? std::make_optional(min_charge_power_per_phase) : std::nullopt,
    };

    ac_minimum_power.discharge_power.emplace(types::units::Power{
        min_discharge_power_per_phase * hw_capabilities.max_phase_count_export,
        min_discharge_power_per_phase,
        hw_capabilities.max_phase_count_export >= 2 ? std::make_optional(min_discharge_power_per_phase) : std::nullopt,
        hw_capabilities.max_phase_count_export == 3 ? std::make_optional(min_discharge_power_per_phase) : std::nullopt,
    });
    r_hlc[0]->call_update_ac_minimum_limits(ac_minimum_power);

    std::vector<types::iso15118::Connector> ac_connectors{types::iso15118::Connector::SinglePhase};
    if (hw_capabilities.max_phase_count_import == 3) {
        ac_connectors.push_back(types::iso15118::Connector::ThreePhase);
    }
    r_hlc[0]->call_update_ac_parameters({50, static_cast<float>(config.ac_nominal_voltage), ac_connectors, std::nullopt,
                                         std::nullopt}); // TODO(sl): Getting nominal frequency
}

void EvseManager::log_v2g_message(types::iso15118::V2gMessages v2g_messages) {

    const std::string msg = types::iso15118::v2g_message_id_to_string(v2g_messages.id);
    const std::string xml = v2g_messages.xml.value_or("");
    const std::string json_str = v2g_messages.v2g_json.value_or("");
    const std::string exi_hex = v2g_messages.exi.value_or("");
    const std::string exi_base64 = v2g_messages.exi_base64.value_or("");

    // All messages from EVSE contain Req and all originating from Car contain Res
    if (msg.find("Res") == std::string::npos) {
        session_log.car(true, fmt::format("V2G {}", msg), xml, exi_hex, exi_base64, json_str);
    } else {
        session_log.evse(true, fmt::format("V2G {}", msg), xml, exi_hex, exi_base64, json_str);
    }
}

void EvseManager::charger_was_authorized() {

    if (hlc_waiting_for_auth_pnc and charger->get_authorized_pnc()) {
        r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                              types::authorization::CertificateStatus::Accepted);
        hlc_waiting_for_auth_eim = false;
        hlc_waiting_for_auth_pnc = false;
        charger->get_stopwatch().mark("Auth PnC Done");
    }

    if (hlc_waiting_for_auth_eim and charger->get_authorized_eim()) {
        r_hlc[0]->call_authorization_response(types::authorization::AuthorizationStatus::Accepted,
                                              types::authorization::CertificateStatus::NoCertificateAvailable);
        hlc_waiting_for_auth_eim = false;
        hlc_waiting_for_auth_pnc = false;
        charger->get_stopwatch().mark("Auth EIM Done");
    }
}

static double get_cable_check_voltage(double ev_max_cpd, double evse_max_cpd) {
    double cable_check_voltage = 500;
    // IEC 61851-23 (2023) CC.4.1.2 / Formular CC.1
    if (ev_max_cpd <= 500) {
        if ((ev_max_cpd + 50) < cable_check_voltage) {
            cable_check_voltage = (ev_max_cpd + 50);
        }
        if (evse_max_cpd < cable_check_voltage) {
            cable_check_voltage = evse_max_cpd;
        }
    } else {
        cable_check_voltage = evse_max_cpd;
        if (1.1 * ev_max_cpd < cable_check_voltage) {
            cable_check_voltage = 1.1 * ev_max_cpd;
        }
    }

    return cable_check_voltage;
}

double EvseManager::get_emergency_over_voltage_threshold() {

    float ev_max_voltage = 500.;

    if (ev_info.maximum_voltage_limit.has_value()) {
        ev_max_voltage = ev_info.maximum_voltage_limit.value();
    } else {
        EVLOG_error << "OverVoltageThreshold: Did not receive EV maximum voltage, falling back to 500V";
    }

    auto evse_caps = get_powersupply_capabilities();

    double negotiated_max_voltage = std::min(ev_max_voltage, evse_caps.max_export_voltage_V);

    double ovp = 550.;

    if (negotiated_max_voltage > 1000) {
        // IEC 61851-23-3 (DRAFT 2025) Table 202
        ovp = 1375.;
    } else if (negotiated_max_voltage > 850) {
        // IEC 61851-23 (2023) 6.3.1.106.2 Table 103
        ovp = 1100.;
    } else if (negotiated_max_voltage > 750) {
        // IEC 61851-23 (2023) 6.3.1.106.2 Table 103
        ovp = 935.;
    } else if (negotiated_max_voltage > 500) {
        // IEC 61851-23 (2023) 6.3.1.106.2 Table 103
        ovp = 825.;
    }

    return ovp;
}

double EvseManager::get_error_over_voltage_threshold() {
    double ev_max_voltage = 500;

    if (ev_info.maximum_voltage_limit.has_value()) {
        ev_max_voltage = ev_info.maximum_voltage_limit.value();
    } else {
        EVLOG_error << "OverVoltageThreshold: Did not receive EV maximum voltage, falling back to 500V";
    }

    return ev_max_voltage;
}

bool EvseManager::cable_check_should_exit() {
    return charger->get_current_state() not_eq Charger::EvseState::PrepareCharging;
}

bool EvseManager::check_voltage_to_protective_earth_in_range(types::isolation_monitor::IsolationMeasurement m) {
    static constexpr double MAX_VOLTAGE_STATIC = 550.0; // defined by IEC 61851-23:2023, $6.3.1.112.2
    if (m.voltage_V.has_value() and m.voltage_to_earth_l1e_V.has_value() and m.voltage_to_earth_l2e_V.has_value()) {
        // This may trigger while ramping down voltage so we should try to check against the minium valid voltage the
        // the power supply can deliver. If the UL1e/2e is poorly synchronized to the actual voltage measurement it may
        // trigger false positives thus limiting the checks to min_export_voltage
        if (m.voltage_V.value() > powersupply_capabilities.min_export_voltage_V) {
            // implemented according to the standard see IEC 61851-23:2023, $6.3.1.112.2
            // if the charger rating is above 500, use as maximum value 110% of actual voltage
            // if the charger rating is below 500, use as maxiumum value 550V
            if (powersupply_capabilities.max_export_voltage_V > 500) {
                double max_allowed_value = m.voltage_V.value() * 1.1;
                return (std::fabs(m.voltage_to_earth_l1e_V.value()) < max_allowed_value and
                        std::fabs(m.voltage_to_earth_l2e_V.value()) < max_allowed_value);
            } else {
                return (std::fabs(m.voltage_to_earth_l1e_V.value()) < MAX_VOLTAGE_STATIC and
                        std::fabs(m.voltage_to_earth_l2e_V.value()) < MAX_VOLTAGE_STATIC);
            }
        }
    }
    // if we can't check, assume all good
    return true;
}

bool EvseManager::check_isolation_resistance_in_range(double resistance) {
    if (resistance < CABLECHECK_INSULATION_FAULT_RESISTANCE_OHM) {
        session_log.evse(false, fmt::format("Isolation measurement FAULT R_F {}.", resistance));
        r_hlc[0]->call_update_isolation_status(types::iso15118::IsolationStatus::Fault);
        return false;
    } else {
        session_log.evse(false, fmt::format("Isolation measurement Ok R_F {}.", resistance));
        r_hlc[0]->call_update_isolation_status(types::iso15118::IsolationStatus::Valid);
    }
    return true;
}

void EvseManager::cable_check() {

    if (r_imd.empty()) {
        // If no IMD is connected, we skip isolation checking.
        EVLOG_info << "No IMD: skipping cable check.";
        r_hlc[0]->call_update_isolation_status(types::iso15118::IsolationStatus::No_IMD);
        r_hlc[0]->call_cable_check_finished(true);
        return;
    }

    // start cable check in a seperate thread.
    std::thread t([this]() {
        session_log.evse(true, "Start cable check...");
        charger->get_stopwatch().report_phase();
        charger->get_stopwatch().mark_phase("CableCheck");

        // Verify output is below 60V initially
        if (not wait_powersupply_DC_below_voltage(CABLECHECK_SAFE_VOLTAGE)) {
            std::ostringstream oss;
            oss << "Voltage did not drop below " << CABLECHECK_SAFE_VOLTAGE << "V within timeout.";
            fail_cable_check(oss.str());
            return;
        }
        charger->get_stopwatch().mark("<60V");

        // normally contactors should be closed before entering cable check routine.
        // On some hardware implementation it may take some time until the confirmation arrives though,
        // so we wait with a timeout here until the contactors are confirmed to be closed.
        // Allow closing from HLC perspective, it will wait for CP state C in Charger IEC state machine as well.
        session_log.car(true, "DC HLC Close contactor (in CableCheck)");
        charger->set_hlc_allow_close_contactor(true);

        Timeout timeout;
        timeout.start(CABLECHECK_CONTACTORS_CLOSE_TIMEOUT);

        while (not timeout.reached() and not cable_check_should_exit()) {
            if (not contactor_open) {
                break;
            }
            std::this_thread::sleep_for(100ms);
        }

        // If relais are still open after timeout, give up
        if (contactor_open) {
            fail_cable_check("CableCheck: Contactors are still open after timeout, giving up.");
            return;
        }

        charger->get_stopwatch().mark("Relay On");

        // Get correct voltage used to test the isolation
        for (int retry_ev_info = 0; retry_ev_info < 10; retry_ev_info++) {
            auto ev_info = get_ev_info();
            if (ev_info.maximum_voltage_limit.has_value()) {
                break;
            }
            std::this_thread::sleep_for(100ms);
        }

        float ev_max_voltage = 500.;

        if (ev_info.maximum_voltage_limit.has_value()) {
            EVLOG_info << "EV reports " << ev_info.maximum_voltage_limit.value() << " V as maximum voltage";
            ev_max_voltage = ev_info.maximum_voltage_limit.value();
        } else {
            EVLOG_error << "CableCheck: Did not receive EV maximum voltage, falling back to 500V";
        }

        auto evse_caps = get_powersupply_capabilities();

        double cable_check_voltage = get_cable_check_voltage(ev_max_voltage, evse_caps.max_export_voltage_V);

        // Allow overriding the cable check voltage from a configuration value
        if (config.dc_isolation_voltage_V > 0) {
            cable_check_voltage = config.dc_isolation_voltage_V;
        }

        charger->get_stopwatch().mark("EVInfo");

        // Set the DC ouput voltage for testing
        if (not powersupply_DC_set(cable_check_voltage, CABLECHECK_CURRENT_LIMIT)) {
            fail_cable_check("CableCheck: Could not set DC power supply voltage and current.");
            return;
        } else {
            EVLOG_info << "CableCheck: Using " << cable_check_voltage << " V";
        }

        // Switch on output voltage
        powersupply_DC_on();

        // Wait until the voltage has rised to the target value.
        // This also handles the short circuit test according to IEC 61851-23 (2023) 6.3.1.109:
        // CC.7.6.20.3: the maximum R for the short circuit test is 110 Ohms.
        // CC.7.6.20.7: maximum current should be reduced to <5A within 1s. We set a current limit below 5A, so the
        // power supply should always achieve that.
        // Within 2.5s present voltage at side B must be below 60V. As the power supply ramp up speed varies greatly,
        // we can only achieve this by limiting the current to I < cable_check_voltage/110 Ohm. The hard coded limit
        // above fulfills that for all voltage ranges.
        if (not wait_powersupply_DC_voltage_reached(cable_check_voltage)) {
            std::ostringstream oss;
            oss << "CableCheck: Voltage did not rise to " << cable_check_voltage << " V within timeout";
            fail_cable_check(oss.str());
            return;
        }

        charger->get_stopwatch().mark("VRampUp");

        // CC 4.1.3: Now relais are closed, voltage is up. We need to perform a self test of the IMD device
        if (config.cable_check_enable_imd_self_test) {
            selftest_result.clear();
            r_imd[0]->call_start_self_test(cable_check_voltage);
            EVLOG_info << "CableCheck: IMD self test started.";

            // Wait for the result of the self test
            bool result{false};
            bool result_received{false};

            for (int wait_seconds = 0; wait_seconds < CABLECHECK_SELFTEST_TIMEOUT; wait_seconds++) {
                if (cable_check_should_exit()) {
                    fail_cable_check("Cancel cable check");
                    return;
                }
                if (selftest_result.wait_for(result, 1s)) {
                    result_received = true;
                    break;
                }
            }

            if (not result_received) {
                fail_cable_check("CableCheck: Did not get a self test result from IMD within timeout");
                return;
            }

            if (not result) {
                EVLOG_error << "CableCheck: IMD Self test failed";
                fail_cable_check("IMD self test failed during cable check");
                return;
            }
        }
        charger->get_stopwatch().mark("Self test");

        // CC.4.1.4: Perform the insulation resistance check
        imd_start();

        if (config.cable_check_wait_number_of_imd_measurements > 0) {
            // read out new isolation resistance value
            isolation_measurement.clear();
            types::isolation_monitor::IsolationMeasurement m;

            EVLOG_info << "CableCheck: Waiting for " << config.cable_check_wait_number_of_imd_measurements
                       << " isolation measurement sample(s)";
            // Wait for N isolation measurement values
            for (int i = 0; i < config.cable_check_wait_number_of_imd_measurements; i++) {
                if (not isolation_measurement.wait_for(m, 5s) or cable_check_should_exit()) {
                    imd_stop();
                    fail_cable_check("CableCheck: Did not receive isolation measurement from IMD within 5 seconds.");
                    return;
                }
            }

            charger->get_stopwatch().mark("Measure");

            // Now the value is valid and can be trusted.
            // Verify it is within ranges. Fault is <100 kOhm
            // Note that 2023 edition removed the warning level which was included in the 2014 edition.
            // Refer to IEC 61851-23 (2023) 6.3.1.105 and CC.4.1.2 / CC.4.1.4
            if (not check_isolation_resistance_in_range(m.resistance_F_Ohm)) {
                imd_stop();
                std::ostringstream oss;
                oss << "Isolation resistance too low: " << m.resistance_F_Ohm << " Ohm";
                error_handling->raise_isolation_resistance_fault(oss.str(), "Resistance");
                fail_cable_check(oss.str());
                return;
            }
        } else {
            // If no measurements are needed after self test, report valid isolation status to ISO stack
            r_hlc[0]->call_update_isolation_status(types::iso15118::IsolationStatus::Valid);
        }

        // We are done with the isolation measurement and can now report success to the EV,
        // but before we do so we need to do a few things for cleanup

        if (config.hack_pause_imd_during_precharge) {
            imd_stop();
        }

        // Sleep before submitting result to spend more time in cable check. This is needed for some solar inverters
        // used as DC chargers for them to warm up. Don't use it.
        std::this_thread::sleep_for(std::chrono::seconds(config.hack_sleep_in_cable_check));
        if (car_manufacturer == types::evse_manager::CarManufacturer::VolkswagenGroup) {
            std::this_thread::sleep_for(std::chrono::seconds(config.hack_sleep_in_cable_check_volkswagen));
        }
        if (config.hack_sleep_in_cable_check > 0 or config.hack_sleep_in_cable_check_volkswagen > 0) {
            charger->get_stopwatch().mark("Sleep");
        }

        if (config.cable_check_wait_below_60V_before_finish) {
            // CC.4.1.2: We need to wait until voltage is below 60V before sending a CableCheck Finished to the EV
            powersupply_DC_off();

            if (not wait_powersupply_DC_below_voltage(CABLECHECK_SAFE_VOLTAGE)) {
                std::ostringstream oss;
                oss << "Voltage did not drop below " << CABLECHECK_SAFE_VOLTAGE << "V within timeout.";
                imd_stop();
                fail_cable_check(oss.str());
                return;
            }
            charger->get_stopwatch().mark("VRampDown");

            EVLOG_info << "CableCheck done, output is below " << CABLECHECK_SAFE_VOLTAGE << "V";
        }

        // Report CableCheck Finished with success to EV
        r_hlc[0]->call_cable_check_finished(true);
        charger->get_stopwatch().report_phase();
        charger->get_stopwatch().mark_phase("PrepareCharging");
    });
    // Detach thread and exit command handler right away
    t.detach();
}

void EvseManager::powersupply_DC_on() {
    if (not powersupply_dc_is_on) {
        session_log.evse(false, "DC power supply: switch ON called, ChargingPhase: " +
                                    types::power_supply_DC::charging_phase_to_string(power_supply_DC_charging_phase));
        r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Export, power_supply_DC_charging_phase);
        powersupply_dc_is_on = true;
    }
}

// input voltage/current is what the evse/car would like to set.
// if it is more then what the energymanager gave us, we can limit it here.
bool EvseManager::powersupply_DC_set(double _voltage, double _current) {
    double voltage = _voltage;
    double current = _current;
    static bool last_is_actually_exporting_to_grid{false};

    const bool charging_phase_changed = last_power_supply_DC_charging_phase not_eq power_supply_DC_charging_phase;
    last_power_supply_DC_charging_phase = power_supply_DC_charging_phase;

    // Some cars always request integer ampere values, so if we offer 14.34A they will request 14.0A.
    // On low power DC charging this makes quite a difference
    // this option will deliver the offered ampere value in those cases

    if (config.hack_fix_hlc_integer_current_requests) {
        auto hlc_limits = charger->get_evse_max_hlc_limits();
        if (hlc_limits.evse_maximum_current_limit - (int)current < 1.)
            current = hlc_limits.evse_maximum_current_limit;
    }

    if (config.sae_j2847_2_bpt_enabled) {
        current = std::abs(current);
    }

    auto caps = get_powersupply_capabilities();

    if (((config.hack_allow_bpt_with_iso2 or sae_bidi_active or session_is_iso_d20_dc_bpt()) and
         current_demand_active) and
        is_actually_exporting_to_grid) {
        if (not last_is_actually_exporting_to_grid and powersupply_dc_is_on) {
            // switching from import from grid to export to grid
            session_log.evse(false, "DC power supply: switch ON in import mode");
            r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Import, power_supply_DC_charging_phase);
        }
        last_is_actually_exporting_to_grid = is_actually_exporting_to_grid;
        // Hack: we are exporting to grid but are in ISO-2 mode
        // check limits of supply
        if (caps.min_import_voltage_V.has_value() and caps.max_import_voltage_V.has_value() and
            voltage >= caps.min_import_voltage_V.value() and voltage <= caps.max_import_voltage_V.value()) {

            if (caps.max_import_current_A.has_value() and current > caps.max_import_current_A.value()) {
                current = caps.max_import_current_A.value();
            }

            if (caps.min_import_current_A.has_value() and current < caps.min_import_current_A.value()) {
                current = caps.min_import_current_A.value();
            }

            // Now it is within limits of DC power supply.
            // now also limit with the limits given by the energymanager.
            // FIXME: dont do this for now, see if the car reduces if we supply new limits.

            session_log.evse(false, fmt::format("DC power supply set: {:.2f}V/{:.2f}A, requested was {:.2f}V/{:.2f}A.",
                                                voltage, current, _voltage, _current));

            // set the new limits for the DC output
            r_powersupply_DC[0]->call_setImportVoltageCurrent(voltage, current);
            return true;
        }
        EVLOG_critical << fmt::format("DC voltage/current out of limits requested: Voltage {:.2f} Current {:.2f}.",
                                      voltage, current);
        return false;
    }
    if (powersupply_dc_is_on and (charging_phase_changed or (((config.hack_allow_bpt_with_iso2 or sae_bidi_active or
                                                               session_is_iso_d20_dc_bpt()) and
                                                              last_is_actually_exporting_to_grid) and
                                                             current_demand_active))) {
        // switching from export to grid to import from grid
        session_log.evse(false, "DC power supply: switch ON in export mode");
        r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Export, power_supply_DC_charging_phase);
        last_is_actually_exporting_to_grid = is_actually_exporting_to_grid;
    }

    // check limits of supply
    if (voltage >= caps.min_export_voltage_V and voltage <= caps.max_export_voltage_V) {

        if (current > caps.max_export_current_A)
            current = caps.max_export_current_A;

        if (current < caps.min_export_current_A)
            current = caps.min_export_current_A;

        // Now it is within limits of DC power supply.
        // now also limit with the limits given by the energymanager.
        // FIXME: dont do this for now, see if the car reduces if we supply new limits.

        session_log.evse(false, fmt::format("DC power supply set: {:.2f}V/{:.2f}A, requested was {:.2f}V/{:.2f}A.",
                                            voltage, current, _voltage, _current));

        // set the new limits for the DC output
        r_powersupply_DC[0]->call_setExportVoltageCurrent(voltage, current);
        return true;
    }
    EVLOG_critical << fmt::format("DC voltage/current out of limits requested: Voltage {:.2f} Current {:.2f}.", voltage,
                                  current);
    return false;
}

void EvseManager::powersupply_DC_off() {
    if (powersupply_dc_is_on) {
        session_log.evse(false, "DC power supply OFF");
        r_powersupply_DC[0]->call_setMode(types::power_supply_DC::Mode::Off, power_supply_DC_charging_phase);
        powersupply_dc_is_on = false;
    }
    power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
}

bool EvseManager::wait_powersupply_DC_voltage_reached(double target_voltage) {
    // wait until the voltage has rised to the target value
    Timeout timeout;
    timeout.start(10s);
    bool voltage_ok = false;
    while (not timeout.reached()) {
        if (cable_check_should_exit()) {
            EVLOG_warning << "Cancel cable check wait voltage reached";
            power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
            powersupply_DC_off();
            r_hlc[0]->call_cable_check_finished(false);
            break;
        }
        types::power_supply_DC::VoltageCurrent m;
        if (powersupply_measurement.wait_for(m, 2000ms)) {
            if (fabs(m.voltage_V - target_voltage) < 10) {
                voltage_ok = true;
                break;
            }
        } else {
            EVLOG_info << "Did not receive voltage measurement from power supply within 2 seconds.";
            power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
            powersupply_DC_off();
            break;
        }
    }
    return voltage_ok;
}

bool EvseManager::wait_powersupply_DC_below_voltage(double target_voltage) {
    // wait until the voltage is below the target voltage
    Timeout timeout;
    timeout.start(10s);
    bool voltage_ok = false;
    while (not timeout.reached()) {
        if (cable_check_should_exit()) {
            EVLOG_warning << "Cancel cable check wait below voltage";
            power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
            powersupply_DC_off();
            r_hlc[0]->call_cable_check_finished(false);
            break;
        }
        types::power_supply_DC::VoltageCurrent m;
        if (powersupply_measurement.wait_for(m, 2000ms)) {
            if (m.voltage_V < target_voltage) {
                voltage_ok = true;
                break;
            }
        } else {
            EVLOG_info << "Did not receive voltage measurement from power supply within 2 seconds.";
            power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
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

// This returns our active local limits, which is either externally set limits
// or hardware capabilties
types::energy::ExternalLimits EvseManager::get_local_energy_limits() {
    types::energy::ExternalLimits active_local_limits;

    std::scoped_lock lock(external_local_limits_mutex);

    // external limits are empty
    if (external_local_energy_limits.schedule_import.empty() and external_local_energy_limits.schedule_export.empty()) {
        if (config.charge_mode == "AC") {
            update_max_current_limit(active_local_limits, get_hw_capabilities().max_current_A_import,
                                     get_hw_capabilities().max_current_A_export);
        } else {
            update_max_watt_limit(active_local_limits, get_powersupply_capabilities().max_export_power_W,
                                  get_powersupply_capabilities().max_import_power_W);
        }
    } else {
        // apply external limits if they are lower
        active_local_limits = external_local_energy_limits;
    }

    // Bidi: set local limits to 0A/0W for exporting to grid, except if bidi is actually active at the moment
    if (not(config.hack_allow_bpt_with_iso2 or sae_bidi_active or session_is_iso_d20_dc_bpt() or
            session_is_iso_d20_ac_bpt())) {
        update_to_zero_discharge_limit(active_local_limits);
    }

    return active_local_limits;
}

void EvseManager::fail_cable_check(const std::string& reason) {
    if (config.charge_mode == "DC") {
        power_supply_DC_charging_phase = types::power_supply_DC::ChargingPhase::Other;
        powersupply_DC_off();
        // CC.4.1.2: We need to wait until voltage is below 60V before sending a CableCheck Finished to the EV
        if (not wait_powersupply_DC_below_voltage(CABLECHECK_SAFE_VOLTAGE)) {
            EVLOG_error << "Voltage did not drop below 60V within timeout, sending CableCheck Finished(false) anyway";
        }
        r_hlc[0]->call_cable_check_finished(false);
    }
    // Raising the cable check error also causes the HLC stack to get notified
    this->error_handling->raise_cable_check_fault(reason);
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

bool EvseManager::session_is_iso_d20_ac_bpt() {
    return selected_d20_energy_service.has_value() &&
           selected_d20_energy_service.value() == types::iso15118::ServiceCategory::AC_BPT;
}

bool EvseManager::session_is_iso_d20_dc_bpt() {
    return selected_d20_energy_service.has_value() and
           selected_d20_energy_service.value() == types::iso15118::ServiceCategory::DC_BPT;
}

static std::optional<float> min_optional(std::optional<float> a, std::optional<float> b) {
    // if both a and b have values, return the smaller one.
    if (a.has_value() and b.has_value()) {
        return (b.value() < a.value() ? b.value() : a.value());
    }
    // if a has a value, return that one.
    if (a.has_value()) {
        return a;
    }

    // else return b. It is either the only value or empty.
    return b;
}

static float min_optional(float a, std::optional<float> b) {
    // if both a and b have values, return the smaller one.
    if (b.has_value()) {
        return (b.value() < a ? b.value() : a);
    }
    // else return a
    return a;
}

types::power_supply_DC::Capabilities EvseManager::get_powersupply_capabilities() {
    types::power_supply_DC::Capabilities caps;
    types::dc_external_derate::ExternalDerating derate;

    {
        std::scoped_lock lock(powersupply_capabilities_mutex);
        caps = powersupply_capabilities;
    }
    {
        std::scoped_lock lock(dc_external_derate_mutex);
        derate = dc_external_derate;
    }

    // Apply external derating if set
    caps.max_export_current_A = min_optional(caps.max_export_current_A, derate.max_export_current_A);
    caps.max_import_current_A = min_optional(caps.max_import_current_A, derate.max_import_current_A);
    caps.max_export_power_W = min_optional(caps.max_export_power_W, derate.max_export_power_W);
    caps.max_import_power_W = min_optional(caps.max_import_power_W, derate.max_import_power_W);

    return caps;
}

void EvseManager::set_external_derating(types::dc_external_derate::ExternalDerating d) {
    std::scoped_lock lock(dc_external_derate_mutex);
    dc_external_derate = d;
}

} // namespace module
