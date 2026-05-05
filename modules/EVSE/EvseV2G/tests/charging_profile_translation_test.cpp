// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
//
// Unit tests for the iso2 PowerDeliveryReq.ChargingProfile → OCPP
// types::ocpp::ChargingSchedule translation helper used by
// publish_iso_power_delivery_req so libocpp can populate
// NotifyEVChargingScheduleRequest.chargingSchedule from the EV's actual
// returned profile (K15.FR.10, K16.FR.05) rather than the empty placeholder.
//
// ISO 15118-2 [V2G2-293] specifies ChargingProfileEntryStart as a
// non-cumulative relative offset in seconds from the start of the
// SAScheduleTuple; OCPP ChargingSchedulePeriod.start_period is also
// "seconds from start of schedule". Translation passes the value through
// directly — no running sum. types::ocpp::ChargingSchedule has no `id`
// field; libocpp overwrites chargingSchedule.id with the EV-selected
// SAScheduleTupleID at notify_ev_charging_schedule_req time, so the
// helper does not carry it.
#include <cstdint>

#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

#include <generated/types/ocpp.hpp>

#include <gtest/gtest.h>

namespace module {
namespace extensions {
namespace testing_internal {
types::ocpp::ChargingSchedule iso2_charging_profile_to_ocpp_schedule(const iso2_ChargingProfileType& profile);
} // namespace testing_internal
} // namespace extensions
} // namespace module

// iso15118_extensionsImpl.cpp references `extern struct v2g_context* v2g_ctx`;
// define a null stub so the impl translation unit links in the test binary.
struct v2g_context* v2g_ctx = nullptr;

namespace {

iso2_ChargingProfileType make_profile_with_entries(std::size_t count) {
    iso2_ChargingProfileType profile{};
    profile.ProfileEntry.arrayLen = static_cast<uint16_t>(count);
    return profile;
}

TEST(ChargingProfileTranslation, EmptyProfileProducesEmptyPeriodVector) {
    const auto profile = make_profile_with_entries(0);

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    EXPECT_EQ(schedule.evse, 0);
    EXPECT_EQ(schedule.charging_rate_unit, "W");
    EXPECT_TRUE(schedule.charging_schedule_period.empty());
    EXPECT_FALSE(schedule.duration.has_value());
    EXPECT_FALSE(schedule.start_schedule.has_value());
}

TEST(ChargingProfileTranslation, SingleEntryMapsValueAndMultiplierToWatts) {
    auto profile = make_profile_with_entries(1);
    profile.ProfileEntry.array[0].ChargingProfileEntryStart = 0;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Value = 11;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Multiplier = 3; // 11 * 10^3 = 11_000 W
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed = 0;

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    ASSERT_EQ(schedule.charging_schedule_period.size(), 1U);
    EXPECT_EQ(schedule.charging_schedule_period[0].start_period, 0);
    EXPECT_DOUBLE_EQ(schedule.charging_schedule_period[0].limit, 11000.0);
    EXPECT_FALSE(schedule.charging_schedule_period[0].number_phases.has_value());
}

TEST(ChargingProfileTranslation, MultipleEntriesPreserveSpecLiteralStartOffsets) {
    auto profile = make_profile_with_entries(3);
    // Spec-literal: each ChargingProfileEntryStart is an independent offset from start of
    // SAScheduleTuple (cumulative). The translation passes through without running-sum.
    profile.ProfileEntry.array[0].ChargingProfileEntryStart = 0;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Value = 16;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Multiplier = 3;
    profile.ProfileEntry.array[1].ChargingProfileEntryStart = 600;
    profile.ProfileEntry.array[1].ChargingProfileEntryMaxPower.Value = 8;
    profile.ProfileEntry.array[1].ChargingProfileEntryMaxPower.Multiplier = 3;
    profile.ProfileEntry.array[2].ChargingProfileEntryStart = 1800;
    profile.ProfileEntry.array[2].ChargingProfileEntryMaxPower.Value = 4;
    profile.ProfileEntry.array[2].ChargingProfileEntryMaxPower.Multiplier = 3;

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    ASSERT_EQ(schedule.charging_schedule_period.size(), 3U);
    EXPECT_EQ(schedule.charging_schedule_period[0].start_period, 0);
    EXPECT_EQ(schedule.charging_schedule_period[1].start_period, 600);
    EXPECT_EQ(schedule.charging_schedule_period[2].start_period, 1800);
    EXPECT_DOUBLE_EQ(schedule.charging_schedule_period[0].limit, 16000.0);
    EXPECT_DOUBLE_EQ(schedule.charging_schedule_period[1].limit, 8000.0);
    EXPECT_DOUBLE_EQ(schedule.charging_schedule_period[2].limit, 4000.0);
}

TEST(ChargingProfileTranslation, NegativeMultiplierScalesDown) {
    auto profile = make_profile_with_entries(1);
    profile.ProfileEntry.array[0].ChargingProfileEntryStart = 0;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Value = 12345;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Multiplier = -2; // 12345 * 10^-2 = 123.45

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    ASSERT_EQ(schedule.charging_schedule_period.size(), 1U);
    // OCPP ChargingSchedulePeriod.limit is float-precision; std::pow(10, -2) is also not
    // exactly 0.01 in IEEE-754. 1e-4 absolute tolerance is well within EV envelope semantics.
    EXPECT_NEAR(schedule.charging_schedule_period[0].limit, 123.45, 1e-4);
}

TEST(ChargingProfileTranslation, NumberOfPhasesForwardedWhenSet) {
    auto profile = make_profile_with_entries(1);
    profile.ProfileEntry.array[0].ChargingProfileEntryStart = 0;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Value = 7;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxPower.Multiplier = 3;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed = 1;
    profile.ProfileEntry.array[0].ChargingProfileEntryMaxNumberOfPhasesInUse = 3;

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    ASSERT_EQ(schedule.charging_schedule_period.size(), 1U);
    ASSERT_TRUE(schedule.charging_schedule_period[0].number_phases.has_value());
    EXPECT_EQ(schedule.charging_schedule_period[0].number_phases.value(), 3);
}

TEST(ChargingProfileTranslation, EntryArrayCappedAtIso2MaxLength) {
    // The iso2 ProfileEntry array is sized to iso2_ProfileEntryType_24_ARRAY_SIZE (24).
    // Even if a malformed wire claims arrayLen > 24, the translation must not read past
    // the C array; cap at the static array size.
    iso2_ChargingProfileType profile{};
    profile.ProfileEntry.arrayLen = iso2_ProfileEntryType_24_ARRAY_SIZE + 5;
    for (std::size_t i = 0; i < iso2_ProfileEntryType_24_ARRAY_SIZE; ++i) {
        profile.ProfileEntry.array[i].ChargingProfileEntryStart = static_cast<uint32_t>(60 * i);
        profile.ProfileEntry.array[i].ChargingProfileEntryMaxPower.Value = 1;
        profile.ProfileEntry.array[i].ChargingProfileEntryMaxPower.Multiplier = 3;
    }

    const auto schedule = module::extensions::testing_internal::iso2_charging_profile_to_ocpp_schedule(profile);

    EXPECT_EQ(schedule.charging_schedule_period.size(), iso2_ProfileEntryType_24_ARRAY_SIZE);
}

} // namespace
