// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

using namespace ocpp::v16;
using ::testing::_;

class MockStatusNotificationCallback {
public:
    MOCK_METHOD(void, Call,
                (FSMState state, ChargePointErrorCode error_code, ocpp::DateTime timestamp,
                 std::optional<ocpp::CiString<50>> info, std::optional<ocpp::CiString<255>> vendor_id,
                 std::optional<ocpp::CiString<50>> vendor_error_code));
};

class ChargePointStateMachineTest : public ::testing::Test {
protected:
    void SetUp() override {
        status_notification_callback = [&](FSMState state, ChargePointErrorCode error_code, ocpp::DateTime timestamp,
                                           std::optional<ocpp::CiString<50>> info,
                                           std::optional<ocpp::CiString<255>> vendor_id,
                                           std::optional<ocpp::CiString<50>> vendor_error_code) {
            mock_callback.Call(state, error_code, timestamp, info, vendor_id, vendor_error_code);
        };

        state_machine = std::make_unique<ChargePointFSM>(status_notification_callback, FSMState::Available);
    }

    std::unique_ptr<ChargePointFSM> state_machine;
    MockStatusNotificationCallback mock_callback;
    std::function<void(FSMState, ChargePointErrorCode, ocpp::DateTime, std::optional<ocpp::CiString<50>>,
                       std::optional<ocpp::CiString<255>>, std::optional<ocpp::CiString<50>>)>
        status_notification_callback;
};

TEST_F(ChargePointStateMachineTest, HandleError) {
    ErrorInfo error_info_1("uuid1", ChargePointErrorCode::ConnectorLockFailure, true);
    ErrorInfo error_info_2("uuid2", ChargePointErrorCode::GroundFailure, true);
    EXPECT_CALL(mock_callback, Call(FSMState::Faulted, ChargePointErrorCode::ConnectorLockFailure, _, _, _, _))
        .Times(1);
    EXPECT_CALL(mock_callback, Call(FSMState::Faulted, ChargePointErrorCode::GroundFailure, _, _, _, _)).Times(1);

    state_machine->handle_error(error_info_1);
    state_machine->handle_error(error_info_2);
}

TEST_F(ChargePointStateMachineTest, HandleError__ChangeState) {
    ErrorInfo error_info_1("uuid1", ChargePointErrorCode::GroundFailure, false, "InfoField", "vendor_id");
    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::GroundFailure, _, _, _, _)).Times(1);
    EXPECT_CALL(mock_callback, Call(FSMState::Preparing, ChargePointErrorCode::GroundFailure, _, _, _, _)).Times(1);

    state_machine->handle_error(error_info_1);
    state_machine->handle_event(FSMEvent::UsageInitiated, ocpp::DateTime(), "AnotherInfoField");
}

TEST_F(ChargePointStateMachineTest, HandleErrorCleared) {
    ErrorInfo error_info("uuid1", ChargePointErrorCode::ConnectorLockFailure, true);
    state_machine->handle_error(error_info);

    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::NoError, _, _, _, _)).Times(1);

    state_machine->handle_error_cleared("uuid1");
}

TEST_F(ChargePointStateMachineTest, HandleErrorCleared__TwoErrors__OneCleared) {
    ErrorInfo error_info_1("uuid1", ChargePointErrorCode::ConnectorLockFailure, false);
    ErrorInfo error_info_2("uuid2", ChargePointErrorCode::GroundFailure, false);

    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::ConnectorLockFailure, _, _, _, _))
        .Times(1);
    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::GroundFailure, _, _, _, _)).Times(1);
    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::NoError, _, _, _, _)).Times(0);

    state_machine->handle_error(error_info_1);
    state_machine->handle_error(error_info_2);
    state_machine->handle_error_cleared("uuid2");

    const auto latest_error = state_machine->get_latest_error();

    EXPECT_TRUE(latest_error.has_value());
    EXPECT_EQ(latest_error.value().error_code, ChargePointErrorCode::ConnectorLockFailure);
}

TEST_F(ChargePointStateMachineTest, HandleError__NonFault) {
    ErrorInfo error_info("uuid1", ChargePointErrorCode::ConnectorLockFailure, false);
    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::ConnectorLockFailure, _, _, _, _))
        .Times(1);

    state_machine->handle_error(error_info);
}

TEST_F(ChargePointStateMachineTest, HandleErrorCleared__NonFault) {
    ErrorInfo error_info("uuid1", ChargePointErrorCode::ConnectorLockFailure, false);

    state_machine->handle_error(error_info);

    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::NoError, _, _, _, _)).Times(1);

    state_machine->handle_error_cleared("uuid1");
}

TEST_F(ChargePointStateMachineTest, HandleErrorCleared__ClearUnknown) {
    ErrorInfo error_info("uuid1", ChargePointErrorCode::ConnectorLockFailure, false);

    state_machine->handle_error(error_info);

    EXPECT_CALL(mock_callback, Call(FSMState::Available, ChargePointErrorCode::NoError, _, _, _, _)).Times(0);

    state_machine->handle_error_cleared("uuid2");
    state_machine->handle_error_cleared("uuid3");
    state_machine->handle_error_cleared("uuid4");
}
