// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/authorization.hpp>

using namespace iso15118;
using AuthStatus = message_20::AuthStatus;

SCENARIO("Authorization state handling") {

    GIVEN("Bad Case - Unknown session") {
        d20::Session session = d20::Session();

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = message_20::Authorization::EIM;
        req.eim_as_req_authorization_mode.emplace();

        const auto res = d20::state::handle_request(req, d20::Session(), AuthStatus::Pending);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.evse_processing == message_20::Processing::Finished);
        }
    }

    GIVEN("Warning - Authorization selection is invalid") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {message_20::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = message_20::Authorization::EIM;
        req.eim_as_req_authorization_mode.emplace();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Pending);

        THEN("ResponseCode: FAILED_UnknownSession, EvseProcessing: Finished") {
            REQUIRE(res.response_code == message_20::ResponseCode::WARNING_AuthorizationSelectionInvalid);
            REQUIRE(res.evse_processing == message_20::Processing::Finished);
        }
    }

    // EIM test cases

    GIVEN("Warning - EIM Authorization Failure") { // [V2G20-2219]

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {message_20::Authorization::EIM, message_20::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = message_20::Authorization::EIM;
        req.eim_as_req_authorization_mode.emplace();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Rejected);

        THEN("ResponseCode: WARNING_EIMAuthorizationFailure, EvseProcessing: Finished") {
            REQUIRE(res.response_code == message_20::ResponseCode::WARNING_EIMAuthorizationFailure);
            REQUIRE(res.evse_processing == message_20::Processing::Finished);
        }
    }

    GIVEN("Good case - EIM waiting for authorization") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {message_20::Authorization::EIM, message_20::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = message_20::Authorization::EIM;
        req.eim_as_req_authorization_mode.emplace();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Pending);

        THEN("ResponseCode: Ok, EvseProcessing: Ongoing") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);
            REQUIRE(res.evse_processing == message_20::Processing::Ongoing);
        }
    }

    GIVEN("Good case - EIM authorized") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {message_20::Authorization::EIM, message_20::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = message_20::Authorization::EIM;
        req.eim_as_req_authorization_mode.emplace();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Accepted);

        THEN("ResponseCode: Ok, EvseProcessing: Finished") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);
            REQUIRE(res.evse_processing == message_20::Processing::Finished);
        }
    }

    // PnC test cases

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Performance Timeout") {} // todo(sl): not here

    // GIVEN("Sequence Timeout") {} // todo(sl): not here
}
