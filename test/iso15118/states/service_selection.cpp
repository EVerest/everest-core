// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/service_selection.hpp>

using namespace iso15118;

SCENARIO("Service selection state handling") {
    GIVEN("Bad case - Unknown session") {

        d20::Session session = d20::Session();

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        session = d20::Session();

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_UnknownSession);
        }
    }

    GIVEN("Bad case: selected_energy_transfer_service false parameter set id - FAILED_ServiceSelectionInvalid") {

        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 1;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Bad case: selected_energy_transfer service is not correct - FAILED_NoEnergyTransferServiceSelected") {

        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::AC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_NoEnergyTransferServiceSelected, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_NoEnergyTransferServiceSelected);
        }
    }

    GIVEN("Good case") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);
        }
    }

    GIVEN("Good case - Check if session variables is set") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == message_20::ServiceCategory::DC);
            REQUIRE(selected_services.selected_control_mode == message_20::ControlMode::Scheduled);
        }
    }

    GIVEN("Good case - DC_BPT") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC_BPT};
        session.offered_services.dc_bpt_parameter_list[0] = {{
                                                                 message_20::DcConnector::Extended,
                                                                 message_20::ControlMode::Scheduled,
                                                                 message_20::MobilityNeedsMode::ProvidedByEvcc,
                                                                 message_20::Pricing::NoPricing,
                                                             },
                                                             message_20::BptChannel::Unified,
                                                             message_20::GeneratorMode::GridFollowing};

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == message_20::ServiceCategory::DC_BPT);
            REQUIRE(selected_services.selected_control_mode == message_20::ControlMode::Scheduled);
        }
    }

    GIVEN("Bad case: selected_vas_list false service id - FAILED_ServiceSelectionInvalid") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {message_20::ServiceCategory::Internet};
        session.offered_services.internet_parameter_list[0] = {
            message_20::Protocol::Http,
            message_20::Port::Port80,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{message_20::ServiceCategory::ParkingStatus, 0}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Bad case: selected_vas_list false parameter set id - FAILED_ServiceSelectionInvalid") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {message_20::ServiceCategory::Internet};
        session.offered_services.internet_parameter_list[0] = {
            message_20::Protocol::Http,
            message_20::Port::Port80,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{message_20::ServiceCategory::Internet, 1}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == message_20::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Good case - DC & Internet & Parking") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {message_20::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            message_20::DcConnector::Extended,
            message_20::ControlMode::Scheduled,
            message_20::MobilityNeedsMode::ProvidedByEvcc,
            message_20::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {message_20::ServiceCategory::Internet,
                                                 message_20::ServiceCategory::ParkingStatus};
        session.offered_services.internet_parameter_list[0] = {
            message_20::Protocol::Http,
            message_20::Port::Port80,
        };

        session.offered_services.parking_parameter_list[0] = {
            message_20::IntendedService::VehicleCheckIn,
            message_20::ParkingStatus::ManualExternal,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = message_20::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{message_20::ServiceCategory::Internet, 0},
                                 {message_20::ServiceCategory::ParkingStatus, 0}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == message_20::ResponseCode::OK);
        }
    }

    // GIVEN("Bad case - FAILED_NoServiceRenegotiationSupported") {} // todo(sl): pause/resume not supported yet

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // todo(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // todo(sl): not here
}
