// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include "dispenser.hpp"
#include "fusion_charger/goose/power_request.hpp"
#include "power_stack_mock/power_stack_mock.hpp"
#include "user_acceptance_tests/dispenser_test_fixture.hpp"

using namespace std;

using namespace user_acceptance_tests::dispenser_fixture;

TEST_F(DispenserWithoutTlsTest, StateCarDisconnected) {
    EXPECT_EQ(get_connection_status(), ConnectionStatus::NOT_CONNECTED);
    EXPECT_EQ(dispenser->get_psu_running_mode(), PSURunningMode::STARTING_UP);
    EXPECT_EQ(connector()->module_placeholder_allocation_failed(), false);
    EXPECT_EQ(connector()->get_output_port_availability(), PsuOutputPortAvailability::NOT_AVAILABLE);
    EXPECT_EQ(dispenser->get_psu_communication_state(), DispenserPsuCommunicationState::INITIALIZING);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY);

    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    EXPECT_EQ(dispenser->get_psu_running_mode(), PSURunningMode::RUNNING);

    power_stack_mock->send_mac_address();
    EXPECT_EQ(dispenser->get_psu_communication_state(), DispenserPsuCommunicationState::READY);
}

TEST_F(DispenserWithoutTlsTest, CarConnectedAndReadyToCharge) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    connector()->on_car_connected();
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED);
    EXPECT_EQ(get_connection_status(), ConnectionStatus::FULL_CONNECTED);

    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(5);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);
    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::ModulePlaceholderRequest);
}

TEST_F(DispenserWithoutTlsTest, ChargingACarUpToRegularDisconnect) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);

    // Export Cable Check
    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCableCheck);
    sleep_for_ms(10);
    auto stop_request_counter_before_charging = get_stop_request_counter();

    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutput);
    EXPECT_EQ(get_last_power_requirement_request()->mode, fusion_charger::goose::Mode::ConstantCurrent);
    EXPECT_EQ(get_last_power_requirement_request()->current, 5);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 200);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);

    connector()->new_export_voltage_current(100, 1);
    sleep_for_ms(10);
    EXPECT_EQ(get_last_power_requirement_request()->current, 1);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 100);

    // OffCableCheck
    connector()->on_mode_phase_change(ModePhase::OffCableCheck);
    sleep_for_ms(10);

    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutputStoppage);
    EXPECT_EQ(get_last_power_requirement_request()->mode, fusion_charger::goose::Mode::ConstantCurrent);
    EXPECT_EQ(get_last_power_requirement_request()->current, 0);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 0);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);

    // Export Precharge
    connector()->on_mode_phase_change(ModePhase::ExportPrecharge);
    sleep_for_ms(10);

    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::PrechargeVoltageOutput);
    EXPECT_EQ(get_last_power_requirement_request()->mode, fusion_charger::goose::Mode::ConstantCurrent);
    EXPECT_EQ(get_last_power_requirement_request()->current, 1);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 100);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);

    connector()->new_export_voltage_current(300, 10);
    sleep_for_ms(10);
    EXPECT_EQ(get_last_power_requirement_request()->current, 10);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 300);

    // Export Charging
    connector()->on_mode_phase_change(ModePhase::ExportCharging);
    sleep_for_ms(10);

    EXPECT_EQ(get_last_power_requirement_request()->requirement_type, fusion_charger::goose::RequirementType::Charging);
    EXPECT_EQ(get_last_power_requirement_request()->mode, fusion_charger::goose::Mode::ConstantCurrent);
    EXPECT_EQ(get_last_power_requirement_request()->current, 10);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 300);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING);

    connector()->new_export_voltage_current(30, 1);
    sleep_for_ms(10);
    EXPECT_EQ(get_last_power_requirement_request()->current, 1);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 30);

    auto stop_request_counter_before_charge_complete = get_stop_request_counter();
    EXPECT_EQ(stop_request_counter_before_charge_complete, stop_request_counter_before_charging);

    // Completed
    connector()->on_mode_phase_change(ModePhase::Off);
    sleep_for_ms(10);

    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_COMPLETE);
    EXPECT_GT(get_stop_request_counter(), stop_request_counter_before_charge_complete);

    // Completed
    connector()->on_car_disconnected();
    EXPECT_EQ(get_connection_status(), ConnectionStatus::NOT_CONNECTED);
    sleep_for_ms(10);

    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY);
}

TEST_F(DispenserWithoutTlsTest, ChargingRestartWithoutDisconnect) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);

    connector()->new_export_voltage_current(200, 5);
    // Export Charging
    connector()->on_mode_phase_change(ModePhase::ExportCharging);
    sleep_for_ms(10);

    // Completed
    connector()->on_mode_phase_change(ModePhase::Off);
    sleep_for_ms(10);

    //////// Restart Charging ////////
    connector()->on_mode_phase_change(ModePhase::ExportCableCheck);
    sleep_for_ms(10);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED);

    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);

    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);
    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutput);
    EXPECT_EQ(get_last_power_requirement_request()->current, 5);
    EXPECT_EQ(get_last_power_requirement_request()->voltage, 200);
}

TEST_F(DispenserWithoutTlsTest, CarDisconnectBeforeHmacKey) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    sleep_for_ms(10);

    connector()->on_car_disconnected();
    sleep_for_ms(10);
    // We can't test stop requests because we don't have an HMAC key
    EXPECT_EQ(get_connection_status(), ConnectionStatus::NOT_CONNECTED);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY);
}

TEST_F(DispenserWithoutTlsTest, CarDisconnectBeforeCableCheck) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    sleep_for_ms(10);
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);
    connector()->new_export_voltage_current(200, 5);
    sleep_for_ms(10);

    auto power_stack_counter = get_stop_request_counter();
    connector()->on_car_disconnected();
    sleep_for_ms(10);
    EXPECT_GT(get_stop_request_counter(), power_stack_counter);
    EXPECT_EQ(get_connection_status(), ConnectionStatus::NOT_CONNECTED);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY);
}

TEST_F(DispenserWithoutTlsTest, CarDisconnectDuringCharging) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    sleep_for_ms(10);
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);
    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCableCheck);
    sleep_for_ms(10);

    auto power_stack_counter = get_stop_request_counter();
    connector()->on_car_disconnected();
    sleep_for_ms(10);
    EXPECT_GT(get_stop_request_counter(), power_stack_counter);
    EXPECT_EQ(get_connection_status(), ConnectionStatus::NOT_CONNECTED);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::STANDBY);
}

TEST_F(DispenserWithoutTlsTest, FaultsGetPropagatedCorrectly) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();
    connector()->on_car_connected();
    sleep_for_ms(10);
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);
    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCableCheck);

    power_stack_mock->set_psu_running_mode(PSURunningMode::FAULTY);

    EXPECT_EQ(dispenser->get_psu_running_mode(), PSURunningMode::FAULTY);
}
