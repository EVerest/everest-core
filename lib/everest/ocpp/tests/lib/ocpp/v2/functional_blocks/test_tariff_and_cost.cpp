// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/asio/io_context.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>
#include <ocpp/v2/messages/CostUpdated.hpp>

#include "component_state_manager_mock.hpp"
#include "connectivity_manager_mock.hpp"
#include "device_model_test_helper.hpp"
#include "evse_manager_fake.hpp"
#include "evse_security_mock.hpp"
#include "message_dispatcher_mock.hpp"
#include "meter_values_mock.hpp"
#include "mocks/database_handler_mock.hpp"

using namespace ocpp::v2;
using ocpp::IdentifierType;
using ocpp::RunningCost;
using ocpp::RunningCostState;
using ocpp::TariffMessage;
using ::testing::_;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Return;

static const std::string TARIFF_FALLBACK_MESSAGE = "Tariff: 0.30 EUR/kWh";
static const std::string OFFLINE_TARIFF_FALLBACK_MESSAGE = "Offline: 0.35 EUR/kWh";
static const std::string TOTAL_COST_FALLBACK_MESSAGE = "Total cost unavailable (offline)";
static const std::string TRANSACTION_ID = "txn-001";

class TariffAndCostTest : public ::testing::Test {
protected:
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    DeviceModel* device_model;
    NiceMock<ConnectivityManagerMock> connectivity_manager;
    NiceMock<DatabaseHandlerMock> database_handler_mock;
    ocpp::EvseSecurityMock evse_security;
    EvseManagerFake evse_manager;
    ComponentStateManagerMock component_state_manager;
    std::atomic<ocpp::OcppProtocolVersion> ocpp_version;
    FunctionalBlockContext functional_block_context;
    NiceMock<MeterValuesMock> meter_values_mock;
    boost::asio::io_context io_context;

    std::optional<TariffMessageCallback> tariff_message_callback_opt;
    std::optional<SetRunningCostCallback> set_running_cost_callback_opt;

    MockFunction<void(const TariffMessage&)> tariff_message_mock;
    MockFunction<void(const RunningCost&, std::uint32_t, std::optional<std::string>)> running_cost_mock;

    TariffAndCostTest() :
        device_model(device_model_test_helper.get_device_model()),
        evse_manager(1),
        ocpp_version(ocpp::OcppProtocolVersion::v201),
        functional_block_context{mock_dispatcher,       *device_model, connectivity_manager,    evse_manager,
                                 database_handler_mock, evse_security, component_state_manager, ocpp_version} {
    }

    std::unique_ptr<TariffAndCost> make_tariff_and_cost() {
        return std::make_unique<TariffAndCost>(functional_block_context, meter_values_mock, tariff_message_callback_opt,
                                               set_running_cost_callback_opt, io_context);
    }

    void set_tariff_enabled(bool available = true, bool enabled = true) {
        const auto& avail = ControllerComponentVariables::TariffCostCtrlrAvailableTariff;
        const auto& en = ControllerComponentVariables::TariffCostCtrlrEnabledTariff;
        ASSERT_EQ(device_model->set_value(avail.component, avail.variable.value(), AttributeEnum::Actual,
                                          available ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
        ASSERT_EQ(device_model->set_value(en.component, en.variable.value(), AttributeEnum::Actual,
                                          enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_cost_enabled(bool available = true, bool enabled = true) {
        const auto& avail = ControllerComponentVariables::TariffCostCtrlrAvailableCost;
        const auto& en = ControllerComponentVariables::TariffCostCtrlrEnabledCost;
        ASSERT_EQ(device_model->set_value(avail.component, avail.variable.value(), AttributeEnum::Actual,
                                          available ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
        ASSERT_EQ(device_model->set_value(en.component, en.variable.value(), AttributeEnum::Actual,
                                          enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_tariff_fallback_message(const std::string& msg) {
        Variable var;
        var.name = "TariffFallbackMessage";
        ASSERT_EQ(device_model->set_value(ControllerComponents::TariffCostCtrlr, var, AttributeEnum::Actual, msg,
                                          "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_total_cost_fallback_message(const std::string& msg) {
        Variable var;
        var.name = "TotalCostFallbackMessage";
        ASSERT_EQ(device_model->set_value(ControllerComponents::TariffCostCtrlr, var, AttributeEnum::Actual, msg,
                                          "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_offline_tariff_fallback_message(const std::string& msg) {
        Variable var;
        var.name = "OfflineTariffFallbackMessage";
        ASSERT_EQ(device_model->set_value(ControllerComponents::TariffCostCtrlr, var, AttributeEnum::Actual, msg,
                                          "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_tariff_fallback_message_instance(const std::string& instance, const std::string& msg) {
        Variable var;
        var.name = "TariffFallbackMessage";
        var.instance = instance;
        ASSERT_EQ(device_model->set_value(ControllerComponents::TariffCostCtrlr, var, AttributeEnum::Actual, msg,
                                          "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    // Helpers for building enhanced messages
    static ocpp::EnhancedMessage<MessageType> make_costupdated_message(const CostUpdatedRequest& req) {
        ocpp::Call<CostUpdatedRequest> call(req);
        ocpp::EnhancedMessage<MessageType> msg;
        msg.messageType = MessageType::CostUpdated;
        msg.message = call;
        return msg;
    }
};

// ---------------------------------------------------------------------------
// handle_message
// ---------------------------------------------------------------------------

TEST_F(TariffAndCostTest, HandleMessage_WrongType_Throws) {
    auto tc = make_tariff_and_cost();

    CostUpdatedRequest req;
    req.totalCost = 1.0f;
    req.transactionId = TRANSACTION_ID;
    ocpp::Call<CostUpdatedRequest> call(req);
    ocpp::EnhancedMessage<MessageType> msg;
    msg.messageType = MessageType::Authorize; // wrong type
    msg.message = call;

    EXPECT_THROW(tc->handle_message(msg), MessageTypeNotImplementedException);
}

TEST_F(TariffAndCostTest, HandleMessage_CostUpdated_CostDisabled_DispatchesResult) {
    // Cost is disabled: handler should dispatch a CostUpdatedResponse but not call the callback.
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _)).Times(0);
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).Times(1);

    CostUpdatedRequest req;
    req.totalCost = 5.0f;
    req.transactionId = TRANSACTION_ID;
    tc->handle_message(make_costupdated_message(req));
}

TEST_F(TariffAndCostTest, HandleMessage_CostUpdated_CostEnabled_CallsRunningCostCallback) {
    set_cost_enabled();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _)).Times(1);
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).Times(1);

    CostUpdatedRequest req;
    req.totalCost = 5.0f;
    req.transactionId = TRANSACTION_ID;
    tc->handle_message(make_costupdated_message(req));
}

TEST_F(TariffAndCostTest, HandleMessage_CostUpdated_NoRunningCostCallback_DispatchesResult) {
    set_cost_enabled();
    // No running cost callback set.
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).Times(1);

    CostUpdatedRequest req;
    req.totalCost = 5.0f;
    req.transactionId = TRANSACTION_ID;
    tc->handle_message(make_costupdated_message(req));
}

// ---------------------------------------------------------------------------
// send_total_cost_fallback_message
// ---------------------------------------------------------------------------

TEST_F(TariffAndCostTest, SendTotalCostFallbackMessage_CostDisabled_NoCallback) {
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).Times(0);
    tc->send_total_cost_fallback_message(TRANSACTION_ID);
}

TEST_F(TariffAndCostTest, SendTotalCostFallbackMessage_CostEnabled_NoCallback_DoesNotCrash) {
    set_cost_enabled();
    // tariff_message_callback_opt remains nullopt
    auto tc = make_tariff_and_cost();

    EXPECT_NO_FATAL_FAILURE(tc->send_total_cost_fallback_message(TRANSACTION_ID));
}

TEST_F(TariffAndCostTest, SendTotalCostFallbackMessage_CostEnabled_NoMessageConfigured_NoCallback) {
    set_cost_enabled();
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    // TotalCostFallbackMessage is empty by default.
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).Times(0);
    tc->send_total_cost_fallback_message(TRANSACTION_ID);
}

TEST_F(TariffAndCostTest, SendTotalCostFallbackMessage_CostEnabled_MessageConfigured_CallsCallback) {
    set_cost_enabled();
    set_total_cost_fallback_message(TOTAL_COST_FALLBACK_MESSAGE);
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).WillOnce(Invoke([](const TariffMessage& msg) {
        ASSERT_EQ(msg.message.size(), 1u);
        EXPECT_EQ(msg.message[0].message, TOTAL_COST_FALLBACK_MESSAGE);
    }));
    tc->send_total_cost_fallback_message(TRANSACTION_ID);
}

TEST_F(TariffAndCostTest, SendTotalCostFallbackMessage_SetsTransactionId) {
    set_cost_enabled();
    set_total_cost_fallback_message(TOTAL_COST_FALLBACK_MESSAGE);
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).WillOnce(Invoke([](const TariffMessage& msg) {
        EXPECT_EQ(msg.identifier_id, TRANSACTION_ID);
        EXPECT_EQ(msg.identifier_type, IdentifierType::TransactionId);
    }));
    tc->send_total_cost_fallback_message(TRANSACTION_ID);
}

// ---------------------------------------------------------------------------
// ensure_personal_message
// ---------------------------------------------------------------------------

TEST_F(TariffAndCostTest, EnsureTariffMessage_TariffDisabled_NoOp) {
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    EXPECT_FALSE(info.personalMessage.has_value());
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_PersonalMessageAlreadySet_NoOp) {
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    MessageContent existing;
    existing.content = "CSMS provided message";
    existing.format = MessageFormatEnum::UTF8;
    info.personalMessage = existing;

    tc->ensure_personal_message(info, false);

    EXPECT_EQ(info.personalMessage.value().content, "CSMS provided message");
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_NoFallbackConfigured_NoOp) {
    set_tariff_enabled();
    // TariffFallbackMessage is empty by default.
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    EXPECT_FALSE(info.personalMessage.has_value());
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_FallbackConfigured_SetsPersonalMessage) {
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    ASSERT_TRUE(info.personalMessage.has_value());
    EXPECT_EQ(std::string(info.personalMessage->content), TARIFF_FALLBACK_MESSAGE);
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_Offline_UsesFallback) {
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, true); // offline=true, no offline-specific message configured

    // Should fall back to TariffFallbackMessage
    ASSERT_TRUE(info.personalMessage.has_value());
    EXPECT_EQ(std::string(info.personalMessage->content), TARIFF_FALLBACK_MESSAGE);
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_Offline_OfflineMessageConfigured_UsesOfflineMessage) {
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    set_offline_tariff_fallback_message(OFFLINE_TARIFF_FALLBACK_MESSAGE);
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, true);

    ASSERT_TRUE(info.personalMessage.has_value());
    EXPECT_EQ(std::string(info.personalMessage->content), OFFLINE_TARIFF_FALLBACK_MESSAGE);
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_DefaultLanguageEntryBecomesPersonalMessage) {
    set_tariff_enabled();
    // Set the base (no-instance) fallback message.
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    // Also set the language-specific "en-US" instance that is present in the example config.
    Variable en_var;
    en_var.name = "TariffFallbackMessage";
    en_var.instance = "en-US";
    ASSERT_EQ(device_model->set_value(ControllerComponents::TariffCostCtrlr, en_var, AttributeEnum::Actual,
                                      "Tariff: 0.30 EUR/kWh (en-US)", "default", true),
              SetVariableStatusEnum::Accepted);
    // Set DisplayMessageCtrlr.Language to "en-US" so it matches the instance above.
    // Note: "en-US" must be in the current valuesList. The example config has "en_US,de,nl" (underscore);
    // since "en-US" ≠ "en_US" the language-specific entry won't be found via the valuesList scan.
    // This test therefore verifies the fallback: when no language match, index 0 (the base message)
    // is used for personalMessage with no extra entries.
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    ASSERT_TRUE(info.personalMessage.has_value());
    // Base message (index 0) is selected since no language match for the default "en_US" entries.
    EXPECT_EQ(std::string(info.personalMessage->content), TARIFF_FALLBACK_MESSAGE);
    // No extra entries since there are no matching per-language instances found via the valuesList.
    if (info.customData.has_value()) {
        const json& cd = info.customData.value();
        if (cd.contains("personalMessageExtra")) {
            EXPECT_TRUE(cd.at("personalMessageExtra").empty());
        }
    }
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_MultipleLanguages_PopulatesPersonalMessageExtra) {
    // With both a base message and a language-specific "de" instance, the base message should
    // become personalMessage (default_language is empty → index 0 is primary), and the "de"
    // message should go into customData.personalMessageExtra per spec 4.3.4.
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    // "de" is in DisplayMessageCtrlr.Language.valuesList ("en_US,de,nl") so it will be found.
    set_tariff_fallback_message_instance("de", "Tarif: 0,30 EUR/kWh");
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    // Base message is primary (index 0 — no default language configured).
    ASSERT_TRUE(info.personalMessage.has_value());
    EXPECT_EQ(std::string(info.personalMessage->content), TARIFF_FALLBACK_MESSAGE);

    // The "de" entry must appear in customData.personalMessageExtra.
    ASSERT_TRUE(info.customData.has_value());
    const json& cd = info.customData.value();
    EXPECT_EQ(cd.at("vendorId"), "org.openchargealliance.multilanguage");
    ASSERT_TRUE(cd.contains("personalMessageExtra"));
    ASSERT_EQ(cd.at("personalMessageExtra").size(), 1u);
    EXPECT_EQ(cd.at("personalMessageExtra").at(0).at("content"), "Tarif: 0,30 EUR/kWh");
    EXPECT_EQ(cd.at("personalMessageExtra").at(0).at("language"), "de");
}

TEST_F(TariffAndCostTest, EnsureTariffMessage_MaxFourExtraLanguages) {
    // personalMessageExtra is capped at 4 entries (spec 4.3.4).
    // We use the base message + "de" (only "de" and "nl" are in valuesList besides "en_US"),
    // so in practice the cap is not hit here — this test verifies no crash with max entries.
    set_tariff_enabled();
    set_tariff_fallback_message(TARIFF_FALLBACK_MESSAGE);
    set_tariff_fallback_message_instance("de", "Tarif: 0,30 EUR/kWh");
    set_tariff_fallback_message_instance("nl", "Tarief: 0,30 EUR/kWh");
    auto tc = make_tariff_and_cost();

    IdTokenInfo info;
    info.status = AuthorizationStatusEnum::Accepted;
    tc->ensure_personal_message(info, false);

    ASSERT_TRUE(info.personalMessage.has_value());
    EXPECT_EQ(std::string(info.personalMessage->content), TARIFF_FALLBACK_MESSAGE);

    ASSERT_TRUE(info.customData.has_value());
    const json& cd = info.customData.value();
    EXPECT_EQ(cd.at("vendorId"), "org.openchargealliance.multilanguage");
    // Two extra languages — both must be present.
    ASSERT_EQ(cd.at("personalMessageExtra").size(), 2u);
    // Extra entries must not exceed the max of 4.
    EXPECT_LE(cd.at("personalMessageExtra").size(), 4u);
}

// ---------------------------------------------------------------------------
// handle_cost_and_tariff
// ---------------------------------------------------------------------------

static TransactionEventRequest make_transaction_event_request(const std::string& transaction_id,
                                                              TransactionEventEnum event_type) {
    TransactionEventRequest req;
    req.eventType = event_type;
    req.timestamp = ocpp::DateTime();
    req.triggerReason = TriggerReasonEnum::Authorized;
    req.seqNo = 0;
    req.transactionInfo.transactionId = transaction_id;
    return req;
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_NeitherEnabled_NoCallbacks) {
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).Times(0);
    EXPECT_CALL(running_cost_mock, Call(_, _, _)).Times(0);

    TransactionEventResponse response;
    MessageContent personal_msg;
    personal_msg.content = "Some tariff info";
    personal_msg.format = MessageFormatEnum::UTF8;
    response.updatedPersonalMessage = personal_msg;
    response.totalCost = 5.0f;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Ended);
    tc->handle_cost_and_tariff(response, req, json{{"totalCost", 5.0}});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_TariffEnabled_WithUpdatedPersonalMessage_CallsTariffCallback) {
    set_tariff_enabled();
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).WillOnce(Invoke([](const TariffMessage& msg) {
        ASSERT_EQ(msg.message.size(), 1u);
        EXPECT_EQ(msg.message[0].message, "Tariff: 0.25 EUR/kWh");
        EXPECT_EQ(msg.identifier_type, IdentifierType::TransactionId);
    }));

    TransactionEventResponse response;
    MessageContent personal_msg;
    personal_msg.content = "Tariff: 0.25 EUR/kWh";
    personal_msg.format = MessageFormatEnum::UTF8;
    response.updatedPersonalMessage = personal_msg;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Updated);
    tc->handle_cost_and_tariff(response, req, json{});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_TariffEnabled_NoUpdatedPersonalMessage_NoTariffCallback) {
    set_tariff_enabled();
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).Times(0);

    TransactionEventResponse response;
    // No updatedPersonalMessage set.

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Updated);
    tc->handle_cost_and_tariff(response, req, json{});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_CostEnabled_WithTotalCost_CallsRunningCostCallback) {
    set_cost_enabled();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _))
        .WillOnce(Invoke([](const RunningCost& cost, std::uint32_t decimals, std::optional<std::string> currency) {
            EXPECT_DOUBLE_EQ(cost.cost, 5.0);
            EXPECT_EQ(cost.state, RunningCostState::Finished);
        }));

    TransactionEventResponse response;
    response.totalCost = 5.0f;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Ended);
    tc->handle_cost_and_tariff(response, req, json{{"totalCost", 5.0}});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_CostEnabled_NoTotalCost_NoRunningCostCallback) {
    set_cost_enabled();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _)).Times(0);

    TransactionEventResponse response;
    // No totalCost.

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Ended);
    tc->handle_cost_and_tariff(response, req, json{});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_BothEnabled_CallsBothCallbacks) {
    set_tariff_enabled();
    set_cost_enabled();
    tariff_message_callback_opt = tariff_message_mock.AsStdFunction();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(tariff_message_mock, Call(_)).Times(1);
    EXPECT_CALL(running_cost_mock, Call(_, _, _)).Times(1);

    TransactionEventResponse response;
    MessageContent personal_msg;
    personal_msg.content = "Tariff info";
    personal_msg.format = MessageFormatEnum::UTF8;
    response.updatedPersonalMessage = personal_msg;
    response.totalCost = 3.5f;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Ended);
    tc->handle_cost_and_tariff(response, req, json{{"totalCost", 3.5}});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_CostEnabled_RunningCostIncludesTariffMessages) {
    set_tariff_enabled();
    set_cost_enabled();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _))
        .WillOnce(Invoke([](const RunningCost& cost, std::uint32_t, std::optional<std::string>) {
            ASSERT_TRUE(cost.cost_messages.has_value());
            ASSERT_EQ(cost.cost_messages->size(), 1u);
            EXPECT_EQ(cost.cost_messages->at(0).message, "Tariff info");
        }));

    TransactionEventResponse response;
    MessageContent personal_msg;
    personal_msg.content = "Tariff info";
    personal_msg.format = MessageFormatEnum::UTF8;
    response.updatedPersonalMessage = personal_msg;
    response.totalCost = 3.5f;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Ended);
    tc->handle_cost_and_tariff(response, req, json{{"totalCost", 3.5}});
}

TEST_F(TariffAndCostTest, HandleCostAndTariff_ChargingState_RunningCostIsCharging) {
    set_cost_enabled();
    set_running_cost_callback_opt = running_cost_mock.AsStdFunction();
    auto tc = make_tariff_and_cost();

    EXPECT_CALL(running_cost_mock, Call(_, _, _))
        .WillOnce(Invoke([](const RunningCost& cost, std::uint32_t, std::optional<std::string>) {
            EXPECT_EQ(cost.state, RunningCostState::Charging);
        }));

    TransactionEventResponse response;
    response.totalCost = 1.0f;

    const auto req = make_transaction_event_request(TRANSACTION_ID, TransactionEventEnum::Updated);
    tc->handle_cost_and_tariff(response, req, json{{"totalCost", 1.0}});
}
