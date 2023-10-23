// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_chargerImpl.hpp"

#include "session_logger.hpp"

#include <iso15118/io/logging.hpp>
#include <iso15118/session/logger.hpp>
#include <iso15118/tbd_controller.hpp>

std::unique_ptr<iso15118::TbdController> controller;
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

    const auto default_cert_path = mod->info.paths.etc / "certs";

    const auto cert_path = get_cert_path(default_cert_path, mod->config.certificate_path);

    iso15118::TbdConfig tbd_config = {
        {iso15118::config::CertificateBackend::EVEREST_LAYOUT, cert_path.string(), mod->config.enable_ssl_logging},
        mod->config.device,
        convert_tls_negotiation_strategy(mod->config.tls_negotiation_strategy),
    };

    controller = std::make_unique<iso15118::TbdController>(std::move(tbd_config));
}

void ISO15118_chargerImpl::ready() {
    controller->loop();
}

void ISO15118_chargerImpl::handle_set_EVSEID(std::string& EVSEID, std::string& EVSEID_DIN) {
    // your code for cmd set_EVSEID goes here
}

void ISO15118_chargerImpl::handle_set_PaymentOptions(Array& PaymentOptions) {
    // your code for cmd set_PaymentOptions goes here
}

void ISO15118_chargerImpl::handle_set_SupportedEnergyTransferMode(Array& SupportedEnergyTransferMode) {
    // your code for cmd set_SupportedEnergyTransferMode goes here
}

void ISO15118_chargerImpl::handle_set_AC_EVSENominalVoltage(double& EVSENominalVoltage) {
    // your code for cmd set_AC_EVSENominalVoltage goes here
}

void ISO15118_chargerImpl::handle_set_DC_EVSECurrentRegulationTolerance(double& EVSECurrentRegulationTolerance) {
    // your code for cmd set_DC_EVSECurrentRegulationTolerance goes here
}

void ISO15118_chargerImpl::handle_set_DC_EVSEPeakCurrentRipple(double& EVSEPeakCurrentRipple) {
    // your code for cmd set_DC_EVSEPeakCurrentRipple goes here
}

void ISO15118_chargerImpl::handle_set_ReceiptRequired(bool& ReceiptRequired) {
    // your code for cmd set_ReceiptRequired goes here
}

void ISO15118_chargerImpl::handle_set_FreeService(bool& FreeService) {
    // your code for cmd set_FreeService goes here
}

void ISO15118_chargerImpl::handle_set_EVSEEnergyToBeDelivered(double& EVSEEnergyToBeDelivered) {
    // your code for cmd set_EVSEEnergyToBeDelivered goes here
}

void ISO15118_chargerImpl::handle_enable_debug_mode(types::iso15118_charger::DebugMode& debug_mode) {
    // your code for cmd enable_debug_mode goes here
}

void ISO15118_chargerImpl::handle_set_Auth_Okay_EIM(bool& auth_okay_eim) {
    // your code for cmd set_Auth_Okay_EIM goes here
}

void ISO15118_chargerImpl::handle_set_Auth_Okay_PnC(types::authorization::AuthorizationStatus& status,
                                                    types::authorization::CertificateStatus& certificateStatus) {
    // your code for cmd set_Auth_Okay_PnC goes here
}

void ISO15118_chargerImpl::handle_set_FAILED_ContactorError(bool& ContactorError) {
    // your code for cmd set_FAILED_ContactorError goes here
}

void ISO15118_chargerImpl::handle_set_RCD_Error(bool& RCD) {
    // your code for cmd set_RCD_Error goes here
}

void ISO15118_chargerImpl::handle_stop_charging(bool& stop_charging) {
    // your code for cmd stop_charging goes here
}

void ISO15118_chargerImpl::handle_set_DC_EVSEPresentVoltageCurrent(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& EVSEPresentVoltage_Current) {
    // your code for cmd set_DC_EVSEPresentVoltageCurrent goes here
}

void ISO15118_chargerImpl::handle_set_AC_EVSEMaxCurrent(double& EVSEMaxCurrent) {
    // your code for cmd set_AC_EVSEMaxCurrent goes here
}

void ISO15118_chargerImpl::handle_set_DC_EVSEMaximumLimits(
    types::iso15118_charger::DC_EVSEMaximumLimits& EVSEMaximumLimits) {
    // your code for cmd set_DC_EVSEMaximumLimits goes here
}

void ISO15118_chargerImpl::handle_set_DC_EVSEMinimumLimits(
    types::iso15118_charger::DC_EVSEMinimumLimits& EVSEMinimumLimits) {
    // your code for cmd set_DC_EVSEMinimumLimits goes here
}

void ISO15118_chargerImpl::handle_set_EVSEIsolationStatus(
    types::iso15118_charger::IsolationStatus& EVSEIsolationStatus) {
    // your code for cmd set_EVSEIsolationStatus goes here
}

void ISO15118_chargerImpl::handle_set_EVSE_UtilityInterruptEvent(bool& EVSE_UtilityInterruptEvent) {
    // your code for cmd set_EVSE_UtilityInterruptEvent goes here
}

void ISO15118_chargerImpl::handle_set_EVSE_Malfunction(bool& EVSE_Malfunction) {
    // your code for cmd set_EVSE_Malfunction goes here
}

void ISO15118_chargerImpl::handle_set_EVSE_EmergencyShutdown(bool& EVSE_EmergencyShutdown) {
    // your code for cmd set_EVSE_EmergencyShutdown goes here
}

void ISO15118_chargerImpl::handle_set_MeterInfo(types::powermeter::Powermeter& powermeter) {
    // your code for cmd set_MeterInfo goes here
}

void ISO15118_chargerImpl::handle_contactor_closed(bool& status) {
    // your code for cmd contactor_closed goes here
}

void ISO15118_chargerImpl::handle_contactor_open(bool& status) {
    // your code for cmd contactor_open goes here
}

void ISO15118_chargerImpl::handle_cableCheck_Finished(bool& status) {
    // your code for cmd cableCheck_Finished goes here
}

void ISO15118_chargerImpl::handle_set_Certificate_Service_Supported(bool& status) {
    // your code for cmd set_Certificate_Service_Supported goes here
}

void ISO15118_chargerImpl::handle_set_Get_Certificate_Response(
    types::iso15118_charger::Response_Exi_Stream_Status& Existream_Status) {
    // your code for cmd set_Get_Certificate_Response goes here
}

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
    // your code for cmd dlink_ready goes here
}

} // namespace charger
} // namespace module
