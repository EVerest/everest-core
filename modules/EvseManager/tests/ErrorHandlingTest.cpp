// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ErrorHandling.hpp>
#include <gtest/gtest.h>

#include <vector>

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
