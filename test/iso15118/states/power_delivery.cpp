// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/power_delivery.hpp>

using namespace iso15118;

SCENARIO("Power delivery state handling") {
    GIVEN("Bad case - Unknown session") {
        d20::Session session = d20::Session();

        message_20::PowerDeliveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.processing = message_20::Processing::Ongoing;
        req.charge_progress = message_20::PowerDeliveryRequest::Progress::Start;

        const auto res = d20::state::handle_request(req, d20::Session());

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.status.has_value() == false);
        }
    }
    GIVEN("Not so bad case - WARNING_StandbyNotAllowed") {
        d20::Session session = d20::Session();

        message_20::PowerDeliveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.processing = message_20::Processing::Ongoing;
        req.charge_progress = message_20::PowerDeliveryRequest::Progress::Standby;

        const auto res = d20::state::handle_request(req, session);

        // Right now standby ist not supported

        THEN("ResponseCode: WARNING_StandbyNotAllowed, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::WARNING_StandbyNotAllowed);
            REQUIRE(res.status.has_value() == false);
        }
    }
    GIVEN("Good case") {
    }
    GIVEN("Bad case - EVPowerProfileInvalid") {
    }
    GIVEN("Bad case - ScheduleSelectionInvalid") {
    }
    GIVEN("Bad case - PowerDeliveryNotApplied") {
    } // todo(sl): evse is not able to deliver energy

    GIVEN("Bad case - PowerToleranceNotConfirmed") {
    } // todo(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Not so bad case - WARNING_PowerToleranceNotConfirmed") {
    } // todo(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Good case - OK_PowerToleranceConfirmed") {
    } // todo(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Bad case - AC ContactorError") {
    } // todo(sl): AC stuff

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // todo(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // todo(sl): not here
}