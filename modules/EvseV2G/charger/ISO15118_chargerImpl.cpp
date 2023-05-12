// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "ISO15118_chargerImpl.hpp"
#include "log.hpp"
#include "v2g_ctx.hpp"

const std::string CERTS_SUB_DIR = "certs"; // relativ path of the certs

using namespace std::chrono_literals;

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
    dlog(DLOG_LEVEL_DEBUG, "auth_mode %s", mod->config.highlevel_authentication_mode.data());
    if (mod->config.highlevel_authentication_mode == "eim") {
        v2g_ctx->evse_v2g_data.payment_option_list[v2g_ctx->evse_v2g_data.payment_option_list_len] =
            iso1paymentOptionType_ExternalPayment;
        v2g_ctx->evse_v2g_data.payment_option_list_len++;
    }
    if (mod->config.highlevel_authentication_mode == "pnc") {
        v2g_ctx->evse_v2g_data.payment_option_list[v2g_ctx->evse_v2g_data.payment_option_list_len] =
            iso1paymentOptionType_Contract;
        v2g_ctx->evse_v2g_data.payment_option_list_len++;
    }

    /* Configure hlc_protocols */
    if (mod->config.supported_DIN70121 == true) {
        v2g_ctx->supported_protocols |= (1 << V2G_PROTO_DIN70121);
    }
    if (mod->config.supported_ISO15118_2 == true) {
        v2g_ctx->supported_protocols |= (1 << V2G_PROTO_ISO15118_2013);
    }

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

    v2g_ctx->terminate_connection_on_failed_response = mod->config.terminate_connection_on_failed_response;

    v2g_ctx->tls_key_logging = mod->config.tls_key_logging;
    v2g_ctx->tls_key_logging_path = mod->config.tls_key_logging_path;

    if (mod->config.tls_key_logging == true) {
        dlog(DLOG_LEVEL_DEBUG, "tls-key-logging enabled (path: %s)", mod->config.tls_key_logging_path.c_str());
    }

    v2g_ctx->certs_path = mod->info.paths.etc / CERTS_SUB_DIR;

    /* Configure if the contract certificate chain should be verified locally */
    v2g_ctx->session.verify_contract_cert_chain = mod->config.verify_contract_cert_chain;
}

void ISO15118_chargerImpl::ready() {
}

void ISO15118_chargerImpl::handle_set_EVSEID(std::string& EVSEID, std::string& EVSEID_DIN) {
    uint8_t len = EVSEID.length();
    if (len < iso1SessionSetupResType_EVSEID_CHARACTERS_SIZE) {
        memcpy(v2g_ctx->evse_v2g_data.evse_id.bytes, reinterpret_cast<uint8_t*>(EVSEID.data()), len);
        v2g_ctx->evse_v2g_data.evse_id.bytesLen = len;
    } else {
        dlog(DLOG_LEVEL_WARNING, "EVSEID_CHARACTERS_SIZE exceeded (received: %u, max: %u)", len,
             iso1SessionSetupResType_EVSEID_CHARACTERS_SIZE);
    }
};

void ISO15118_chargerImpl::handle_set_PaymentOptions(Array& PaymentOptions) {
    v2g_ctx->evse_v2g_data.payment_option_list_len = 0;

    for (auto& element : PaymentOptions) {
        if (element.is_string()) {
            types::iso15118_charger::PaymentOption payment_option =
                types::iso15118_charger::string_to_payment_option(element.get<std::string>());
            if (payment_option == types::iso15118_charger::PaymentOption::Contract) {
                v2g_ctx->evse_v2g_data.payment_option_list[v2g_ctx->evse_v2g_data.payment_option_list_len] =
                    iso1paymentOptionType_Contract;
                v2g_ctx->evse_v2g_data.payment_option_list_len++;
            } else if (payment_option == types::iso15118_charger::PaymentOption::ExternalPayment) {
                v2g_ctx->evse_v2g_data.payment_option_list[v2g_ctx->evse_v2g_data.payment_option_list_len] =
                    iso1paymentOptionType_ExternalPayment;
                v2g_ctx->evse_v2g_data.payment_option_list_len++;
            } else if (v2g_ctx->evse_v2g_data.payment_option_list_len == 0) {
                dlog(DLOG_LEVEL_WARNING, "Unable to configure PaymentOptions %s", element.get<std::string>());
            }
        }
    }
}

void ISO15118_chargerImpl::handle_set_SupportedEnergyTransferMode(Array& SupportedEnergyTransferMode) {
    uint16_t& energyArrayLen =
        (v2g_ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen);
    iso1EnergyTransferModeType* energyArray =
        v2g_ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.array;
    energyArrayLen = 0;

    v2g_ctx->is_dc_charger = true;

    for (auto& element : SupportedEnergyTransferMode) {
        if (element.is_string()) {
            types::iso15118_charger::EnergyTransferMode energy_transfer_mode =
                types::iso15118_charger::string_to_energy_transfer_mode(element.get<std::string>());
            switch (energy_transfer_mode) {
            case types::iso15118_charger::EnergyTransferMode::AC_single_phase_core:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_AC_single_phase_core;
                v2g_ctx->is_dc_charger = false;
                break;
            case types::iso15118_charger::EnergyTransferMode::AC_three_phase_core:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_AC_three_phase_core;
                v2g_ctx->is_dc_charger = false;
                break;
            case types::iso15118_charger::EnergyTransferMode::DC_core:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_DC_core;
                break;
            case types::iso15118_charger::EnergyTransferMode::DC_extended:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_DC_extended;
                break;
            case types::iso15118_charger::EnergyTransferMode::DC_combo_core:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_DC_combo_core;
                break;
            case types::iso15118_charger::EnergyTransferMode::DC_unique:
                energyArray[(energyArrayLen)++] = iso1EnergyTransferModeType_DC_unique;
                break;
            default:
                if (energyArrayLen == 0) {
                    dlog(DLOG_LEVEL_WARNING, "Unable to configure SupportedEnergyTransferMode %s",
                         element.get<std::string>());
                }
                break;
            }
        }
    }
};

void ISO15118_chargerImpl::handle_set_AC_EVSENominalVoltage(double& EVSENominalVoltage) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_nominal_voltage, (long long int)EVSENominalVoltage,
                            iso1unitSymbolType_V);
};

void ISO15118_chargerImpl::handle_set_DC_EVSECurrentRegulationTolerance(double& EVSECurrentRegulationTolerance) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_current_regulation_tolerance,
                            (long long int)EVSECurrentRegulationTolerance, iso1unitSymbolType_A);
    v2g_ctx->evse_v2g_data.evse_current_regulation_tolerance_is_used = 1;
};

void ISO15118_chargerImpl::handle_set_DC_EVSEPeakCurrentRipple(double& EVSEPeakCurrentRipple) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_peak_current_ripple, (long long int)EVSEPeakCurrentRipple,
                            iso1unitSymbolType_A);
};

void ISO15118_chargerImpl::handle_set_ReceiptRequired(bool& ReceiptRequired) {
    v2g_ctx->evse_v2g_data.receipt_required = (int)ReceiptRequired;
};

void ISO15118_chargerImpl::handle_set_FreeService(bool& FreeService) {
    v2g_ctx->evse_v2g_data.charge_service.FreeService = (int)FreeService;
};

void ISO15118_chargerImpl::handle_set_EVSEEnergyToBeDelivered(double& EVSEEnergyToBeDelivered) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_energy_to_be_delivered, (long long int)EVSEEnergyToBeDelivered,
                            iso1unitSymbolType_Wh);
    v2g_ctx->evse_v2g_data.evse_energy_to_be_delivered_is_used = 1;
};

void ISO15118_chargerImpl::handle_enable_debug_mode(types::iso15118_charger::DebugMode& debug_mode) {
    if (debug_mode == types::iso15118_charger::DebugMode::None) {
        v2g_ctx->debugMode = false;
    } else {
        v2g_ctx->debugMode = true;
    }
};

void ISO15118_chargerImpl::handle_set_Auth_Okay_EIM(bool& auth_okay_eim) {
    if (auth_okay_eim == true) {
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_AUTH] = (uint8_t)iso1EVSEProcessingType_Finished;
    }
};

void ISO15118_chargerImpl::handle_set_Auth_Okay_PnC(types::authorization::AuthorizationStatus& status,
                                                    types::authorization::CertificateStatus& certificateStatus) {
    if (status == types::authorization::AuthorizationStatus::Accepted &&
        certificateStatus == types::authorization::CertificateStatus::Accepted) {
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_AUTH] = (uint8_t)iso1EVSEProcessingType_Finished;
    } else {
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_AUTH] = (uint8_t)iso1EVSEProcessingType_Ongoing;
    }
};

void ISO15118_chargerImpl::handle_set_FAILED_ContactorError(bool& ContactorError){
    // TODO your code for cmd set_FAILED_ContactorError goes here
};

void ISO15118_chargerImpl::handle_set_RCD_Error(bool& RCD) {
    v2g_ctx->evse_v2g_data.rcd = (int)RCD;
};

void ISO15118_chargerImpl::handle_stop_charging(bool& stop_charging) {
    // FIXME we need to use locks on v2g-ctx in all commands as they are running in different threads

    if (stop_charging) {
        // spawn new thread to not block command handler
        std::thread([this, stop_charging] {
            // try to gracefully shutdown charging session
            v2g_ctx->evse_v2g_data.evse_notification = iso1EVSENotificationType_StopCharging;
            memset(v2g_ctx->evse_v2g_data.evse_status_code, iso1DC_EVSEStatusCodeType_EVSE_Shutdown,
                   sizeof(v2g_ctx->evse_v2g_data.evse_status_code));

            int i;
            bool timeout_reached = true;
            // allow 10 seconds for graceful shutdown
            for (i = 0; i < 10; i++) {
                if (v2g_ctx->is_connection_terminated) {
                    timeout_reached = false;
                    break;
                }
                std::this_thread::sleep_for(1s);
            }

            // If it did not stop within timeout, stop session using FAILED reply
            if (timeout_reached) {
                v2g_ctx->stop_hlc = stop_charging;
            }
        }).detach();
    } else {
        v2g_ctx->stop_hlc = false;
    }
};

void ISO15118_chargerImpl::handle_set_DC_EVSEPresentVoltageCurrent(
    types::iso15118_charger::DC_EVSEPresentVoltage_Current& EVSEPresentVoltage_Current) {
    populate_physical_value_float(&v2g_ctx->evse_v2g_data.evse_present_voltage,
                                  EVSEPresentVoltage_Current.EVSEPresentVoltage, 1, iso1unitSymbolType_V);
    populate_physical_value_float(&v2g_ctx->evse_v2g_data.evse_present_current,
                                  static_cast<float>(*EVSEPresentVoltage_Current.EVSEPresentCurrent), 1,
                                  iso1unitSymbolType_A);
};

void ISO15118_chargerImpl::handle_set_AC_EVSEMaxCurrent(double& EVSEMaxCurrent) {
    v2g_ctx->basic_config.evse_ac_current_limit = (float)EVSEMaxCurrent;
};

void ISO15118_chargerImpl::handle_set_DC_EVSEMaximumLimits(
    types::iso15118_charger::DC_EVSEMaximumLimits& EVSEMaximumLimits) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_maximum_current_limit,
                            (long long int)EVSEMaximumLimits.EVSEMaximumCurrentLimit, iso1unitSymbolType_A);
    v2g_ctx->evse_v2g_data.evse_maximum_current_limit_is_used = 1;

    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_maximum_power_limit,
                            (long long int)EVSEMaximumLimits.EVSEMaximumPowerLimit, iso1unitSymbolType_W);
    v2g_ctx->evse_v2g_data.evse_maximum_power_limit_is_used = 1;

    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_maximum_voltage_limit,
                            (long long int)EVSEMaximumLimits.EVSEMaximumVoltageLimit, iso1unitSymbolType_V);
    v2g_ctx->evse_v2g_data.evse_maximum_voltage_limit_is_used = 1;
};

void ISO15118_chargerImpl::handle_set_DC_EVSEMinimumLimits(
    types::iso15118_charger::DC_EVSEMinimumLimits& EVSEMinimumLimits) {
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_minimum_current_limit,
                            (long long int)EVSEMinimumLimits.EVSEMinimumCurrentLimit, iso1unitSymbolType_A);
    populate_physical_value(&v2g_ctx->evse_v2g_data.evse_minimum_voltage_limit,
                            (long long int)EVSEMinimumLimits.EVSEMinimumVoltageLimit, iso1unitSymbolType_V);
};

void ISO15118_chargerImpl::handle_set_EVSEIsolationStatus(
    types::iso15118_charger::IsolationStatus& EVSEIsolationStatus) {
    v2g_ctx->evse_v2g_data.evse_isolation_status = (uint8_t)EVSEIsolationStatus;
    v2g_ctx->evse_v2g_data.evse_isolation_status_is_used = 1;
};

void ISO15118_chargerImpl::handle_set_EVSE_UtilityInterruptEvent(bool& EVSE_UtilityInterruptEvent) {
    // utility interrupt event
    if (EVSE_UtilityInterruptEvent == true)
        memset(v2g_ctx->evse_v2g_data.evse_status_code, (int)iso1DC_EVSEStatusCodeType_EVSE_UtilityInterruptEvent,
               sizeof(v2g_ctx->evse_v2g_data.evse_status_code));
};

void ISO15118_chargerImpl::handle_set_EVSE_Malfunction(bool& EVSE_Malfunction) {
    // EVSE Malfunction
    if (EVSE_Malfunction == true)
        memset(v2g_ctx->evse_v2g_data.evse_status_code, (int)iso1DC_EVSEStatusCodeType_EVSE_Malfunction,
               sizeof(v2g_ctx->evse_v2g_data.evse_status_code));
};

void ISO15118_chargerImpl::handle_set_EVSE_EmergencyShutdown(bool& EVSE_EmergencyShutdown) {
    /* signal changes to possible waiters, according to man page, it never returns an error code */
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    v2g_ctx->intl_emergency_shutdown = EVSE_EmergencyShutdown;
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    /* unlock */
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
};

void ISO15118_chargerImpl::handle_set_MeterInfo(types::powermeter::Powermeter& powermeter) {
    // Signal for ChargingStatus and CurrentDemand that MeterInfo is used
    v2g_ctx->meter_info.meter_info_is_used = 1;
    v2g_ctx->meter_info.meter_reading = powermeter.energy_Wh_import.total;

    if (powermeter.meter_id.is_initialized() == true) {
        uint8_t len = powermeter.meter_id.get().length();
        if (len < iso1MeterInfoType_MeterID_CHARACTERS_SIZE) {
            memcpy(v2g_ctx->meter_info.meter_id.bytes, powermeter.meter_id.get().c_str(), len);
            v2g_ctx->meter_info.meter_id.bytesLen = len;
        } else {
            dlog(DLOG_LEVEL_WARNING, "MeterID_CHARACTERS_SIZEexceeded (received: %u, max: %u)", len,
                 iso1MeterInfoType_MeterID_CHARACTERS_SIZE);
        }
    }
};

void ISO15118_chargerImpl::handle_contactor_closed(bool& status) {
    /* signal changes to possible waiters, according to man page, it never returns an error code */
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    v2g_ctx->contactor_is_closed = status;
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    /* unlock */
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
};

void ISO15118_chargerImpl::handle_contactor_open(bool& status) {
    /* signal changes to possible waiters, according to man page, it never returns an error code */
    pthread_mutex_lock(&v2g_ctx->mqtt_lock);
    v2g_ctx->contactor_is_closed = !status;
    pthread_cond_signal(&v2g_ctx->mqtt_cond);
    /* unlock */
    pthread_mutex_unlock(&v2g_ctx->mqtt_lock);
};

void ISO15118_chargerImpl::handle_cableCheck_Finished(bool& status) {
    if (status == true) {
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_ISOLATION] = (uint8_t)iso1EVSEProcessingType_Finished;
    } else {
        v2g_ctx->evse_v2g_data.evse_processing[PHASE_ISOLATION] = (uint8_t)iso1EVSEProcessingType_Ongoing;
    }
};

void ISO15118_chargerImpl::handle_set_Certificate_Service_Supported(bool& status) {
    // your code for cmd handle_set_Certificate_Service_Supported goes here
}

void ISO15118_chargerImpl::handle_set_Get_Certificate_Response(
    types::iso15118_charger::Response_Exi_Stream_Status& Existream_Status) {
    // your code for cmd handle_set_Response_Certificate_Install goes here
}

} // namespace charger
} // namespace module
