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

using BspErrors = module::BspErrors;
using EvseManagerErrors = module::EvseManagerErrors;
using AcRcdErrors = module::AcRcdErrors;
using IMDErrors = module::IMDErrors;
using ConnectorLockErrors = module::ConnectorLockErrors;

class ActiveErrorsTest : public testing::Test {
public:
    module::ActiveErrors errors;

    std::vector<BspErrors> bsp_errors = {
        BspErrors::DiodeFault,
        BspErrors::VentilationNotAvailable,
        BspErrors::BrownOut,
        BspErrors::EnergyManagement,
        BspErrors::PermanentFault,
        BspErrors::MREC2GroundFailure,
        BspErrors::MREC4OverCurrentFailure,
        BspErrors::MREC5OverVoltage,
        BspErrors::MREC6UnderVoltage,
        BspErrors::MREC8EmergencyStop,
        BspErrors::MREC10InvalidVehicleMode,
        BspErrors::MREC14PilotFault,
        BspErrors::MREC15PowerLoss,
        BspErrors::MREC17EVSEContactorFault,
        BspErrors::MREC19CableOverTempStop,
        BspErrors::MREC20PartialInsertion,
        BspErrors::MREC23ProximityFault,
        BspErrors::MREC24ConnectorVoltageHigh,
        BspErrors::MREC25BrokenLatch,
        BspErrors::MREC26CutCable,
        BspErrors::VendorError,
    };

    std::vector<EvseManagerErrors> evse_manager_errors = {
        EvseManagerErrors::MREC4OverCurrentFailure,
        EvseManagerErrors::Internal,
        EvseManagerErrors::PowermeterTransactionStartFailed,
    };

    std::vector<AcRcdErrors> ac_rcd_errors = {
        AcRcdErrors::MREC2GroundFailure,
        AcRcdErrors::VendorError,
        AcRcdErrors::Selftest,
        AcRcdErrors::AC,
        AcRcdErrors::DC,
    };

    std::vector<IMDErrors> imd_errors = {
        IMDErrors::DeviceFault,
        IMDErrors::CommunicationFault,
        IMDErrors::VendorWarning,
        IMDErrors::VendorError,
    };

    std::vector<ConnectorLockErrors> connector_lock_errors = {
        ConnectorLockErrors::ConnectorLockCapNotCharged,
        ConnectorLockErrors::ConnectorLockUnexpectedOpen,
        ConnectorLockErrors::ConnectorLockUnexpectedClose,
        ConnectorLockErrors::ConnectorLockFailedLock,
        ConnectorLockErrors::ConnectorLockFailedUnlock,
        ConnectorLockErrors::MREC1ConnectorLockFailure,
        ConnectorLockErrors::VendorError,
    };

    void SetUp() override {
        errors.bsp.reset();
        errors.evse_manager.reset();
        errors.ac_rcd.reset();
        errors.imd.reset();
        errors.connector_lock.reset();
    }
};

//-----------------------------------------------------------------------------
TEST_F(ActiveErrorsTest, bsp_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : bsp_errors) {
        SCOPED_TRACE(std::to_string(static_cast<std::size_t>(p)));
        EXPECT_FALSE(errors.bsp.is_set(p));
        errors.bsp.set(p);
        EXPECT_TRUE(errors.bsp.is_set(p));
        EXPECT_FALSE(errors.all_cleared());
        errors.bsp.reset(p);
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, evse_manager_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : evse_manager_errors) {
        SCOPED_TRACE(std::to_string(static_cast<std::size_t>(p)));
        EXPECT_FALSE(errors.evse_manager.is_set(p));
        errors.evse_manager.set(p);
        EXPECT_TRUE(errors.evse_manager.is_set(p));
        EXPECT_FALSE(errors.all_cleared());
        errors.evse_manager.reset(p);
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, ac_rcd_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : ac_rcd_errors) {
        SCOPED_TRACE(std::to_string(static_cast<std::size_t>(p)));
        EXPECT_FALSE(errors.ac_rcd.is_set(p));
        errors.ac_rcd.set(p);
        EXPECT_TRUE(errors.ac_rcd.is_set(p));
        EXPECT_FALSE(errors.all_cleared());
        errors.ac_rcd.reset(p);
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, imd_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : imd_errors) {
        SCOPED_TRACE(std::to_string(static_cast<std::size_t>(p)));
        EXPECT_FALSE(errors.imd.is_set(p));
        errors.imd.set(p);
        EXPECT_TRUE(errors.imd.is_set(p));
        EXPECT_FALSE(errors.all_cleared());
        errors.imd.reset(p);
        EXPECT_TRUE(errors.all_cleared());
    }
}

TEST_F(ActiveErrorsTest, connector_lock_errors) {
    ASSERT_TRUE(errors.all_cleared());
    for (auto& p : connector_lock_errors) {
        SCOPED_TRACE(std::to_string(static_cast<std::size_t>(p)));
        EXPECT_FALSE(errors.connector_lock.is_set(p));
        errors.connector_lock.set(p);
        EXPECT_TRUE(errors.connector_lock.is_set(p));
        EXPECT_FALSE(errors.all_cleared());
        errors.connector_lock.reset(p);
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
        eh.signal_error.connect([this](bool stop_charging) {
            this->called_signal_error = true;
            this->signal_error = true;
        });
        eh.signal_error_cleared.connect([this](bool stop_charging) {
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

TEST(ErrorHandlingTest, modify_error_bsp) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> imd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd, evse_managerImpl,
                                         imd);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    Everest::error::Error error("evse_board_support/VendorError", "", "K2Faults::FAULT_CT_CLAMP",
                                "Vendor specific error code. Will stop charging session.", id);

    bool bResult;
    bResult = error_handling.modify_error_bsp(error, true);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_bsp(error, false);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // VendorWarning not treated as an active error
    ehs.reset();
    Everest::error::Error warning("evse_board_support/VendorWarning", "", "K2Faults::FAULT_CT_CLAMP",
                                  "Vendor specific error code. Will not stop charging session.", id);
    bResult = error_handling.modify_error_bsp(warning, true);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_bsp(warning, false);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // TODO: test raise_error() and clear_error() with error object

    // ehs.reset();
    // Everest::error::Error err_object;
    // err_object.type = "evse_board_support/VendorError";
    // err_object.message = "K2Faults::FAULT_CT_CLAMP";
    // err_object.description = "Vendor specific error code. Will stop charging session.";
    // err_object.origin = id;
    // manager.get_error_manager_impl("")->raise_error(err_object);
    // EXPECT_FALSE(error_handling.active_errors.all_cleared());
    // EXPECT_TRUE(ehs.signal_error);
    // EXPECT_FALSE(ehs.signal_error_cleared);
    // EXPECT_FALSE(ehs.signal_all_errors_cleared);
    // EXPECT_TRUE(ehs.called_signal_error);
    // EXPECT_FALSE(ehs.called_signal_error_cleared);
    // EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // ehs.reset();
    // manager.get_error_manager_impl("")->clear_error("evse_board_support/VendorError");
    // EXPECT_TRUE(error_handling.active_errors.all_cleared());
    // EXPECT_FALSE(ehs.signal_error);
    // EXPECT_TRUE(ehs.signal_error_cleared);
    // EXPECT_TRUE(ehs.signal_all_errors_cleared);
    // EXPECT_FALSE(ehs.called_signal_error);
    // EXPECT_TRUE(ehs.called_signal_error_cleared);
    // EXPECT_TRUE(ehs.called_signal_all_errors_cleared);
}

TEST(ErrorHandlingTest, modify_error_connector_lock) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> imd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd, evse_managerImpl,
                                         imd);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    Everest::error::Error error("connector_lock/ConnectorLockUnexpectedOpen", "", "Will stop charging session.",
                                "no description", id);

    bool bResult;
    bResult = error_handling.modify_error_connector_lock(error, true);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_connector_lock(error, false);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // VendorWarning not treated as an active error
    ehs.reset();
    Everest::error::Error warning("connector_lock/VendorWarning", "", "Will not stop charging session.",
                                  "no description", id);
    bResult = error_handling.modify_error_connector_lock(warning, true);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_connector_lock(warning, false);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);
}

TEST(ErrorHandlingTest, modify_error_ac_rcd) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> imd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd, evse_managerImpl,
                                         imd);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    Everest::error::Error error("ac_rcd/AC", "", "Will stop charging session.", "no description", id);

    bool bResult;
    bResult = error_handling.modify_error_ac_rcd(error, true);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_ac_rcd(error, false);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // VendorWarning not treated as an active error
    ehs.reset();
    Everest::error::Error warning("ac_rcd/VendorWarning", "", "Will not stop charging session.", "no description", id);
    bResult = error_handling.modify_error_ac_rcd(warning, true);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_ac_rcd(warning, false);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);
}

TEST(ErrorHandlingTest, modify_error_imd) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> imd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd, evse_managerImpl,
                                         imd);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    Everest::error::Error error("isolation_monitor/CommunicationFault", "", "Will stop charging session.",
                                "no description", id);

    bool bResult;
    bResult = error_handling.modify_error_imd(error, true);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_imd(error, false);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    // VendorWarning not treated as an active error
    ehs.reset();
    Everest::error::Error warning("isolation_monitor/VendorWarning", "", "Will not stop charging session.",
                                  "no description", id);
    bResult = error_handling.modify_error_imd(warning, true);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_imd(warning, false);
    EXPECT_FALSE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);
}

TEST(ErrorHandlingTest, modify_error_evse_manager) {
    stub::EvseManagerModuleAdapter manager;
    Requirement req("", 0);
    const std::unique_ptr<evse_board_supportIntf> evse_board_support =
        std::make_unique<evse_board_supportIntf>(&manager, req, "manager");
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> ISO15118_charger;
    const std::vector<std::unique_ptr<connector_lockIntf>> connector_lock;
    const std::vector<std::unique_ptr<ac_rcdIntf>> ac_rcd;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> imd;
    const std::unique_ptr<evse_managerImplBase> evse_managerImpl = std::make_unique<stub::evse_managerImplStub>();

    module::ErrorHandling error_handling(evse_board_support, ISO15118_charger, connector_lock, ac_rcd, evse_managerImpl,
                                         imd);

    // signals are "raised" via raise_error() and clear_error()
    // but not via modify_error_bsp()
    ErrorHandlingSignals ehs(error_handling);

    EXPECT_FALSE(error_handling.hlc);
    ImplementationIdentifier id("evse_manager", "main");
    std::string error{"evse_manager/PowermeterTransactionStartFailed"};

    bool bResult;
    bResult = error_handling.modify_error_evse_manager(error, true);
    EXPECT_TRUE(bResult);
    EXPECT_FALSE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);

    ehs.reset();
    bResult = error_handling.modify_error_evse_manager(error, false);
    EXPECT_TRUE(bResult);
    EXPECT_TRUE(error_handling.active_errors.all_cleared());
    EXPECT_FALSE(ehs.called_signal_error);
    EXPECT_FALSE(ehs.called_signal_error_cleared);
    EXPECT_FALSE(ehs.called_signal_all_errors_cleared);
}

} // namespace module
