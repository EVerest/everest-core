// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_chargerImpl.hpp"

#include "session_logger.hpp"

#include <iso15118/io/logging.hpp>
#include <iso15118/session/feedback.hpp>
#include <iso15118/session/logger.hpp>
#include <iso15118/tbd_controller.hpp>

std::unique_ptr<iso15118::TbdController> controller;

iso15118::TbdConfig tbd_config;
iso15118::session::feedback::Callbacks callbacks;

std::unique_ptr<SessionLogger> session_logger;

namespace module {
namespace charger {

static std::filesystem::path get_cert_path(const std::filesystem::path& initial_path, const std::string& config_path) {
    if (config_path.empty()) {
        return initial_path;
    }

    if (*config_path.begin() == '/') {
        return config_path;
    } else {
        return initial_path / config_path;
    }
}

static iso15118::config::TlsNegotiationStrategy convert_tls_negotiation_strategy(const std::string& strategy) {
    using Strategy = iso15118::config::TlsNegotiationStrategy;
    if (strategy.compare("ACCEPT_CLIENT_OFFER") == 0) {
        return Strategy::ACCEPT_CLIENT_OFFER;
    } else if (strategy.compare("ENFORCE_TLS") == 0) {
        return Strategy::ENFORCE_TLS;
    } else if (strategy.compare("ENFORCE_NO_TLS") == 0) {
        return Strategy::ENFORCE_NO_TLS;
    } else {
        EVLOG_AND_THROW(Everest::EverestConfigError("Invalid choice for tls_negotiation_strategy: " + strategy));
        // better safe than sorry
    }
}

void ISO15118_chargerImpl::init() {
    // setup logging routine
    iso15118::io::set_logging_callback([](const std::string& msg) { EVLOG_info << msg; });

    session_logger = std::make_unique<SessionLogger>(mod->config.logging_path);

    callbacks.dc_charge_target = [this](const iso15118::session::feedback::DcChargeTarget& charge_target) {
        types::iso15118_charger::DC_EVTargetValues tmp;
        tmp.DC_EVTargetVoltage = charge_target.voltage;
        tmp.DC_EVTargetCurrent = charge_target.current;
        publish_DC_EVTargetVoltageCurrent(tmp);
    };

    callbacks.dc_max_limits = [this](const iso15118::session::feedback::DcMaximumLimits& max_limits) {
        types::iso15118_charger::DC_EVMaximumLimits tmp;
        tmp.DC_EVMaximumVoltageLimit = max_limits.voltage;
        tmp.DC_EVMaximumCurrentLimit = max_limits.current;
        tmp.DC_EVMaximumPowerLimit = max_limits.power;
        publish_DC_EVMaximumLimits(tmp);
    };

    callbacks.signal = [this](iso15118::session::feedback::Signal signal) {
        using Signal = iso15118::session::feedback::Signal;
        switch (signal) {
        case Signal::CHARGE_LOOP_STARTED:
            publish_currentDemand_Started(nullptr);
            break;
        case Signal::SETUP_FINISHED:
            publish_V2G_Setup_Finished(nullptr);
            break;
            ;
        case Signal::START_CABLE_CHECK:
            publish_Start_CableCheck(nullptr);
            break;
        case Signal::REQUIRE_AUTH_EIM:
            publish_Require_Auth_EIM(nullptr);
            break;
        case Signal::CHARGE_LOOP_FINISHED:
            publish_currentDemand_Finished(nullptr);
            break;
        case Signal::DC_OPEN_CONTACTOR:
            publish_DC_Open_Contactor(nullptr);
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

    const auto default_cert_path = mod->info.paths.etc / "certs";

    const auto cert_path = get_cert_path(default_cert_path, mod->config.certificate_path);

    tbd_config = {
        {
            iso15118::config::CertificateBackend::EVEREST_LAYOUT,
            cert_path.string(),
            mod->config.private_key_password,
            mod->config.enable_ssl_logging,
        },
        mod->config.device,
        convert_tls_negotiation_strategy(mod->config.tls_negotiation_strategy),
    };

    controller = std::make_unique<iso15118::TbdController>(tbd_config, callbacks);
}

void ISO15118_chargerImpl::ready() {
    // FIXME (aw): this is just plain stupid ...
    while (true) {
        try {
            controller->loop();
        } catch (const std::exception& e) {
            EVLOG_error << "D20Evse chrashed: " << e.what();
            publish_dlink_error(nullptr);
        }

        controller.reset();

        const auto RETRY_INTERVAL = std::chrono::milliseconds(1000);

        EVLOG_info << "Trying to restart in " << std::to_string(RETRY_INTERVAL.count()) << " milliseconds";

        std::this_thread::sleep_for(RETRY_INTERVAL);

        controller = std::make_unique<iso15118::TbdController>(tbd_config, callbacks);
    }
}

void ISO15118_chargerImpl::handle_setup(
    types::iso15118_charger::EVSEID& evse_id,
    std::vector<types::iso15118_charger::EnergyTransferMode>& supported_energy_transfer_modes,
    types::iso15118_charger::SAE_J2847_Bidi_Mode& sae_j2847_mode, bool& debug_mode,
    types::iso15118_charger::SetupPhysicalValues& physical_values) {
    // your code for cmd setup goes here
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                                bool& supported_certificate_service) {
    // your code for cmd session_setup goes here
}

void ISO15118_chargerImpl::handle_certificate_response(
    types::iso15118_charger::Response_Exi_Stream_Status& exi_stream_status) {
    // your code for cmd certificate_response goes here
}

void ISO15118_chargerImpl::handle_authorization_response(
    types::authorization::AuthorizationStatus& authorization_status,
    types::authorization::CertificateStatus& certificate_status) {

    // Todo(sl): Currently PnC is not supported
    bool authorized = false;

    if (authorization_status == types::authorization::AuthorizationStatus::Accepted) {
        authorized = true;
    }

    controller->send_control_event(iso15118::d20::AuthorizationResponse{authorized});
}

void ISO15118_chargerImpl::handle_ac_contactor_closed(bool& status) {
    // your code for cmd ac_contactor_closed goes here
}

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
    // your code for cmd dlink_ready goes here
}

void ISO15118_chargerImpl::handle_cable_check_finished(bool& status) {
    controller->send_control_event(iso15118::d20::CableCheckFinished{status});
}

void ISO15118_chargerImpl::handle_receipt_is_required(bool& receipt_required) {
    // your code for cmd receipt_is_required goes here
}

void ISO15118_chargerImpl::handle_stop_charging(bool& stop) {
    // your code for cmd stop_charging goes here
}

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    // your code for cmd update_ac_max_current goes here
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(
    types::iso15118_charger::DC_EVSEMaximumLimits& maximum_limits) {
    // your code for cmd update_dc_maximum_limits goes here
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(
    types::iso15118_charger::DC_EVSEMinimumLimits& minimum_limits) {
    // your code for cmd update_dc_minimum_limits goes here
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118_charger::IsolationStatus& isolation_status) {
    // your code for cmd update_isolation_status goes here
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& present_voltage_current) {

    float voltage = present_voltage_current.EVSEPresentVoltage;
    float current = 0;
    if (present_voltage_current.EVSEPresentCurrent.has_value()) {
        current = present_voltage_current.EVSEPresentCurrent.value();
    }
    controller->send_control_event(iso15118::d20::PresentVoltageCurrent{voltage, current});
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
