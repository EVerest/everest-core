// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "smart_charging_test_utils.hpp"

using ocpp::DateTime;

TEST_F(CompositeScheduleTestFixtureV21, setpoint_tx_profile) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/setpoints/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time = ocpp::DateTime("2024-01-17T18:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-18T06:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 1500.0;
    period1.numberPhases = 1;
    period1.setpoint = 1500.0; // overriden by limit
    period1.dischargeLimit = -2000.0;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 1080;
    period2.limit = 11000.0;
    period2.numberPhases = 1;
    period2.setpoint = -8000.0;
    period2.dischargeLimit = -10000.0;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 14000;
    period3.limit = 6000.0;
    period3.numberPhases = 1;
    period3.setpoint = -5000.0; // overriden by dischargeLimit
    period3.dischargeLimit = -5000.0;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 28000;
    period4.limit = 15000.0;
    period4.numberPhases = 1;
    period4.setpoint = -4000.0;
    // period4.operationMode = OperationModeEnum::CentralSetpoint;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 43200;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2MaxOverridesHigherLimits) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/max/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/max/1/", DEFAULT_EVSE_ID);

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time = ocpp::DateTime("2024-01-17T00:00:00");
    const DateTime end_time = ocpp::DateTime("2024-01-17T04:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 10.0;
    period1.dischargeLimit = -23.0;
    period1.setpoint = 10.0;
    period1.numberPhases = 1;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 20.0;
    period2.dischargeLimit = -23.0;
    period2.setpoint = 20.0;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 7200;
    period3.limit = 30.0;
    period3.setpoint = 28.0;
    period3.numberPhases = 1;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 10800;
    period4.limit = 40.0;
    period4.setpoint = 28.0;
    period4.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 14400;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::A;

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, false);
    ASSERT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2StackLevel_Recurring_Period_TimeOfDay) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/stack/", DEFAULT_EVSE_ID);
    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time_monday("2025-03-24T16:00:00");
    const DateTime end_time_monday("2025-03-24T21:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = DEFAULT_LIMIT_WATT;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 2000;
    period2.numberPhases = 3;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 14400;
    period3.limit = DEFAULT_LIMIT_WATT;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 18000;
    expected.scheduleStart = start_time_monday;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(
        start_time_monday, end_time_monday, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2StackLevel_Recurring_Period_TimeOfDay_Excluded_Weekly) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/stack/", DEFAULT_EVSE_ID);
    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time_sunday("2025-03-23T16:00:00");
    const DateTime end_time_sunday("2025-03-23T21:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 999999;
    period1.numberPhases = 3;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 18000;
    expected.scheduleStart = start_time_sunday;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(
        start_time_sunday, end_time_sunday, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2StackLevel_Recurring_Period_TimeOfDay_NotExcluded_ChristmasOtherYear) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/stack/", DEFAULT_EVSE_ID);
    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time_christmas("2025-12-25T16:00:00");
    const DateTime end_time_christmas("2025-12-25T21:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = DEFAULT_LIMIT_WATT;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 3600;
    period2.limit = 2000;
    period2.numberPhases = 3;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 14400;
    period3.limit = DEFAULT_LIMIT_WATT;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 18000;
    expected.scheduleStart = start_time_christmas;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(
        start_time_christmas, end_time_christmas, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2StackLevel_Recurring_Period_TimeOfDay_Excluded_Christmas) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/stack/", DEFAULT_EVSE_ID);
    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");

    const DateTime start_time_christmas("2020-12-25T16:00:00");
    const DateTime end_time_christmas("2020-12-25T21:00:00");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 999999;
    period1.numberPhases = 3;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 18000;
    expected.scheduleStart = start_time_christmas;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(
        start_time_christmas, end_time_christmas, DEFAULT_EVSE_ID, ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2ChargingRateUnitCombine) {
    // One is in W and one is in ampere and one has limits for L2 and L3 as well and the other has one limit for all
    // three.
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/charging_rate_unit_combine/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/charging_rate_unit_combine/1/", DEFAULT_EVSE_ID);

    const DateTime start_time("2024-01-17T18:00:00.000Z");
    const DateTime end_time("2024-01-17T21:00:00.000Z");

    ChargingSchedulePeriod period1; // TxProfile stacklevel 1
    period1.startPeriod = 0;
    period1.limit = 500.0;
    period1.limit_L2 = 500.0;
    period1.limit_L3 = 500.0;
    period1.numberPhases = 3;
    period1.setpoint = 500.0;    // overriden by limit
    period1.setpoint_L2 = 500.0; // overriden by limit
    period1.setpoint_L3 = 500.0; // overriden by limit
    period1.dischargeLimit = -1000.0;
    period1.dischargeLimit_L2 = -1000.0;
    period1.dischargeLimit_L3 = -1000.0;
    ChargingSchedulePeriod period2; // TxProfile stacklevel 0
    period2.startPeriod = 2400;
    period2.limit = 400.0;
    period2.limit_L2 = 400.0;
    period2.limit_L3 = 400.0;
    period2.numberPhases = 3;
    period2.dischargeLimit = -300.0;
    period2.dischargeLimit_L2 = -300.0;
    period2.dischargeLimit_L3 = -300.0;
    period2.setpoint = 400.0;
    period2.setpoint_L2 = 400.0;
    period2.setpoint_L3 = 400.0;
    ChargingSchedulePeriod period3; // Charging station max profile
    period3.startPeriod = 3000;
    period3.limit = 43700.0;
    period3.limit_L2 = 87400.0;
    period3.limit_L3 = 131100.0;
    period3.numberPhases = 3;
    ChargingSchedulePeriod period4; // Charging station max profile
    period4.startPeriod = 3600;
    period4.limit = 46000.0;
    period4.limit_L2 = 92000;
    period4.limit_L3 = 138000;
    period4.numberPhases = 3;
    ChargingSchedulePeriod period5; // Charging station max profile
    period5.startPeriod = 7200;
    period5.limit = 48300.0;
    period5.limit_L2 = 96600.0;
    period5.limit_L3 = 144900.0;
    period5.numberPhases = 3;
    // period4.operationMode = OperationModeEnum::CentralSetpoint;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4, period5};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 10800;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");
    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2Different_Number_Phases) {
    // One has limits for L2 and L3 as well and the other has one limit for all three.
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/different_number_phases/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/different_number_phases/1/", DEFAULT_EVSE_ID);

    const DateTime start_time("2024-01-17T18:00:00.000Z");
    const DateTime end_time("2024-01-17T21:00:00.000Z");

    ChargingSchedulePeriod period1; // TxProfile
    period1.startPeriod = 0;
    period1.limit = 160.0; // The lowest limit of the three
    period1.numberPhases = 1;
    period1.dischargeLimit = -900.0;
    period1.setpoint = 160.0;
    ChargingSchedulePeriod period2; // Charging station max profile
    period2.startPeriod = 3000;
    period2.limit = 190.0;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3; // Charging station max profile
    period3.startPeriod = 3600;
    period3.limit = 200.0;
    period3.numberPhases = 1;
    ChargingSchedulePeriod period4; // Charging station max profile
    period4.startPeriod = 7200;
    period4.limit = 210.0;
    period4.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 10800;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::A;

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");
    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V2Different_Number_Phases_W) {
    // One has limits for L2 and L3 as well and the other has one limit for all three.
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/different_number_phases/0/", STATION_WIDE_ID);
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/different_number_phases/1/", DEFAULT_EVSE_ID);

    const DateTime start_time("2024-01-17T18:00:00.000Z");
    const DateTime end_time("2024-01-17T21:00:00.000Z");

    ChargingSchedulePeriod period1; // TxProfile
    period1.startPeriod = 0;
    period1.limit = 160.0 * 230;
    period1.numberPhases = 1;
    period1.dischargeLimit = -900.0 * 230;
    period1.setpoint = 160.0 * 230;
    ChargingSchedulePeriod period2; // Charging station max profile
    period2.startPeriod = 3000;
    period2.limit = 190.0 * 230;
    period2.numberPhases = 1;
    ChargingSchedulePeriod period3; // Charging station max profile
    period3.startPeriod = 3600;
    period3.limit = 200.0 * 230;
    period3.numberPhases = 1;
    ChargingSchedulePeriod period4; // Charging station max profile
    period4.startPeriod = 7200;
    period4.limit = 210.0 * 230;
    period4.numberPhases = 1;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 10800;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");
    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V21NoLimitSpecified) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/no_limit_specified/", DEFAULT_EVSE_ID);

    const DateTime start_time("2024-01-17T18:00:00.000Z");
    const DateTime end_time("2024-01-17T21:00:00.000Z");

    // Period was not valid because there was no limit specified. So the charging station default is used.
    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = DEFAULT_LIMIT_AMPERE;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 10800;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::A;

    evse_manager->open_transaction(DEFAULT_EVSE_ID, "f1522902-1170-416f-8e43-9e3bce28fde7");
    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::A, false, false);
    EXPECT_EQ(actual, expected);
}

TEST_F(CompositeScheduleTestFixtureV21, V21DifferentNumberPhases_StationWide) {
    this->load_charging_profiles_for_evse(BASE_JSON_PATH_V21 + "/station_wide_three_phases/", STATION_WIDE_ID);
    const DateTime start_time("2024-01-17T18:00:00.000Z");
    const DateTime end_time("2024-01-17T21:00:00.000Z");

    ChargingSchedulePeriod period1;
    period1.startPeriod = 0;
    period1.limit = 12420.0f;
    period1.limit_L2 = 12430.0f;
    period1.limit_L3 = 12440.0f;
    period1.numberPhases = 3;
    ChargingSchedulePeriod period2;
    period2.startPeriod = 300;
    period2.limit = 8280.0f;
    period2.limit_L2 = 8290.0f;
    period2.limit_L3 = 8300.0f;
    period2.numberPhases = 3;
    ChargingSchedulePeriod period3;
    period3.startPeriod = 600;
    period3.limit = 2070.0f;
    period3.limit_L2 = 2070.0f;
    period3.limit_L3 = 2070.0f;
    period3.numberPhases = 3;
    ChargingSchedulePeriod period4;
    period4.startPeriod = 900;
    period4.limit = 1380.0f;
    period4.limit_L2 = 1380.0f;
    period4.limit_L3 = 1380.0f;
    period4.numberPhases = 3;
    ChargingSchedulePeriod period5;
    period5.startPeriod = 1200;
    period5.limit = 690.0f;
    period5.limit_L2 = 690.0f;
    period5.limit_L3 = 690.0f;
    period5.numberPhases = 3;
    ChargingSchedulePeriod period6;
    period6.startPeriod = 3600;
    period6.limit = 200000.0f;
    period6.limit_L2 = 400000.0f;
    period6.limit_L3 = 600000.0f;
    period6.numberPhases = 3;
    ChargingSchedulePeriod period7;
    period7.startPeriod = 7200;
    period7.limit = 210000.0f;
    period7.limit_L2 = 420000.0f;
    period7.limit_L3 = 630000.0f;
    period7.numberPhases = 3;
    CompositeSchedule expected;
    expected.chargingSchedulePeriod = {period1, period2, period3, period4, period5, period6, period7};
    expected.evseId = DEFAULT_EVSE_ID;
    expected.duration = 10800;
    expected.scheduleStart = start_time;
    expected.chargingRateUnit = ChargingRateUnitEnum::W;

    CompositeSchedule actual = handler->calculate_composite_schedule(start_time, end_time, DEFAULT_EVSE_ID,
                                                                     ChargingRateUnitEnum::W, false, true);
    EXPECT_EQ(actual, expected);
}
