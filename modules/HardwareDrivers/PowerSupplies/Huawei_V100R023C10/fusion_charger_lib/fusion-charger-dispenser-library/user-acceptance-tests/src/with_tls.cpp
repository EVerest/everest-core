// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "dispenser.hpp"
#include "fusion_charger/goose/power_request.hpp"
#include "fusion_charger/modbus/registers/error.hpp"
#include "power_stack_mock/power_stack_mock.hpp"
#include "user_acceptance_tests/dispenser_test_fixture.hpp"
#include "gmock/gmock.h"

using namespace std;

using namespace user_acceptance_tests::dispenser_fixture;

TEST_F(DispenserWithTlsTest, StateCarDisconnected) {
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

TEST_F(DispenserWithTlsTest, CarConnectedAndReadyToCharge) {
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

TEST_F(DispenserWithTlsTest, ChargingACarUpToRegularDisconnect) {
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

TEST_F(DispenserWithTlsTest, ChargingRestartWithoutDisconnect) {
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

TEST_F(DispenserWithTlsTest, CarDisconnectBeforeHmacKey) {
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

TEST_F(DispenserWithTlsTest, CarDisconnectBeforeCableCheck) {
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

TEST_F(DispenserWithTlsTest, CarDisconnectDuringCharging) {
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

TEST_F(DispenserWithTlsTest, FaultsGetPropagatedCorrectly) {
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

TEST_F(DispenserWithTlsTest, CheckMaximumRatedChargeCurrentIsSet) {
    EXPECT_EQ(get_maximum_rated_charge_current(), 100);
}

TEST_F(DispenserWithTlsTest, ReadDispenserInformationAndConfiguration) {
    auto expected_dispenser_information = DispenserInformation{.manufacturer = 0x0002,
                                                               .model = 0x0080,
                                                               .protocol_version = 1,
                                                               .hardware_version = 3,
                                                               .software_version = "v1.2.3+456"};

    EXPECT_EQ(power_stack_mock->get_dispenser_information(), expected_dispenser_information);

    auto expected_esn = "01234567890ABCDEF";
    EXPECT_EQ(power_stack_mock->get_dispenser_esn(), expected_esn);
}

// This leads to the everest module restarting the dispenser object tested
// here
// (stop(); start(); is called)
TEST_F(DispenserWithTlsTest, PsuCommunicationStateIsFailedAfterModbusHeartbeatTimeout) {
    sleep_for_ms(2'100);
    auto init_state = dispenser->get_psu_communication_state();

    EXPECT_EQ(init_state, DispenserPsuCommunicationState::FAILED);
}

TEST_F(DispenserWithTlsTest, CarConnectWithoutModePhaseChangeContinuesSendingModulePlaceholderRequests) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    connector()->on_car_connected();
    sleep_for_ms(10);

    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);
    auto post_connect_stop_frame_counter = get_stop_request_counter();
    auto post_connect_power_requirement_counter = get_power_requirements_counter();

    sleep_for_ms(50);

    // Still sends module placeholder requests
    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::ModulePlaceholderRequest);
    EXPECT_GT(get_power_requirements_counter(), post_connect_power_requirement_counter);

    // No stop requests sent
    EXPECT_EQ(get_stop_request_counter(), post_connect_stop_frame_counter);
}

TEST_F(DispenserWithTlsTest, TimeSyncEverySecond) {
    auto time1 = power_stack_mock->get_utc_time();
    EXPECT_NEAR(time1, (std::uint32_t)std::time(NULL), 1);

    sleep_for_ms(1'000);
    auto time2 = power_stack_mock->get_utc_time();

    EXPECT_EQ(time1 + 1, time2);
}

TEST_F(DispenserWithTlsTest, ErrorReportingDefaultState) {
    EXPECT_THAT(dispenser->get_raised_errors(), testing::IsEmpty());
}

TEST_F(DispenserWithTlsTest, ErrorReportingPowerUnit) {
    power_stack_mock->write_registers(0x4000, {(std::uint16_t)AlarmStatus::ALARM});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::PowerUnit;
    error_event_1.error_subcategory.power_unit = ErrorSubcategoryPowerUnit::HighVoltageDoorStatusSensor;
    error_event_1.payload.alarm = AlarmStatus::ALARM;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1));

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::PowerUnit;
    error_event_2.error_subcategory.power_unit = ErrorSubcategoryPowerUnit::HighVoltageDoorStatusSensor;
    error_event_2.payload.alarm = AlarmStatus::ALARM;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_2));

    power_stack_mock->write_registers(0x4000, {(std::uint16_t)AlarmStatus::NORMAL});
    sleep_for_ms(10);

    EXPECT_THAT(dispenser->get_raised_errors(), testing::IsEmpty());
}

TEST_F(DispenserWithTlsTest, ErrorReportingChargingPowerUnit) {
    power_stack_mock->write_registers(0x4008, {(std::uint16_t)AlarmStatus::ALARM});
    power_stack_mock->write_registers(0x4012, {25});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::ChargingPowerUnit;
    error_event_1.error_subcategory.charging_power_unit = ErrorSubcategoryChargingPowerUnit::SoftStartFault;
    error_event_1.payload.alarm = AlarmStatus::ALARM;

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::ChargingPowerUnit;
    error_event_2.error_subcategory.charging_power_unit = ErrorSubcategoryChargingPowerUnit::ModbusTcpCertificate;
    error_event_2.payload.error_flags = 25;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1, error_event_2));
}

TEST_F(DispenserWithTlsTest, ErrorReportingAcBranch) {
    power_stack_mock->write_registers(0x4020, {0, 1});
    power_stack_mock->write_registers(0x4022, {1, 0});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::AcBranch;
    error_event_1.error_subcategory.ac_branch = ErrorSubcategoryAcBranch::AcBranch1;
    error_event_1.payload.error_flags = 1;

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::AcBranch;
    error_event_2.error_subcategory.ac_branch = ErrorSubcategoryAcBranch::AcBranch2;
    error_event_2.payload.error_flags = 0x10000;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1, error_event_2));
}

TEST_F(DispenserWithTlsTest, ErrorReportingAcDcRectifier) {
    power_stack_mock->write_registers(0x4040, {0, 1});
    power_stack_mock->write_registers(0x404A, {1, 0});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::AcDcRectifier;
    error_event_1.error_subcategory.ac_dc_rectifier = ErrorSubcategoryAcDcRectifier::rectifier_1;
    error_event_1.payload.error_flags = 1;

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::AcDcRectifier;
    error_event_2.error_subcategory.ac_dc_rectifier = ErrorSubcategoryAcDcRectifier::rectifier_6;
    error_event_2.payload.error_flags = 0x10000;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1, error_event_2));
}

TEST_F(DispenserWithTlsTest, ErrorReportingDcDcChargingModule) {
    power_stack_mock->write_registers(0x4070, {0, 1});
    power_stack_mock->write_registers(0x4086, {1, 0});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::DcDcChargingModule;
    error_event_1.error_subcategory.dc_dc_charging_module = ErrorSubcategoryDcDcChargingModule::DcDcModule1;
    error_event_1.payload.error_flags = 1;

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::DcDcChargingModule;
    error_event_2.error_subcategory.dc_dc_charging_module = ErrorSubcategoryDcDcChargingModule::DcDcModule12;
    error_event_2.payload.error_flags = 0x10000;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1, error_event_2));
}

TEST_F(DispenserWithTlsTest, ErrorReportingCoolingSection) {
    power_stack_mock->write_registers(0x40D0, {0x12, 0x3456});
    sleep_for_ms(10);

    ErrorEvent error_event;
    error_event.error_category = ErrorCategory::CoolingSection;
    error_event.error_subcategory.cooling_section = ErrorSubcategoryCoolingSection::CoolingUnit1;
    error_event.payload.error_flags = 0x123456;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event));
}

TEST_F(DispenserWithTlsTest, ErrorReportingPowerDistributionModule) {
    power_stack_mock->write_registers(0x40E0, {0, 1});
    power_stack_mock->write_registers(0x40E8, {1, 0});
    sleep_for_ms(10);

    ErrorEvent error_event_1;
    error_event_1.error_category = ErrorCategory::ErrorSubcategoryPowerDistributionModule;
    error_event_1.error_subcategory.power_distribution_module =
        ErrorSubcategoryPowerDistributionModule::PowerDistributionModule1;
    error_event_1.payload.error_flags = 1;

    ErrorEvent error_event_2;
    error_event_2.error_category = ErrorCategory::ErrorSubcategoryPowerDistributionModule;
    error_event_2.error_subcategory.power_distribution_module =
        ErrorSubcategoryPowerDistributionModule::PowerDistributionModule5;
    error_event_2.payload.error_flags = 0x10000;

    EXPECT_THAT(dispenser->get_raised_errors(), testing::ElementsAre(error_event_1, error_event_2));
}

TEST_F(DispenserWithTlsTest, String) {
    ErrorEvent e;
    e.error_category = ErrorCategory::PowerUnit;
    e.error_subcategory.power_unit = ErrorSubcategoryPowerUnit::HighVoltageDoorStatusSensor;
    e.payload.alarm = AlarmStatus::ALARM;

    printf("%s\n", e.to_error_log_string().c_str());

    e.error_category = ErrorCategory::ChargingPowerUnit;
    e.error_subcategory = {.charging_power_unit = ErrorSubcategoryChargingPowerUnit::PhaseSequenceAbornmalAlarm};
    e.payload.alarm = AlarmStatus::NORMAL;
    printf("%s\n", e.to_error_log_string().c_str());

    e.error_category = ErrorCategory::ChargingPowerUnit;
    e.error_subcategory = {.charging_power_unit = ErrorSubcategoryChargingPowerUnit::ModbusTcpCertificate};
    e.payload.error_flags = 2;
    printf("%s\n", e.to_error_log_string().c_str());

    e.error_category = ErrorCategory::CoolingSection;
    e.error_subcategory = {.cooling_section = ErrorSubcategoryCoolingSection::CoolingUnit1};
    e.payload.error_flags = 0x12345678;
    printf("%s\n", e.to_error_log_string().c_str());

    e.payload.error_flags = 0x102;
    printf("%s\n", e.to_error_log_string().c_str());
}

TEST_F(DispenserWithTlsTest, module_placeholder_allocation_timeout) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    power_stack_mock->set_enable_answer_module_placeholder_allocation(false);

    connector()->on_car_connected();

    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCharging);
    power_stack_mock->send_hmac_key(1);

    sleep_for_ms(100);

    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);

    // Wait 3s while keeping the modbus connection happy
    for (int i = 0; i < 4; i++) {
        sleep_for_ms(1000);
        power_stack_mock->send_mac_address();
    }

    sleep_for_ms(500);

    // connection still established
    EXPECT_EQ(dispenser->get_psu_communication_state(), DispenserPsuCommunicationState::READY);

    EXPECT_EQ(connector()->module_placeholder_allocation_failed(), true);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING);
}

TEST_F(DispenserWithTlsTest, module_placeholder_allocation_timeout_then_normal) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    power_stack_mock->set_enable_answer_module_placeholder_allocation(false);

    connector()->on_car_connected();

    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCharging);
    power_stack_mock->send_hmac_key(1);

    sleep_for_ms(100);

    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING_STARTING);

    // Wait 3s while keeping the modbus connection happy
    for (int i = 0; i < 3; i++) {
        sleep_for_ms(1000);
        power_stack_mock->send_mac_address();
    }

    sleep_for_ms(500);

    // connection still established
    EXPECT_EQ(dispenser->get_psu_communication_state(), DispenserPsuCommunicationState::READY);

    EXPECT_EQ(connector()->module_placeholder_allocation_failed(), true);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING);
    connector()->on_car_disconnected();

    power_stack_mock->set_enable_answer_module_placeholder_allocation(true);

    sleep_for_ms(100);
    connector()->on_car_connected();
    power_stack_mock->send_hmac_key(1);
    connector()->new_export_voltage_current(200, 5);
    connector()->on_mode_phase_change(ModePhase::ExportCharging);
    sleep_for_ms(100);
    EXPECT_EQ(connector()->module_placeholder_allocation_failed(), false);
    EXPECT_EQ(connector()->get_working_status(), WorkingStatus::CHARGING);
}

TEST_F(DispenserWithTlsTest, acquire_hmac_post_start) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    auto stop_frame_counter = this->get_stop_request_counter();
    sleep_for_ms(10);

    connector()->on_mode_phase_change(ModePhase::ExportCableCheck);

    EXPECT_EQ(this->get_stop_request_counter(), stop_frame_counter);

    auto thread = std::thread([this]() {
        while (connector()->get_working_status() != WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED) {
            std::this_thread::sleep_for(100ms);
            // send mac address to keep modbus alive
            this->power_stack_mock->send_mac_address();
        }

        power_stack_mock->send_hmac_key(1);
    });
    connector()->car_connect_disconnect_cycle(std::chrono::seconds(10));

    sleep_for_ms(10);

    EXPECT_GT(this->get_stop_request_counter(), stop_frame_counter);

    thread.join();

    // Now do a simple charge (and check that on_mode_phase_change is
    // persistent/restored)
    connector()->new_export_voltage_current(200, 5);
    connector()->on_car_connected();
    power_stack_mock->send_hmac_key(1);
    sleep_for_ms(10);

    EXPECT_EQ(get_last_power_requirement_request()->requirement_type,
              fusion_charger::goose::RequirementType::InsulationDetectionVoltageOutput);
}

TEST_F(DispenserWithTlsTest, acquire_hmac_post_start_timeout) {
    power_stack_mock->set_psu_running_mode(PSURunningMode::RUNNING);
    power_stack_mock->send_mac_address();

    auto stop_frame_counter = this->get_stop_request_counter();
    sleep_for_ms(10);

    EXPECT_EQ(this->get_stop_request_counter(), stop_frame_counter);

    // Same as acquire_hmac_post_start but without sending the hmac key
    auto thread = std::thread([this]() {
        while (connector()->get_working_status() != WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED) {
            std::this_thread::sleep_for(100ms);
            // send mac address to keep modbus alive
            this->power_stack_mock->send_mac_address();
        }
    });

    auto time_before = std::chrono::steady_clock::now();
    connector()->car_connect_disconnect_cycle(std::chrono::milliseconds(3000));
    auto time_needed = std::chrono::steady_clock::now() - time_before;
    auto ms_needed = std::chrono::duration_cast<std::chrono::milliseconds>(time_needed).count();

    EXPECT_NEAR(ms_needed, 3000, 150); // wait for 3 seconds (timeout)

    sleep_for_ms(10);

    // No stop requests sent (as no hmac key was sent)
    EXPECT_EQ(this->get_stop_request_counter(), stop_frame_counter);

    thread.join();
}
