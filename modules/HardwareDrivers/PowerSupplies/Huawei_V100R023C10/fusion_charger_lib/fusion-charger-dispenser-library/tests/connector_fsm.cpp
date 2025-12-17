// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <connector.hpp>

struct ConnectorFSM_Fixture : public ::testing::Test {
    ConnectorFSM::Callbacks callbacks{
        .state_transition = [this](States state) { state_transition_counter++; },
        .mode_phase_transition = [this](ModePhase mode_phase) { mode_phase_transition_counter++; },
        .any_transition = [this](States state, ModePhase mode_phase) { any_transition_counter++; },
    };
    ConnectorFSM fsm{callbacks, logs::log_printf};

    std::uint32_t state_transition_counter = 0;
    std::uint32_t mode_phase_transition_counter = 0;
    std::uint32_t any_transition_counter = 0;

    void SetUp() override {
        state_transition_counter = 0;
        mode_phase_transition_counter = 0;
        any_transition_counter = 0;
    }
};

TEST_F(ConnectorFSM_Fixture, initial_state) {
    EXPECT_EQ(fsm.get_state(), States::CarDisconnected);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::Off);
}

TEST_F(ConnectorFSM_Fixture, connect_car) {
    fsm.on_car_connected();
    EXPECT_EQ(fsm.get_state(), States::NoKeyYet);
    EXPECT_EQ(state_transition_counter, 1);
    EXPECT_EQ(mode_phase_transition_counter, 0);
    EXPECT_EQ(any_transition_counter, 1);
}

TEST_F(ConnectorFSM_Fixture, connect_car_twice) {
    fsm.on_car_connected();
    fsm.on_car_connected();
    EXPECT_EQ(fsm.get_state(), States::NoKeyYet);
    EXPECT_EQ(state_transition_counter, 1);
    EXPECT_EQ(mode_phase_transition_counter, 0);
    EXPECT_EQ(any_transition_counter, 1);
}

TEST_F(ConnectorFSM_Fixture, regular_state_flow) {
    fsm.on_car_connected();
    EXPECT_EQ(fsm.get_state(), States::NoKeyYet);
    fsm.on_hmac_key_received();
    EXPECT_EQ(fsm.get_state(), States::ConnectedNoAllocation);
    fsm.on_module_placeholder_allocation_response(true);
    EXPECT_EQ(fsm.get_state(), States::Running);
    fsm.on_mode_phase_change(ModePhase::ExportCableCheck);
    EXPECT_EQ(fsm.get_state(), States::Running);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::ExportCableCheck);
    fsm.on_mode_phase_change(ModePhase::OffCableCheck);
    EXPECT_EQ(fsm.get_state(), States::Running);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::OffCableCheck);
    fsm.on_mode_phase_change(ModePhase::ExportPrecharge);
    EXPECT_EQ(fsm.get_state(), States::Running);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::ExportPrecharge);
    fsm.on_mode_phase_change(ModePhase::ExportCharging);
    EXPECT_EQ(fsm.get_state(), States::Running);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::ExportCharging);
    fsm.on_mode_phase_change(ModePhase::Off);
    EXPECT_EQ(fsm.get_state(), States::Completed);
    EXPECT_EQ(fsm.get_mode_phase(), ModePhase::Off);
    fsm.on_car_disconnected();
    EXPECT_EQ(fsm.get_state(), States::CarDisconnected);
}

TEST_F(ConnectorFSM_Fixture, car_disconnect_from_any) {
    // From NoKeyYet
    fsm.on_car_connected();
    EXPECT_EQ(fsm.get_state(), States::NoKeyYet);
    fsm.on_car_disconnected();
    EXPECT_EQ(fsm.get_state(), States::CarDisconnected);

    // From ConnectedNoAllocation
    fsm.on_car_connected();
    fsm.on_hmac_key_received();
    EXPECT_EQ(fsm.get_state(), States::ConnectedNoAllocation);
    fsm.on_car_disconnected();
    EXPECT_EQ(fsm.get_state(), States::CarDisconnected);

    // From Running
    fsm.on_car_connected();
    fsm.on_hmac_key_received();
    fsm.on_module_placeholder_allocation_response(true);
    EXPECT_EQ(fsm.get_state(), States::Running);
    fsm.on_car_disconnected();
    EXPECT_EQ(fsm.get_state(), States::CarDisconnected);

    // From Completed
    fsm.on_car_connected();
    fsm.on_hmac_key_received();
    fsm.on_module_placeholder_allocation_response(true);
    fsm.on_mode_phase_change(ModePhase::Off);
    EXPECT_EQ(fsm.get_state(), States::Completed);
    fsm.on_car_disconnected();
}
