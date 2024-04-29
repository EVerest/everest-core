// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // sleep

#include "log.hpp"
#include "v2g_ctx.hpp"

void init_physical_value(struct iso1PhysicalValueType* const physicalValue, iso1unitSymbolType unit) {
    physicalValue->Multiplier = 0;
    physicalValue->Unit = unit;
    physicalValue->Value = 0;
}

// Only for AC
bool populate_physical_value(struct iso1PhysicalValueType* pv, long long int value, iso1unitSymbolType unit) {
    struct iso1PhysicalValueType physic_tmp = {pv->Multiplier, pv->Unit, pv->Value}; // To restore
    pv->Unit = unit;
    pv->Multiplier = 0; // with integers, we don't need negative multipliers for precision, so start at 0

    // if the value is too large to be represented in 16 signed bits, increase the multiplier
    while ((value > INT16_MAX) || (value < INT16_MIN)) {
        pv->Multiplier++;
        value /= 10;
    }

    if ((pv->Multiplier < PHY_VALUE_MULT_MIN) || (pv->Multiplier > PHY_VALUE_MULT_MAX)) {
        memcpy(pv, &physic_tmp, sizeof(struct iso1PhysicalValueType));
        dlog(DLOG_LEVEL_WARNING, "Physical value out of scope. Ignore value");
        return false;
    }

    pv->Value = value;

    return true;
}

void populate_physical_value_float(struct iso1PhysicalValueType* pv, float value, uint8_t decimal_places,
                                   iso1unitSymbolType unit) {
    if (false == populate_physical_value(pv, (long long int)value, unit)) {
        return;
    }

    if (pv->Multiplier == 0) {
        for (uint8_t idx = 0; idx < decimal_places; idx++) {
            if (((long int)(value * 10) < INT16_MAX) && ((long int)(value * 10) > INT16_MIN)) {
                pv->Multiplier--;
                value *= 10;
            }
        }
    }

    if (pv->Multiplier != -decimal_places) {
        dlog(DLOG_LEVEL_WARNING,
             "Possible precision loss while converting to physical value type, requested %i, actual %i (value %f)",
             decimal_places, -pv->Multiplier, value);
    }

    pv->Value = value;
}

void setMinPhysicalValue(struct iso1PhysicalValueType* ADstPhyValue, const struct iso1PhysicalValueType* ASrcPhyValue,
                         unsigned int* AIsUsed) {

    if (((NULL != AIsUsed) && (0 == *AIsUsed)) || ((pow(10, ASrcPhyValue->Multiplier) * ASrcPhyValue->Value) <
                                                   (pow(10, ADstPhyValue->Multiplier) * ADstPhyValue->Value))) {
        ADstPhyValue->Multiplier = ASrcPhyValue->Multiplier;
        ADstPhyValue->Value = ASrcPhyValue->Value;

        if (NULL != AIsUsed) {
            *AIsUsed = 1;
        }
    }
}

static void* v2g_ctx_eventloop(void* data) {
    struct v2g_context* ctx = static_cast<struct v2g_context*>(data);

    while (!ctx->shutdown) {
        int rv;

        rv = event_base_loop(ctx->event_base, 0);
        if (rv == -1)
            break;

        /* if no events are registered, restart looping */
        if (rv == 1)
            sleep(1); /* FIXME this is bad since we actually do busy-waiting here */
    }

    return NULL;
}

static int v2g_ctx_start_events(struct v2g_context* ctx) {
    pthread_attr_t attr;
    int rv;

    /* create the thread in detached state so we don't need to join it later */
    if (pthread_attr_init(&attr) != 0) {
        dlog(DLOG_LEVEL_ERROR, "pthread_attr_init failed: %s", strerror(errno));
        return -1;
    }
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        dlog(DLOG_LEVEL_ERROR, "pthread_attr_setdetachstate failed: %s", strerror(errno));
        return -1;
    }

    rv = pthread_create(&ctx->event_thread, NULL, v2g_ctx_eventloop, ctx);
    return rv ? -1 : 0;
}

void v2g_ctx_init_charging_session(struct v2g_context* const ctx, bool is_connection_terminated) {
    v2g_ctx_init_charging_state(ctx, is_connection_terminated); // Init charging state
    v2g_ctx_init_charging_values(ctx);                          // Loads the internal default config
}

void v2g_ctx_init_charging_state(struct v2g_context* const ctx, bool is_connection_terminated) {
    ctx->stop_hlc = false;
    ctx->intl_emergency_shutdown = false;
    ctx->is_connection_terminated = is_connection_terminated;
    ctx->last_v2g_msg = V2G_UNKNOWN_MSG;
    ctx->current_v2g_msg = V2G_UNKNOWN_MSG;
    ctx->state = 0; // WAIT_FOR_SESSIONSETUP
    ctx->selected_protocol = V2G_UNKNOWN_PROTOCOL;
    ctx->session.renegotiation_required = false;
    ctx->session.is_charging = false;

    /* Reset timer */
    if (ctx->com_setup_timeout != NULL) {
        event_free(ctx->com_setup_timeout);
        ctx->com_setup_timeout = NULL;
    }
}

void v2g_ctx_init_charging_values(struct v2g_context* const ctx) {
    static bool initialize_once = false;
    const char init_service_name[] = {"EVCharging_Service"};

    if (ctx->hlc_pause_active != true) {
        ctx->evse_v2g_data.session_id =
            (uint64_t)0; /* store associated session id, this is zero until SessionSetupRes is sent */
    }
    ctx->evse_v2g_data.notification_max_delay = (uint32_t)0;
    ctx->evse_v2g_data.evse_isolation_status = (uint8_t)iso1isolationLevelType_Invalid;
    ctx->evse_v2g_data.evse_isolation_status_is_used = (unsigned int)1; // Shall be used in DIN
    ctx->evse_v2g_data.evse_notification = (uint8_t)0;
    ctx->evse_v2g_data.evse_status_code[PHASE_PARAMETER] = iso1DC_EVSEStatusCodeType_EVSE_Ready; // [V2G-DC-453]
    ctx->evse_v2g_data.evse_status_code[PHASE_ISOLATION] = iso1DC_EVSEStatusCodeType_EVSE_Ready;
    ctx->evse_v2g_data.evse_status_code[PHASE_PRECHARGE] = iso1DC_EVSEStatusCodeType_EVSE_Ready;
    ctx->evse_v2g_data.evse_status_code[PHASE_CHARGE] = iso1DC_EVSEStatusCodeType_EVSE_Ready;
    memset(ctx->evse_v2g_data.evse_processing, iso1EVSEProcessingType_Ongoing, PHASE_LENGTH);
    ctx->evse_v2g_data.evse_processing[PHASE_PARAMETER] = iso1EVSEProcessingType_Finished; // Skip parameter phase

    if (ctx->hlc_pause_active != true) {
        ctx->evse_v2g_data.charge_service.ServiceCategory = iso1serviceCategoryType_EVCharging;
        ctx->evse_v2g_data.charge_service.ServiceID = (uint16_t)1;
        memcpy(ctx->evse_v2g_data.charge_service.ServiceName.characters, init_service_name, sizeof(init_service_name));
        ctx->evse_v2g_data.charge_service.ServiceName.charactersLen = sizeof(init_service_name);
        ctx->evse_v2g_data.charge_service.ServiceName_isUsed = 0;
        // ctx->evse_v2g_data.chargeService.ServiceScope.characters
        // ctx->evse_v2g_data.chargeService.ServiceScope.charactersLen
        ctx->evse_v2g_data.charge_service.ServiceScope_isUsed = (unsigned int)0;
    }
    ctx->meter_info.meter_info_is_used = false;

    ctx->evse_v2g_data.evse_service_list_len = (uint16_t)0;
    memset(&ctx->evse_v2g_data.service_parameter_list, 0,
           sizeof(struct iso1ServiceParameterListType) * iso1ServiceListType_Service_ARRAY_SIZE);

    if (initialize_once == false) {
        ctx->evse_v2g_data.charge_service.FreeService = 0;
        std::string evse_id = std::string("DE*CBY*ETE1*234");
        strcpy(reinterpret_cast<char*>(ctx->evse_v2g_data.evse_id.bytes), evse_id.data());
        ctx->evse_v2g_data.evse_id.bytesLen = evse_id.size();
        ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.array[0] =
            iso1EnergyTransferModeType_AC_single_phase_core;
        ctx->evse_v2g_data.charge_service.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen = 1;
        ctx->evse_v2g_data.date_time_now_is_used = (unsigned int)0;

        // evse power values
        init_physical_value(&ctx->evse_v2g_data.evse_current_regulation_tolerance, iso1unitSymbolType_A);
        ctx->evse_v2g_data.evse_current_regulation_tolerance_is_used = (unsigned int)0; // optional in din
        init_physical_value(&ctx->evse_v2g_data.evse_energy_to_be_delivered, iso1unitSymbolType_Wh);
        ctx->evse_v2g_data.evse_energy_to_be_delivered_is_used = (unsigned int)0; // optional in din
        init_physical_value(&ctx->evse_v2g_data.evse_maximum_current_limit, iso1unitSymbolType_A);
        ctx->evse_v2g_data.evse_maximum_current_limit_is_used = (unsigned int)0;
        ctx->evse_v2g_data.evse_current_limit_achieved = (int)0;
        init_physical_value(&ctx->evse_v2g_data.evse_maximum_power_limit, iso1unitSymbolType_W);
        ctx->evse_v2g_data.evse_maximum_power_limit_is_used = (unsigned int)0;
        ctx->evse_v2g_data.evse_power_limit_achieved = (int)0;
        init_physical_value(&ctx->evse_v2g_data.evse_maximum_voltage_limit, iso1unitSymbolType_V);

        ctx->evse_v2g_data.evse_maximum_voltage_limit_is_used = (unsigned int)0; // mandatory
        ctx->evse_v2g_data.evse_voltage_limit_achieved = (int)0;
        init_physical_value(&ctx->evse_v2g_data.evse_minimum_current_limit, iso1unitSymbolType_A);
        init_physical_value(&ctx->evse_v2g_data.evse_minimum_voltage_limit, iso1unitSymbolType_V);
        init_physical_value(&ctx->evse_v2g_data.evse_peak_current_ripple, iso1unitSymbolType_A);
        // AC evse power values
        init_physical_value(&ctx->evse_v2g_data.evse_nominal_voltage, iso1unitSymbolType_V);
        ctx->evse_v2g_data.rcd = (int)0; // 0 if RCD has not detected an error
        ctx->contactor_is_closed = false;

        ctx->evse_v2g_data.payment_option_list[0] = iso1paymentOptionType_ExternalPayment;
        ctx->evse_v2g_data.payment_option_list_len = (uint8_t)1; // One option must be set

        ctx->evse_v2g_data.evse_service_list[0].FreeService = (int)0;
        ctx->evse_v2g_data.evse_service_list[0].ServiceID =
            4; // 4 (UseCaseInformation) A list containing information on all other services than charging services. The
               // EVCC and the SECC shall use the ServiceIDs in the range from 1 to 4 as defined in this
        // ctx->evse_v2g_data.evse_service_list[0].ServiceCategory Not needed at the moment, because it is a fixed value
        // in din and iso
    }

    init_physical_value(&ctx->evse_v2g_data.evse_present_voltage, iso1unitSymbolType_V);
    init_physical_value(&ctx->evse_v2g_data.evse_present_current, iso1unitSymbolType_A);

    if (ctx->hlc_pause_active != true) {
        // SAScheduleTupleID#PMaxScheduleTupleID#Start#Duration#PMax#
        init_physical_value(&ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
                                 .PMaxSchedule.PMaxScheduleEntry.array[0]
                                 .PMax,
                            iso1unitSymbolType_W);
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
            .PMaxSchedule.PMaxScheduleEntry.array[0]
            .RelativeTimeInterval.duration = (uint32_t)0;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
            .PMaxSchedule.PMaxScheduleEntry.array[0]
            .RelativeTimeInterval.duration_isUsed = (unsigned int)1;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
            .PMaxSchedule.PMaxScheduleEntry.array[0]
            .RelativeTimeInterval.start = (uint32_t)0;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
            .PMaxSchedule.PMaxScheduleEntry.array[0]
            .RelativeTimeInterval_isUsed = (unsigned int)1; // Optional: In DIN/ISO it must be set to 1
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0]
            .PMaxSchedule.PMaxScheduleEntry.array[0]
            .TimeInterval_isUsed = (unsigned int)0;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0].PMaxSchedule.PMaxScheduleEntry.arrayLen = 1;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0].SalesTariff_isUsed = (unsigned int)0;
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0].SAScheduleTupleID =
            (uint8_t)1; // [V2G2-773]  1 to 255
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.arrayLen = (uint16_t)1;
        ctx->evse_v2g_data.evse_sa_schedule_list_is_used = false;

        // ctx->evse_v2g_data.evseSAScheduleTuple.SalesTariff
        ctx->evse_v2g_data.evse_sa_schedule_list.SAScheduleTuple.array[0].SalesTariff_isUsed =
            (unsigned int)0; // Not supported in DIN
    } else {
        ctx->evse_v2g_data.evse_sa_schedule_list_is_used = true;
    }
    if (ctx->evse_v2g_data.cert_install_res_b64_buffer.empty() == false) {
        ctx->evse_v2g_data.cert_install_res_b64_buffer.clear();
    }

    // AC paramter
    ctx->evse_v2g_data.rcd = (int)0; // 0 if RCD has not detected an error
    ctx->contactor_is_closed = false;
    ctx->evse_v2g_data.receipt_required = (int)0;

    // Specific SAE J2847 bidi values
    ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2g = false;
    ctx->evse_v2g_data.sae_bidi_data.enabled_sae_v2h = false;
    ctx->evse_v2g_data.sae_bidi_data.sae_v2h_minimal_soc = 20;
    ctx->evse_v2g_data.sae_bidi_data.discharging = false;

    // Init EV received v2g-data to an invalid state
    memset(&ctx->ev_v2g_data, 0xff, sizeof(ctx->ev_v2g_data));

    /* Init session values */
    if (ctx->hlc_pause_active != true) {
        ctx->session.iso_selected_payment_option = iso1paymentOptionType_ExternalPayment;
    } else {
        ctx->evse_v2g_data.payment_option_list[0] = ctx->session.iso_selected_payment_option;
        ctx->evse_v2g_data.payment_option_list_len = (uint8_t)1; // One option must be set
    }
    memset(ctx->session.gen_challenge, 0, sizeof(ctx->session.gen_challenge));

    initialize_once = true;
}

struct v2g_context* v2g_ctx_create(ISO15118_chargerImplBase* p_chargerImplBase, evse_securityIntf* r_security) {
    struct v2g_context* ctx;

    ctx = static_cast<v2g_context*>(calloc(1, sizeof(*ctx)));
    if (!ctx)
        return NULL;

    ctx->r_security = r_security;
    ctx->p_charger = p_chargerImplBase;

    ctx->tls_security = TLS_SECURITY_PROHIBIT; // default

    /* This evse parameter will be initialized once */
    ctx->basic_config.evse_ac_current_limit = 0.0f;

    ctx->local_tcp_addr = NULL;
    ctx->local_tcp_addr = NULL;

    ctx->is_dc_charger = true;

    v2g_ctx_init_charging_session(ctx, true);

    /* interface from config file or options */
    ctx->if_name = "eth1";

    ctx->network_read_timeout = 1000;
    ctx->network_read_timeout_tls = 5000;

    ctx->sdp_socket = -1;
    ctx->tcp_socket = -1;
    ctx->tls_socket.fd = -1;
    memset(&ctx->tls_log_ctx, 0, sizeof(keylogDebugCtx));
    ctx->tls_key_logging = false;
    ctx->debugMode = false;

    /* according to man page, both functions never return an error */
    evthread_use_pthreads();
    pthread_mutex_init(&ctx->mqtt_lock, NULL);
    pthread_condattr_init(&ctx->mqtt_attr);
    pthread_condattr_setclock(&ctx->mqtt_attr, CLOCK_MONOTONIC);
    pthread_cond_init(&ctx->mqtt_cond, &ctx->mqtt_attr);

    ctx->event_base = event_base_new();
    if (!ctx->event_base) {
        dlog(DLOG_LEVEL_ERROR, "event_base_new failed");
        goto free_out;
    }

    if (v2g_ctx_start_events(ctx) != 0)
        goto free_out;

    ctx->com_setup_timeout = NULL;

    ctx->hlc_pause_active = false;

    return ctx;

free_out:
    if (ctx->event_base) {
        event_base_loopbreak(ctx->event_base);
        event_base_free(ctx->event_base);
    }
    free(ctx->local_tls_addr);
    free(ctx->local_tcp_addr);
    free(ctx);
    return NULL;
}

static void v2g_ctx_free_tls(struct v2g_context* ctx) {
    mbedtls_net_free(&ctx->tls_socket);

    for (uint8_t idx = 0; idx < ctx->num_of_tls_crt; idx++) {
        mbedtls_pk_free(&ctx->evse_tls_crt_key[idx]);
        mbedtls_x509_crt_free(&ctx->evseTlsCrt[idx]);
    }

    free(ctx->evseTlsCrt);
    ctx->evseTlsCrt = NULL;
    free(ctx->evse_tls_crt_key);
    ctx->evse_tls_crt_key = NULL;

    mbedtls_x509_crt_free(&ctx->v2g_root_crt);
    mbedtls_ssl_config_free(&ctx->ssl_config);

    if (NULL != ctx->tls_log_ctx.file) {
        fclose(ctx->tls_log_ctx.file);
        memset(&ctx->tls_log_ctx, 0, sizeof(ctx->tls_log_ctx));
    }
}

void v2g_ctx_free(struct v2g_context* ctx) {
    if (ctx->event_base) {
        event_base_loopbreak(ctx->event_base);
        event_base_free(ctx->event_base);
    }

    pthread_cond_destroy(&ctx->mqtt_cond);
    pthread_mutex_destroy(&ctx->mqtt_lock);

    v2g_ctx_free_tls(ctx);

    free(ctx->local_tls_addr);
    ctx->local_tls_addr = NULL;
    free(ctx->local_tcp_addr);
    ctx->local_tcp_addr = NULL;
    free(ctx);
}

void stop_timer(struct event** event_timer, char const* const timer_name, struct v2g_context* ctx) {
    pthread_mutex_lock(&ctx->mqtt_lock);
    if (NULL != *event_timer) {
        event_free(*event_timer);
        *event_timer = NULL; // Reset timer pointer
        if (NULL != timer_name) {
            dlog(DLOG_LEVEL_TRACE, "%s stopped", (timer_name == NULL) ? "Timer" : timer_name);
        }
    }
    pthread_mutex_unlock(&ctx->mqtt_lock);
}

void publish_DC_EVMaximumLimits(struct v2g_context* ctx, const float& v2g_dc_ev_max_current_limit,
                                const unsigned int& v2g_dc_ev_max_current_limit_is_used,
                                const float& v2g_dc_ev_max_power_limit,
                                const unsigned int& v2g_dc_ev_max_power_limit_is_used,
                                const float& v2g_dc_ev_max_voltage_limit,
                                const unsigned int& v2g_dc_ev_max_voltage_limit_is_used) {
    types::iso15118_charger::DC_EVMaximumLimits DC_EVMaximumLimits;
    bool publish_message = false;

    if (v2g_dc_ev_max_current_limit_is_used == (unsigned int)1) {
        DC_EVMaximumLimits.DC_EVMaximumCurrentLimit = v2g_dc_ev_max_current_limit;
        if (ctx->ev_v2g_data.ev_maximum_current_limit != DC_EVMaximumLimits.DC_EVMaximumCurrentLimit.value()) {
            ctx->ev_v2g_data.ev_maximum_current_limit = v2g_dc_ev_max_current_limit;
            publish_message = true;
        }
    }
    if (v2g_dc_ev_max_power_limit_is_used == (unsigned int)1) {
        DC_EVMaximumLimits.DC_EVMaximumPowerLimit = v2g_dc_ev_max_power_limit;
        if (ctx->ev_v2g_data.ev_maximum_power_limit != v2g_dc_ev_max_power_limit) {
            ctx->ev_v2g_data.ev_maximum_power_limit = v2g_dc_ev_max_power_limit;
            publish_message = true;
        }
    }
    if (v2g_dc_ev_max_voltage_limit_is_used == (unsigned int)1) {
        DC_EVMaximumLimits.DC_EVMaximumVoltageLimit = v2g_dc_ev_max_voltage_limit;
        if (ctx->ev_v2g_data.ev_maximum_voltage_limit != DC_EVMaximumLimits.DC_EVMaximumVoltageLimit.value()) {
            ctx->ev_v2g_data.ev_maximum_voltage_limit = v2g_dc_ev_max_voltage_limit;
            publish_message = true;
        }
    }

    if (publish_message == true) {
        ctx->p_charger->publish_DC_EVMaximumLimits(DC_EVMaximumLimits);
    }
}

void publish_DC_EVTargetVoltageCurrent(struct v2g_context* ctx, const float& v2g_dc_ev_target_voltage,
                                       const float& v2g_dc_ev_target_current) {
    if ((ctx->ev_v2g_data.v2g_target_voltage != v2g_dc_ev_target_voltage) ||
        (ctx->ev_v2g_data.v2g_target_current != v2g_dc_ev_target_current)) {
        types::iso15118_charger::DC_EVTargetValues DC_EVTargetValues;
        DC_EVTargetValues.DC_EVTargetVoltage = v2g_dc_ev_target_voltage;
        DC_EVTargetValues.DC_EVTargetCurrent = v2g_dc_ev_target_current;

        ctx->ev_v2g_data.v2g_target_voltage = v2g_dc_ev_target_voltage;
        ctx->ev_v2g_data.v2g_target_current = v2g_dc_ev_target_current;

        ctx->p_charger->publish_DC_EVTargetVoltageCurrent(DC_EVTargetValues);
    }
}

void publish_DC_EVRemainingTime(struct v2g_context* ctx, const float& v2g_dc_ev_remaining_time_to_full_soc,
                                const unsigned int& v2g_dc_ev_remaining_time_to_full_soc_is_used,
                                const float& v2g_dc_ev_remaining_time_to_bulk_soc,
                                const unsigned int& v2g_dc_ev_remaining_time_to_bulk_soc_is_used) {
    types::iso15118_charger::DC_EVRemainingTime DC_EVRemainingTime;
    const char* format = "%Y-%m-%dT%H:%M:%SZ";
    char buffer[100];
    std::time_t time_now_in_sec = time(NULL);
    bool publish_message = false;

    if (v2g_dc_ev_remaining_time_to_full_soc_is_used == (unsigned int)1) {
        if (ctx->ev_v2g_data.remaining_time_to_full_soc != v2g_dc_ev_remaining_time_to_full_soc) {
            std::time_t time_to_full_soc = time_now_in_sec + v2g_dc_ev_remaining_time_to_full_soc;
            std::strftime(buffer, sizeof(buffer), format, std::gmtime(&time_to_full_soc));
            DC_EVRemainingTime.EV_RemainingTimeToFullSoC = std::string(buffer);
            ctx->ev_v2g_data.remaining_time_to_full_soc = v2g_dc_ev_remaining_time_to_full_soc;
            publish_message = true;
        }
    }
    if (v2g_dc_ev_remaining_time_to_bulk_soc_is_used == (unsigned int)1) {
        if (ctx->ev_v2g_data.remaining_time_to_bulk_soc != v2g_dc_ev_remaining_time_to_bulk_soc) {
            std::time_t time_to_bulk_soc = time_now_in_sec + v2g_dc_ev_remaining_time_to_bulk_soc;
            std::strftime(buffer, sizeof(buffer), format, std::gmtime(&time_to_bulk_soc));
            DC_EVRemainingTime.EV_RemainingTimeToBulkSoC = std::string(buffer);
            ctx->ev_v2g_data.remaining_time_to_bulk_soc = v2g_dc_ev_remaining_time_to_bulk_soc;
            publish_message = true;
        }
    }

    if (publish_message == true) {
        ctx->p_charger->publish_DC_EVRemainingTime(DC_EVRemainingTime);
    }
}

/*!
 * \brief log_selected_energy_transfer_type This function prints the selected energy transfer mode.
 * \param selected_energy_transfer_mode is the selected energy transfer mode
 */
void log_selected_energy_transfer_type(int selected_energy_transfer_mode) {
    if (selected_energy_transfer_mode >= iso1EnergyTransferModeType_AC_single_phase_core &&
        selected_energy_transfer_mode <= iso1EnergyTransferModeType_DC_unique) {
        dlog(DLOG_LEVEL_INFO, "Selected energy transfer mode: %s",
             selected_energy_transfer_mode_string[selected_energy_transfer_mode]);
    } else {
        dlog(DLOG_LEVEL_WARNING, "Selected energy transfer mode %d is invalid", selected_energy_transfer_mode);
    }
}

bool add_service_to_service_list(struct v2g_context* v2g_ctx, const struct iso1ServiceType& evse_service,
                                 const int16_t* parameter_set_id, uint8_t parameter_set_id_len) {

    uint8_t write_idx = 0;
    bool service_found = false;

    /* Try to find service in service list */
    for (uint8_t idx = 0; idx < v2g_ctx->evse_v2g_data.evse_service_list_len; idx++) {
        if (v2g_ctx->evse_v2g_data.evse_service_list[idx].ServiceID == evse_service.ServiceID) {
            write_idx = idx;
            service_found = true;
            break;
        }
    }

    if (service_found == false &&
        (v2g_ctx->evse_v2g_data.evse_service_list_len < iso1ServiceListType_Service_ARRAY_SIZE)) {
        write_idx = v2g_ctx->evse_v2g_data.evse_service_list_len;
        v2g_ctx->evse_v2g_data.evse_service_list_len++;
    } else if (v2g_ctx->evse_v2g_data.evse_service_list_len == iso1ServiceListType_Service_ARRAY_SIZE) {
        dlog(DLOG_LEVEL_ERROR, "Maximum service list size reached. Unable to add service ID %u",
             evse_service.ServiceID);
        return false;
    }

    // Write service to the service list
    v2g_ctx->evse_v2g_data.evse_service_list[write_idx] = evse_service;

    // Configure parameter-set-id if requiered
    for (uint8_t idx = 0; idx < parameter_set_id_len; idx++) {
        configure_parameter_set(&v2g_ctx->evse_v2g_data.service_parameter_list[write_idx], parameter_set_id[idx],
                                evse_service.ServiceID);
    }

    return true;
}

void configure_parameter_set(struct iso1ServiceParameterListType* parameterSetList, int16_t parameterSetId,
                             uint16_t serviceId) {

    bool parameter_set_id_found = false;
    uint8_t write_idx = 0;
    for (uint8_t idx = 0; idx < parameterSetList->ParameterSet.arrayLen; idx++) {
        if (parameterSetList->ParameterSet.array[idx].ParameterSetID == parameterSetId) {
            parameter_set_id_found = true;
            write_idx = idx;
            break;
        }
    }
    if ((parameter_set_id_found == false) &&
        (parameterSetList->ParameterSet.arrayLen < iso1ServiceParameterListType_ParameterSet_ARRAY_SIZE)) {
        write_idx = parameterSetList->ParameterSet.arrayLen;
        parameterSetList->ParameterSet.arrayLen++;
    } else if (parameterSetList->ParameterSet.arrayLen == iso1ServiceParameterListType_ParameterSet_ARRAY_SIZE) {
        dlog(DLOG_LEVEL_ERROR, "Maximum parameter-set list size reached. Unable to add parameter-set-ID %d",
             parameterSetId);
        return;
    }

    /* Get an free parameter-set-entry */
    struct iso1ParameterSetType* parameterSet = &parameterSetList->ParameterSet.array[write_idx];
    parameterSet->ParameterSetID = parameterSetId;
    if (serviceId == 2) {
        /* Configure parameter-set-ID of the certificate service */
        /* Service to install a Contract Certificate (Ref. Table 106 —
         * ServiceParameterList for certificate service) */
        if (parameterSet->ParameterSetID == 1) {
            /* Configure parameter name */
            strcpy(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.characters, "Service");
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.charactersLen =
                std::string(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.characters).size();
            /* Configure parameter value */
            strcpy(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue.characters,
                   "Installation");
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue.charactersLen =
                std::string(parameterSet->Parameter.array[write_idx].stringValue.characters).size();
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue_isUsed = 1;
            parameterSet->Parameter.arrayLen = 1;
        }
        /* Service to update a Contract Certificate */
        else if (parameterSet->ParameterSetID == 2) {
            /* Configure parameter name */
            strcpy(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.characters, "Service");
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.charactersLen =
                std::string(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].Name.characters).size();
            /* Configure parameter value */
            strcpy(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue.characters, "Update");
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue.charactersLen =
                std::string(parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue.characters)
                    .size();
            parameterSet->Parameter.array[parameterSet->Parameter.arrayLen].stringValue_isUsed = 1;
            parameterSet->Parameter.arrayLen = 1;
        }
    } else {
        dlog(DLOG_LEVEL_WARNING, "Parameter-set-ID of service ID %u is not supported", serviceId);
    }
}
