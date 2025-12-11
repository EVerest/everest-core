// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/authorization_setup.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

SCENARIO("Authorization setup state handling") {

    GIVEN("Bad Case - Unknown session") {
        auto session = d20::Session();

        message_20::AuthorizationSetupRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        session = d20::Session();

        const auto cert_install_service = false;
        const std::vector<dt::Authorization> authorization_services = {dt::Authorization::EIM};

        const auto res = d20::state::handle_request(req, session, cert_install_service, authorization_services);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.certificate_installation_service == false);
            REQUIRE(res.authorization_services.size() == 1);
            REQUIRE(res.authorization_services[0] == dt::Authorization::EIM);
            REQUIRE(std::holds_alternative<dt::EIM_ASResAuthorizationMode>(res.authorization_mode));
        }
    }

    GIVEN("Good Case - EIM only , cert_install_service not provided") {

        auto session = d20::Session();

        message_20::AuthorizationSetupRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        const auto cert_install_service = false;
        const std::vector<dt::Authorization> authorization_services = {dt::Authorization::EIM};

        const auto res = d20::state::handle_request(req, session, cert_install_service, authorization_services);

        THEN("ResponseCode: Ok, cert_install = false, authorization_servie = EIM") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.certificate_installation_service == false);
            REQUIRE(res.authorization_services.size() == 1);
            REQUIRE(res.authorization_services[0] == dt::Authorization::EIM);
            REQUIRE(std::holds_alternative<dt::EIM_ASResAuthorizationMode>(res.authorization_mode));
            REQUIRE(session.offered_services.auth_services[0] == dt::Authorization::EIM);
        }
    }

    GIVEN("Good Case - PnC only, cert_install_service not provided") {

        auto session = d20::Session();

        message_20::AuthorizationSetupRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        const auto cert_install_service = false;
        const std::vector<dt::Authorization> authorization_services = {dt::Authorization::PnC};

        const auto res = d20::state::handle_request(req, session, cert_install_service, authorization_services);

        THEN("ResponseCode: Ok, cert_install = false, authorization_servie = PnC, authorization_mode = PnC_Mode") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.certificate_installation_service == false);
            REQUIRE(res.authorization_services.size() == 1);
            REQUIRE(res.authorization_services[0] == dt::Authorization::PnC);
            REQUIRE(std::holds_alternative<dt::PnC_ASResAuthorizationMode>(res.authorization_mode));
            const auto& auth_mode = std::get<dt::PnC_ASResAuthorizationMode>(res.authorization_mode);
            REQUIRE(auth_mode.gen_challenge.empty() == false);
            REQUIRE(session.offered_services.auth_services[0] == dt::Authorization::PnC);
        }
    }

    GIVEN("Good Case - EIM + PnC, cert_install_service provided") {

        auto session = d20::Session();

        message_20::AuthorizationSetupRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        const auto cert_install_service = true;
        const std::vector<dt::Authorization> authorization_services = {dt::Authorization::PnC, dt::Authorization::EIM};

        const auto res = d20::state::handle_request(req, session, cert_install_service, authorization_services);

        THEN("ResponseCode: Ok, cert_install = true, authorization_servie = EIM & PnC, authorization_mode = PnC_Mode") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.certificate_installation_service == true);
            REQUIRE(res.authorization_services.size() == 2);
            REQUIRE((res.authorization_services[0] == dt::Authorization::EIM ||
                     res.authorization_services[0] == dt::Authorization::PnC));
            REQUIRE((res.authorization_services[1] == dt::Authorization::EIM ||
                     res.authorization_services[1] == dt::Authorization::PnC));
            REQUIRE(std::holds_alternative<dt::PnC_ASResAuthorizationMode>(res.authorization_mode));
            const auto& auth_mode = std::get<dt::PnC_ASResAuthorizationMode>(res.authorization_mode);
            REQUIRE(auth_mode.gen_challenge.empty() == false);

            REQUIRE((session.offered_services.auth_services[0] == dt::Authorization::EIM ||
                     session.offered_services.auth_services[0] == dt::Authorization::PnC));
            REQUIRE((session.offered_services.auth_services[1] == dt::Authorization::EIM ||
                     session.offered_services.auth_services[1] == dt::Authorization::PnC));
        }
    }

    // GIVEN("Bad Case - sequence error") {} // TODO(sl): not here

    // GIVEN("Performance Timeout") {} // TODO(sl): not here

    // GIVEN("Sequence Timeout") {} // TODO(sl): not here
}
