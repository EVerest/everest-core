/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "VirtualPaymentTerminal/VirtualPT.hpp"

#include <chrono>
#include <limits>
#include <optional>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(VirtualPT, construct_without_preauthorizations) {
    pterminal::VirtualPT vpt;

    auto ids = vpt.get_open_preauthorizations_ids();

    EXPECT_TRUE(ids.empty());
}

TEST(VirtualPT, construct_with_preauthorizations) {
    pterminal::VirtualPT vpt;

    vpt.add_open_preauthorization(0, 1000);
    vpt.add_open_preauthorization(1, 2000);
    vpt.add_open_preauthorization(2, 3000);

    auto ids = vpt.get_open_preauthorizations_ids();

    EXPECT_EQ(ids.size(), 3);
}

TEST(VirtualPT, cancel_preauthorizations) {
    pterminal::VirtualPT vpt;

    vpt.add_open_preauthorization(0, 1000);
    vpt.add_open_preauthorization(1, 2000);
    vpt.add_open_preauthorization(2, 3000);

    auto ids = vpt.get_open_preauthorizations_ids();
    auto size = ids.size();
    EXPECT_EQ(size, 3);

    for (const auto& id : ids) {
        auto success = vpt.cancel_preauthorization(id);
        EXPECT_TRUE(success);
        EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), --size);
    }
}

TEST(VirtualPT, cancel_non_existing_preauthorization) {
    pterminal::VirtualPT vpt;

    const pterminal::receipt_id_t DOES_NOT_EXIST = 99;
    auto success = vpt.cancel_preauthorization(DOES_NOT_EXIST);

    EXPECT_FALSE(success);
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, partial_reversal_on_non_existing_preauthorization) {
    pterminal::VirtualPT vpt;

    const pterminal::receipt_id_t DOES_NOT_EXIST = 99;
    auto success = vpt.partial_reversal(DOES_NOT_EXIST, 0);

    EXPECT_FALSE(success);
}

TEST(VirtualPT, legal_partial_reversal_on_existing_preauthorization) {
    pterminal::VirtualPT vpt;

    vpt.add_open_preauthorization(0, 1000);
    vpt.add_open_preauthorization(1, 2000);
    vpt.add_open_preauthorization(2, 3000);

    auto ids = vpt.get_open_preauthorizations_ids();
    auto size = ids.size();

    auto success = vpt.partial_reversal(ids[0], 0);

    EXPECT_TRUE(success);
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), size - 1);
}

TEST(VirtualPT, illegal_partial_reversal_on_existing_preauthorization) {
    pterminal::VirtualPT vpt;

    vpt.add_open_preauthorization(0, 1000);
    vpt.add_open_preauthorization(1, 2000);
    vpt.add_open_preauthorization(2, 3000);

    auto ids = vpt.get_open_preauthorizations_ids();
    auto size = ids.size();

    auto success = vpt.partial_reversal(ids[0], std::numeric_limits<pterminal::money_amount_t>::max());

    EXPECT_FALSE(success);
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), size);
}

TEST(VirtualPT, publish_preauthorizations_callback_after_setting_it) {
    pterminal::VirtualPT vpt;

    testing::MockFunction<void(std::string)> mock_callback;

    // Should match after setting the callback
    EXPECT_CALL(mock_callback, Call("[]")).Times(testing::Exactly(1));

    vpt.set_publish_preauthorizations_callback(mock_callback.AsStdFunction());
}

TEST(VirtualPT, publish_preauthorizations_callback_after_cancel) {
    pterminal::VirtualPT vpt;

    vpt.add_open_preauthorization(0, 1000);
    vpt.add_open_preauthorization(1, 2000);
    vpt.add_open_preauthorization(2, 3000);

    testing::MockFunction<void(std::string)> mock_callback;

    // Should match after setting the callback
    EXPECT_CALL(mock_callback, Call("[0,1,2]")).Times(testing::Exactly(1));
    // Should match after scancel_preauthorization()
    EXPECT_CALL(mock_callback, Call("[0,2]")).Times(testing::Exactly(1));

    vpt.set_publish_preauthorizations_callback(mock_callback.AsStdFunction());
    vpt.cancel_preauthorization(1);
}

TEST(VirtualPT, present_cards_without_reading) {
    pterminal::VirtualPT vpt;

    vpt.present_RFID_Card({"RFID"});
    vpt.present_Banking_Card({"Banking"});

    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, present_rfid_card_with_reading) {
    pterminal::VirtualPT vpt;

    auto rfid_card_id = std::string("RFID");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_RFID_Card(rfid_card_id);

    present_card_thread.join();

    EXPECT_TRUE(card_data.has_value());
    EXPECT_EQ(card_data->card_id, rfid_card_id);
    EXPECT_FALSE(card_data->is_banking_card);
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, present_banking_card_with_reading) {
    pterminal::VirtualPT vpt;

    auto banking_card_id = std::string("BANKING");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_Banking_Card(banking_card_id);

    present_card_thread.join();

    EXPECT_TRUE(card_data.has_value());
    EXPECT_EQ(card_data->card_id, banking_card_id);
    EXPECT_TRUE(card_data->is_banking_card);
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, cancel_read) {
    pterminal::VirtualPT vpt;

    auto rfid_card_id = std::string("RFID");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.cancel_read();

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_RFID_Card(rfid_card_id);

    present_card_thread.join();

    EXPECT_FALSE(card_data.has_value());
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, req_preauthorization_without_having_read_card) {
    pterminal::VirtualPT vpt;

    auto id = vpt.request_preauthorization({"CARD01"}, 5000);

    EXPECT_FALSE(id.has_value());
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, req_preauthorization_with_RFID_card) {
    pterminal::VirtualPT vpt;

    auto rfid_card_id = std::string("RFID");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_RFID_Card(rfid_card_id);

    present_card_thread.join();

    EXPECT_TRUE(card_data.has_value());

    auto id = vpt.request_preauthorization({"RFID"}, 5000);

    EXPECT_FALSE(id.has_value());
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 0);
}

TEST(VirtualPT, req_preauthorization_with_Banking_card) {
    pterminal::VirtualPT vpt;

    auto banking_card_id = std::string("BANKING");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_Banking_Card(banking_card_id);

    present_card_thread.join();

    EXPECT_TRUE(card_data.has_value());

    auto id = vpt.request_preauthorization({"BANKING"}, 5000);

    EXPECT_TRUE(id.has_value());
    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 1);

    EXPECT_EQ(vpt.get_open_preauthorizations_ids().size(), 1);
}

TEST(VirtualPT, publish_preauthorizations_callback_after_card_presented) {
    pterminal::VirtualPT vpt;

    testing::MockFunction<void(std::string)> mock_callback;

    EXPECT_CALL(mock_callback, Call("[]")).Times(testing::Exactly(1));
    EXPECT_CALL(mock_callback, Call("[0]")).Times(testing::Exactly(1));

    vpt.set_publish_preauthorizations_callback(mock_callback.AsStdFunction());

    auto banking_card_id = std::string("BANKING");

    std::optional<pterminal::PTCardInteractionRecord> card_data;

    std::thread present_card_thread([&]() { card_data = vpt.read_card(); });

    // Make sure read_card() is ready (i.e. waiting for the condition_variable)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    vpt.present_Banking_Card(banking_card_id);

    present_card_thread.join();

    EXPECT_TRUE(card_data.has_value());

    auto result = vpt.request_preauthorization({"BANKING"}, 5000);
    // This should have triggered the second call of the mock_callback!
}