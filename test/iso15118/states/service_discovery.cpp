// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/service_discovery.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

SCENARIO("Service discovery state handling") {

    GIVEN("Bad Case - Unknown session") {
        auto session = d20::Session();

        message_20::ServiceDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        session = d20::Session();

        std::vector<dt::ServiceCategory> energy_services = {dt::ServiceCategory::DC};

        const auto res = d20::state::handle_request(req, session, energy_services, {});

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
            REQUIRE(res.service_renegotiation_supported == false);
            REQUIRE(res.energy_transfer_service_list.size() == 1);
            REQUIRE(res.energy_transfer_service_list[0].free_service == false);
            REQUIRE(res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::AC);
            REQUIRE(res.vas_list.has_value() == false);
        }
    }

    GIVEN("Good Case - Setting services") {

        d20::Session session = d20::Session();

        message_20::ServiceDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        std::vector<dt::ServiceCategory> supported_energy_transfer_services = {dt::ServiceCategory::DC,
                                                                               dt::ServiceCategory::DC_BPT};

        const auto res = d20::state::handle_request(req, session, supported_energy_transfer_services, {});

        THEN("ResponseCode: OK, energy_transfer_service_list: DC & DC_WPT, vaslist: empty") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.service_renegotiation_supported == false);
            REQUIRE(res.energy_transfer_service_list.size() == 2);
            REQUIRE(res.energy_transfer_service_list[0].free_service == false);
            REQUIRE((res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::DC ||
                     res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::DC_BPT));
            REQUIRE(res.energy_transfer_service_list[1].free_service == false);
            REQUIRE((res.energy_transfer_service_list[1].service_id == dt::ServiceCategory::DC ||
                     res.energy_transfer_service_list[1].service_id == dt::ServiceCategory::DC_BPT));
            REQUIRE(res.vas_list.has_value() == false);
        }
    }

    GIVEN("Good Case - Setting services + vas list") {

        d20::Session session = d20::Session();

        message_20::ServiceDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        std::vector<dt::ServiceCategory> supported_energy_transfer_services = {dt::ServiceCategory::DC,
                                                                               dt::ServiceCategory::DC_BPT};
        std::vector<dt::ServiceCategory> supported_vas_services = {dt::ServiceCategory::ParkingStatus};

        const auto res =
            d20::state::handle_request(req, session, supported_energy_transfer_services, supported_vas_services);

        THEN("ResponseCode: OK, energy_transfer_service_list: DC & DC_BPT, vaslist: ParkingStatus") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.service_renegotiation_supported == false);
            REQUIRE(res.energy_transfer_service_list.size() == 2);
            REQUIRE(res.energy_transfer_service_list[0].free_service == false);
            REQUIRE((res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::DC ||
                     res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::DC_BPT));
            REQUIRE(res.energy_transfer_service_list[1].free_service == false);
            REQUIRE((res.energy_transfer_service_list[1].service_id == dt::ServiceCategory::DC ||
                     res.energy_transfer_service_list[1].service_id == dt::ServiceCategory::DC_BPT));
            REQUIRE(res.vas_list.has_value() == true);
            REQUIRE(res.vas_list.value()[0].free_service == false);
            REQUIRE(res.vas_list.value()[0].service_id == dt::ServiceCategory::ParkingStatus);
        }
    }

    GIVEN("Good Case - Filter supported_service_providers") {

        d20::Session session = d20::Session();

        message_20::ServiceDiscoveryRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;

        auto& supported_service_ids = req.supported_service_ids.emplace();
        supported_service_ids.push_back(2);
        supported_service_ids.push_back(65);

        std::vector<dt::ServiceCategory> supported_energy_transfer_services = {dt::ServiceCategory::DC,
                                                                               dt::ServiceCategory::DC_BPT};
        std::vector<dt::ServiceCategory> supported_vas_services = {dt::ServiceCategory::ParkingStatus};

        const auto res =
            d20::state::handle_request(req, session, supported_energy_transfer_services, supported_vas_services);

        THEN("ResponseCode: OK, energy_transfer_service_list: DC, vaslist: empty") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
            REQUIRE(res.service_renegotiation_supported == false);
            REQUIRE(res.energy_transfer_service_list.size() == 1);
            REQUIRE(res.energy_transfer_service_list[0].free_service == false);
            REQUIRE(res.energy_transfer_service_list[0].service_id == dt::ServiceCategory::DC);
            REQUIRE(res.vas_list.has_value() == false);
        }
    }

    // [V2G20-1644]
    GIVEN("Good case - Resuming secc shall provide the same service ids and parameter set ids "
          "(ServiceRenegotiationSupported: false)") {
    } // Todo(sl): Fill out

    GIVEN("Bad case - EV supported_service_ids do not match with evse supported services") {
    } // Todo(sl): Fill out

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Performance Timeout") {} // todo(sl): not here

    // GIVEN("Sequence Timeout") {} // todo(sl): not here
}
