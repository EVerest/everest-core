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

#ifdef BUILD_TESTING_MODULE_EVSE_MANAGER
#include <gtest/gtest_prod.h>
#endif

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

    inline bool bsp_any_set() {
        return bsp.DiodeFault || bsp.VentilationNotAvailable || bsp.BrownOut || bsp.EnergyManagement ||
               bsp.PermanentFault || bsp.MREC2GroundFailure || bsp.MREC4OverCurrentFailure || bsp.MREC5OverVoltage ||
               bsp.MREC6UnderVoltage || bsp.MREC8EmergencyStop || bsp.MREC10InvalidVehicleMode ||
               bsp.MREC14PilotFault || bsp.MREC15PowerLoss || bsp.MREC17EVSEContactorFault ||
               bsp.MREC19CableOverTempStop || bsp.MREC20PartialInsertion || bsp.MREC23ProximityFault ||
               bsp.MREC24ConnectorVoltageHigh || bsp.MREC25BrokenLatch || bsp.MREC26CutCable || bsp.VendorError;
    }

    struct evse_manager_errors {
        std::atomic_bool MREC4OverCurrentFailure{false};
        std::atomic_bool Internal{false};
        std::atomic_bool PowermeterTransactionStartFailed{false};
    } evse_manager;

    inline bool evse_manager_any_set() {
        return evse_manager.MREC4OverCurrentFailure || evse_manager.Internal ||
               evse_manager.PowermeterTransactionStartFailed;
    }

    struct ac_rcd_errors {
        std::atomic_bool MREC2GroundFailure{false};
        std::atomic_bool VendorError{false};
        std::atomic_bool Selftest{false};
        std::atomic_bool AC{false};
        std::atomic_bool DC{false};
    } ac_rcd;

    inline bool ac_rcd_any_set() {
        return ac_rcd.MREC2GroundFailure || ac_rcd.VendorError || ac_rcd.Selftest || ac_rcd.AC || ac_rcd.DC;
    }

    struct connector_lock_errors {
        std::atomic_bool ConnectorLockCapNotCharged{false};
        std::atomic_bool ConnectorLockUnexpectedOpen{false};
        std::atomic_bool ConnectorLockUnexpectedClose{false};
        std::atomic_bool ConnectorLockFailedLock{false};
        std::atomic_bool ConnectorLockFailedUnlock{false};
        std::atomic_bool MREC1ConnectorLockFailure{false};
        std::atomic_bool VendorError{false};
    } connector_lock;

    inline bool connector_lock_any_set() {
        return connector_lock.ConnectorLockCapNotCharged || connector_lock.ConnectorLockUnexpectedOpen ||
               connector_lock.ConnectorLockUnexpectedClose || connector_lock.ConnectorLockFailedLock ||
               connector_lock.ConnectorLockFailedUnlock || connector_lock.MREC1ConnectorLockFailure ||
               connector_lock.VendorError;
    }

    inline bool any_set() {
        return bsp_any_set() || evse_manager_any_set() || ac_rcd_any_set() || connector_lock_any_set();
    }

    inline bool all_cleared() {
        return !any_set();
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

    bool modify_error_bsp(const Everest::error::Error& error, bool active, types::evse_manager::ErrorEnum evse_error);
    bool modify_error_connector_lock(const Everest::error::Error& error, bool active,
                                     types::evse_manager::ErrorEnum evse_error);
    bool modify_error_ac_rcd(const Everest::error::Error& error, bool active,
                             types::evse_manager::ErrorEnum evse_error);

    bool modify_error_evse_manager(const std::string& error_type, bool active,
                                   types::evse_manager::ErrorEnum evse_error);
    bool hlc{false};

    ActiveErrors active_errors;

#ifdef BUILD_TESTING_MODULE_EVSE_MANAGER
    FRIEND_TEST(ErrorHandlingTest, vendor);
#endif
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
