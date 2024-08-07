// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "ISO15118_chargerImpl.hpp"
#include "log.hpp"
#include "v2g_ctx.hpp"

const std::string CERTS_SUB_DIR = "certs"; // relativ path of the certs

using namespace std::chrono_literals;
using BidiMode = types::iso15118_charger::SAE_J2847_Bidi_Mode;

namespace module {
namespace charger {

void ISO15118_chargerImpl::init() {
    if (!v2g_ctx) {
        dlog(DLOG_LEVEL_ERROR, "v2g_ctx not created");
        return;
    }

    /* Configure if_name and auth_mode */
    v2g_ctx->if_name = mod->config.device.data();
    dlog(DLOG_LEVEL_DEBUG, "if_name %s", v2g_ctx->if_name);

    /* Configure tls_security */
    if (mod->config.tls_security == "force") {
        v2g_ctx->tls_security = TLS_SECURITY_FORCE;
        dlog(DLOG_LEVEL_DEBUG, "tls_security force");
    } else if (mod->config.tls_security == "allow") {
        v2g_ctx->tls_security = TLS_SECURITY_ALLOW;
        dlog(DLOG_LEVEL_DEBUG, "tls_security allow");
    } else {
        v2g_ctx->tls_security = TLS_SECURITY_PROHIBIT;
        dlog(DLOG_LEVEL_DEBUG, "tls_security prohibit");
    }

    v2g_ctx->network_read_timeout_tls = mod->config.tls_timeout;

    v2g_ctx->certs_path = mod->info.paths.etc / CERTS_SUB_DIR;

    // Subscribe all vars
    mod->r_iso2->subscribe_Require_Auth_EIM([this]() {
        if (not mod->selected_iso20()) {
            publish_Require_Auth_EIM(nullptr);
        }
    });
    mod->r_iso20->subscribe_Require_Auth_EIM([this]() {
        if (mod->selected_iso20()) {
            publish_Require_Auth_EIM(nullptr);
        }
    });

    mod->r_iso2->subscribe_Require_Auth_PnC([this](const auto value) {
        if (not mod->selected_iso20()) {
            publish_Require_Auth_PnC(value);
        }
    });
    mod->r_iso20->subscribe_Require_Auth_PnC([this](const auto value) {
        if (mod->selected_iso20()) {
            publish_Require_Auth_PnC(value);
        }
    });

    mod->r_iso2->subscribe_AC_Close_Contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_AC_Close_Contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_AC_Close_Contactor([this]() {
        if (mod->selected_iso20()) {
            publish_AC_Close_Contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_AC_Open_Contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_AC_Open_Contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_AC_Open_Contactor([this]() {
        if (mod->selected_iso20()) {
            publish_AC_Open_Contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_Start_CableCheck([this]() {
        if (not mod->selected_iso20()) {
            publish_Start_CableCheck(nullptr);
        }
    });
    mod->r_iso20->subscribe_Start_CableCheck([this]() {
        if (mod->selected_iso20()) {
            publish_Start_CableCheck(nullptr);
        }
    });

    mod->r_iso2->subscribe_DC_Open_Contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_DC_Open_Contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_DC_Open_Contactor([this]() {
        if (mod->selected_iso20()) {
            publish_DC_Open_Contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_V2G_Setup_Finished([this]() {
        if (not mod->selected_iso20()) {
            publish_V2G_Setup_Finished(nullptr);
        }
    });
    mod->r_iso20->subscribe_V2G_Setup_Finished([this]() {
        if (mod->selected_iso20()) {
            publish_V2G_Setup_Finished(nullptr);
        }
    });

    mod->r_iso2->subscribe_currentDemand_Started([this]() {
        if (not mod->selected_iso20()) {
            publish_currentDemand_Started(nullptr);
        }
    });
    mod->r_iso20->subscribe_currentDemand_Started([this]() {
        if (mod->selected_iso20()) {
            publish_currentDemand_Started(nullptr);
        }
    });

    mod->r_iso2->subscribe_currentDemand_Finished([this]() {
        if (not mod->selected_iso20()) {
            publish_currentDemand_Finished(nullptr);
        }
    });
    mod->r_iso20->subscribe_currentDemand_Finished([this]() {
        if (mod->selected_iso20()) {
            publish_currentDemand_Finished(nullptr);
        }
    });

    mod->r_iso2->subscribe_sae_bidi_mode_active([this]() {
        if (not mod->selected_iso20()) {
            publish_sae_bidi_mode_active(nullptr);
        }
    });
    mod->r_iso20->subscribe_sae_bidi_mode_active([this]() {
        if (mod->selected_iso20()) {
            publish_sae_bidi_mode_active(nullptr);
        }
    });

    mod->r_iso2->subscribe_EVCCIDD([this](const auto id) {
        if (not mod->selected_iso20()) {
            publish_EVCCIDD(id);
        }
    });
    mod->r_iso20->subscribe_EVCCIDD([this](const auto id) {
        if (mod->selected_iso20()) {
            publish_EVCCIDD(id);
        }
    });

    mod->r_iso2->subscribe_SelectedPaymentOption([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_SelectedPaymentOption(o);
        }
    });
    mod->r_iso20->subscribe_SelectedPaymentOption([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_SelectedPaymentOption(o);
        }
    });

    mod->r_iso2->subscribe_RequestedEnergyTransferMode([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_RequestedEnergyTransferMode(o);
        }
    });
    mod->r_iso20->subscribe_RequestedEnergyTransferMode([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_RequestedEnergyTransferMode(o);
        }
    });

    mod->r_iso2->subscribe_DepartureTime([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DepartureTime(o);
        }
    });
    mod->r_iso20->subscribe_DepartureTime([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DepartureTime(o);
        }
    });

    mod->r_iso2->subscribe_AC_EAmount([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_AC_EAmount(o);
        }
    });
    mod->r_iso20->subscribe_AC_EAmount([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_AC_EAmount(o);
        }
    });

    mod->r_iso2->subscribe_AC_EVMaxVoltage([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_AC_EVMaxVoltage(o);
        }
    });
    mod->r_iso20->subscribe_AC_EVMaxVoltage([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_AC_EVMaxVoltage(o);
        }
    });

    mod->r_iso2->subscribe_AC_EVMaxCurrent([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_AC_EVMaxCurrent(o);
        }
    });
    mod->r_iso20->subscribe_AC_EVMaxCurrent([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_AC_EVMaxCurrent(o);
        }
    });

    mod->r_iso2->subscribe_AC_EVMinCurrent([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_AC_EVMinCurrent(o);
        }
    });
    mod->r_iso20->subscribe_AC_EVMinCurrent([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_AC_EVMinCurrent(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVEnergyCapacity([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVEnergyCapacity(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVEnergyCapacity([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVEnergyCapacity(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVEnergyRequest([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVEnergyRequest(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVEnergyRequest([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVEnergyRequest(o);
        }
    });

    mod->r_iso2->subscribe_DC_FullSOC([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_FullSOC(o);
        }
    });
    mod->r_iso20->subscribe_DC_FullSOC([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_FullSOC(o);
        }
    });

    mod->r_iso2->subscribe_DC_BulkSOC([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_BulkSOC(o);
        }
    });
    mod->r_iso20->subscribe_DC_BulkSOC([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_BulkSOC(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVStatus([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVStatus(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVStatus([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVStatus(o);
        }
    });

    mod->r_iso2->subscribe_DC_BulkChargingComplete([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_BulkChargingComplete(o);
        }
    });
    mod->r_iso20->subscribe_DC_BulkChargingComplete([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_BulkChargingComplete(o);
        }
    });

    mod->r_iso2->subscribe_DC_ChargingComplete([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_ChargingComplete(o);
        }
    });
    mod->r_iso20->subscribe_DC_ChargingComplete([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_ChargingComplete(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVTargetVoltageCurrent([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVTargetVoltageCurrent(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVTargetVoltageCurrent([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVTargetVoltageCurrent(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVMaximumLimits([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVMaximumLimits(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVMaximumLimits([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVMaximumLimits(o);
        }
    });

    mod->r_iso2->subscribe_DC_EVRemainingTime([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_DC_EVRemainingTime(o);
        }
    });
    mod->r_iso20->subscribe_DC_EVRemainingTime([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_DC_EVRemainingTime(o);
        }
    });

    mod->r_iso2->subscribe_Certificate_Request([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_Certificate_Request(o);
        }
    });
    mod->r_iso20->subscribe_Certificate_Request([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_Certificate_Request(o);
        }
    });

    mod->r_iso2->subscribe_dlink_terminate([this]() {
        if (not mod->selected_iso20()) {
            publish_dlink_terminate(nullptr);
        }
    });
    mod->r_iso20->subscribe_dlink_terminate([this]() {
        if (mod->selected_iso20()) {
            publish_dlink_terminate(nullptr);
        }
    });

    mod->r_iso2->subscribe_dlink_error([this]() {
        if (not mod->selected_iso20()) {
            publish_dlink_error(nullptr);
        }
    });
    mod->r_iso20->subscribe_dlink_error([this]() {
        if (mod->selected_iso20()) {
            publish_dlink_error(nullptr);
        }
    });

    mod->r_iso2->subscribe_dlink_pause([this]() {
        if (not mod->selected_iso20()) {
            publish_dlink_pause(nullptr);
        }
    });
    mod->r_iso20->subscribe_dlink_pause([this]() {
        if (mod->selected_iso20()) {
            publish_dlink_pause(nullptr);
        }
    });

    mod->r_iso2->subscribe_EV_AppProtocol([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_EV_AppProtocol(o);
        }
    });
    mod->r_iso20->subscribe_EV_AppProtocol([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_EV_AppProtocol(o);
        }
    });

    mod->r_iso2->subscribe_V2G_Messages([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_V2G_Messages(o);
        }
    });
    mod->r_iso20->subscribe_V2G_Messages([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_V2G_Messages(o);
        }
    });

    mod->r_iso2->subscribe_Selected_Protocol([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_Selected_Protocol(o);
        }
    });
    mod->r_iso20->subscribe_Selected_Protocol([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_Selected_Protocol(o);
        }
    });
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_setup(
    types::iso15118_charger::EVSEID& evse_id,
    std::vector<types::iso15118_charger::EnergyTransferMode>& supported_energy_transfer_modes,
    types::iso15118_charger::SAE_J2847_Bidi_Mode& sae_j2847_mode, bool& debug_mode) {
    mod->r_iso20->call_setup(evse_id, supported_energy_transfer_modes, sae_j2847_mode, debug_mode);
    mod->r_iso2->call_setup(evse_id, supported_energy_transfer_modes, sae_j2847_mode, debug_mode);
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                                bool& supported_certificate_service) {
    mod->r_iso20->call_session_setup(payment_options, supported_certificate_service);
    mod->r_iso2->call_session_setup(payment_options, supported_certificate_service);
}

void ISO15118_chargerImpl::handle_certificate_response(
    types::iso15118_charger::Response_Exi_Stream_Status& exi_stream_status) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_certificate_response(exi_stream_status);
    } else {
        mod->r_iso2->call_certificate_response(exi_stream_status);
    }
}

void ISO15118_chargerImpl::handle_authorization_response(
    types::authorization::AuthorizationStatus& authorization_status,
    types::authorization::CertificateStatus& certificate_status) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_authorization_response(authorization_status, certificate_status);
    } else {
        mod->r_iso2->call_authorization_response(authorization_status, certificate_status);
    }
}

void ISO15118_chargerImpl::handle_ac_contactor_closed(bool& status) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_ac_contactor_closed(status);
    } else {
        mod->r_iso2->call_ac_contactor_closed(status);
    }
}

void ISO15118_chargerImpl::handle_dlink_ready(bool& value) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_dlink_ready(value);
    } else {
        mod->r_iso2->call_dlink_ready(value);
    }
}

void ISO15118_chargerImpl::handle_cable_check_finished(bool& status) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_cable_check_finished(status);
    } else {
        mod->r_iso2->call_cable_check_finished(status);
    }
}

void ISO15118_chargerImpl::handle_receipt_is_required(bool& receipt_required) {
    mod->r_iso20->call_receipt_is_required(receipt_required);
    mod->r_iso2->call_receipt_is_required(receipt_required);
}

void ISO15118_chargerImpl::handle_stop_charging(bool& stop) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_stop_charging(stop);
    } else {
        mod->r_iso2->call_stop_charging(stop);
    }
}

void ISO15118_chargerImpl::handle_set_charging_parameters(
    types::iso15118_charger::SetupPhysicalValues& physical_values) {
    mod->r_iso20->call_set_charging_parameters(physical_values);
    mod->r_iso2->call_set_charging_parameters(physical_values);
}

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    mod->r_iso20->call_update_ac_max_current(max_current);
    mod->r_iso2->call_update_ac_max_current(max_current);
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(
    types::iso15118_charger::DC_EVSEMaximumLimits& maximum_limits) {
    mod->r_iso20->call_update_dc_maximum_limits(maximum_limits);
    mod->r_iso2->call_update_dc_maximum_limits(maximum_limits);
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(
    types::iso15118_charger::DC_EVSEMinimumLimits& minimum_limits) {
    mod->r_iso20->call_update_dc_minimum_limits(minimum_limits);
    mod->r_iso2->call_update_dc_minimum_limits(minimum_limits);
}

void ISO15118_chargerImpl::handle_update_isolation_status(types::iso15118_charger::IsolationStatus& isolation_status) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_update_isolation_status(isolation_status);
    } else {
        mod->r_iso2->call_update_isolation_status(isolation_status);
    }
}

void ISO15118_chargerImpl::handle_update_dc_present_values(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& present_voltage_current) {
    mod->r_iso20->call_update_dc_present_values(present_voltage_current);
    mod->r_iso2->call_update_dc_present_values(present_voltage_current);
}

void ISO15118_chargerImpl::handle_update_meter_info(types::powermeter::Powermeter& powermeter) {
    mod->r_iso20->call_update_meter_info(powermeter);
    mod->r_iso2->call_update_meter_info(powermeter);
}

void ISO15118_chargerImpl::handle_send_error(types::iso15118_charger::EvseError& error) {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_send_error(error);
    } else {
        mod->r_iso2->call_send_error(error);
    }
}

void ISO15118_chargerImpl::handle_reset_error() {
    if (mod->selected_iso20()) {
        mod->r_iso20->call_reset_error();
    } else {
        mod->r_iso2->call_reset_error();
    }
}

} // namespace charger
} // namespace module
