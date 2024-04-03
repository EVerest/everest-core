// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <transaction_handler.hpp>

namespace module {

class TransactionHandlerTest : public ::testing::Test {

protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    std::shared_ptr<TransactionData> transaction_data() {
        return std::make_shared<TransactionData>(1, "123", ocpp::DateTime(), ocpp::v201::TriggerReasonEnum::Authorized,
                                                 ocpp::v201::ChargingStateEnum::Idle);
    }
};

TEST_F(TransactionHandlerTest, test_authorized) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::Authorized}, {TxStartStopPoint::Authorized});

    transaction_handler.add_transaction_data(1, transaction_data());

    auto res = transaction_handler.submit_event(1, TxEvent::AUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);
    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::AUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::DEAUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);
}

TEST_F(TransactionHandlerTest, test_power_path_closed) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::PowerPathClosed}, {TxStartStopPoint::PowerPathClosed});

    transaction_handler.add_transaction_data(1, transaction_data());

    auto res = transaction_handler.submit_event(1, TxEvent::AUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::EV_CONNECTED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);

    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::PARKING_BAY_UNOCCUPIED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::EV_CONNECTED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::EV_DISCONNECTED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);
}

TEST_F(TransactionHandlerTest, test_ev_connected) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::EVConnected}, {TxStartStopPoint::EVConnected});

    transaction_handler.add_transaction_data(1, transaction_data());

    auto res = transaction_handler.submit_event(1, TxEvent::EV_CONNECTED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);
    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::DEAUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::EV_DISCONNECTED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);
}

TEST_F(TransactionHandlerTest, test_parking_bay_occupied) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::ParkingBayOccupancy},
                                           {TxStartStopPoint::ParkingBayOccupancy});

    transaction_handler.add_transaction_data(1, transaction_data());

    auto res = transaction_handler.submit_event(1, TxEvent::PARKING_BAY_OCCUPIED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);
    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::EV_DISCONNECTED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::PARKING_BAY_UNOCCUPIED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);
}

TEST_F(TransactionHandlerTest, test_multiple) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::EVConnected, TxStartStopPoint::Authorized},
                                           {TxStartStopPoint::PowerPathClosed});
    transaction_handler.add_transaction_data(1, transaction_data());

    auto res = transaction_handler.submit_event(1, TxEvent::EV_CONNECTED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);
    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::DEAUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);

    transaction_handler.reset_transaction_data(1);

    transaction_handler.add_transaction_data(1, transaction_data());

    res = transaction_handler.submit_event(1, TxEvent::AUTHORIZED);
    ASSERT_EQ(res, TxEventEffect::START_TRANSACTION);
    transaction_handler.get_transaction_data(1)->started = true;

    res = transaction_handler.submit_event(1, TxEvent::SIGNED_START_DATA_RECEIVED);
    ASSERT_EQ(res, TxEventEffect::NONE);

    res = transaction_handler.submit_event(1, TxEvent::EV_DISCONNECTED);
    ASSERT_EQ(res, TxEventEffect::STOP_TRANSACTION);
}

TEST_F(TransactionHandlerTest, test_invalid_params) {
    TransactionHandler transaction_handler(2, {TxStartStopPoint::EVConnected, TxStartStopPoint::Authorized},
                                           {TxStartStopPoint::PowerPathClosed});
    ASSERT_THROW(transaction_handler.get_transaction_data(3), std::out_of_range);
    ASSERT_THROW(transaction_handler.reset_transaction_data(3), std::out_of_range);
    ASSERT_THROW(transaction_handler.add_transaction_data(-1, transaction_data()), std::out_of_range);
    ASSERT_THROW(transaction_handler.add_transaction_data(3, transaction_data()), std::out_of_range);
}

} // namespace module
