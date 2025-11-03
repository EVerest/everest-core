// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <din_server.hpp>

#include <cstring>
#include <gtest/gtest.h>

#include "ISO15118_chargerImplStub.hpp"
#include "cbv2g/din/din_msgDefDatatypes.h"
#include "utest_log.hpp"
#include "v2g.hpp"

#include <memory>

void publish_dc_ev_maximum_limits(struct v2g_context* ctx, const float& v2g_dc_ev_max_current_limit,
                                  const unsigned int& v2g_dc_ev_max_current_limit_is_used,
                                  const float& v2g_dc_ev_max_power_limit,
                                  const unsigned int& v2g_dc_ev_max_power_limit_is_used,
                                  const float& v2g_dc_ev_max_voltage_limit,
                                  const unsigned int& v2g_dc_ev_max_voltage_limit_is_used) {
}

void stop_timer(struct event** event_timer, char const* const timer_name, struct v2g_context* ctx) {
}

void log_selected_energy_transfer_type(int selected_energy_transfer_mode) {
}

uint64_t v2g_session_id_from_exi(bool is_iso, void* exi_in) {
    return 0;
}

void publish_dc_ev_target_voltage_current(struct v2g_context* ctx, const float& v2g_dc_ev_target_voltage,
                                          const float& v2g_dc_ev_target_current) {
}

void publish_dc_ev_remaining_time(struct v2g_context* ctx, const float& v2g_dc_ev_remaining_time_to_full_soc,
                                  const unsigned int& v2g_dc_ev_remaining_time_to_full_soc_is_used,
                                  const float& v2g_dc_ev_remaining_time_to_bulk_soc,
                                  const unsigned int& v2g_dc_ev_remaining_time_to_bulk_soc_is_used) {
}

namespace {
class DinServerTest : public testing::Test {
protected:
    std::unique_ptr<v2g_connection> conn;
    std::unique_ptr<v2g_context> ctx;
    std::unique_ptr<din_exiDocument> exi_in;
    std::unique_ptr<din_exiDocument> exi_out;

    module::stub::ModuleAdapterStub adapter;
    module::stub::ISO15118_chargerImplStub charger;

    DinServerTest() : charger(adapter) {
    }

    void SetUp() override {
        conn = std::make_unique<v2g_connection>();
        ctx = std::make_unique<v2g_context>();
        exi_in = std::make_unique<din_exiDocument>();
        exi_out = std::make_unique<din_exiDocument>();

        module::stub::clear_logs();
        conn->ctx = ctx.get();
        conn->ctx->p_charger = &charger;

        conn->exi_in.dinEXIDocument = exi_in.get();
        conn->exi_out.dinEXIDocument = exi_out.get();
    }

    void TearDown() override {
    }
};

class DinServerTestValidateResponseCode
    : public DinServerTest,
      public testing::WithParamInterface<
          std::tuple<int /*din_responseCodeType*/, bool, bool, bool, int /*V2gMsgTypeId*/, uint64_t, uint64_t, bool>> {
};

// For all test cases:
// TODO: Define helper functions to set the conn and ctx variables

// ----------------------------------------------------------------

// Potential test for SessionSetup:
// Bad Case:
// Setting no EvseID -> A check should be added -> But a default value is in ctx provided.
TEST_F(DinServerTest, session_setup_generating_new_session_id) {
    // Setting up session_setup_req
    auto& session_setup_req = exi_in->V2G_Message.Body.SessionSetupReq;
    exi_in->V2G_Message.Body.SessionSetupReq_isUsed = true;
    init_din_SessionSetupReqType(&session_setup_req);

    const uint8_t evcc_id[8] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    memcpy(session_setup_req.EVCCID.bytes, evcc_id, sizeof(evcc_id));
    session_setup_req.EVCCID.bytesLen = sizeof(evcc_id);

    // Setting up conn
    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG;

    ctx->evse_v2g_data.session_id = 0;
    ctx->evse_v2g_data.date_time_now_is_used = 0;

    ctx->ev_v2g_data.received_session_id = 0;

    std::string evse_id = std::string("DE*PNX*TET1*234");
    strcpy(reinterpret_cast<char*>(ctx->evse_v2g_data.evse_id.bytes), evse_id.data());
    ctx->evse_v2g_data.evse_id.bytesLen = evse_id.size();

    // Setting up session_setup_res
    auto& session_setup_res = exi_out->V2G_Message.Body.SessionSetupRes;
    exi_out->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
    init_din_SessionSetupResType(&session_setup_res);

    EXPECT_EQ(states::handle_din_session_setup(conn.get()), V2G_EVENT_NO_EVENT);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 0);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 3);

    EXPECT_EQ(session_setup_res.DateTimeNow_isUsed, false);
    // Checking if session id is generated
    EXPECT_GT(ctx->evse_v2g_data.session_id, 0);
    // Checking if evse id was set correctly
    EXPECT_EQ(evse_id, std::string(reinterpret_cast<char*>(session_setup_res.EVSEID.bytes)));
}

TEST_F(DinServerTest, session_setup_old_session_id) {
    // Setting up session_setup_req
    auto& session_setup_req = exi_in->V2G_Message.Body.SessionSetupReq;
    exi_in->V2G_Message.Body.SessionSetupReq_isUsed = true;
    init_din_SessionSetupReqType(&session_setup_req);

    const uint8_t evcc_id[8] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    memcpy(session_setup_req.EVCCID.bytes, evcc_id, sizeof(evcc_id));
    session_setup_req.EVCCID.bytesLen = sizeof(evcc_id);

    // Setting up conn
    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG;

    ctx->evse_v2g_data.session_id = 4158610156;
    ctx->evse_v2g_data.date_time_now_is_used = 0;

    ctx->ev_v2g_data.received_session_id = 0;

    std::string evse_id = std::string("DE*PNX*TET1*234");
    strcpy(reinterpret_cast<char*>(ctx->evse_v2g_data.evse_id.bytes), evse_id.data());
    ctx->evse_v2g_data.evse_id.bytesLen = evse_id.size();

    // Setting up session_setup_res
    auto& session_setup_res = exi_out->V2G_Message.Body.SessionSetupRes;
    exi_out->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
    init_din_SessionSetupResType(&session_setup_res);

    EXPECT_EQ(states::handle_din_session_setup(conn.get()), V2G_EVENT_NO_EVENT);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 0);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 2);

    EXPECT_EQ(session_setup_res.DateTimeNow_isUsed, false);
    // Checking if session id is generated
    EXPECT_EQ(ctx->evse_v2g_data.session_id, 4158610156);
    // Checking if evse id was set correctly
    EXPECT_EQ(evse_id, std::string(reinterpret_cast<char*>(session_setup_res.EVSEID.bytes)));
}

TEST_F(DinServerTest, session_setup_datetime_is_used) {
    // Setting up session_setup_req
    auto& session_setup_req = exi_in->V2G_Message.Body.SessionSetupReq;
    exi_in->V2G_Message.Body.SessionSetupReq_isUsed = true;
    init_din_SessionSetupReqType(&session_setup_req);

    const uint8_t evcc_id[8] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    memcpy(session_setup_req.EVCCID.bytes, evcc_id, sizeof(evcc_id));
    session_setup_req.EVCCID.bytesLen = sizeof(evcc_id);

    // Setting up conn
    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG;

    ctx->evse_v2g_data.session_id = 0;
    ctx->evse_v2g_data.date_time_now_is_used = true;

    ctx->ev_v2g_data.received_session_id = 0;

    std::string evse_id = std::string("DE*PNX*TET1*234");
    strcpy(reinterpret_cast<char*>(ctx->evse_v2g_data.evse_id.bytes), evse_id.data());
    ctx->evse_v2g_data.evse_id.bytesLen = evse_id.size();

    // Setting up session_setup_res
    auto& session_setup_res = exi_out->V2G_Message.Body.SessionSetupRes;
    exi_out->V2G_Message.Body.SessionSetupRes_isUsed = 1u;
    init_din_SessionSetupResType(&session_setup_res);

    EXPECT_EQ(states::handle_din_session_setup(conn.get()), V2G_EVENT_NO_EVENT);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 0);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 3);

    EXPECT_EQ(session_setup_res.DateTimeNow_isUsed, true);
    EXPECT_GT(session_setup_res.DateTimeNow, 0);
    // Checking if session id is generated
    EXPECT_GT(ctx->evse_v2g_data.session_id, 0);
    // Checking if evse id was set correctly
    EXPECT_EQ(evse_id, std::string(reinterpret_cast<char*>(session_setup_res.EVSEID.bytes)));
}

TEST_F(DinServerTest, din_service_discovery_good_case) {

    // TODO(sl): Maybe add this to check exi_out proberly
    exi_out->V2G_Message.Body.ServiceDiscoveryRes_isUsed = true;
    init_din_ServiceDiscoveryResType(&exi_out->V2G_Message.Body.ServiceDiscoveryRes);

    // TODO: Setting the correct session_id + received_session_id via functions

    EXPECT_EQ(states::handle_din_service_discovery(conn.get()), V2G_EVENT_NO_EVENT);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 1);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 0);
}

TEST_F(DinServerTest, handle_din_contract_authentication_check_evse_processing_finished) {

    // TODO: set a prober session id
    ctx->evse_v2g_data.session_id = 0;
    ctx->evse_v2g_data.date_time_now_is_used = 0;

    ctx->current_v2g_msg = V2G_AUTHORIZATION_MSG;
    ctx->ev_v2g_data.received_session_id = 0;

    ctx->evse_v2g_data.evse_processing[PHASE_AUTH] = 0;
    EXPECT_EQ(states::handle_din_contract_authentication(conn.get()), V2G_EVENT_NO_EVENT); // TODO

    auto& res = exi_out->V2G_Message.Body.ContractAuthenticationRes;

    // EXPECT_EQ(res.ResponseCode, din_responseCodeType_OK);
    EXPECT_EQ(res.EVSEProcessing, din_EVSEProcessingType_Finished);
    EXPECT_EQ(ctx->state, WAIT_FOR_CHARGEPARAMETERDISCOVERY);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 1);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 0);
}

TEST_F(DinServerTest, handle_din_contract_authentication_check_evse_processing_ongoing) {

    // TODO: set a prober session id
    ctx->evse_v2g_data.session_id = 0;
    ctx->evse_v2g_data.date_time_now_is_used = 0;

    ctx->current_v2g_msg = V2G_AUTHORIZATION_MSG;
    ctx->ev_v2g_data.received_session_id = 0;

    ctx->evse_v2g_data.evse_processing[PHASE_AUTH] = 1;
    EXPECT_EQ(states::handle_din_contract_authentication(conn.get()), V2G_EVENT_NO_EVENT); // TODO

    auto& res = exi_out->V2G_Message.Body.ContractAuthenticationRes;

    // EXPECT_EQ(res.ResponseCode, din_responseCodeType_OK);
    EXPECT_EQ(res.EVSEProcessing, din_EVSEProcessingType_Ongoing);
    EXPECT_EQ(ctx->state, WAIT_FOR_AUTHORIZATION);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_ERROR).size(), 1);
    EXPECT_EQ(module::stub::get_logs(dloglevel_t::DLOG_LEVEL_INFO).size(), 0);
}

// if not otherwise specified, the following testcases are happy paths

TEST_F(DinServerTest, din_validate_response_code_TERMINATE_CONNECTION) {

    // which response code is actually irrelevant here and was picked at random
    auto tmp = din_responseCodeType_FAILED_TariffSelectionInvalid;

    // only this bool determines the outcome
    ctx->is_connection_terminated = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_TERMINATE_CONNECTION);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_failed_response_FAILED) {

    // which response code is actually irrelevant here and was picked at random
    // FAILED code
    auto tmp = din_responseCodeType_FAILED_ChallengeInvalid;

    ctx->is_connection_terminated = false;

    ctx->terminate_connection_on_failed_response = false;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_failed_response_OK) {

    // which response code is actually irrelevant here and was picked at random
    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->terminate_connection_on_failed_response = false;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_OK) {

    // which response code is actually irrelevant here and was picked at random
    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_OK_bad_path_1) {

    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = true; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_NE(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_OK_bad_path_2) {

    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = true;

    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_NE(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_OK_bad_path_3) {

    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_CERTIFICATE_INSTALLATION_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_NE(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_NO_EVENT_OK_bad_path_4) {

    // OK code
    auto tmp = din_responseCodeType_OK;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = true;

    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG; // &&
    ctx->evse_v2g_data.session_id = 6;
    ctx->ev_v2g_data.received_session_id = 6;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_NE(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_NO_EVENT);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_1) {

    auto tmp = din_responseCodeType_FAILED_WrongEnergyTransferType;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = true; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_METERING_RECEIPT_MSG; // &&
    ctx->evse_v2g_data.session_id = 6;
    ctx->ev_v2g_data.received_session_id = 6;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_2) {

    auto tmp = din_responseCodeType_FAILED_MeteringSignatureNotValid;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = true;

    ctx->current_v2g_msg = V2G_CHARGING_STATUS_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 6;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_3) {

    auto tmp = din_responseCodeType_OK_CertificateExpiresSoon;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_UNKNOWN_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 1;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_4) {

    auto tmp = din_responseCodeType_OK_CertificateExpiresSoon;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_UNKNOWN_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 1;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_5) {

    auto tmp = din_responseCodeType_OK_CertificateExpiresSoon;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_CABLE_CHECK_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}

TEST_F(DinServerTest, din_validate_response_code_EVENT_SEND_AND_TERMINATE_6) {

    auto tmp = din_responseCodeType_FAILED_SequenceError;

    ctx->is_connection_terminated = false;

    ctx->stop_hlc = false; //||
    ctx->intl_emergency_shutdown = false;

    ctx->current_v2g_msg = V2G_SESSION_SETUP_MSG; // &&
    ctx->evse_v2g_data.session_id = 1;
    ctx->ev_v2g_data.received_session_id = 2;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&tmp, conn.get()), V2G_EVENT_SEND_AND_TERMINATE);
}
TEST_F(DinServerTest, din_validate_response_code_V2G_DC_390) {
    // The response message shall contain the ResponseCode “FAILED_SequenceError” if the
    // SECC has received an unexpected request message.

    auto given_response_code = din_responseCodeType_OK;
    constexpr auto expected_response_code = din_responseCodeType_FAILED_SequenceError;
    constexpr auto expected_response = V2G_EVENT_NO_EVENT;

    ctx->is_connection_terminated = false;

    ctx->terminate_connection_on_failed_response = false;

    ctx->current_v2g_msg = V2G_UNKNOWN_MSG;

    EXPECT_EQ(utils::din_validate_response_code(&given_response_code, conn.get()), expected_response);
    // given response code should change in the function call
    EXPECT_EQ(given_response_code, expected_response_code);
}

TEST_F(DinServerTest, din_validate_response_code_V2G_DC_391) {
    // The response message shall contain the ResponseCode “FAILED_UnknownSession” if the
    // SessionID in a request message does not match the SessionID provided by the SECC in the SessionSetupRes
    // message.

    auto given_response_code = din_responseCodeType_OK;
    constexpr auto expected_response_code = din_responseCodeType_FAILED_UnknownSession;
    constexpr auto expected_response = V2G_EVENT_NO_EVENT;

    ctx->is_connection_terminated = false;

    ctx->terminate_connection_on_failed_response = false;
    ctx->evse_v2g_data.session_id = 1234;
    ctx->ev_v2g_data.received_session_id = 5678;

    EXPECT_EQ(utils::din_validate_response_code(&given_response_code, conn.get()), expected_response);
    // given response code should change in the function call
    EXPECT_EQ(given_response_code, expected_response_code);
}

TEST_F(DinServerTest, din_validate_response_code_V2G_DC_665) {
    // If the SECC receives a request message that it expects according to the message sequence
    // specified in this chapter, and if the SECC cannot process this request message, e. g. due to
    // errors in the message parameters or due to impeding conditions in the EVSE, the SECC
    // shall:
    // [1.] without any delay, carry out an “EVSE-initiated emergency shutdown” as specified in
    // IEC 61851-23, which includes turning off the CP oscillator, if it is turned on,
    // [2.] respond with the corresponding response message with parameter ResponseCode
    // equal to “FAILED”, if possible, and
    // [3.] close the TCP connection according to [V2G-DC-116].

    auto given_response_code = din_responseCodeType_FAILED;
    constexpr auto min_expected_response_code = din_responseCodeType_FAILED;
    constexpr auto expected_response = V2G_EVENT_SEND_AND_TERMINATE;

    ctx->is_connection_terminated = false;

    ctx->terminate_connection_on_failed_response = true;

    EXPECT_EQ(utils::din_validate_response_code(&given_response_code, conn.get()), expected_response);
    EXPECT_GE(given_response_code, min_expected_response_code);
}

} // namespace
