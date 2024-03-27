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

#include "EnumFlags.hpp"
#include "Timeout.hpp"
#include "utils/thread.hpp"

namespace module {

// bit field to track which errors are active at the moment. Only tracks errors that stop charging.

enum class BspErrors : std::uint8_t {
    DiodeFault,
    VentilationNotAvailable,
    BrownOut,
    EnergyManagement,
    PermanentFault,
    MREC2GroundFailure,
    MREC4OverCurrentFailure,
    MREC5OverVoltage,
    MREC6UnderVoltage,
    MREC8EmergencyStop,
    MREC10InvalidVehicleMode,
    MREC14PilotFault,
    MREC15PowerLoss,
    MREC17EVSEContactorFault,
    MREC19CableOverTempStop,
    MREC20PartialInsertion,
    MREC23ProximityFault,
    MREC24ConnectorVoltageHigh,
    MREC25BrokenLatch,
    MREC26CutCable,
    VendorError,
    last = VendorError
};

enum class EvseManagerErrors : std::uint8_t {
    MREC4OverCurrentFailure,
    Internal,
    PowermeterTransactionStartFailed,
    last = PowermeterTransactionStartFailed
};

enum class AcRcdErrors : std::uint8_t {
    MREC2GroundFailure,
    VendorError,
    Selftest,
    AC,
    DC,
    last = DC
};

enum class ConnectorLockErrors : std::uint8_t {
    ConnectorLockCapNotCharged,
    ConnectorLockUnexpectedOpen,
    ConnectorLockUnexpectedClose,
    ConnectorLockFailedLock,
    ConnectorLockFailedUnlock,
    MREC1ConnectorLockFailure,
    VendorError,
    last = VendorError
};

struct ActiveErrors {
    AtomicEnumFlags<BspErrors, std::uint32_t> bsp;
    AtomicEnumFlags<EvseManagerErrors, std::uint8_t> evse_manager;
    AtomicEnumFlags<AcRcdErrors, std::uint8_t> ac_rcd;
    AtomicEnumFlags<ConnectorLockErrors, std::uint8_t> connector_lock;

    inline bool all_cleared() {
        return bsp.all_reset() && evse_manager.all_reset() && ac_rcd.all_reset() && connector_lock.all_reset();
    };
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

#ifdef BUILD_TESTING_MODULE_EVSE_MANAGER
    FRIEND_TEST(ErrorHandlingTest, modify_error_bsp);
    FRIEND_TEST(ErrorHandlingTest, modify_error_connector_lock);
    FRIEND_TEST(ErrorHandlingTest, modify_error_ac_rcd);
    FRIEND_TEST(ErrorHandlingTest, modify_error_evse_manager);
#endif
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
