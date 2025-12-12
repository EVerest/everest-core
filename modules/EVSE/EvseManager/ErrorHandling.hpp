// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

/*
 The ErrorHandling class handles all errors from BSP/ConnectorLock etc
 and classifies them for charging:

 If we can continue charging despite the error, we treat them as warnings and do not track them here. If e.g. the BSP
 raises a high temperature error it needs to limit the output power by itself.

 The decision whether an error requires a replug or not to clear depends on the reporting module. It will need to clear
 them at the appropriate time.
*/

#ifndef SRC_ERROR_HANDLING_H_
#define SRC_ERROR_HANDLING_H_

#include "ld-ev.hpp"

#include <chrono>
#include <mutex>
#include <optional>
#include <queue>

#include <generated/interfaces/ISO15118_charger/Interface.hpp>
#include <generated/interfaces/ac_rcd/Interface.hpp>
#include <generated/interfaces/connector_lock/Interface.hpp>
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/evse_manager/Implementation.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/isolation_monitor/Interface.hpp>
#include <generated/interfaces/over_voltage_monitor/Interface.hpp>
#include <generated/interfaces/power_supply_DC/Interface.hpp>
#include <generated/interfaces/powermeter/Interface.hpp>
#include <sigslot/signal.hpp>

#include "Timeout.hpp"
#include "utils/thread.hpp"

namespace module {

class EvseManager;

// ErrorHandling events
enum class ErrorHandlingEvents : std::uint8_t {
    ForceErrorShutdown,
    ForceEmergencyShutdown,
    AllErrorsPreventingChargingCleared,
    AllErrorCleared
};

enum class ShutdownType {
    None,
    ErrorShutdown,
    EmergencyShutdown
};

class ErrorHandling {

public:
    // We need the r_bsp reference to be able to talk to the bsp driver module
    explicit ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& r_bsp,
                           const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& r_hlc,
                           const std::vector<std::unique_ptr<connector_lockIntf>>& r_connector_lock,
                           const std::vector<std::unique_ptr<ac_rcdIntf>>& r_ac_rcd,
                           const std::unique_ptr<evse_managerImplBase>& _p_evse,
                           const std::vector<std::unique_ptr<isolation_monitorIntf>>& _r_imd,
                           const std::vector<std::unique_ptr<power_supply_DCIntf>>& _r_powersupply,
                           const std::vector<std::unique_ptr<powermeterIntf>>& _r_powermeter,
                           const std::vector<std::unique_ptr<over_voltage_monitorIntf>>& _r_over_voltage_monitor,
                           bool _inoperative_error_use_vendor_id);

    // Signal that error set has changed. Bool argument is true if it is preventing charging at the moment and false if
    // charging can continue.
    sigslot::signal<ErrorHandlingEvents> signal_error;
    // Signal that all errors are cleared (both those preventing charging and not)
    sigslot::signal<> signal_all_errors_cleared;

    void raise_overcurrent_error(const std::string& description);
    void clear_overcurrent_error();

    void raise_internal_error(const std::string& description);
    void clear_internal_error();

    void raise_authorization_timeout_error(const std::string& description);
    void clear_authorization_timeout_error();

    void raise_powermeter_transaction_start_failed_error(const std::string& description);
    void clear_powermeter_transaction_start_failed_error();

    void raise_isolation_resistance_fault(const std::string& description, const std::string& sub_type);
    void clear_isolation_resistance_fault(const std::string& sub_type);

    void raise_cable_check_fault(const std::string& description);
    void clear_cable_check_fault();

protected:
    void raise_inoperative_error(const Everest::error::Error& caused_by);

private:
    void process_error();
    void clear_inoperative_error();
    std::optional<Everest::error::Error> errors_prevent_charging();

    std::mutex error_mutex;

    const std::unique_ptr<evse_board_supportIntf>& r_bsp;
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& r_hlc;
    const std::vector<std::unique_ptr<connector_lockIntf>>& r_connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>>& r_ac_rcd;
    const std::unique_ptr<evse_managerImplBase>& p_evse;
    const std::vector<std::unique_ptr<isolation_monitorIntf>>& r_imd;
    const std::vector<std::unique_ptr<power_supply_DCIntf>>& r_powersupply;
    const std::vector<std::unique_ptr<powermeterIntf>>& r_powermeter;
    const std::vector<std::unique_ptr<over_voltage_monitorIntf>>& r_over_voltage_monitor;
    const bool inoperative_error_use_vendor_id;
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
