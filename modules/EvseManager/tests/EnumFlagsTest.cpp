// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EnumFlags.hpp>
#include <gtest/gtest.h>

#include <atomic>

namespace {

enum class ErrorHandlingFlags : std::uint8_t {
    prevent_charging,
    prevent_charging_welded,
    all_errors_cleared,
    last = all_errors_cleared
};

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

TEST(AtomicEnumFlagsTest, init) {
    module::AtomicEnumFlags<ErrorHandlingFlags, std::uint8_t> flags;
    EXPECT_TRUE(flags.all_reset());
}

TEST(AtomicEnumFlagsTest, init_large) {
    module::AtomicEnumFlags<BspErrors, std::uint32_t> flags;
    EXPECT_TRUE(flags.all_reset());
}

TEST(AtomicEnumFlagsTest, set_reset_one) {
    module::AtomicEnumFlags<ErrorHandlingFlags, std::uint8_t> flags;
    EXPECT_TRUE(flags.all_reset());

    flags.set(ErrorHandlingFlags::all_errors_cleared);
    EXPECT_FALSE(flags.all_reset());
    flags.reset(ErrorHandlingFlags::all_errors_cleared);
    EXPECT_TRUE(flags.all_reset());
}

TEST(AtomicEnumFlagsTest, set_reset_two) {
    module::AtomicEnumFlags<ErrorHandlingFlags, std::uint8_t> flags;
    EXPECT_TRUE(flags.all_reset());

    flags.set(ErrorHandlingFlags::all_errors_cleared);
    EXPECT_FALSE(flags.all_reset());
    flags.set(ErrorHandlingFlags::prevent_charging);
    EXPECT_FALSE(flags.all_reset());
    flags.reset(ErrorHandlingFlags::all_errors_cleared);
    EXPECT_FALSE(flags.all_reset());
    flags.reset(ErrorHandlingFlags::prevent_charging);
    EXPECT_TRUE(flags.all_reset());
}

TEST(AtomicEnumFlagsTest, set_reset_three) {
    module::AtomicEnumFlags<ErrorHandlingFlags, std::uint8_t> flags;
    EXPECT_TRUE(flags.all_reset());

    flags.set(ErrorHandlingFlags::all_errors_cleared);
    EXPECT_FALSE(flags.all_reset());
    flags.set(ErrorHandlingFlags::prevent_charging);
    EXPECT_FALSE(flags.all_reset());
    flags.set(ErrorHandlingFlags::prevent_charging_welded);
    EXPECT_FALSE(flags.all_reset());
    flags.reset();
    EXPECT_TRUE(flags.all_reset());
}

} // namespace
