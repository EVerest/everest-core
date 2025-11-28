// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ISO15118_chargerImpl.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "session_logger.hpp"
#include "utils.hpp"

#include <iso15118/io/logging.hpp>
#include <iso15118/session/logger.hpp>

namespace module {
namespace charger {

static constexpr auto WAIT_FOR_SETUP_DONE_MS{std::chrono::milliseconds(200)};

std::mutex GEL; // Global EVerest Lock

namespace dt = iso15118::message_20::datatypes;

namespace {

iso15118::config::TlsNegotiationStrategy convert_tls_negotiation_strategy(const std::string& strategy) {
    using Strategy = iso15118::config::TlsNegotiationStrategy;
    if (strategy == "ACCEPT_CLIENT_OFFER") {
        return Strategy::ACCEPT_CLIENT_OFFER;
    }
    if (strategy == "ENFORCE_TLS") {
        return Strategy::ENFORCE_TLS;
    }
    if (strategy == "ENFORCE_NO_TLS") {
        return Strategy::ENFORCE_NO_TLS;
    }
    EVLOG_AND_THROW(Everest::EverestConfigError("Invalid choice for tls_negotiation_strategy: " + strategy));
    // better safe than sorry
}

types::iso15118::DisplayParameters convert_display_parameters(const dt::DisplayParameters& in) {
    return {in.present_soc,
            in.min_soc,
            in.target_soc,
            in.max_soc,
            in.remaining_time_to_min_soc,
            in.remaining_time_to_target_soc,
            in.remaining_time_to_max_soc,
            in.charging_complete,
            convert_from_optional(in.battery_energy_capacity),
            in.inlet_hot};
}

auto fill_mobility_needs_modes_from_config(const module::Conf& module_config) {

    std::vector<iso15118::d20::ControlMobilityNeedsModes> mobility_needs_modes{};

    if (module_config.supported_dynamic_mode) {
        mobility_needs_modes.push_back({dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedByEvcc});
        if (module_config.supported_mobility_needs_mode_provided_by_secc) {
            mobility_needs_modes.push_back({dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedBySecc});
        }
    }

    if (module_config.supported_scheduled_mode) {
        mobility_needs_modes.push_back({dt::ControlMode::Scheduled, dt::MobilityNeedsMode::ProvidedByEvcc});
    }

    if (mobility_needs_modes.empty()) {
        EVLOG_warning << "Control mobility modes are empty! Setting dynamic mode as default!";
        mobility_needs_modes.push_back({dt::ControlMode::Dynamic, dt::MobilityNeedsMode::ProvidedByEvcc});
    }

    return mobility_needs_modes;
}

types::iso15118::AcEvDynamicModeValues fill_ac_ev_dynamic_control_mode(const dt::Dynamic_AC_CLReqControlMode& mode) {
    types::iso15118::AcEvDynamicModeValues values{};

    values.departure_time = mode.departure_time;
    values.target_energy_request = dt::from_RationalNumber(mode.target_energy_request);
    values.max_energy_request = dt::from_RationalNumber(mode.max_energy_request);
    values.min_energy_request = dt::from_RationalNumber(mode.min_energy_request);

    return values;
}

types::iso15118::EnergyTransferMode get_energy_transfer_mode(const dt::ServiceCategory& service_category,
                                                             const std::optional<dt::AcConnector>& ac_connector) {
    using EnergyTransferMode = types::iso15118::EnergyTransferMode;

    EnergyTransferMode requested_energy_transfer = EnergyTransferMode::AC_single_phase_core;

    if (service_category == dt::ServiceCategory::AC && ac_connector.has_value()) {
        if (ac_connector.value() == dt::AcConnector::SinglePhase) {
            requested_energy_transfer = EnergyTransferMode::AC_single_phase_core;
        } else if (ac_connector.value() == dt::AcConnector::ThreePhase) {
            requested_energy_transfer = EnergyTransferMode::AC_three_phase_core;
        }
    } else if (service_category == dt::ServiceCategory::AC_BPT) {
        requested_energy_transfer = EnergyTransferMode::AC_BPT;
    } else if (service_category == dt::ServiceCategory::DC) {
        requested_energy_transfer = EnergyTransferMode::DC;
    } else if (service_category == dt::ServiceCategory::DC_ACDP) {
        requested_energy_transfer = EnergyTransferMode::DC_ACDP;
    } else if (service_category == dt::ServiceCategory::DC_BPT) {
        requested_energy_transfer = EnergyTransferMode::DC_BPT;
    } else if (service_category == dt::ServiceCategory::DC_ACDP_BPT) {
        requested_energy_transfer = EnergyTransferMode::DC_ACDP_BPT;
    }

    return requested_energy_transfer;
}

} // namespace

void ISO15118_chargerImpl::init() {

    // setup logging routine
    iso15118::io::set_logging_callback([](const iso15118::LogLevel& level, const std::string& msg) {
        switch (level) {
        case iso15118::LogLevel::Error:
            EVLOG_error << msg;
            break;
        case iso15118::LogLevel::Warning:
            EVLOG_warning << msg;
            break;
        case iso15118::LogLevel::Info:
            EVLOG_info << msg;
            break;
        case iso15118::LogLevel::Debug:
            EVLOG_debug << msg;
            break;
        case iso15118::LogLevel::Trace:
            EVLOG_verbose << msg;
            break;
        default:
            EVLOG_critical << "(Loglevel not defined) - " << msg;
            break;
        }
    });

    supported_vas_services_per_provider.reserve(mod->r_iso15118_vas.size());

    for (size_t i = 0; i < mod->r_iso15118_vas.size(); i++) {
        supported_vas_services_per_provider.emplace_back();

        this->mod->r_iso15118_vas[i]->subscribe_offered_vas(
            [this, i](const types::iso15118_vas::OfferedServices& offered_services) {
                std::vector<uint16_t> service_ids;
                std::scoped_lock lock(vas_mutex);

                service_ids.reserve(offered_services.services.size());
                for (const auto& item : offered_services.services) {
                    service_ids.push_back(item.service_id);
                }

                EVLOG_verbose << fmt::format("Updated Supported VAS services for provider #{} ({} service{})", i,
                                             offered_services.services.size(),
                                             offered_services.services.size() > 1 ? "s" : "");
                supported_vas_services_per_provider[i] = service_ids;

                // report to controller if it exists, also check for duplicate service ids
                update_supported_vas_services();
            });
    }
}

void ISO15118_chargerImpl::ready() {
    while (true) {
        if (setup_steps_done.all()) {
            break;
        }
        std::this_thread::sleep_for(WAIT_FOR_SETUP_DONE_MS);
    }

    const auto session_logger = std::make_unique<SessionLogger>(mod->config.logging_path);

    // Obtain certificate location from the security module
    const auto certificate_response = mod->r_security->call_get_leaf_certificate_info(
        types::evse_security::LeafCertificateType::V2G, types::evse_security::EncodingFormat::PEM, false);

    if (certificate_response.status != types::evse_security::GetCertificateInfoStatus::Accepted or
        !certificate_response.info.has_value()) {
        EVLOG_AND_THROW(Everest::EverestConfigError("V2G certificate not found"));
    }

    const auto& certificate_info = certificate_response.info.value();
    std::string path_chain;

    if (certificate_info.certificate.has_value()) {
        path_chain = certificate_info.certificate.value();
    } else if (certificate_info.certificate_single.has_value()) {
        path_chain = certificate_info.certificate_single.value();
    } else {
        EVLOG_AND_THROW(Everest::EverestConfigError("V2G certificate not found"));
    }

    const auto v2g_root_cert_path = mod->r_security->call_get_verify_file(types::evse_security::CaCertificateType::V2G);
    const auto mo_root_cert_path = mod->r_security->call_get_verify_file(types::evse_security::CaCertificateType::MO);

    // TODO(mlitre): Should be updated once libiso supports service renegotiation
    this->mod->p_extensions->publish_service_renegotiation_supported(false);

    const iso15118::TbdConfig tbd_config = {
        {
            iso15118::config::CertificateBackend::EVEREST_LAYOUT,
            {},                                 ///< config_string
            path_chain,                         ///< path_certificate_chain
            certificate_info.key,               ///< path_certificate_key
            certificate_info.password,          ///< private_key_password
            v2g_root_cert_path,                 ///< path_certificate_v2g_root
            mo_root_cert_path,                  ///< path_certificate_mo_root
            mod->config.enable_ssl_logging,     ///< enable_ssl_logging
            mod->config.enable_tls_key_logging, ///< enable_tls_key_logging
            mod->config.enforce_tls_1_3,        ///< enforce_tls_1_3
            mod->config.tls_key_logging_path,   ///< tls_key_logging_path
        },
        mod->config.device,
        convert_tls_negotiation_strategy(mod->config.tls_negotiation_strategy),
        mod->config.enable_sdp_server,
    };
    const auto callbacks = create_callbacks();

    setup_config.control_mobility_modes = fill_mobility_needs_modes_from_config(mod->config);

    if (not mod->config.custom_protocol_namespace.empty()) {
        setup_config.custom_protocol.emplace(mod->config.custom_protocol_namespace);
    }

    controller = std::make_unique<iso15118::TbdController>(tbd_config, callbacks, setup_config);

    // if the vas providers report their supported vas services before the controller exists,
    // we need to update the controller with the supported vas services after instantiation
    {
        std::scoped_lock lock(vas_mutex);
        update_supported_vas_services();
    }

    try {
        controller->loop();
    } catch (const std::exception& e) {
        EVLOG_error << e.what();
    }
}

void ISO15118_chargerImpl::update_supported_vas_services() {
    iso15118::d20::SupportedVASs supported_vas_services;

    for (const auto& provider_services : supported_vas_services_per_provider) {
        for (const auto& service_id : provider_services) {
            // Check for duplicate service IDs across all providers
            if (std::find(supported_vas_services.begin(), supported_vas_services.end(), service_id) !=
                supported_vas_services.end()) {
                EVLOG_error << "Duplicate VAS service ID found: " << std::to_string(service_id)
                            << ". Skipping this service.";
                continue;
            }

            supported_vas_services.push_back(service_id);
        }
    }

    if (this->controller) {
        EVLOG_verbose << fmt::format("Updated controller VAS list: {}", fmt::join(supported_vas_services, ","));
        this->controller->update_supported_vas_services(supported_vas_services);
    } else {
        EVLOG_verbose << "Controller not initialized, skipping setting supported VAS services.";
    }
}

std::optional<size_t> ISO15118_chargerImpl::get_vas_provider_index(uint16_t service_id) {
    std::scoped_lock lock(vas_mutex);

    for (size_t i = 0; i < supported_vas_services_per_provider.size(); i++) {
        const auto& provider_services = supported_vas_services_per_provider[i];
        if (std::find(provider_services.begin(), provider_services.end(), service_id) != provider_services.end()) {
            return i;
        }
    }

    return std::nullopt; // Service ID not found in any provider's list
}

iso15118::session::feedback::Callbacks ISO15118_chargerImpl::create_callbacks() {

    using ScheduleControlModeDC = dt::Scheduled_DC_CLReqControlMode;
    using BPT_ScheduleReqControlModeDC = dt::BPT_Scheduled_DC_CLReqControlMode;
    using DynamicReqControlModeDC = dt::Dynamic_DC_CLReqControlMode;
    using BPT_DynamicReqControlModeDC = dt::BPT_Dynamic_DC_CLReqControlMode;

    using ScheduleControlModeAC = dt::Scheduled_AC_CLReqControlMode;
    using BPT_ScheduleReqControlModeAC = dt::BPT_Scheduled_AC_CLReqControlMode;
    using DynamicReqControlModeAC = dt::Dynamic_AC_CLReqControlMode;
    using BPT_DynamicReqControlModeAC = dt::BPT_Dynamic_AC_CLReqControlMode;

    namespace feedback = iso15118::session::feedback;

    feedback::Callbacks callbacks;

    callbacks.dc_pre_charge_target_voltage = [this](float target_voltage) {
        publish_dc_ev_target_voltage_current({target_voltage, 0});
    };

    callbacks.notify_ev_charging_needs =
        [this](const dt::ServiceCategory& service_category, const std::optional<dt::AcConnector>& ac_connector,
               const dt::ControlMode& control_mode, const dt::MobilityNeedsMode& mobility_needs_mode,
               const feedback::EvseTransferLimits& evse_limits, const feedback::EvTransferLimits& ev_limits,
               const feedback::EvSEControlMode& ev_control_mode,
               const std::vector<dt::ServiceCategory>& ev_energy_services) {
            // Everest types sent to OCPP

            types::iso15118::ChargingNeeds charging_needs;

            charging_needs.requested_energy_transfer = get_energy_transfer_mode(service_category, ac_connector);
            if (!ev_energy_services.empty()) {
                charging_needs.available_energy_transfer = std::vector<types::iso15118::EnergyTransferMode>{};
                charging_needs.available_energy_transfer->reserve(ev_energy_services.size());
                for (const auto& energy_transfer : ev_energy_services) {
                    charging_needs.available_energy_transfer->emplace_back(
                        get_energy_transfer_mode(energy_transfer, std::nullopt));
                }
            }

            if (control_mode == dt::ControlMode::Scheduled) {
                charging_needs.control_mode = types::iso15118::ControlMode::ScheduledControl;
            } else if (control_mode == dt::ControlMode::Dynamic) {
                charging_needs.control_mode = types::iso15118::ControlMode::DynamicControl;
            } else {
                EVLOG_error << "Invalid value received for control mode! Not sending 'ChargingNeeds'.";
                return;
            }

            if (mobility_needs_mode == dt::MobilityNeedsMode::ProvidedByEvcc) {
                charging_needs.mobility_needs_mode = types::iso15118::MobilityNeedsMode::EVCC;
            } else if (mobility_needs_mode == dt::MobilityNeedsMode::ProvidedBySecc) {
                charging_needs.mobility_needs_mode = types::iso15118::MobilityNeedsMode::EVCC_SECC;
            } else {
                EVLOG_error << "Invalid value received for mobility needs mode! Not sending 'ChargingNeeds'.";
                return;
            }

            // For dash20 the data we will publish will be the v2xChargingParameters
            types::iso15118::V2XChargingParameters& v2x_charging_parameters =
                charging_needs.v2x_charging_parameters.emplace();

            if (const auto* dc_evse_limits = std::get_if<iso15118::d20::DcTransferLimits>(&evse_limits)) {
                if (const auto* dc_ev_limits = std::get_if<dt::DC_CPDReqEnergyTransferMode>(&ev_limits)) {
                    fill_v2x_charging_parameters(v2x_charging_parameters, *dc_evse_limits, *dc_ev_limits);
                } else if (const auto* dc_ev_limits = std::get_if<dt::BPT_DC_CPDReqEnergyTransferMode>(&ev_limits)) {
                    fill_v2x_charging_parameters(v2x_charging_parameters, *dc_evse_limits, *dc_ev_limits);
                }
            } else if (const auto* ac_evse_limits = std::get_if<iso15118::d20::AcTransferLimits>(&evse_limits)) {
                if (const auto* ac_ev_limits = std::get_if<dt::AC_CPDReqEnergyTransferMode>(&ev_limits)) {
                    fill_v2x_charging_parameters(v2x_charging_parameters, *ac_evse_limits, *ac_ev_limits);
                } else if (const auto* ac_ev_limits = std::get_if<dt::BPT_AC_CPDReqEnergyTransferMode>(&ev_limits)) {
                    fill_v2x_charging_parameters(v2x_charging_parameters, *ac_evse_limits, *ac_ev_limits);
                }
            } else {
                EVLOG_error << "Invalid type received for EVSE limits! Not sending 'ChargingNeeds'.";
                return;
            }

            if (const auto* ev_se_control_mode = std::get_if<dt::Scheduled_SEReqControlMode>(&ev_control_mode)) {
                fill_v2x_charging_parameters(v2x_charging_parameters, *ev_se_control_mode);
            } else if (const auto* ev_se_control_mode = std::get_if<dt::Dynamic_SEReqControlMode>(&ev_control_mode)) {
                fill_v2x_charging_parameters(v2x_charging_parameters, *ev_se_control_mode);
            } else {
                EVLOG_error << "Invalid type received for EV Control Mode! Not sending 'ChargingNeeds'.";
                return;
            }

            // Publish charging needs through the extensions
            this->mod->p_extensions->publish_charging_needs(charging_needs);
        };

    callbacks.dc_charge_loop_req = [this](const feedback::DcChargeLoopReq& dc_charge_loop_req) {
        if (const auto* dc_control_mode = std::get_if<feedback::DcReqControlMode>(&dc_charge_loop_req)) {
            if (const auto* scheduled_mode = std::get_if<ScheduleControlModeDC>(dc_control_mode)) {
                const auto target_voltage = dt::from_RationalNumber(scheduled_mode->target_voltage);
                const auto target_current = dt::from_RationalNumber(scheduled_mode->target_current);

                publish_dc_ev_target_voltage_current({target_voltage, target_current});

                if (scheduled_mode->max_charge_current and scheduled_mode->max_voltage and
                    scheduled_mode->max_charge_power) {
                    const auto max_current = dt::from_RationalNumber(scheduled_mode->max_charge_current.value());
                    const auto max_voltage = dt::from_RationalNumber(scheduled_mode->max_voltage.value());
                    const auto max_power = dt::from_RationalNumber(scheduled_mode->max_charge_power.value());
                    publish_dc_ev_maximum_limits({max_current, max_power, max_voltage});
                }

            } else if (const auto* bpt_scheduled_mode = std::get_if<BPT_ScheduleReqControlModeDC>(dc_control_mode)) {
                const auto target_voltage = dt::from_RationalNumber(bpt_scheduled_mode->target_voltage);
                const auto target_current = dt::from_RationalNumber(bpt_scheduled_mode->target_current);
                publish_dc_ev_target_voltage_current({target_voltage, target_current});

                if (bpt_scheduled_mode->max_charge_current and bpt_scheduled_mode->max_voltage and
                    bpt_scheduled_mode->max_charge_power) {
                    const auto max_current = dt::from_RationalNumber(bpt_scheduled_mode->max_charge_current.value());
                    const auto max_voltage = dt::from_RationalNumber(bpt_scheduled_mode->max_voltage.value());
                    const auto max_power = dt::from_RationalNumber(bpt_scheduled_mode->max_charge_power.value());
                    publish_dc_ev_maximum_limits({max_current, max_power, max_voltage});
                }

                // publish_dc_ev_maximum_limits({max_limits.current, max_limits.power, max_limits.voltage});
            } else if (const auto* dynamic_mode = std::get_if<DynamicReqControlModeDC>(dc_control_mode)) {
                publish_d20_dc_dynamic_charge_mode(convert_dynamic_values(*dynamic_mode));
            } else if (const auto* bpt_dynamic_mode = std::get_if<BPT_DynamicReqControlModeDC>(dc_control_mode)) {
                publish_d20_dc_dynamic_charge_mode(convert_dynamic_values(*bpt_dynamic_mode));
            }
        } else if (const auto* display_parameters = std::get_if<dt::DisplayParameters>(&dc_charge_loop_req)) {
            publish_display_parameters(convert_display_parameters(*display_parameters));
        } else if (const auto* present_voltage = std::get_if<feedback::PresentVoltage>(&dc_charge_loop_req)) {
            publish_dc_ev_present_voltage(dt::from_RationalNumber(*present_voltage));
        } else if (const auto* meter_info_requested = std::get_if<feedback::MeterInfoRequested>(&dc_charge_loop_req)) {
            if (*meter_info_requested) {
                EVLOG_info << "Meter info is requested from EV";
                publish_meter_info_requested(nullptr);
            }
        }
    };

    callbacks.dc_max_limits = [this](const feedback::DcMaximumLimits& max_limits) {
        publish_dc_ev_maximum_limits({max_limits.current, max_limits.power, max_limits.voltage});
    };

    callbacks.ac_limits = [this](const feedback::AcLimits& limits) {
        if (const auto* ac_transfer_mode = std::get_if<dt::AC_CPDReqEnergyTransferMode>(&limits)) {
            publish_ac_ev_power_limits(fill_ac_ev_power_limits(*ac_transfer_mode));
        } else if (const auto* ac_bpt_transfer_mode = std::get_if<dt::BPT_AC_CPDReqEnergyTransferMode>(&limits)) {
            publish_ac_ev_power_limits(fill_ac_ev_power_limits(*ac_bpt_transfer_mode));
        }
    };

    callbacks.ac_charge_loop_req = [this](const feedback::AcChargeLoopReq& ac_charge_loop_req) {
        if (const auto* ac_control_mode = std::get_if<feedback::AcReqControlMode>(&ac_charge_loop_req)) {
            if (const auto* scheduled_mode = std::get_if<ScheduleControlModeAC>(ac_control_mode)) {
                if (scheduled_mode->max_charge_power.has_value() or scheduled_mode->max_charge_power_L2.has_value() or
                    scheduled_mode->max_charge_power_L3.has_value() or scheduled_mode->min_charge_power.has_value() or
                    scheduled_mode->min_charge_power_L2.has_value() or
                    scheduled_mode->min_charge_power_L3.has_value()) {
                    publish_ac_ev_power_limits(fill_ac_ev_power_limits(*scheduled_mode));
                }
                publish_ac_ev_present_powers(fill_ac_ev_present_power_values(*scheduled_mode));
                // TODO(SL): publish_scheduled_control_mode
            } else if (const auto* bpt_scheduled_mode = std::get_if<BPT_ScheduleReqControlModeAC>(ac_control_mode)) {
                if (bpt_scheduled_mode->max_charge_power.has_value() or
                    bpt_scheduled_mode->max_charge_power_L2.has_value() or
                    bpt_scheduled_mode->max_charge_power_L3.has_value() or
                    bpt_scheduled_mode->min_charge_power.has_value() or
                    bpt_scheduled_mode->min_charge_power_L2.has_value() or
                    bpt_scheduled_mode->min_charge_power_L3.has_value()) {
                    publish_ac_ev_power_limits(fill_ac_ev_power_limits(*bpt_scheduled_mode));
                }
                publish_ac_ev_present_powers(fill_ac_ev_present_power_values(*bpt_scheduled_mode));
                // TODO(SL): publish_scheduled_control_mode
            } else if (const auto* dynamic_mode = std::get_if<DynamicReqControlModeAC>(ac_control_mode)) {
                publish_ac_ev_power_limits(fill_ac_ev_power_limits(*dynamic_mode));
                publish_ac_ev_present_powers(fill_ac_ev_present_power_values(*dynamic_mode));
                publish_ac_ev_dynamic_control_mode(fill_ac_ev_dynamic_control_mode(*dynamic_mode));
            } else if (const auto* bpt_dynamic_mode = std::get_if<BPT_DynamicReqControlModeAC>(ac_control_mode)) {
                publish_ac_ev_power_limits(fill_ac_ev_power_limits(*bpt_dynamic_mode));
                publish_ac_ev_present_powers(fill_ac_ev_present_power_values(*bpt_dynamic_mode));
                auto ev_dynamic_values = fill_ac_ev_dynamic_control_mode(*bpt_dynamic_mode);
                ev_dynamic_values.max_v2x_energy_request =
                    convert_from_optional(bpt_dynamic_mode->max_v2x_energy_request);
                ev_dynamic_values.min_v2x_energy_request =
                    convert_from_optional(bpt_dynamic_mode->min_v2x_energy_request);
                publish_ac_ev_dynamic_control_mode(ev_dynamic_values);
            }
        } else if (const auto* display_parameters = std::get_if<dt::DisplayParameters>(&ac_charge_loop_req)) {
            publish_display_parameters(convert_display_parameters(*display_parameters));
        } else if (const auto* meter_info_requested = std::get_if<feedback::MeterInfoRequested>(&ac_charge_loop_req)) {
            if (*meter_info_requested) {
                EVLOG_info << "Meter info is requested from EV";
                publish_meter_info_requested(nullptr);
            }
        }
    };

    callbacks.signal = [this](feedback::Signal signal) {
        using Signal = feedback::Signal;
        switch (signal) {
        case Signal::PRE_CHARGE_STARTED:
            publish_start_pre_charge(nullptr);
            break;
        case Signal::CHARGE_LOOP_STARTED:
            publish_current_demand_started(nullptr);
            break;
        case Signal::SETUP_FINISHED:
            publish_v2g_setup_finished(nullptr);
            break;
        case Signal::START_CABLE_CHECK:
            publish_start_cable_check(nullptr);
            break;
        case Signal::REQUIRE_AUTH_EIM:
            publish_require_auth_eim(nullptr);
            break;
        case Signal::CHARGE_LOOP_FINISHED:
            publish_current_demand_finished(nullptr);
            break;
        case Signal::DC_OPEN_CONTACTOR:
            publish_dc_open_contactor(nullptr);
            break;
        case Signal::AC_CLOSE_CONTACTOR:
            publish_ac_close_contactor(nullptr);
            break;
        case Signal::AC_OPEN_CONTACTOR:
            publish_ac_open_contactor(nullptr);
            break;
        case Signal::DLINK_TERMINATE:
            publish_dlink_terminate(nullptr);
            break;
        case Signal::DLINK_PAUSE:
            publish_dlink_pause(nullptr);
            break;
        case Signal::DLINK_ERROR:
            publish_dlink_error(nullptr);
            break;
        }
    };

    callbacks.v2g_message = [this](iso15118::message_20::Type id) {
        const auto v2g_message_id = convert_v2g_message_type(id);
        publish_v2g_messages({v2g_message_id});
    };

    callbacks.evccid = [this](const std::string& evccid) { publish_evcc_id(evccid); };

    callbacks.selected_protocol = [this](const std::string& protocol) { publish_selected_protocol(protocol); };

    callbacks.selected_service_parameters = [this](const iso15118::d20::SelectedServiceParameters& parameters) {
        types::iso15118::SelectedServiceParameters selected_parameters{};

        if (parameters.selected_energy_service == dt::ServiceCategory::AC) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::AC;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::DC) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::DC;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::WPT) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::WPT;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::DC_ACDP) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::DC_ACDP;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::AC_BPT) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::AC_BPT;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::DC_BPT) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::DC_BPT;
        } else if (parameters.selected_energy_service == dt::ServiceCategory::DC_ACDP_BPT) {
            selected_parameters.energy_transfer = types::iso15118::ServiceCategory::DC_ACDP_BPT;
        } else {
            EVLOG_critical << "Energy service is apparently no energy service!";
        }

        if (const auto* ac_connector = std::get_if<dt::AcConnector>(&parameters.selected_connector)) {
            if (*ac_connector == dt::AcConnector::SinglePhase) {
                selected_parameters.connector = types::iso15118::Connector::SinglePhase;
            } else {
                selected_parameters.connector = types::iso15118::Connector::ThreePhase;
            }
        } else if (const auto* dc_connector = std::get_if<dt::DcConnector>(&parameters.selected_connector)) {
            if (*dc_connector == dt::DcConnector::Core) {
                selected_parameters.connector = types::iso15118::Connector::Core;
            } else if (*dc_connector == dt::DcConnector::Extended) {
                selected_parameters.connector = types::iso15118::Connector::Extended;
            } else if (*dc_connector == dt::DcConnector::Dual2) {
                selected_parameters.connector = types::iso15118::Connector::Dual2;
            } else {
                selected_parameters.connector = types::iso15118::Connector::Dual4;
            }
        }

        if (parameters.selected_control_mode == dt::ControlMode::Scheduled) {
            selected_parameters.control_mode = types::iso15118::ControlMode::ScheduledControl;
        } else {
            selected_parameters.control_mode = types::iso15118::ControlMode::DynamicControl;
        }
        if (parameters.selected_mobility_needs_mode == dt::MobilityNeedsMode::ProvidedByEvcc) {
            selected_parameters.mobility_needs_mode = types::iso15118::MobilityNeedsMode::EVCC;
        } else {
            selected_parameters.mobility_needs_mode = types::iso15118::MobilityNeedsMode::EVCC_SECC;
        }
        selected_parameters.pricing = static_cast<types::iso15118::Pricing>(parameters.selected_pricing);

        if (parameters.selected_bpt_channel.has_value()) {
            if (parameters.selected_bpt_channel.value() == dt::BptChannel::Unified) {
                selected_parameters.bpt_channel = types::iso15118::BptChannel::Unified;
            } else {
                selected_parameters.bpt_channel = types::iso15118::BptChannel::Separated;
            }
        }
        if (parameters.selected_generator_mode.has_value()) {
            if (parameters.selected_generator_mode.value() == dt::GeneratorMode::GridFollowing) {
                selected_parameters.generator_mode = types::iso15118::GeneratorMode::GridFollowing;
            } else {
                selected_parameters.generator_mode = types::iso15118::GeneratorMode::GridForming;
            }
        }

        publish_selected_service_parameters(selected_parameters);
    };

    callbacks.selected_vas_services = [this](const dt::VasSelectedServiceList& selected_services) {
        // Group selected services by VAS provider index
        std::map<size_t, std::vector<types::iso15118_vas::SelectedService>> services_by_provider;

        for (const auto& service : selected_services) {
            const auto provider_index = get_vas_provider_index(service.service_id);
            if (not provider_index.has_value()) {
                EVLOG_warning << "Selected Service ID " << service.service_id
                              << " is not supported by any VAS provider";
                continue;
            }

            types::iso15118_vas::SelectedService converted_service{service.service_id, service.parameter_set_id};
            services_by_provider[*provider_index].push_back(converted_service);
        }

        // Notify providers about their selected services
        for (const auto& [provider_index, services] : services_by_provider) {
            mod->r_iso15118_vas[provider_index]->call_selected_services(services);
        }
    };

    callbacks.get_vas_parameters = [this](uint16_t service_id) -> std::optional<dt::ServiceParameterList> {
        const auto provider_index = get_vas_provider_index(service_id);
        if (not provider_index.has_value()) {
            // note: this can only happen if the service ID was removed from the provider after ServiceDiscoveryRes
            // was sent
            EVLOG_warning << "Service ID " << service_id << " is not supported by any VAS provider";
            return std::nullopt;
        }

        const auto vas_parameters = mod->r_iso15118_vas[*provider_index]->call_get_service_parameters(service_id);

        if (vas_parameters.empty()) {
            EVLOG_warning << "No parameters found for service ID " << service_id << " in provider #" << *provider_index;
            return std::nullopt;
        }

        try {
            return convert_parameter_set_list(vas_parameters);
        } catch (const std::exception& e) {
            EVLOG_error << "Failed to convert VAS parameters for service ID " << service_id << " from provider #"
                        << *provider_index << ": " << e.what();
            return std::nullopt;
        }
    };
    callbacks.ev_information = [this](const iso15118::d20::EVInformation& ev_info) {
        types::iso15118::EvInformation info = convert_ev_info(ev_info);
        this->mod->p_extensions->publish_ev_info(info);
    };

    callbacks.ev_termination = [this](const std::string& ev_termination_code,
                                      const std::string& ev_termination_explanation) {
        types::iso15118::EvTermination termination_ctx;
        termination_ctx.ev_termination_code = ev_termination_code;
        termination_ctx.ev_termination_explanation = ev_termination_explanation;
        this->mod->p_charger->publish_ev_termination(termination_ctx);
    };

    callbacks.response_code = [this](const iso15118::message_20::datatypes::ResponseCode& code) {
        namespace rc = iso15118::message_20::datatypes;

        switch (code) {
        case rc::ResponseCode::OK:
            this->mod->p_charger->publish_response_code("OK");
            break;
        case rc::ResponseCode::OK_CertificateExpiresSoon:
            this->mod->p_charger->publish_response_code("OK_CertificateExpiresSoon");
            break;
        case rc::ResponseCode::OK_NewSessionEstablished:
            this->mod->p_charger->publish_response_code("OK_NewSessionEstablished");
            break;
        case rc::ResponseCode::OK_OldSessionJoined:
            this->mod->p_charger->publish_response_code("OK_OldSessionJoined");
            break;
        case rc::ResponseCode::OK_PowerToleranceConfirmed:
            this->mod->p_charger->publish_response_code("OK_PowerToleranceConfirmed");
            break;
        case rc::ResponseCode::WARNING_AuthorizationSelectionInvalid:
            this->mod->p_charger->publish_response_code("WARNING_AuthorizationSelectionInvalid");
            break;
        case rc::ResponseCode::WARNING_CertificateExpired:
            this->mod->p_charger->publish_response_code("WARNING_CertificateExpired");
            break;
        case rc::ResponseCode::WARNING_CertificateNotYetValid:
            this->mod->p_charger->publish_response_code("WARNING_CertificateNotYetValid");
            break;
        case rc::ResponseCode::WARNING_CertificateRevoked:
            this->mod->p_charger->publish_response_code("WARNING_CertificateRevoked");
            break;
        case rc::ResponseCode::WARNING_CertificateValidationError:
            this->mod->p_charger->publish_response_code("WARNING_CertificateValidationError");
            break;
        case rc::ResponseCode::WARNING_ChallengeInvalid:
            this->mod->p_charger->publish_response_code("WARNING_ChallengeInvalid");
            break;
        case rc::ResponseCode::WARNING_EIMAuthorizationFailure:
            this->mod->p_charger->publish_response_code("WARNING_EIMAuthorizationFailure");
            break;
        case rc::ResponseCode::WARNING_eMSPUnknown:
            this->mod->p_charger->publish_response_code("WARNING_eMSPUnknown");
            break;
        case rc::ResponseCode::WARNING_EVPowerProfileViolation:
            this->mod->p_charger->publish_response_code("WARNING_EVPowerProfileViolation");
            break;
        case rc::ResponseCode::WARNING_GeneralPnCAuthorizationError:
            this->mod->p_charger->publish_response_code("WARNING_GeneralPnCAuthorizationError");
            break;
        case rc::ResponseCode::WARNING_NoCertificateAvailable:
            this->mod->p_charger->publish_response_code("WARNING_NoCertificateAvailable");
            break;
        case rc::ResponseCode::WARNING_NoContractMatchingPCIDFound:
            this->mod->p_charger->publish_response_code("WARNING_NoContractMatchingPCIDFound");
            break;
        case rc::ResponseCode::WARNING_PowerToleranceNotConfirmed:
            this->mod->p_charger->publish_response_code("WARNING_PowerToleranceNotConfirmed");
            break;
        case rc::ResponseCode::WARNING_ScheduleRenegotiationFailed:
            this->mod->p_charger->publish_response_code("WARNING_ScheduleRenegotiationFailed");
            break;
        case rc::ResponseCode::WARNING_StandbyNotAllowed:
            this->mod->p_charger->publish_response_code("WARNING_StandbyNotAllowed");
            break;
        case rc::ResponseCode::WARNING_WPT:
            this->mod->p_charger->publish_response_code("WARNING_WPT");
            break;
        case rc::ResponseCode::FAILED:
            this->mod->p_charger->publish_response_code("FAILED");
            break;
        case rc::ResponseCode::FAILED_AssociationError:
            this->mod->p_charger->publish_response_code("FAILED_AssociationError");
            break;
        case rc::ResponseCode::FAILED_ContactorError:
            this->mod->p_charger->publish_response_code("FAILED_ContactorError");
            break;
        case rc::ResponseCode::FAILED_EVPowerProfileInvalid:
            this->mod->p_charger->publish_response_code("FAILED_EVPowerProfileInvalid");
            break;
        case rc::ResponseCode::FAILED_EVPowerProfileViolation:
            this->mod->p_charger->publish_response_code("FAILED_EVPowerProfileViolation");
            break;
        case rc::ResponseCode::FAILED_MeteringSignatureNotValid:
            this->mod->p_charger->publish_response_code("FAILED_MeteringSignatureNotValid");
            break;
        case rc::ResponseCode::FAILED_NoEnergyTransferServiceSelected:
            this->mod->p_charger->publish_response_code("FAILED_NoEnergyTransferServiceSelected");
            break;
        case rc::ResponseCode::FAILED_NoServiceRenegotiationSupported:
            this->mod->p_charger->publish_response_code("FAILED_NoServiceRenegotiationSupported");
            break;
        case rc::ResponseCode::FAILED_PauseNotAllowed:
            this->mod->p_charger->publish_response_code("FAILED_PauseNotAllowed");
            break;
        case rc::ResponseCode::FAILED_PowerDeliveryNotApplied:
            this->mod->p_charger->publish_response_code("FAILED_PowerDeliveryNotApplied");
            break;
        case rc::ResponseCode::FAILED_PowerToleranceNotConfirmed:
            this->mod->p_charger->publish_response_code("FAILED_PowerToleranceNotConfirmed");
            break;
        case rc::ResponseCode::FAILED_ScheduleRenegotiation:
            this->mod->p_charger->publish_response_code("FAILED_ScheduleRenegotiation");
            break;
        case rc::ResponseCode::FAILED_ScheduleSelectionInvalid:
            this->mod->p_charger->publish_response_code("FAILED_ScheduleSelectionInvalid");
            break;
        case rc::ResponseCode::FAILED_SequenceError:
            this->mod->p_charger->publish_response_code("FAILED_SequenceError");
            break;
        case rc::ResponseCode::FAILED_ServiceIDInvalid:
            this->mod->p_charger->publish_response_code("FAILED_ServiceIDInvalid");
            break;
        case rc::ResponseCode::FAILED_ServiceSelectionInvalid:
            this->mod->p_charger->publish_response_code("FAILED_ServiceSelectionInvalid");
            break;
        case rc::ResponseCode::FAILED_SignatureError:
            this->mod->p_charger->publish_response_code("FAILED_SignatureError");
            break;
        case rc::ResponseCode::FAILED_UnknownSession:
            this->mod->p_charger->publish_response_code("FAILED_UnknownSession");
            break;
        case rc::ResponseCode::FAILED_WrongChargeParameter:
            this->mod->p_charger->publish_response_code("FAILED_WrongChargeParameter");
            break;
        default:
            this->mod->p_charger->publish_response_code("FAILED");
        }
    };

    return callbacks;
}

void ISO15118_chargerImpl::handle_setup(types::iso15118::EVSEID& evse_id,
                                        [[maybe_unused]] types::iso15118::SaeJ2847BidiMode& sae_j2847_mode,
                                        [[maybe_unused]] bool& debug_mode) {

    std::scoped_lock lock(GEL);
    setup_config.evse_id = evse_id.evse_id; // TODO(SL): Check format for d20

    setup_steps_done.set(to_underlying_value(SetupStep::SETUP));
}

void ISO15118_chargerImpl::handle_set_charging_parameters(types::iso15118::SetupPhysicalValues& physical_values) {
    // your code for cmd set_charging_parameters goes here
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118::PaymentOption>& payment_options,
                                                bool& supported_certificate_service,
                                                [[maybe_unused]] bool& central_contract_validation_allowed) {
    std::scoped_lock lock(GEL);

    std::vector<dt::Authorization> auth_services;

    for (auto& option : payment_options) {
        if (option == types::iso15118::PaymentOption::ExternalPayment) {
            auth_services.push_back(dt::Authorization::EIM);
        } else if (option == types::iso15118::PaymentOption::Contract) {
            // auth_services.push_back(iso15118::message_20::Authorization::PnC);
            EVLOG_warning << "Currently Plug&Charge is not supported and ignored";
        }
    }

    setup_config.authorization_services = auth_services;
    setup_config.enable_certificate_install_service = supported_certificate_service;

    setup_steps_done.set(to_underlying_value(SetupStep::AUTH_SETUP));
}

void ISO15118_chargerImpl::handle_bpt_setup(types::iso15118::BptSetup& bpt_config) {
    std::scoped_lock lock(GEL);

    // Existing values in bpt_setup_config will be overwritten
    auto& bpt_setup = setup_config.bpt_setup_config.emplace();

    bpt_setup.bpt_channel = bpt_config.bpt_channel == types::iso15118::BptChannel::Unified ? dt::BptChannel::Unified
                                                                                           : dt::BptChannel::Separated;
    bpt_setup.generator_mode = bpt_config.generator_mode == types::iso15118::GeneratorMode::GridFollowing
                                   ? dt::GeneratorMode::GridFollowing
                                   : dt::GeneratorMode::GridForming;

    if (bpt_config.grid_code_detection.has_value()) {
        bpt_setup.grid_code_detection_method =
            bpt_config.grid_code_detection.value() == types::iso15118::GridCodeIslandingDetectionMethod::Active
                ? dt::GridCodeIslandingDetectionMethod::Active
                : dt::GridCodeIslandingDetectionMethod::Passive;
    }
}

void ISO15118_chargerImpl::handle_set_powersupply_capabilities(types::power_supply_DC::Capabilities& capabilities) {
    std::scoped_lock lock(GEL);

    setup_config.powersupply_limits.charge_limits.current.max = dt::from_float(capabilities.max_export_current_A);
    setup_config.powersupply_limits.charge_limits.current.min = dt::from_float(capabilities.min_export_current_A);
    setup_config.powersupply_limits.charge_limits.power.max = dt::from_float(capabilities.max_export_power_W);
    setup_config.powersupply_limits.charge_limits.power.min =
        dt::from_float(capabilities.min_export_current_A * capabilities.min_export_voltage_V);
    setup_config.powersupply_limits.voltage.max = dt::from_float(capabilities.max_export_voltage_V);
    setup_config.powersupply_limits.voltage.min = dt::from_float(capabilities.min_export_voltage_V);

    // Discharge Limits
    if (capabilities.max_import_power_W.has_value() or
        (capabilities.min_import_current_A.has_value() and capabilities.min_import_voltage_V.has_value()) or
        capabilities.max_import_current_A.has_value() or capabilities.min_import_current_A.has_value()) {
        auto& discharge_power = (setup_config.powersupply_limits.discharge_limits.has_value())
                                    ? setup_config.powersupply_limits.discharge_limits.value()
                                    : setup_config.powersupply_limits.discharge_limits.emplace();

        if (mod->config.negative_bidirectional_limits) {
            discharge_power.power.max = dt::from_float(-std::fabs(capabilities.max_import_power_W.value_or(0.0)));
            discharge_power.power.min = dt::from_float(-std::fabs(capabilities.min_import_current_A.value_or(0.0)) *
                                                       capabilities.min_import_voltage_V.value_or(0.0));
            discharge_power.current.max = dt::from_float(-std::fabs(capabilities.max_import_current_A.value_or(0.0)));
            discharge_power.current.min = dt::from_float(-std::fabs(capabilities.min_import_current_A.value_or(0.0)));
        } else {
            discharge_power.power.max = dt::from_float(capabilities.max_import_power_W.value_or(0.0));
            discharge_power.power.min = dt::from_float(capabilities.min_import_current_A.value_or(0.0) *
                                                       capabilities.min_import_voltage_V.value_or(0.0));
            discharge_power.current.max = dt::from_float(capabilities.max_import_current_A.value_or(0.0));
            discharge_power.current.min = dt::from_float(capabilities.min_import_current_A.value_or(0.0));
        }
    }

    if (controller) {
        controller->update_powersupply_limits(setup_config.powersupply_limits);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::MAX_LIMITS));
    setup_steps_done.set(to_underlying_value(SetupStep::MIN_LIMITS));
}

void ISO15118_chargerImpl::handle_authorization_response(
    types::authorization::AuthorizationStatus& authorization_status,
    [[maybe_unused]] types::authorization::CertificateStatus& certificate_status) {

    std::scoped_lock lock(GEL);
    // Todo(sl): Currently PnC is not supported
    bool authorized = false;

    if (authorization_status == types::authorization::AuthorizationStatus::Accepted) {
        authorized = true;
    }

    if (controller) {
        controller->send_control_event(iso15118::d20::AuthorizationResponse{authorized});
    }
}

void ISO15118_chargerImpl::handle_ac_contactor_closed(bool& status) {
    std::scoped_lock lock(GEL);
    if (controller) {
        controller->send_control_event(iso15118::d20::ClosedContactor{status});
    }
}

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
    // your code for cmd dlink_ready goes here
}

void ISO15118_chargerImpl::handle_cable_check_finished(bool& status) {

    std::scoped_lock lock(GEL);
    if (controller) {
        controller->send_control_event(iso15118::d20::CableCheckFinished{status});
    }
}

void ISO15118_chargerImpl::handle_receipt_is_required(bool& receipt_required) {
    // your code for cmd receipt_is_required goes here
}

void ISO15118_chargerImpl::handle_stop_charging(bool& stop) {

    std::scoped_lock lock(GEL);
    if (controller) {
        controller->send_control_event(iso15118::d20::StopCharging{stop});
    }
}

void ISO15118_chargerImpl::handle_pause_charging(bool& pause) {
    std::scoped_lock lock(GEL);
    if (controller) {
        controller->send_control_event(iso15118::d20::PauseCharging{pause});
    }
}

void ISO15118_chargerImpl::handle_no_energy_pause_charging(types::iso15118::NoEnergyPauseMode& mode) {
    // your code for cmd no_energy_pause_charging goes here
}

void ISO15118_chargerImpl::handle_update_energy_transfer_modes(
    std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes) {

    std::scoped_lock lock(GEL);

    std::vector<dt::ServiceCategory> services;

    for (const auto& mode : supported_energy_transfer_modes) {
        switch (mode) {
        case types::iso15118::EnergyTransferMode::AC_single_phase_core:
        case types::iso15118::EnergyTransferMode::AC_two_phase:
        case types::iso15118::EnergyTransferMode::AC_three_phase_core:
            services.push_back(dt::ServiceCategory::AC);
            break;
        case types::iso15118::EnergyTransferMode::AC_BPT:
        case types::iso15118::EnergyTransferMode::AC_BPT_DER:
            services.push_back(dt::ServiceCategory::AC_BPT);
            break;
        case types::iso15118::EnergyTransferMode::AC_DER:
            services.push_back(dt::ServiceCategory::AC_DER);
            break;
        case types::iso15118::EnergyTransferMode::DC:
        case types::iso15118::EnergyTransferMode::DC_core:
        case types::iso15118::EnergyTransferMode::DC_extended:
        case types::iso15118::EnergyTransferMode::DC_combo_core:
        case types::iso15118::EnergyTransferMode::DC_unique:
            services.push_back(dt::ServiceCategory::DC);
            break;
        case types::iso15118::EnergyTransferMode::DC_BPT:
            services.push_back(dt::ServiceCategory::DC_BPT);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP:
            services.push_back(dt::ServiceCategory::DC_ACDP);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP_BPT:
            services.push_back(dt::ServiceCategory::DC_ACDP_BPT);
            break;
        case types::iso15118::EnergyTransferMode::WPT:
            services.push_back(dt::ServiceCategory::WPT);
            break;
        }
    }

    setup_config.supported_energy_services = services;

    if (controller) {
        controller->update_energy_modes(services);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::ENERGY_SERVICE));
}

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    // your code for cmd update_ac_max_current goes here
}

void ISO15118_chargerImpl::handle_update_ac_parameters(types::iso15118::AcParameters& ac_parameters) {
    std::scoped_lock lock(GEL);

    setup_config.ac_limits.nominal_frequency = dt::from_float(ac_parameters.nominal_frequency);
    setup_config.ac_limits.max_power_asymmetry = convert_from_optional(ac_parameters.max_power_asymmetry);
    setup_config.ac_limits.power_ramp_limitation = convert_from_optional(ac_parameters.power_ramp_limitation);

    // Exisiting values in ac_setup_config will be overwritten
    auto& ac_setup_config = setup_config.ac_setup_config.emplace();
    ac_setup_config.voltage = static_cast<uint32_t>(ac_parameters.nominal_voltage);
    for (const auto& connector : ac_parameters.connectors) {
        if (connector == types::iso15118::Connector::SinglePhase) {
            ac_setup_config.connectors.emplace_back(dt::AcConnector::SinglePhase);
        } else if (connector == types::iso15118::Connector::ThreePhase) {
            ac_setup_config.connectors.emplace_back(dt::AcConnector::ThreePhase);
        }
    }

    if (controller) {
        controller->update_ac_limits(setup_config.ac_limits);
    }
}

void ISO15118_chargerImpl::handle_update_ac_maximum_limits(types::iso15118::AcEvseMaximumPower& maximum_limits) {
    std::scoped_lock lock(GEL);

    // NOTE(SL): Only the total values are used here right now. The forwarding of the individual L1, L2 and L3 values
    // will come later.
    setup_config.ac_limits.charge_power.max = dt::from_float(maximum_limits.charge_power.total);

    if (maximum_limits.discharge_power.has_value()) {
        auto& discharge_power = (setup_config.ac_limits.discharge_power.has_value())
                                    ? setup_config.ac_limits.discharge_power.value()
                                    : setup_config.ac_limits.discharge_power.emplace();
        discharge_power.max = dt::from_float((mod->config.negative_bidirectional_limits)
                                                 ? -std::fabs(maximum_limits.discharge_power.value().total)
                                                 : maximum_limits.discharge_power.value().total);
    }

    if (controller) {
        controller->update_ac_limits(setup_config.ac_limits);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::MAX_LIMITS));
}

void ISO15118_chargerImpl::handle_update_ac_minimum_limits(types::iso15118::AcEvseMinimumPower& minimum_limits) {
    std::scoped_lock lock(GEL);

    // NOTE(SL): Only the total values are used here right now. The forwarding of the individual L1, L2 and L3 values
    // will come later.
    setup_config.ac_limits.charge_power.min = dt::from_float(minimum_limits.charge_power.total);

    if (minimum_limits.discharge_power.has_value()) {
        auto& discharge_power = (setup_config.ac_limits.discharge_power.has_value())
                                    ? setup_config.ac_limits.discharge_power.value()
                                    : setup_config.ac_limits.discharge_power.emplace();
        discharge_power.max = dt::from_float((mod->config.negative_bidirectional_limits)
                                                 ? -std::fabs(minimum_limits.discharge_power.value().total)
                                                 : minimum_limits.discharge_power.value().total);
    }

    if (controller) {
        controller->update_ac_limits(setup_config.ac_limits);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::MIN_LIMITS));
}

void ISO15118_chargerImpl::handle_update_ac_target_values(types::iso15118::AcTargetValues& target_values) {
    std::scoped_lock lock(GEL);

    if (controller) {
        iso15118::d20::AcTargetPower target_power;

        // NOTE(SL): How to add L2 and L3 values?
        target_power.target_active_power = dt::from_float(target_values.target_active_power.total);
        if (target_values.target_reactive_power.has_value()) {
            target_power.target_reactive_power = dt::from_float(target_values.target_reactive_power.value().total);
        }
        target_power.target_frequency = convert_from_optional(target_values.target_frequency);
        controller->send_control_event(target_power);
    }
}

void ISO15118_chargerImpl::handle_update_ac_present_power(types::units::Power& present_power) {
    std::scoped_lock lock(GEL);

    if (controller) {
        iso15118::d20::AcPresentPower ac_present_power;
        // NOTE(SL): Only the total values are used here right now. The forwarding of the individual L1, L2 and L3
        // values will come later.
        ac_present_power.present_active_power.emplace(dt::from_float(present_power.total));
        controller->send_control_event(ac_present_power);
    }
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(types::iso15118::DcEvseMaximumLimits& maximum_limits) {

    std::scoped_lock lock(GEL);
    setup_config.dc_limits.charge_limits.current.max = dt::from_float(maximum_limits.evse_maximum_current_limit);
    setup_config.dc_limits.charge_limits.power.max = dt::from_float(maximum_limits.evse_maximum_power_limit);
    setup_config.dc_limits.voltage.max = dt::from_float(maximum_limits.evse_maximum_voltage_limit);

    if (maximum_limits.evse_maximum_discharge_current_limit.has_value() or
        maximum_limits.evse_maximum_discharge_power_limit.has_value()) {
        auto& discharge_limits = (setup_config.dc_limits.discharge_limits.has_value())
                                     ? setup_config.dc_limits.discharge_limits.value()
                                     : setup_config.dc_limits.discharge_limits.emplace();
        if (maximum_limits.evse_maximum_discharge_current_limit.has_value()) {
            discharge_limits.current.max =
                dt::from_float((mod->config.negative_bidirectional_limits)
                                   ? -std::fabs(maximum_limits.evse_maximum_discharge_current_limit.value())
                                   : maximum_limits.evse_maximum_discharge_current_limit.value());
        }

        if (maximum_limits.evse_maximum_discharge_power_limit.has_value()) {
            discharge_limits.power.max =
                dt::from_float((mod->config.negative_bidirectional_limits)
                                   ? -std::fabs(maximum_limits.evse_maximum_discharge_power_limit.value())
                                   : maximum_limits.evse_maximum_discharge_power_limit.value());
        }
    }

    if (controller) {
        controller->update_dc_limits(setup_config.dc_limits);
    }
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(types::iso15118::DcEvseMinimumLimits& minimum_limits) {

    std::scoped_lock lock(GEL);
    setup_config.dc_limits.charge_limits.current.min = dt::from_float(minimum_limits.evse_minimum_current_limit);

    setup_config.dc_limits.charge_limits.power.min = dt::from_float(minimum_limits.evse_minimum_power_limit);
    setup_config.dc_limits.voltage.min = dt::from_float(minimum_limits.evse_minimum_voltage_limit);

    if (minimum_limits.evse_minimum_discharge_current_limit.has_value() or
        minimum_limits.evse_minimum_discharge_power_limit.has_value()) {

        auto& discharge_limits = (setup_config.dc_limits.discharge_limits.has_value())
                                     ? setup_config.dc_limits.discharge_limits.value()
                                     : setup_config.dc_limits.discharge_limits.emplace();
        if (minimum_limits.evse_minimum_discharge_current_limit.has_value()) {
            discharge_limits.current.min =
                dt::from_float((mod->config.negative_bidirectional_limits)
                                   ? -std::fabs(minimum_limits.evse_minimum_discharge_current_limit.value())
                                   : minimum_limits.evse_minimum_discharge_current_limit.value());
        }

        if (minimum_limits.evse_minimum_discharge_power_limit.has_value()) {
            discharge_limits.power.min =
                dt::from_float((mod->config.negative_bidirectional_limits)
                                   ? -std::fabs(minimum_limits.evse_minimum_discharge_power_limit.value())
                                   : minimum_limits.evse_minimum_discharge_power_limit.value());
        }
    }

    if (controller) {
        controller->update_dc_limits(setup_config.dc_limits);
    }
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118::IsolationStatus& isolation_status) {
    // your code for cmd update_isolation_status goes here
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118::DcEvsePresentVoltageCurrent& present_voltage_current) {

    float voltage = present_voltage_current.evse_present_voltage;
    float current = present_voltage_current.evse_present_current.value_or(0);

    std::scoped_lock lock(GEL);
    if (controller) {
        controller->send_control_event(iso15118::d20::PresentVoltageCurrent{voltage, current});
    }
}

void ISO15118_chargerImpl::handle_update_meter_info(types::powermeter::Powermeter& powermeter) {
    // your code for cmd update_meter_info goes here
}

void ISO15118_chargerImpl::handle_send_error(types::iso15118::EvseError& error) {
    // your code for cmd send_error goes here
}

void ISO15118_chargerImpl::handle_reset_error() {
    // your code for cmd reset_error goes here
}

} // namespace charger
} // namespace module
