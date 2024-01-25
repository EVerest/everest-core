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
#include <queue>

#include <generated/interfaces/ISO15118_charger/Interface.hpp>
#include <generated/interfaces/ac_rcd/Interface.hpp>
#include <generated/interfaces/connector_lock/Interface.hpp>
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/evse_manager/Implementation.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <sigslot/signal.hpp>

#include "Timeout.hpp"
#include "utils/thread.hpp"

namespace module {

// bit field to track which errors are active at the moment. Only tracks errors that stop charging.
struct ActiveErrors {

    struct bsp_errors {
        std::atomic_bool DiodeFault{false};
        std::atomic_bool VentilationNotAvailable{false};
        std::atomic_bool BrownOut{false};
        std::atomic_bool EnergyManagement{false};
        std::atomic_bool PermanentFault{false};
        std::atomic_bool MREC2GroundFailure{false};
        std::atomic_bool MREC4OverCurrentFailure{false};
        std::atomic_bool MREC5OverVoltage{false};
        std::atomic_bool MREC6UnderVoltage{false};
        std::atomic_bool MREC8EmergencyStop{false};
        std::atomic_bool MREC10InvalidVehicleMode{false};
        std::atomic_bool MREC14PilotFault{false};
        std::atomic_bool MREC15PowerLoss{false};
        std::atomic_bool MREC17EVSEContactorFault{false};
        std::atomic_bool MREC19CableOverTempStop{false};
        std::atomic_bool MREC20PartialInsertion{false};
        std::atomic_bool MREC23ProximityFault{false};
        std::atomic_bool MREC24ConnectorVoltageHigh{false};
        std::atomic_bool MREC25BrokenLatch{false};
        std::atomic_bool MREC26CutCable{false};
        std::atomic_bool VendorError{false};
    } bsp;

    struct evse_manager_errors {
        std::atomic_bool MREC4OverCurrentFailure{false};
        std::atomic_bool Internal{false};
        std::atomic_bool PowermeterTransactionStartFailed{false};
    } evse_manager;

    struct ac_rcd_errors {
        std::atomic_bool MREC2GroundFailure{false};
        std::atomic_bool VendorError{false};
        std::atomic_bool Selftest{false};
        std::atomic_bool AC{false};
        std::atomic_bool DC{false};
    } ac_rcd;

    struct connector_lock_errors {
        std::atomic_bool ConnectorLockCapNotCharged{false};
        std::atomic_bool ConnectorLockUnexpectedOpen{false};
        std::atomic_bool ConnectorLockUnexpectedClose{false};
        std::atomic_bool ConnectorLockFailedLock{false};
        std::atomic_bool ConnectorLockFailedUnlock{false};
        std::atomic_bool MREC1ConnectorLockFailure{false};
        std::atomic_bool VendorError{false};
    } connector_lock;

    bool all_cleared() {
        return not(bsp.DiodeFault or bsp.VentilationNotAvailable or bsp.BrownOut or bsp.EnergyManagement or
                   bsp.PermanentFault or bsp.MREC2GroundFailure or bsp.MREC4OverCurrentFailure or
                   bsp.MREC5OverVoltage or bsp.MREC6UnderVoltage or bsp.MREC8EmergencyStop or
                   bsp.MREC10InvalidVehicleMode or bsp.MREC14PilotFault or bsp.MREC15PowerLoss or
                   bsp.MREC17EVSEContactorFault or bsp.MREC19CableOverTempStop or bsp.MREC20PartialInsertion or
                   bsp.MREC23ProximityFault or bsp.MREC24ConnectorVoltageHigh or bsp.MREC25BrokenLatch or
                   bsp.MREC26CutCable or evse_manager.MREC4OverCurrentFailure or evse_manager.Internal or
                   evse_manager.PowermeterTransactionStartFailed or ac_rcd.MREC2GroundFailure or ac_rcd.VendorError or
                   ac_rcd.Selftest or ac_rcd.AC or ac_rcd.DC or connector_lock.ConnectorLockCapNotCharged or
                   connector_lock.ConnectorLockUnexpectedOpen or connector_lock.ConnectorLockUnexpectedClose or
                   connector_lock.ConnectorLockFailedLock or connector_lock.ConnectorLockFailedUnlock or
                   connector_lock.MREC1ConnectorLockFailure or connector_lock.VendorError);
    }
};

class ErrorHandling {
public:
    // We need the r_bsp reference to be able to talk to the bsp driver module
    explicit ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& r_bsp,
                           const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& r_hlc,
                           const std::vector<std::unique_ptr<connector_lockIntf>>& r_connector_lock,
                           const std::vector<std::unique_ptr<ac_rcdIntf>>& r_ac_rcd,
                           const std::unique_ptr<evse_managerImplBase>& _p_evse);

    // Signal that one error has been raised. Bool argument is true if it preventing charging.
    sigslot::signal<types::evse_manager::Error, bool> signal_error;
    // Signal that one error has been cleared. Bool argument is true if it was preventing charging.
    sigslot::signal<types::evse_manager::Error, bool> signal_error_cleared;
    // Signal that all errors are cleared (both those preventing charging and not)
    sigslot::signal<> signal_all_errors_cleared;

    void raise_overcurrent_error(const std::string& description);
    void clear_overcurrent_error();

    void raise_internal_error(const std::string& description);
    void clear_internal_error();

    void raise_powermeter_transaction_start_failed_error(const std::string& description);
    void clear_powermeter_transaction_start_failed_error();

private:
    const std::unique_ptr<evse_board_supportIntf>& r_bsp;
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& r_hlc;
    const std::vector<std::unique_ptr<connector_lockIntf>>& r_connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>>& r_ac_rcd;
    const std::unique_ptr<evse_managerImplBase>& p_evse;

    bool modify_error_bsp(const Everest::error::Error& error, bool active, types::evse_manager::ErrorEnum& evse_error);
    bool modify_error_connector_lock(const Everest::error::Error& error, bool active,
                                     types::evse_manager::ErrorEnum& evse_error);
    bool modify_error_ac_rcd(const Everest::error::Error& error, bool active,
                             types::evse_manager::ErrorEnum& evse_error);

    bool modify_error_evse_manager(const std::string& error_type, bool active,
                                   types::evse_manager::ErrorEnum& evse_error);
    bool hlc{false};

    ActiveErrors active_errors;
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
