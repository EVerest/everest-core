// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "ErrorHandling.hpp"

namespace module {

ErrorHandling::ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& _r_bsp,
                             const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& _r_hlc,
                             const std::vector<std::unique_ptr<connector_lockIntf>>& _r_connector_lock,
                             const std::vector<std::unique_ptr<ac_rcdIntf>>& _r_ac_rcd,
                             const std::unique_ptr<evse_managerImplBase>& _p_evse,
                             const std::vector<std::unique_ptr<isolation_monitorIntf>>& _r_imd,
                             const std::vector<std::unique_ptr<power_supply_DCIntf>>& _r_powersupply) :
    r_bsp(_r_bsp),
    r_hlc(_r_hlc),
    r_connector_lock(_r_connector_lock),
    r_ac_rcd(_r_ac_rcd),
    p_evse(_p_evse),
    r_imd(_r_imd),
    r_powersupply(_r_powersupply) {

    // Subscribe to bsp driver to receive Errors from the bsp hardware
    r_bsp->subscribe_all_errors([this](const Everest::error::Error& error) { process_error(); },
                                [this](const Everest::error::Error& error) { process_error(); });

    // Subscribe to connector lock to receive errors from connector lock hardware
    if (r_connector_lock.size() > 0) {
        r_connector_lock[0]->subscribe_all_errors([this](const Everest::error::Error& error) { process_error(); },
                                                  [this](const Everest::error::Error& error) { process_error(); });
    }

    // Subscribe to ac_rcd to receive errors from AC RCD hardware
    if (r_ac_rcd.size() > 0) {
        r_ac_rcd[0]->subscribe_all_errors([this](const Everest::error::Error& error) { process_error(); },
                                          [this](const Everest::error::Error& error) { process_error(); });
    }

    // Subscribe to ac_rcd to receive errors from IMD hardware
    if (r_imd.size() > 0) {
        r_imd[0]->subscribe_all_errors([this](const Everest::error::Error& error) { process_error(); },
                                       [this](const Everest::error::Error& error) { process_error(); });
    }

    // Subscribe to powersupply to receive errors from DC powersupply hardware
    if (r_powersupply.size() > 0) {
        r_powersupply[0]->subscribe_all_errors([this](const Everest::error::Error& error) { process_error(); },
                                               [this](const Everest::error::Error& error) { process_error(); });
    }
}

void ErrorHandling::raise_overcurrent_error(const std::string& description) {
    // raise externally
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/MREC4OverCurrentFailure", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);
    process_error();
}

void ErrorHandling::clear_overcurrent_error() {
    // clear externally
    if (p_evse->error_state_monitor->is_error_active("evse_manager/MREC4OverCurrentFailure", "")) {
        p_evse->clear_error("evse_manager/MREC4OverCurrentFailure", "");
    }
    process_error();
}

// Find out if the current error set is fatal to charging or not
void ErrorHandling::process_error() {
    if (errors_prevent_charging()) {
        // signal to charger a new error has been set that prevents charging
        raise_inoperative_error();
    } else {
        // signal an error that does not prevent charging
        clear_inoperative_error();
    }

    // All errors cleared signal is for OCPP 1.6. It is triggered when there are no errors anymore,
    // even those that did not block charging.
    const int error_count =
        p_evse->error_state_monitor->get_active_errors().size() +
        r_bsp->error_state_monitor->get_active_errors().size() +
        (r_connector_lock.size() > 0 ? r_connector_lock[0]->error_state_monitor->get_active_errors().size() : 0) +
        (r_ac_rcd.size() > 0 ? r_ac_rcd[0]->error_state_monitor->get_active_errors().size() : 0) +
        (r_imd.size() > 0 ? r_imd[0]->error_state_monitor->get_active_errors().size() : 0) +
        (r_powersupply.size() > 0 ? r_powersupply[0]->error_state_monitor->get_active_errors().size() : 0);

    if (error_count == 0) {
        signal_all_errors_cleared();
    }
}

// Check all errors from p_evse and all requirements to see if they block charging
bool ErrorHandling::errors_prevent_charging() {
    auto contains = [](auto v, auto e) {
        if (std::find(v.begin(), v.end(), e) != v.end()) {
            return true;
        } else {
            return false;
        }
    };

    auto is_fatal = [contains](auto errors, auto ignore_list) {
        for (const auto e : errors) {
            if (not contains(ignore_list, e->type)) {
                return true;
            }
        }
        return false;
    };

    if (is_fatal(p_evse->error_state_monitor->get_active_errors(), ignore_errors_evse)) {
        return true;
    }

    if (is_fatal(r_bsp->error_state_monitor->get_active_errors(), ignore_errors_bsp)) {
        return true;
    }

    if (r_connector_lock.size() > 0 and
        is_fatal(r_connector_lock[0]->error_state_monitor->get_active_errors(), ignore_errors_connector_lock)) {
        return true;
    }

    if (r_ac_rcd.size() > 0 and is_fatal(r_ac_rcd[0]->error_state_monitor->get_active_errors(), ignore_errors_ac_rcd)) {
        return true;
    }

    if (r_imd.size() > 0 and is_fatal(r_imd[0]->error_state_monitor->get_active_errors(), ignore_errors_imd)) {
        return true;
    }

    if (r_powersupply.size() > 0 and
        is_fatal(r_powersupply[0]->error_state_monitor->get_active_errors(), ignore_errors_powersupply)) {
        return true;
    }

    return false;
}

void ErrorHandling::raise_inoperative_error() {
    if (p_evse->error_state_monitor->is_error_active("evse_manager/Inoperative", "")) {
        // dont raise if already raised
        return;
    }

    if (r_hlc.size() > 0) {
        r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
    }

    // raise externally
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/Inoperative", "", "FIXME add some info for causing error", Everest::error::Severity::High);
    p_evse->raise_error(error_object);

    signal_error(true);
}

void ErrorHandling::clear_inoperative_error() {
    // clear externally
    if (p_evse->error_state_monitor->is_error_active("evse_manager/Inoperative", "")) {
        p_evse->clear_error("evse_manager/Inoperative");
        signal_error(false);
    }
}

void ErrorHandling::raise_internal_error(const std::string& description) {
    // raise externally
    Everest::error::Error error_object =
        p_evse->error_factory->create_error("evse_manager/Internal", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);
    process_error();
}

void ErrorHandling::clear_internal_error() {
    // clear externally
    if (p_evse->error_state_monitor->is_error_active("evse_manager/Internal", "")) {
        p_evse->clear_error("evse_manager/Internal");
        process_error();
    }
}

void ErrorHandling::raise_powermeter_transaction_start_failed_error(const std::string& description) {
    // raise externally
    Everest::error::Error error_object = p_evse->error_factory->create_error(
        "evse_manager/PowermeterTransactionStartFailed", "", description, Everest::error::Severity::High);
    p_evse->raise_error(error_object);
    process_error();
}

void ErrorHandling::clear_powermeter_transaction_start_failed_error() {
    // clear externally
    if (p_evse->error_state_monitor->is_error_active("evse_manager/PowermeterTransactionStartFailed", "")) {
        p_evse->clear_error("evse_manager/PowermeterTransactionStartFailed");
        process_error();
    }
}

} // namespace module
