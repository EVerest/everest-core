// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SCOPED_LOCK_TIMEOUT
#define SCOPED_LOCK_TIMEOUT

#include "everest/exceptions.hpp"
#include "everest/logging.hpp"
#include <mutex>
#include <signal.h>

#include "backtrace.hpp"

/*
 Simple helper class for scoped lock with timeout
*/
namespace Everest {

enum class MutexDescription {
    Undefined,
    Charger_signal_loop,
    Charger_signal_error,
    Charger_signal_error_cleared,
    Charger_mainloop,
    Charger_process_event,
    Charger_pause_charging,
    Charger_resume_charging,
    Charger_waiting_for_power,
    Charger_resume_power_available,
    Charger_cancel_transaction,
    Charger_setup,
    Charger_get_current_state,
    Charger_get_authorized_pnc,
    Charger_get_authorized_eim,
    Charger_get_authorized_pnc_ready_for_hlc,
    Charger_get_authorized_eim_ready_for_hlc,
    Charger_authorize,
    Charger_deauthorize,
    Charger_disable,
    Charger_enable,
    Charger_set_faulted,
    Charger_get_max_current,
    Charger_set_current_drawn_by_vehicle,
    Charger_request_error_sequence,
    Charger_set_matching_started,
    Charger_notify_currentdemand_started,
    Charger_inform_new_evse_max_hlc_limits,
    Charger_get_evse_max_hlc_limits,
    Charger_dlink_pause,
    Charger_dlink_terminate,
    Charger_dlink_error,
    Charger_set_hlc_charging_active,
    Charger_set_hlc_allow_close_contactor,
    Charger_set_hlc_error,
    Charger_errors_prevent_charging,
    IEC_process_bsp_event,
    IEC_state_machine,
    IEC_set_pwm,
    IEC_set_pwm_off,
    IEC_set_pwm_F,
    IEC_allow_power_on,
    IEC_force_unlock,
    EVSE_set_ev_info,
    EVSE_publish_ev_info,
    EVSE_subscribe_DC_EVMaximumLimits,
    EVSE_subscribe_DepartureTime,
    EVSE_subscribe_AC_EAmount,
    EVSE_subscribe_AC_EVMaxVoltage,
    EVSE_subscribe_AC_EVMaxCurrent,
    EVSE_subscribe_AC_EVMinCurrent,
    EVSE_subscribe_DC_EVEnergyCapacity,
    EVSE_subscribe_DC_EVEnergyRequest,
    EVSE_subscribe_DC_FullSOC,
    EVSE_subscribe_DC_BulkSOC,
    EVSE_subscribe_DC_EVRemainingTime,
    EVSE_subscribe_DC_EVStatus,
    EVSE_subscribe_Require_Auth_EIM,
    EVSE_publish_provided_token,
    EVSE_subscribe_EVCCIDD,
    EVSE_subscribe_Require_Auth_PnC,
    EVSE_subscribe_Require_Auth_PnC2,
    EVSE_signal_event,
    EVSE_subscribe_powermeter,
    EVSE_get_latest_powermeter_data_billing,
    EVSE_get_reservation_id,
    EVSE_reserve,
    EVSE_cancel_reservation,
    EVSE_is_reserved,
    EVSE_get_hlc_enabled,
    EVSE_get_hlc_waiting_for_auth_pnc,
    EVSE_charger_was_authorized,
    EVSE_get_ev_info
};

static std::string to_string(MutexDescription d) {
    switch (d) {
    case MutexDescription::Charger_signal_loop:
        return "Charger.cpp: error_handling->signal_loop";
    case MutexDescription::Charger_signal_error:
        return "Charger.cpp: error_handling->signal_error";
    case MutexDescription::Charger_signal_error_cleared:
        return "Charger.cpp: error_handling->signal_all_errors_cleared";
    case MutexDescription::Charger_mainloop:
        return "Charger.cpp: mainloop";
    case MutexDescription::Charger_process_event:
        return "Charger.cpp: process_event";
    case MutexDescription::Charger_pause_charging:
        return "Charger.cpp: pause_charging";
    case MutexDescription::Charger_resume_charging:
        return "Charger.cpp: resume_charging";
    case MutexDescription::Charger_waiting_for_power:
        return "Charger.cpp: pause_charging_wait_for_power";
    case MutexDescription::Charger_resume_power_available:
        return "Charger.cpp: resume_charging_power_available";
    case MutexDescription::Charger_cancel_transaction:
        return "Charger.cpp: cancel_transaction";
    case MutexDescription::Charger_setup:
        return "Charger.cpp: setup";
    case MutexDescription::Charger_get_current_state:
        return "Charger.cpp: get_current_state";
    case MutexDescription::Charger_get_authorized_pnc:
        return "Charger.cpp: get_authorized_pnc";
    case MutexDescription::Charger_get_authorized_eim:
        return "Charger.cpp: get_authorized_eim";
    case MutexDescription::Charger_get_authorized_pnc_ready_for_hlc:
        return "Charger.cpp: get_authorized_pnc_ready_for_hlc";
    case MutexDescription::Charger_get_authorized_eim_ready_for_hlc:
        return "Charger.cpp: get_authorized_eim_ready_for_hlc";
    case MutexDescription::Charger_authorize:
        return "Charger.cpp: authorize";
    case MutexDescription::Charger_deauthorize:
        return "Charger.cpp: deauthorize";
    case MutexDescription::Charger_disable:
        return "Charger.cpp: disable";
    case MutexDescription::Charger_enable:
        return "Charger.cpp: enable";
    case MutexDescription::Charger_set_faulted:
        return "Charger.cpp: set_faulted";
    case MutexDescription::Charger_get_max_current:
        return "Charger.cpp: get_max_current";
    case MutexDescription::Charger_set_current_drawn_by_vehicle:
        return "Charger.cpp: set_current_drawn_by_vehicle";
    case MutexDescription::Charger_request_error_sequence:
        return "Charger.cpp: request_error_sequence";
    case MutexDescription::Charger_set_matching_started:
        return "Charger.cpp: set_matching_started";
    case MutexDescription::Charger_notify_currentdemand_started:
        return "Charger.cpp: notify_currentdemand_started";
    case MutexDescription::Charger_inform_new_evse_max_hlc_limits:
        return "Charger.cpp: inform_new_evse_max_hlc_limits";
    case MutexDescription::Charger_get_evse_max_hlc_limits:
        return "Charger.cpp: get_evse_max_hlc_limits";
    case MutexDescription::Charger_dlink_pause:
        return "Charger.cpp: dlink_pause";
    case MutexDescription::Charger_dlink_terminate:
        return "Charger.cpp: dlink_error";
    case MutexDescription::Charger_dlink_error:
        return "Charger.cpp: dlink_error";
    case MutexDescription::Charger_set_hlc_charging_active:
        return "Charger.cpp: set_hlc_charging_active";
    case MutexDescription::Charger_set_hlc_allow_close_contactor:
        return "Charger.cpp: set_hlc_allow_close_contactor";
    case MutexDescription::Charger_set_hlc_error:
        return "Charger.cpp: set_hlc_error";
    case MutexDescription::Charger_errors_prevent_charging:
        return "Charger.cpp: errors_prevent_charging";
    case MutexDescription::IEC_process_bsp_event:
        return "IECStateMachine::process_bsp_event";
    case MutexDescription::IEC_state_machine:
        return "IECStateMachine::state_machine";
    case MutexDescription::IEC_set_pwm:
        return "IECStateMachine::set_pwm";
    case MutexDescription::IEC_set_pwm_off:
        return "IECStateMachine::set_pwm_off";
    case MutexDescription::IEC_set_pwm_F:
        return "IECStateMachine::set_pwm_F";
    case MutexDescription::IEC_allow_power_on:
        return "IECStateMachine::allow_power_on";
    case MutexDescription::IEC_force_unlock:
        return "IECStateMachine::force_unlock";
    case MutexDescription::EVSE_set_ev_info:
        return "EvseManager.cpp: set ev_info present_voltage/current";
    case MutexDescription::EVSE_publish_ev_info:
        return "EvseManager.cpp: publish_ev_info";
    case MutexDescription::EVSE_subscribe_DC_EVMaximumLimits:
        return "EvseManager.cpp: subscribe_DC_EVMaximumLimits";
    case MutexDescription::EVSE_subscribe_DepartureTime:
        return "EvseManager.cpp: subscribe_DepartureTime";
    case MutexDescription::EVSE_subscribe_AC_EAmount:
        return "EvseManager.cpp: subscribe_AC_EAmount";
    case MutexDescription::EVSE_subscribe_AC_EVMaxVoltage:
        return "EvseManager.cpp: subscribe_AC_EVMaxVoltage";
    case MutexDescription::EVSE_subscribe_AC_EVMaxCurrent:
        return "EvseManager.cpp: subscribe_AC_EVMaxCurrent";
    case MutexDescription::EVSE_subscribe_AC_EVMinCurrent:
        return "EvseManager.cpp: subscribe_AC_EVMinCurrent";
    case MutexDescription::EVSE_subscribe_DC_EVEnergyCapacity:
        return "EvseManager.cpp: subscribe_DC_EVEnergyCapacity";
    case MutexDescription::EVSE_subscribe_DC_EVEnergyRequest:
        return "EvseManager.cpp: subscribe_DC_EVEnergyRequest";
    case MutexDescription::EVSE_subscribe_DC_FullSOC:
        return "EvseManager.cpp: subscribe_DC_FullSOC";
    case MutexDescription::EVSE_subscribe_DC_BulkSOC:
        return "EvseManager.cpp: subscribe_DC_BulkSOC";
    case MutexDescription::EVSE_subscribe_DC_EVRemainingTime:
        return "EvseManager.cpp: subscribe_DC_EVRemainingTime";
    case MutexDescription::EVSE_subscribe_DC_EVStatus:
        return "EvseManager.cpp subscribe_DC_EVStatus";
    case MutexDescription::EVSE_subscribe_Require_Auth_EIM:
        return "EvseManager.cpp: subscribe_Require_Auth_EIM";
    case MutexDescription::EVSE_publish_provided_token:
        return "EvseManager.cpp: publish_provided_token";
    case MutexDescription::EVSE_subscribe_EVCCIDD:
        return "EvseManager.cpp: subscribe_EVCCIDD";
    case MutexDescription::EVSE_subscribe_Require_Auth_PnC:
        return "EvseManager.cpp: subscribe_Require_Auth_PnC 1";
    case MutexDescription::EVSE_subscribe_Require_Auth_PnC2:
        return "EvseManager.cpp: subscribe_Require_Auth_PnC 2";
    case MutexDescription::EVSE_signal_event:
        return "EvseManager.cpp: bsp->signal_event.connect";
    case MutexDescription::EVSE_subscribe_powermeter:
        return "EvseManager.cpp: subscribe_powermeter";
    case MutexDescription::EVSE_get_latest_powermeter_data_billing:
        return "EvseManager.cpp: get_latest_powermeter_data_billing";
    case MutexDescription::EVSE_get_reservation_id:
        return "EvseManager.cpp: get_reservation_id";
    case MutexDescription::EVSE_reserve:
        return "EvseManager.cpp: reserve";
    case MutexDescription::EVSE_cancel_reservation:
        return "EvseManager.cpp: cancel_reservation";
    case MutexDescription::EVSE_is_reserved:
        return "EvseManager.cpp: is_reserved";
    case MutexDescription::EVSE_get_hlc_enabled:
        return "EvseManager.cpp: get_hlc_enabled";
    case MutexDescription::EVSE_get_hlc_waiting_for_auth_pnc:
        return "EvseManager.cpp: get_hlc_waiting_for_auth_pnc";
    case MutexDescription::EVSE_charger_was_authorized:
        return "EvseManager.cpp: charger_was_authorized";
    case MutexDescription::EVSE_get_ev_info:
        return "EvseManager.cpp: get_ev_info";
    }
    return "Undefined";
}

class timed_mutex_traceable : public std::timed_mutex {
#ifdef EVEREST_USE_BACKTRACES
public:
    MutexDescription description;
    pthread_t p_id;
#endif
};

template <typename mutex_type> class scoped_lock_timeout {
public:
    explicit scoped_lock_timeout(mutex_type& __m, MutexDescription description) : mutex(__m) {
        if (not mutex.try_lock_for(deadlock_timeout)) {
#ifdef EVEREST_USE_BACKTRACES
            request_backtrace(pthread_self());
            request_backtrace(mutex.p_id);
            // Give some time for other timeouts to report their state and backtraces
            std::this_thread::sleep_for(std::chrono::seconds(10));

            std::string different_thread;
            if (mutex.p_id not_eq pthread_self()) {
                different_thread = " from a different thread.";
            } else {
                different_thread = " from the same thread";
            }

            EVLOG_AND_THROW(EverestTimeoutError("Mutex deadlock detected: Failed to lock " + to_string(description) +
                                                ", mutex held by " + to_string(mutex.description) + different_thread));
#endif
        } else {
            locked = true;
#ifdef EVEREST_USE_BACKTRACES
            mutex.description = description;
            mutex.p_id = pthread_self();
#endif
        }
    }

    ~scoped_lock_timeout() {
        if (locked) {
            mutex.unlock();
        }
    }

    scoped_lock_timeout(const scoped_lock_timeout&) = delete;
    scoped_lock_timeout& operator=(const scoped_lock_timeout&) = delete;

private:
    bool locked{false};
    mutex_type& mutex;

    // This should be lower then command timeouts from framework (by default 300s)
    static constexpr auto deadlock_timeout = std::chrono::seconds(120);
};
} // namespace Everest

#endif
