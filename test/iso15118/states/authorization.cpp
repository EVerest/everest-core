// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/authorization.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

using AuthStatus = message_20::datatypes::AuthStatus;

SCENARIO("Authorization state handling") {

    GIVEN("Bad Case - Unknown session") {
        d20::Session session = d20::Session();

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = dt::Authorization::EIM;
        req.authorization_mode.emplace<dt::EIM_ASReqAuthorizationMode>();

        const auto res = d20::state::handle_request(req, d20::Session(), AuthStatus::Pending, false);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.evse_processing == dt::Processing::Finished);
        }
    }

    GIVEN("Warning - Authorization selection is invalid") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {dt::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = dt::Authorization::EIM;
        req.authorization_mode.emplace<dt::EIM_ASReqAuthorizationMode>();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Pending, false);

        THEN("ResponseCode: FAILED_UnknownSession, EvseProcessing: Finished") {
            REQUIRE(res.response_code == dt::ResponseCode::WARNING_AuthorizationSelectionInvalid);
            REQUIRE(res.evse_processing == dt::Processing::Finished);
        }
    }

    // EIM test cases

    GIVEN("Warning - EIM Authorization Failure") { // [V2G20-2219]

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {dt::Authorization::EIM, dt::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = dt::Authorization::EIM;
        req.authorization_mode.emplace<dt::EIM_ASReqAuthorizationMode>();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Rejected, false);

        THEN("ResponseCode: WARNING_EIMAuthorizationFailure, EvseProcessing: Finished") {
            REQUIRE(res.response_code == dt::ResponseCode::WARNING_EIMAuthorizationFailure);
            REQUIRE(res.evse_processing == dt::Processing::Finished);
        }
    }

    GIVEN("Good case - EIM waiting for authorization") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {dt::Authorization::EIM, dt::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = dt::Authorization::EIM;
        req.authorization_mode.emplace<dt::EIM_ASReqAuthorizationMode>();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Pending, false);

        THEN("ResponseCode: Ok, EvseProcessing: Ongoing") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.evse_processing == dt::Processing::Ongoing);
        }
    }

    GIVEN("Good case - EIM authorized") {

        d20::Session session = d20::Session();
        session.offered_services.auth_services = {dt::Authorization::EIM, dt::Authorization::PnC};

        message_20::AuthorizationRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.selected_authorization_service = dt::Authorization::EIM;
        req.authorization_mode.emplace<dt::EIM_ASReqAuthorizationMode>();

        const auto res = d20::state::handle_request(req, session, AuthStatus::Accepted, false);

        THEN("ResponseCode: Ok, EvseProcessing: Finished") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.evse_processing == dt::Processing::Finished);
        }
    }

    // GIVEN("Bad Case - Ongoing timeout reached") {}

    // PnC test cases

    // GIVEN("Bad Case - sequence error") {} // TODO(sl): not here

    // GIVEN("Performance Timeout") {} // TODO(sl): not here

    // GIVEN("Sequence Timeout") {} // TODO(sl): not here
}
