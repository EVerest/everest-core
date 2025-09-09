// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/power_delivery.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

SCENARIO("Power delivery state handling") {
    GIVEN("Bad case - Unknown session") {
        d20::Session session = d20::Session();

        message_20::PowerDeliveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.processing = dt::Processing::Ongoing;
        req.charge_progress = dt::Progress::Start;

        const auto res = d20::state::handle_request(req, d20::Session(), false);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.status.has_value() == false);
        }
    }
    GIVEN("Not so bad case - WARNING_StandbyNotAllowed") {
        d20::Session session = d20::Session();

        message_20::PowerDeliveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.processing = dt::Processing::Ongoing;
        req.charge_progress = dt::Progress::Standby;

        const auto res = d20::state::handle_request(req, session, false);

        // Right now standby ist not supported

        THEN("ResponseCode: WARNING_StandbyNotAllowed, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::WARNING_StandbyNotAllowed);
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
    } // TODO(sl): evse is not able to deliver energy

    GIVEN("Bad case - PowerToleranceNotConfirmed") {
    } // TODO(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Not so bad case - WARNING_PowerToleranceNotConfirmed") {
    } // TODO(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Good case - OK_PowerToleranceConfirmed") {
    } // TODO(sl): Scheduled Mode + Provided PowerTolerance in ScheduleExchangeRes
    GIVEN("Bad case - AC ContactorError") {
        d20::Session session = d20::Session();

        message_20::PowerDeliveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        req.processing = dt::Processing::Ongoing;
        req.charge_progress = dt::Progress::Start;

        const auto res = d20::state::handle_request(req, session, true);

        THEN("ResponseCode: FAILED_ContactorError, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_ContactorError);
            REQUIRE(res.status.has_value() == false);
        }
    } // TODO(sl): AC stuff

    // GIVEN("Bad Case - sequence error") {} // TODO(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // TODO(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // TODO(sl): not here
}
