// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include "fusion_charger/goose/power_request.hpp"
#include "user_acceptance_tests/dispenser_test_fixture.hpp"

using namespace std;

using namespace user_acceptance_tests::dispenser_fixture;
using namespace fusion_charger::goose;

TEST_F(DispenserWithMultipleConnectors, ChargingSession) {
    set_up_psu_for_operation();

    assert_working_status(
        {WorkingStatus::STANDBY, WorkingStatus::STANDBY, WorkingStatus::STANDBY, WorkingStatus::STANDBY});

    connect_car(1);
    connect_car(3);

    assert_working_status({WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::STANDBY,
                           WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::STANDBY});

    connect_car(2);
    send_hmac_key(2);

    set_export_values(2, 200, 15);
    assert_requirement_type({nullopt, RequirementType::ModulePlaceholderRequest, nullopt, nullopt});

    set_mode_phase(2, ModePhase::ExportCharging);
    assert_working_status({WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::CHARGING,
                           WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::STANDBY});

    connect_car(3);
    send_hmac_key(3);
    set_export_values(3, 300, 30);
    set_mode_phase(3, ModePhase::ExportCharging);
    assert_requirement_type({nullopt, RequirementType::Charging, RequirementType::Charging, nullopt});
    assert_working_status({WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::CHARGING,
                           WorkingStatus::CHARGING, WorkingStatus::STANDBY});

    set_mode_phase(2, ModePhase::Off);

    auto current_stop_request_coutner = get_stop_request_counter();
    assert_working_status({WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::CHARGING_COMPLETE,
                           WorkingStatus::CHARGING, WorkingStatus::STANDBY});

    // Wait for the stop requests to be send to the mock
    sleep_for_ms(20);

    current_stop_request_coutner[1] += 1;
    assert_stop_request_counter_greater_or_equal(current_stop_request_coutner);

    disconnect_car(2);
    assert_working_status({WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED, WorkingStatus::STANDBY,
                           WorkingStatus::CHARGING, WorkingStatus::STANDBY});
}

TEST_F(DispenserWithMultipleConnectors, ConnectorCallbacks) {
    auto actual_1 = power_stack_mock->get_connector_callback_values(1);
    auto expected_1 = ConnectorCallbackResults{
        .connector_upstream_voltage = 100.0f,
        .output_voltage = 101.0f,
        .output_current = 102.0f,
        .contactor_status = ContactorStatus::ON,
        .electronic_lock_status = ElectronicLockStatus::UNLOCKED,
    };
    EXPECT_EQ(actual_1, expected_1);

    auto actual_2 = power_stack_mock->get_connector_callback_values(2);
    auto expected_2 = ConnectorCallbackResults{
        .connector_upstream_voltage = 200.0f,
        .output_voltage = 201.0f,
        .output_current = 202.0f,
        .contactor_status = ContactorStatus::ON,
        .electronic_lock_status = ElectronicLockStatus::LOCKED,
    };
    EXPECT_EQ(actual_2, expected_2);

    auto actual_3 = power_stack_mock->get_connector_callback_values(3);
    auto expected_3 = ConnectorCallbackResults{
        .connector_upstream_voltage = 300.0f,
        .output_voltage = 301.0f,
        .output_current = 302.0f,
        .contactor_status = ContactorStatus::OFF,
        .electronic_lock_status = ElectronicLockStatus::UNLOCKED,
    };
    EXPECT_EQ(actual_3, expected_3);

    auto actual_4 = power_stack_mock->get_connector_callback_values(4);
    auto expected_4 = ConnectorCallbackResults{
        .connector_upstream_voltage = 400.0f,
        .output_voltage = 401.0f,
        .output_current = 402.0f,
        .contactor_status = ContactorStatus::OFF,
        .electronic_lock_status = ElectronicLockStatus::LOCKED,
    };
    EXPECT_EQ(actual_4, expected_4);
}
