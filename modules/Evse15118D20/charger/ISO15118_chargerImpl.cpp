// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ISO15118_chargerImpl.hpp"

#include "session_logger.hpp"
#include "utils.hpp"

#include <iso15118/io/logging.hpp>
#include <iso15118/session/logger.hpp>

namespace module {
namespace charger {

static constexpr auto WAIT_FOR_SETUP_DONE_MS{std::chrono::milliseconds(200)};

std::mutex GEL; // Global EVerest Lock

namespace {

std::filesystem::path construct_cert_path(const std::filesystem::path& initial_path, const std::string& config_path) {
    if (config_path.empty()) {
        return initial_path;
    }

    if (config_path.front() == '/') {
        return config_path;
    } else {
        return initial_path / config_path;
    }
}

iso15118::config::TlsNegotiationStrategy convert_tls_negotiation_strategy(const std::string& strategy) {
    using Strategy = iso15118::config::TlsNegotiationStrategy;
    if (strategy == "ACCEPT_CLIENT_OFFER") {
        return Strategy::ACCEPT_CLIENT_OFFER;
    } else if (strategy == "ENFORCE_TLS") {
        return Strategy::ENFORCE_TLS;
    } else if (strategy == "ENFORCE_NO_TLS") {
        return Strategy::ENFORCE_NO_TLS;
    } else {
        EVLOG_AND_THROW(Everest::EverestConfigError("Invalid choice for tls_negotiation_strategy: " + strategy));
        // better safe than sorry
    }
}

types::iso15118_charger::DisplayParameters
convert_display_parameters(const iso15118::session::feedback::DisplayParameters& in) {
    return {in.present_soc,
            in.minimum_soc,
            in.target_soc,
            in.maximum_soc,
            in.remaining_time_to_minimum_soc,
            in.remaining_time_to_target_soc,
            in.remaining_time_to_maximum_soc,
            in.battery_energy_capacity,
            in.inlet_hot};
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
}

void ISO15118_chargerImpl::ready() {

    while (true) {
        if (setup_steps_done.all()) {
            break;
        }
        std::this_thread::sleep_for(WAIT_FOR_SETUP_DONE_MS);
    }

    const auto session_logger = std::make_unique<SessionLogger>(mod->config.logging_path);

    const auto default_cert_path = mod->info.paths.etc / "certs";
    const auto cert_path = construct_cert_path(default_cert_path, mod->config.certificate_path);
    const iso15118::TbdConfig tbd_config = {
        {
            iso15118::config::CertificateBackend::EVEREST_LAYOUT,
            cert_path.string(),
            mod->config.private_key_password,
            mod->config.enable_ssl_logging,
            mod->config.enable_tls_key_logging,
        },
        mod->config.device,
        convert_tls_negotiation_strategy(mod->config.tls_negotiation_strategy),
    };
    const auto callbacks = create_callbacks();

    controller = std::make_unique<iso15118::TbdController>(tbd_config, callbacks, setup_config);
    controller->loop();
}

iso15118::session::feedback::Callbacks ISO15118_chargerImpl::create_callbacks() {
    iso15118::session::feedback::Callbacks callbacks;

    callbacks.dc_charge_target = [this](const iso15118::session::feedback::DcChargeTarget& charge_target) {
        publish_dc_ev_target_voltage_current({charge_target.voltage, charge_target.current});
    };

    callbacks.dc_max_limits = [this](const iso15118::session::feedback::DcMaximumLimits& max_limits) {
        publish_dc_ev_maximum_limits({max_limits.current, max_limits.power, max_limits.voltage});
    };

    callbacks.signal = [this](iso15118::session::feedback::Signal signal) {
        using Signal = iso15118::session::feedback::Signal;
        switch (signal) {
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

    callbacks.display_parameters = [this](const iso15118::session::feedback::DisplayParameters parameters) {
        publish_display_parameters(convert_display_parameters(parameters));
    };

    return callbacks;
}

void ISO15118_chargerImpl::handle_setup(
    types::iso15118_charger::EVSEID& evse_id,
    std::vector<types::iso15118_charger::SupportedEnergyMode>& supported_energy_transfer_modes,
    types::iso15118_charger::SaeJ2847BidiMode& sae_j2847_mode, bool& debug_mode) {

    std::scoped_lock lock(GEL);
    setup_config.evse_id = evse_id.evse_id; // TODO(SL): Check format for d20

    std::vector<iso15118::message_20::ServiceCategory> services;

    for (const auto& mode : supported_energy_transfer_modes) {
        if (mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::AC_single_phase_core ||
            mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::AC_three_phase_core) {
            if (mode.bidirectional) {
                services.push_back(iso15118::message_20::ServiceCategory::AC_BPT);
            } else {
                services.push_back(iso15118::message_20::ServiceCategory::AC);
            }
        } else if (mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::DC_core ||
                   mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::DC_extended ||
                   mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::DC_combo_core ||
                   mode.energy_transfer_mode == types::iso15118_charger::EnergyTransferMode::DC_unique) {
            if (mode.bidirectional) {
                services.push_back(iso15118::message_20::ServiceCategory::DC_BPT);
            } else {
                services.push_back(iso15118::message_20::ServiceCategory::DC);
            }
        }
    }

    setup_config.supported_energy_services = services;

    setup_steps_done.set(to_underlying_value(SetupStep::ENERGY_SERVICE));
}

void ISO15118_chargerImpl::handle_set_charging_parameters(
    types::iso15118_charger::SetupPhysicalValues& physical_values) {
    // your code for cmd set_charging_parameters goes here
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                                bool& supported_certificate_service) {
    std::scoped_lock lock(GEL);

    std::vector<iso15118::message_20::Authorization> auth_services;

    for (auto& option : payment_options) {
        if (option == types::iso15118_charger::PaymentOption::ExternalPayment) {
            auth_services.push_back(iso15118::message_20::Authorization::EIM);
        } else if (option == types::iso15118_charger::PaymentOption::Contract) {
            // auth_services.push_back(iso15118::message_20::Authorization::PnC);
            EVLOG_warning << "Currently Plug&Charge is not supported and ignored";
        }
    }

    setup_config.authorization_services = auth_services;
    setup_config.enable_certificate_install_service = supported_certificate_service;

    setup_steps_done.set(to_underlying_value(SetupStep::AUTH_SETUP));
}

void ISO15118_chargerImpl::handle_certificate_response(
    types::iso15118_charger::ResponseExiStreamStatus& exi_stream_status) {
    // your code for cmd certificate_response goes here
}

void ISO15118_chargerImpl::handle_authorization_response(
    types::authorization::AuthorizationStatus& authorization_status,
    types::authorization::CertificateStatus& certificate_status) {

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
    // your code for cmd ac_contactor_closed goes here
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

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    // your code for cmd update_ac_max_current goes here
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(
    types::iso15118_charger::DcEvseMaximumLimits& maximum_limits) {

    std::scoped_lock lock(GEL);
    setup_config.dc_limits.charge_limits.current.max =
        iso15118::message_20::from_float(maximum_limits.evse_maximum_current_limit);
    setup_config.dc_limits.charge_limits.power.max =
        iso15118::message_20::from_float(maximum_limits.evse_maximum_power_limit);
    setup_config.dc_limits.voltage.max = iso15118::message_20::from_float(maximum_limits.evse_maximum_voltage_limit);

    if (maximum_limits.evse_maximum_discharge_current_limit.has_value() or
        maximum_limits.evse_maximum_discharge_power_limit.has_value()) {
        auto& discharge_limits = setup_config.dc_limits.discharge_limits.emplace();

        if (maximum_limits.evse_maximum_discharge_current_limit.has_value()) {
            discharge_limits.current.max =
                iso15118::message_20::from_float(*maximum_limits.evse_maximum_discharge_current_limit);
        }

        if (maximum_limits.evse_maximum_discharge_power_limit.has_value()) {
            discharge_limits.power.max =
                iso15118::message_20::from_float(*maximum_limits.evse_maximum_discharge_power_limit);
        }
    }

    if (controller) {
        controller->update_dc_limits(setup_config.dc_limits);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::MAX_LIMITS));
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(
    types::iso15118_charger::DcEvseMinimumLimits& minimum_limits) {

    std::scoped_lock lock(GEL);
    setup_config.dc_limits.charge_limits.current.min =
        iso15118::message_20::from_float(minimum_limits.evse_minimum_current_limit);

    setup_config.dc_limits.charge_limits.power.min =
        iso15118::message_20::from_float(minimum_limits.evse_minimum_power_limit);
    setup_config.dc_limits.voltage.min = iso15118::message_20::from_float(minimum_limits.evse_minimum_voltage_limit);

    if (minimum_limits.evse_minimum_discharge_current_limit.has_value() or
        minimum_limits.evse_minimum_discharge_power_limit.has_value()) {
        auto& discharge_limits = setup_config.dc_limits.discharge_limits.emplace();

        if (minimum_limits.evse_minimum_discharge_current_limit.has_value()) {
            discharge_limits.current.min =
                iso15118::message_20::from_float(*minimum_limits.evse_minimum_discharge_current_limit);
        }

        if (minimum_limits.evse_minimum_discharge_power_limit.has_value()) {
            discharge_limits.power.min =
                iso15118::message_20::from_float(*minimum_limits.evse_minimum_discharge_power_limit);
        }
    }

    if (controller) {
        controller->update_dc_limits(setup_config.dc_limits);
    }

    setup_steps_done.set(to_underlying_value(SetupStep::MIN_LIMITS));
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118_charger::IsolationStatus& isolation_status) {
    // your code for cmd update_isolation_status goes here
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118_charger::DcEvsePresentVoltageCurrent& present_voltage_current) {

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

void ISO15118_chargerImpl::handle_send_error(types::iso15118_charger::EvseError& error) {
    // your code for cmd send_error goes here
}

void ISO15118_chargerImpl::handle_reset_error() {
    // your code for cmd reset_error goes here
}

} // namespace charger
} // namespace module
