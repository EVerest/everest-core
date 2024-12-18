// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "ISO15118_chargerImpl.hpp"
#include "log.hpp"
#include "v2g_ctx.hpp"

const std::string CERTS_SUB_DIR = "certs"; // relativ path of the certs

using namespace std::chrono_literals;
using BidiMode = types::iso15118_charger::SaeJ2847BidiMode;

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
    mod->r_iso2->subscribe_require_auth_eim([this]() {
        if (not mod->selected_iso20()) {
            publish_require_auth_eim(nullptr);
        }
    });
    mod->r_iso20->subscribe_require_auth_eim([this]() {
        if (mod->selected_iso20()) {
            publish_require_auth_eim(nullptr);
        }
    });

    mod->r_iso2->subscribe_require_auth_pnc([this](const auto value) {
        if (not mod->selected_iso20()) {
            publish_require_auth_pnc(value);
        }
    });
    mod->r_iso20->subscribe_require_auth_pnc([this](const auto value) {
        if (mod->selected_iso20()) {
            publish_require_auth_pnc(value);
        }
    });

    mod->r_iso2->subscribe_ac_close_contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_ac_close_contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_ac_close_contactor([this]() {
        if (mod->selected_iso20()) {
            publish_ac_close_contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_ac_open_contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_ac_open_contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_ac_open_contactor([this]() {
        if (mod->selected_iso20()) {
            publish_ac_open_contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_start_cable_check([this]() {
        if (not mod->selected_iso20()) {
            publish_start_cable_check(nullptr);
        }
    });
    mod->r_iso20->subscribe_start_cable_check([this]() {
        if (mod->selected_iso20()) {
            publish_start_cable_check(nullptr);
        }
    });

    mod->r_iso2->subscribe_dc_open_contactor([this]() {
        if (not mod->selected_iso20()) {
            publish_dc_open_contactor(nullptr);
        }
    });
    mod->r_iso20->subscribe_dc_open_contactor([this]() {
        if (mod->selected_iso20()) {
            publish_dc_open_contactor(nullptr);
        }
    });

    mod->r_iso2->subscribe_v2g_setup_finished([this]() {
        if (not mod->selected_iso20()) {
            publish_v2g_setup_finished(nullptr);
        }
    });
    mod->r_iso20->subscribe_v2g_setup_finished([this]() {
        if (mod->selected_iso20()) {
            publish_v2g_setup_finished(nullptr);
        }
    });

    mod->r_iso2->subscribe_current_demand_started([this]() {
        if (not mod->selected_iso20()) {
            publish_current_demand_started(nullptr);
        }
    });
    mod->r_iso20->subscribe_current_demand_started([this]() {
        if (mod->selected_iso20()) {
            publish_current_demand_started(nullptr);
        }
    });

    mod->r_iso2->subscribe_current_demand_finished([this]() {
        if (not mod->selected_iso20()) {
            publish_current_demand_finished(nullptr);
        }
    });
    mod->r_iso20->subscribe_current_demand_finished([this]() {
        if (mod->selected_iso20()) {
            publish_current_demand_finished(nullptr);
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

    mod->r_iso2->subscribe_evcc_id([this](const auto id) {
        if (not mod->selected_iso20()) {
            publish_evcc_id(id);
        }
    });
    mod->r_iso20->subscribe_evcc_id([this](const auto id) {
        if (mod->selected_iso20()) {
            publish_evcc_id(id);
        }
    });

    mod->r_iso2->subscribe_selected_payment_option([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_selected_payment_option(o);
        }
    });
    mod->r_iso20->subscribe_selected_payment_option([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_selected_payment_option(o);
        }
    });

    mod->r_iso2->subscribe_requested_energy_transfer_mode([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_requested_energy_transfer_mode(o);
        }
    });
    mod->r_iso20->subscribe_requested_energy_transfer_mode([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_requested_energy_transfer_mode(o);
        }
    });

    mod->r_iso2->subscribe_departure_time([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_departure_time(o);
        }
    });
    mod->r_iso20->subscribe_departure_time([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_departure_time(o);
        }
    });

    mod->r_iso2->subscribe_ac_eamount([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_ac_eamount(o);
        }
    });
    mod->r_iso20->subscribe_ac_eamount([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_ac_eamount(o);
        }
    });

    mod->r_iso2->subscribe_ac_ev_max_voltage([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_ac_ev_max_voltage(o);
        }
    });
    mod->r_iso20->subscribe_ac_ev_max_voltage([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_ac_ev_max_voltage(o);
        }
    });

    mod->r_iso2->subscribe_ac_ev_max_current([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_ac_ev_max_current(o);
        }
    });
    mod->r_iso20->subscribe_ac_ev_max_current([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_ac_ev_max_current(o);
        }
    });

    mod->r_iso2->subscribe_ac_ev_min_current([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_ac_ev_min_current(o);
        }
    });
    mod->r_iso20->subscribe_ac_ev_min_current([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_ac_ev_min_current(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_energy_capacity([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_energy_capacity(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_energy_capacity([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_energy_capacity(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_energy_request([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_energy_request(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_energy_request([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_energy_request(o);
        }
    });

    mod->r_iso2->subscribe_dc_full_soc([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_full_soc(o);
        }
    });
    mod->r_iso20->subscribe_dc_full_soc([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_full_soc(o);
        }
    });

    mod->r_iso2->subscribe_dc_bulk_soc([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_bulk_soc(o);
        }
    });
    mod->r_iso20->subscribe_dc_bulk_soc([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_bulk_soc(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_status([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_status(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_status([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_status(o);
        }
    });

    mod->r_iso2->subscribe_dc_bulk_charging_complete([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_bulk_charging_complete(o);
        }
    });
    mod->r_iso20->subscribe_dc_bulk_charging_complete([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_bulk_charging_complete(o);
        }
    });

    mod->r_iso2->subscribe_dc_charging_complete([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_charging_complete(o);
        }
    });
    mod->r_iso20->subscribe_dc_charging_complete([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_charging_complete(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_target_voltage_current([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_target_voltage_current(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_target_voltage_current([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_target_voltage_current(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_maximum_limits([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_maximum_limits(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_maximum_limits([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_maximum_limits(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_remaining_time([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_remaining_time(o);
        }
    });
    mod->r_iso20->subscribe_dc_ev_remaining_time([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_remaining_time(o);
        }
    });

    mod->r_iso2->subscribe_certificate_request([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_certificate_request(o);
        }
    });
    mod->r_iso20->subscribe_certificate_request([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_certificate_request(o);
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

    mod->r_iso2->subscribe_ev_app_protocol([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_ev_app_protocol(o);
        }
    });
    mod->r_iso20->subscribe_ev_app_protocol([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_ev_app_protocol(o);
        }
    });

    mod->r_iso2->subscribe_v2g_messages([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_v2g_messages(o);
        }
    });
    mod->r_iso20->subscribe_v2g_messages([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_v2g_messages(o);
        }
    });

    mod->r_iso2->subscribe_selected_protocol([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_selected_protocol(o);
        }
    });
    mod->r_iso20->subscribe_selected_protocol([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_selected_protocol(o);
        }
    });

    mod->r_iso2->subscribe_display_parameters([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_display_parameters(o);
        }
    });
    mod->r_iso20->subscribe_display_parameters([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_display_parameters(o);
        }
    });

    mod->r_iso20->subscribe_d20_dc_dynamic_charge_mode([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_d20_dc_dynamic_charge_mode(o);
        }
    });

    mod->r_iso2->subscribe_dc_ev_present_voltage([this](const auto o) {
        if (not mod->selected_iso20()) {
            publish_dc_ev_present_voltage(o);
        }
    });

    mod->r_iso20->subscribe_dc_ev_present_voltage([this](const auto o) {
        if (mod->selected_iso20()) {
            publish_dc_ev_present_voltage(o);
        }
    });

    mod->r_iso2->subscribe_meter_info_requested([this]() {
        if (not mod->selected_iso20()) {
            publish_meter_info_requested(nullptr);
        }
    });

    mod->r_iso20->subscribe_meter_info_requested([this]() {
        if (mod->selected_iso20()) {
            publish_meter_info_requested(nullptr);
        }
    });
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_setup(
    types::iso15118_charger::EVSEID& evse_id,
    std::vector<types::iso15118_charger::SupportedEnergyMode>& supported_energy_transfer_modes,
    types::iso15118_charger::SaeJ2847BidiMode& sae_j2847_mode, bool& debug_mode) {
    mod->r_iso20->call_setup(evse_id, supported_energy_transfer_modes, sae_j2847_mode, debug_mode);
    mod->r_iso2->call_setup(evse_id, supported_energy_transfer_modes, sae_j2847_mode, debug_mode);
}

void ISO15118_chargerImpl::handle_set_charging_parameters(
    types::iso15118_charger::SetupPhysicalValues& physical_values) {
    mod->r_iso20->call_set_charging_parameters(physical_values);
    mod->r_iso2->call_set_charging_parameters(physical_values);
}

void ISO15118_chargerImpl::handle_session_setup(std::vector<types::iso15118_charger::PaymentOption>& payment_options,
                                                bool& supported_certificate_service) {
    mod->r_iso20->call_session_setup(payment_options, supported_certificate_service);
    mod->r_iso2->call_session_setup(payment_options, supported_certificate_service);
}

void ISO15118_chargerImpl::handle_certificate_response(
    types::iso15118_charger::ResponseExiStreamStatus& exi_stream_status) {
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

void ISO15118_chargerImpl::handle_update_ac_max_current(double& max_current) {
    mod->r_iso20->call_update_ac_max_current(max_current);
    mod->r_iso2->call_update_ac_max_current(max_current);
}

void ISO15118_chargerImpl::handle_update_dc_maximum_limits(
    types::iso15118_charger::DcEvseMaximumLimits& maximum_limits) {
    mod->r_iso20->call_update_dc_maximum_limits(maximum_limits);
    mod->r_iso2->call_update_dc_maximum_limits(maximum_limits);
}

void ISO15118_chargerImpl::handle_update_dc_minimum_limits(
    types::iso15118_charger::DcEvseMinimumLimits& minimum_limits) {
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
    types::iso15118_charger::DcEvsePresentVoltageCurrent& present_voltage_current) {
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
