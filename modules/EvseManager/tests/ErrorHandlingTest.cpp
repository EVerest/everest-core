// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EvseManagerStub.hpp>
#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Tests that don't need access to private members/methods

namespace {

class ActiveErrorsTest : public testing::Test {
public:
    module::ActiveErrors errors;
    struct items {
        std::atomic_bool* item;
        const char* name;
    };

    std::vector<items> bsp_errors = {
        {&errors.bsp.DiodeFault, "DiodeFault"},
        {&errors.bsp.VentilationNotAvailable, "VentilationNotAvailable"},
        {&errors.bsp.BrownOut, "BrownOut"},
        {&errors.bsp.EnergyManagement, "EnergyManagement"},
        {&errors.bsp.PermanentFault, "PermanentFault"},
        {&errors.bsp.MREC2GroundFailure, "MREC2GroundFailure"},
        {&errors.bsp.MREC4OverCurrentFailure, "MREC4OverCurrentFailure"},
        {&errors.bsp.MREC5OverVoltage, "MREC5OverVoltage"},
        {&errors.bsp.MREC6UnderVoltage, "MREC6UnderVoltage"},
        {&errors.bsp.MREC8EmergencyStop, "MREC8EmergencyStop"},
        {&errors.bsp.MREC10InvalidVehicleMode, "MREC10InvalidVehicleMode"},
        {&errors.bsp.MREC14PilotFault, "MREC14PilotFault"},
        {&errors.bsp.MREC15PowerLoss, "MREC15PowerLoss"},
        {&errors.bsp.MREC17EVSEContactorFault, "MREC17EVSEContactorFault"},
        {&errors.bsp.MREC19CableOverTempStop, "MREC19CableOverTempStop"},
        {&errors.bsp.MREC20PartialInsertion, "MREC20PartialInsertion"},
        {&errors.bsp.MREC23ProximityFault, "MREC23ProximityFault"},
        {&errors.bsp.MREC24ConnectorVoltageHigh, "MREC24ConnectorVoltageHigh"},
        {&errors.bsp.MREC25BrokenLatch, "MREC25BrokenLatch"},
        {&errors.bsp.MREC26CutCable, "MREC26CutCable"},
        {&errors.bsp.VendorError, "VendorError"},
    };

    std::vector<items> evse_manager_errors = {
        {&errors.evse_manager.MREC4OverCurrentFailure, "MREC4OverCurrentFailure"},
        {&errors.evse_manager.Internal, "Internal"},
        {&errors.evse_manager.PowermeterTransactionStartFailed, "PowermeterTransactionStartFailed"},
    };

    std::vector<items> ac_rcd_errors = {
        {&errors.ac_rcd.MREC2GroundFailure, "MREC2GroundFailure"},
        {&errors.ac_rcd.VendorError, "VendorError"},
        {&errors.ac_rcd.Selftest, "Selftest"},
        {&errors.ac_rcd.AC, "AC"},
        {&errors.ac_rcd.DC, "DC"},
    };

    std::vector<items> connector_lock_errors = {
        {&errors.connector_lock.ConnectorLockCapNotCharged, "ConnectorLockCapNotCharged"},
        {&errors.connector_lock.ConnectorLockUnexpectedOpen, "ConnectorLockUnexpectedOpen"},
        {&errors.connector_lock.ConnectorLockUnexpectedClose, "ConnectorLockUnexpectedClose"},
        {&errors.connector_lock.ConnectorLockFailedLock, "ConnectorLockFailedLock"},
        {&errors.connector_lock.ConnectorLockFailedUnlock, "ConnectorLockFailedUnlock"},
        {&errors.connector_lock.MREC1ConnectorLockFailure, "MREC1ConnectorLockFailure"},
        {&errors.connector_lock.VendorError, "VendorError"},
    };

    void SetUp() override {
        for (auto p : bsp_errors) {
            *p.item = false;
        }
        for (auto p : evse_manager_errors) {
            *p.item = false;
        }
        for (auto p : ac_rcd_errors) {
            *p.item = false;
        }
        for (auto p : connector_lock_errors) {
            *p.item = false;
        }
    }
};

//-----------------------------------------------------------------------------
TEST_F(ActiveErrorsTest, bsp_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : bsp_errors) {
        SCOPED_TRACE(p.name);
        EXPECT_FALSE(*p.item);
        *p.item = true;
        EXPECT_TRUE(*p.item);
        EXPECT_FALSE(errors.all_cleared());
        *p.item = false;
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, evse_manager_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : evse_manager_errors) {
        SCOPED_TRACE(p.name);
        EXPECT_FALSE(*p.item);
        *p.item = true;
        EXPECT_TRUE(*p.item);
        EXPECT_FALSE(errors.all_cleared());
        *p.item = false;
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, ac_rcd_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : ac_rcd_errors) {
        SCOPED_TRACE(p.name);
        EXPECT_FALSE(*p.item);
        *p.item = true;
        EXPECT_TRUE(*p.item);
        EXPECT_FALSE(errors.all_cleared());
        *p.item = false;
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, connector_lock_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : connector_lock_errors) {
        SCOPED_TRACE(p.name);
        EXPECT_FALSE(*p.item);
        *p.item = true;
        EXPECT_TRUE(*p.item);
        EXPECT_FALSE(errors.all_cleared());
        *p.item = false;
        EXPECT_TRUE(errors.all_cleared());
    }
}

} // namespace

// ----------------------------------------------------------------------------
// Tests that need access to private members/methods

namespace module {

struct ErrorHandlingSignals {
    std::atomic<bool> signal_error = false;
    std::atomic<bool> signal_error_cleared = false;
    std::atomic<bool> signal_all_errors_cleared = false;
    std::atomic<bool> called_signal_error = false;
    std::atomic<bool> called_signal_error_cleared = false;
    std::atomic<bool> called_signal_all_errors_cleared = false;

    ErrorHandlingSignals(module::ErrorHandling& eh) {
        register_callbacks(eh);
    }

    void register_callbacks(module::ErrorHandling& eh) {
        eh.signal_error.connect([this](types::evse_manager::Error err, bool stop_charging) {
            this->called_signal_error = true;
            this->signal_error = true;
        });
        eh.signal_error_cleared.connect([this](types::evse_manager::Error err, bool stop_charging) {
            this->called_signal_error_cleared = true;
            this->signal_error_cleared = true;
        });
        eh.signal_all_errors_cleared.connect([this]() {
            this->called_signal_all_errors_cleared = true;
            this->signal_all_errors_cleared = true;
        });
    }

    void reset() {
        signal_error = false;
        signal_error_cleared = false;
        signal_all_errors_cleared = false;
        called_signal_error = false;
        called_signal_error_cleared = false;
        called_signal_all_errors_cleared = false;
    }
};

TEST(ErrorHandlingTest, vendor) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd,
                                         evse_managerImpl);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    Everest::error::Error error("evse_board_support/VendorError", "K2Faults::FAULT_CT_CLAMP",
                                "Vendor specific error code. Will stop charging session.", id);

    bool bResult;
    bResult = error_handling.modify_error_bsp(error, true, types::evse_manager::ErrorEnum::VendorError);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_bsp(error, false, types::evse_manager::ErrorEnum::VendorError);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // VendorWarning not treated as an active error
    ehs.reset();
    Everest::error::Error warning("evse_board_support/VendorWarning", "K2Faults::FAULT_CT_CLAMP",
                                  "Vendor specific error code. Will not stop charging session.", id);
    bResult = error_handling.modify_error_bsp(warning, true, types::evse_manager::ErrorEnum::VendorWarning);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_bsp(warning, false, types::evse_manager::ErrorEnum::VendorWarning);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    manager.raise_error("evse_board_support/VendorError", "K2Faults::FAULT_CT_CLAMP",
                        "Vendor specific error code. Will stop charging session.");
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_TRUE(ehs.signal_error);
    EXPECT_FALSE(ehs.signal_error_cleared);
    EXPECT_FALSE(ehs.signal_all_errors_cleared);
    EXPECT_TRUE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    manager.clear_error("evse_board_support/VendorError", "K2Faults::FAULT_CT_CLAMP",
                        "Vendor specific error code. Will stop charging session.");
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.signal_error);
    EXPECT_TRUE(ehs.signal_error_cleared);
    EXPECT_TRUE(ehs.signal_all_errors_cleared);
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_TRUE(ehs.called_signal_error_cleared);
    EXPECT_TRUE(ehs.called_signal_all_errors_cleared);
}

} // namespace module