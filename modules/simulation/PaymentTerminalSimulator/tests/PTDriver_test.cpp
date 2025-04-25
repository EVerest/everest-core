/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "PaymentTerminalDriver/PTDriver.hpp"
#include "VirtualPaymentTerminal/VirtualPT.hpp"
// #include "include/types.hpp"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace pterminal {

class PaymentTerminalMock : public PaymentTerminalIfc, public PTSimulationIfc {
public:
    MOCK_METHOD(std::vector<receipt_id_t>, get_open_preauthorizations_ids, (), (override, const));
    MOCK_METHOD(std::optional<PTCardInteractionRecord>, read_card, (), (override));
    MOCK_METHOD(std::optional<receipt_id_t>, request_preauthorization, (std::string, money_amount_t), (override));
    MOCK_METHOD(void, cancel_read, (), (override));
    MOCK_METHOD(bool, cancel_preauthorization, (receipt_id_t), (override));
    MOCK_METHOD(bool, partial_reversal, (receipt_id_t, money_amount_t), (override));

    MOCK_METHOD(void, add_open_preauthorization, (const receipt_id_t&, money_amount_t), (override));
    MOCK_METHOD(void, present_RFID_Card, (const std::string&), (override));
    MOCK_METHOD(void, present_Banking_Card, (const std::string&), (override));
    MOCK_METHOD(void, set_publish_preauthorizations_callback, (const std::function<void(std::string)>&), (override));
};

class PTDriverFixture : public ::testing::Test {
protected:
    std::shared_ptr<::testing::NiceMock<PaymentTerminalMock>> vpt;
    PTDriver pt_drv;

    PTDriverFixture() : vpt(std::make_shared<::testing::NiceMock<PaymentTerminalMock>>()), pt_drv(vpt) {
    }

    void SetUp() override {
        // Set a global default for all tests
        ON_CALL(*vpt, get_open_preauthorizations_ids()).WillByDefault(::testing::Return(std::vector<receipt_id_t>{}));
    }
};

TEST_F(PTDriverFixture, construct) {
    EXPECT_TRUE(true);
}

TEST_F(PTDriverFixture, try_cancel_nonexisting_preauthorization) {
    bool success = pt_drv.cancel_preauthorization({"DOESNOTEXIST"});

    EXPECT_CALL(*vpt, cancel_preauthorization(::testing::_)).Times(testing::Exactly(0));

    EXPECT_FALSE(success);
}

TEST_F(PTDriverFixture, try_reverse_nonexisting_preauthorization) {
    bool success = pt_drv.partial_reversal({"DOESNOTEXIST"}, 4000);

    EXPECT_CALL(*vpt, partial_reversal(::testing::_, ::testing::_)).Times(testing::Exactly(0));

    EXPECT_FALSE(success);
}

TEST_F(PTDriverFixture, try_preauthorization_without_card) {
    auto result = pt_drv.request_preauthorization_for_id("ABCD", 5000);

    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).Times(testing::Exactly(0));

    EXPECT_FALSE(result.has_value());
}

TEST_F(PTDriverFixture, read_rfid_card) {
    const std::string card_id{"RFID1234"};

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = false};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });

    auto card = pt_drv.read_card();

    EXPECT_TRUE(card.has_value());
    EXPECT_TRUE(card->auth_id.has_value());
    EXPECT_EQ(card->auth_id.value(), card_id);
    EXPECT_EQ(card->card_id, card_id);
    EXPECT_EQ(card->is_banking_card, false);
}

TEST_F(PTDriverFixture, read_banking_card) {
    const std::string card_id{"BANK7890"};

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });

    auto card = pt_drv.read_card();

    EXPECT_TRUE(card.has_value());
    EXPECT_FALSE(card->auth_id.has_value());
    EXPECT_EQ(card->card_id, card_id);
    EXPECT_EQ(card->is_banking_card, true);
}

TEST_F(PTDriverFixture, cancel_read_card) {
    std::mutex mtx;
    std::condition_variable cvar;
    std::atomic_bool canceled{false};

    EXPECT_CALL(*vpt, read_card()).WillOnce([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        cvar.wait(lock, [&]() { return canceled.load(); });
        return std::nullopt;
    });

    EXPECT_CALL(*vpt, cancel_read()).WillOnce([&]() {
        canceled.store(true);
        cvar.notify_all();
    });

    std::optional<PTDriver::CardData> card;
    std::thread read_thread([&] { card = pt_drv.read_card(); });
    {
        using namespace std::chrono;
        std::this_thread::sleep_for(50ms);
    }
    pt_drv.cancel_read_card();

    read_thread.join();

    EXPECT_FALSE(card.has_value());
}

TEST_F(PTDriverFixture, read_preauthorize_banking_card) {
    const std::string card_id{"BANK7890"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });

    auto card = pt_drv.read_card();
    auto preauth = pt_drv.request_preauthorization_for_id("ABCD", 5000);

    EXPECT_TRUE(preauth.has_value());
    EXPECT_EQ(preauth.value().receipt_id, receipt_id);
    EXPECT_EQ(preauth.value().card_id, card_id);
    EXPECT_EQ(preauth.value().amount, 5000);
}

TEST_F(PTDriverFixture, read_preauthorize_read_banking_card) {
    const std::string card_id{"BANK7890"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card())
        .Times(2)
        .WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; })
        .WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });

    auto card1 = pt_drv.read_card();
    auto preauth = pt_drv.request_preauthorization_for_id("BANK7890", 5000);
    auto card2 = pt_drv.read_card();

    EXPECT_TRUE(card2.has_value());
    EXPECT_TRUE(card2->auth_id.has_value());
    EXPECT_EQ(card2->auth_id.value(), card_id);
    EXPECT_EQ(card2->card_id, card_id);
    EXPECT_EQ(card2->is_banking_card, true);
}

TEST_F(PTDriverFixture, read_double_preauthorize_banking_card) {
    const std::string card_id{"BANK7890"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });

    auto card = pt_drv.read_card();
    auto preauth1 = pt_drv.request_preauthorization_for_id("BANK7890", 5000);
    auto preauth2 = pt_drv.request_preauthorization_for_id("BANK7890", 6000);

    EXPECT_TRUE(preauth1.has_value());
    EXPECT_EQ(preauth1.value().receipt_id, receipt_id);
    EXPECT_EQ(preauth1.value().amount, 5000);
    EXPECT_FALSE(preauth2.has_value());
}

TEST_F(PTDriverFixture, read_preauthorize_cancel_banking_card) {
    const std::string card_id{"BANK7890"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });
    EXPECT_CALL(*vpt, cancel_preauthorization(::testing::_)).WillOnce([=]() { return true; });

    auto card = pt_drv.read_card();
    auto preauth = pt_drv.request_preauthorization_for_id("ABCD", 5000);
    auto success = pt_drv.cancel_preauthorization("ABCD");

    EXPECT_TRUE(success);
}

TEST_F(PTDriverFixture, read_preauthorize_reverse_banking_card) {
    const std::string card_id{"BANK7890"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = true};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });
    EXPECT_CALL(*vpt, partial_reversal(::testing::_, ::testing::_)).WillOnce([=]() { return true; });

    auto card = pt_drv.read_card();
    auto preauth = pt_drv.request_preauthorization_for_id("ABCD", 5000);
    auto success = pt_drv.partial_reversal("ABCD", 4000);

    EXPECT_TRUE(success);
}

TEST_F(PTDriverFixture, read_preauthorize_rfid_card) {
    const std::string card_id{"RFID1234"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record{.card_id = card_id, .is_banking_card = false};

    EXPECT_CALL(*vpt, read_card()).WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record}; });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).Times(testing::Exactly(0));

    auto card = pt_drv.read_card();
    auto preauth = pt_drv.request_preauthorization_for_id("ABCD", 5000);

    EXPECT_FALSE(preauth.has_value());
}

TEST(PTDriver, restore_preauthorization_wrong_id) {
    auto vpt = std::make_shared<::testing::NiceMock<PaymentTerminalMock>>();

    ON_CALL(*vpt, get_open_preauthorizations_ids()).WillByDefault(::testing::Return(std::vector<receipt_id_t>{0x9999}));

    PTDriver pt_drv(vpt);

    const bank_session_token_t bank_session_id{"BANKTOKEN"};

    PTDriver::Preauthorization preauth = {
        .receipt_id = 0x0102,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success = pt_drv.restore_preauthorization(bank_session_id, preauth);

    EXPECT_FALSE(success);
}


/////////////////////////////// TEST: Banking_Id already stored; AND try to restore twice

TEST(PTDriver, restore_preauthorization) {
    auto vpt = std::make_shared<::testing::NiceMock<PaymentTerminalMock>>();

    ON_CALL(*vpt, get_open_preauthorizations_ids()).WillByDefault(::testing::Return(std::vector<receipt_id_t>{0x0102}));

    PTDriver pt_drv(vpt);

    const bank_session_token_t bank_session_id{"BANKTOKEN"};

    PTDriver::Preauthorization preauth = {
        .receipt_id = 0x0102,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success_restore = pt_drv.restore_preauthorization(bank_session_id, preauth);

    EXPECT_CALL(*vpt, cancel_preauthorization(::testing::_)).WillOnce([=]() { return true; });
    auto success_cancel = pt_drv.cancel_preauthorization(bank_session_id);

    EXPECT_TRUE(success_restore);
    EXPECT_TRUE(success_cancel);
}

TEST(PTDriver, restore_preauthorization_twice) {
    auto vpt = std::make_shared<::testing::NiceMock<PaymentTerminalMock>>();

    ON_CALL(*vpt, get_open_preauthorizations_ids()).WillByDefault(::testing::Return(std::vector<receipt_id_t>{0x0102, 0x0103}));

    PTDriver pt_drv(vpt);

    const bank_session_token_t bank_session_id_1{"BANKTOKEN_1"};
    const bank_session_token_t bank_session_id_2{"BANKTOKEN_2"};

    PTDriver::Preauthorization preauth_1 = {
        .receipt_id = 0x0102,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success_1 = pt_drv.restore_preauthorization(bank_session_id_1, preauth_1);

    PTDriver::Preauthorization preauth_2 = {
        .receipt_id = 0x0102,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success_2 = pt_drv.restore_preauthorization(bank_session_id_2, preauth_2);

    EXPECT_TRUE(success_1);
    EXPECT_FALSE(success_2);
}

TEST(PTDriver, restore_preauthorization_identical_bank_id) {
    auto vpt = std::make_shared<::testing::NiceMock<PaymentTerminalMock>>();

    ON_CALL(*vpt, get_open_preauthorizations_ids()).WillByDefault(::testing::Return(std::vector<receipt_id_t>{0x0102, 0x0103}));

    PTDriver pt_drv(vpt);

    const bank_session_token_t bank_session_id_1{"BANKTOKEN_1"};

    PTDriver::Preauthorization preauth_1 = {
        .receipt_id = 0x0102,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success_1 = pt_drv.restore_preauthorization(bank_session_id_1, preauth_1);

    PTDriver::Preauthorization preauth_2 = {
        .receipt_id = 0x0103,
        .card_id = "CARDID",
        .amount = 5000,
    };

    auto success_2 = pt_drv.restore_preauthorization(bank_session_id_1, preauth_2);

    EXPECT_TRUE(success_1);
    EXPECT_FALSE(success_2);
}

TEST_F(PTDriverFixture, interrupt_reading) {
    const std::string card_id_1{"BANK7890_1"};
    const std::string card_id_2{"BANK7890_2"};
    const receipt_id_t receipt_id = 0x2345;

    PTCardInteractionRecord pt_card_record_1{.card_id = card_id_1, .is_banking_card = true};
    PTCardInteractionRecord pt_card_record_2{.card_id = card_id_2, .is_banking_card = true};

    std::mutex mtx;
    std::condition_variable cvar;
    std::atomic_bool canceled{false};

    EXPECT_CALL(*vpt, read_card())
        .WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record_1}; })
        .WillOnce([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            cvar.wait(lock, [&]() { return canceled.load(); });
            return std::nullopt;
        })
        .WillOnce([=]() { return std::optional<PTCardInteractionRecord>{pt_card_record_2}; });

    EXPECT_CALL(*vpt, cancel_read()).WillOnce([&]() {
        canceled.store(true);
        cvar.notify_all();
    });
    EXPECT_CALL(*vpt, request_preauthorization(::testing::_, ::testing::_)).WillOnce([=]() {
        return std::optional<receipt_id_t>{receipt_id};
    });

    auto card_1 = pt_drv.read_card();

    std::optional<PTDriver::CardData> card_2;
    std::thread read_thread([&] { card_2 = pt_drv.read_card(); });
    {
        using namespace std::chrono;
        std::this_thread::sleep_for(50ms);
    }
    auto preauth_1 = pt_drv.request_preauthorization_for_id(card_id_1, 5000);

    read_thread.join();

    EXPECT_TRUE(card_1.has_value());
    EXPECT_FALSE(card_1->auth_id.has_value());
    EXPECT_EQ(card_1->card_id, card_id_1);
    EXPECT_EQ(card_1->is_banking_card, true);
    EXPECT_TRUE(card_2.has_value());
    EXPECT_FALSE(card_2->auth_id.has_value());
    EXPECT_EQ(card_2->card_id, card_id_2);
    EXPECT_EQ(card_2->is_banking_card, true);
    EXPECT_TRUE(preauth_1.has_value());
    EXPECT_EQ(preauth_1.value().receipt_id, receipt_id);
    EXPECT_EQ(preauth_1.value().card_id, card_id_1);
    EXPECT_EQ(preauth_1.value().amount, 5000);
}

} // namespace pterminal
