// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <iso15118/detail/d20/state/service_selection.hpp>

using namespace iso15118;

namespace dt = message_20::datatypes;

SCENARIO("Service selection state handling") {
    GIVEN("Bad case - Unknown session") {

        d20::Session session = d20::Session();

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        session = d20::Session();

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_UnknownSession, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_UnknownSession);
        }
    }

    GIVEN("Bad case: selected_energy_transfer_service false parameter set id - FAILED_ServiceSelectionInvalid") {

        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 1;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Bad case: selected_energy_transfer service is not correct - FAILED_NoEnergyTransferServiceSelected") {

        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::AC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_NoEnergyTransferServiceSelected, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_NoEnergyTransferServiceSelected);
        }
    }

    GIVEN("Good case") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
        }
    }

    GIVEN("Good case - Check if session variables is set") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == dt::ServiceCategory::DC);
            REQUIRE(selected_services.selected_control_mode == dt::ControlMode::Scheduled);
        }
    }

    GIVEN("Good case - DC_BPT") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC_BPT};
        session.offered_services.dc_bpt_parameter_list[0] = {{
                                                                 dt::DcConnector::Extended,
                                                                 dt::ControlMode::Scheduled,
                                                                 dt::MobilityNeedsMode::ProvidedByEvcc,
                                                                 dt::Pricing::NoPricing,
                                                             },
                                                             dt::BptChannel::Unified,
                                                             dt::GeneratorMode::GridFollowing};

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == dt::ServiceCategory::DC_BPT);
            REQUIRE(selected_services.selected_control_mode == dt::ControlMode::Scheduled);
        }
    }

    GIVEN("Bad case: selected_vas_list false service id - FAILED_ServiceSelectionInvalid") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {dt::ServiceCategory::Internet};
        session.offered_services.internet_parameter_list[0] = {
            dt::Protocol::Http,
            dt::Port::Port80,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{dt::ServiceCategory::ParkingStatus, 0}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Bad case: selected_vas_list false parameter set id - FAILED_ServiceSelectionInvalid") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {dt::ServiceCategory::Internet};
        session.offered_services.internet_parameter_list[0] = {
            dt::Protocol::Http,
            dt::Port::Port80,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{dt::ServiceCategory::Internet, 1}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: FAILED_ServiceSelectionInvalid, mandatory fields should be set") {
            REQUIRE(res.response_code == dt::ResponseCode::FAILED_ServiceSelectionInvalid);
        }
    }

    GIVEN("Good case - DC & Internet & Parking") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::DC};
        session.offered_services.dc_parameter_list[0] = {
            dt::DcConnector::Extended,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        session.offered_services.vas_services = {dt::ServiceCategory::Internet, dt::ServiceCategory::ParkingStatus};
        session.offered_services.internet_parameter_list[0] = {
            dt::Protocol::Http,
            dt::Port::Port80,
        };

        session.offered_services.parking_parameter_list[0] = {
            dt::IntendedService::VehicleCheckIn,
            dt::ParkingStatus::ManualExternal,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::DC;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        req.selected_vas_list = {{dt::ServiceCategory::Internet, 0}, {dt::ServiceCategory::ParkingStatus, 0}};

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);
        }
    }

    GIVEN("Good case - MCS") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::MCS};
        session.offered_services.mcs_parameter_list[0] = {
            dt::McsConnector::Mcs,
            dt::ControlMode::Scheduled,
            dt::MobilityNeedsMode::ProvidedByEvcc,
            dt::Pricing::NoPricing,
        };

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::MCS;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == dt::ServiceCategory::MCS);
            REQUIRE(selected_services.selected_control_mode == dt::ControlMode::Scheduled);
            REQUIRE(*std::get_if<dt::McsConnector>(&selected_services.selected_connector) == dt::McsConnector::Mcs);
        }
    }

    GIVEN("Good case - MCS_BPT") {
        d20::Session session = d20::Session();

        session.offered_services.energy_services = {dt::ServiceCategory::MCS_BPT};
        session.offered_services.mcs_bpt_parameter_list[0] = {{
                                                                  dt::McsConnector::Mcs,
                                                                  dt::ControlMode::Scheduled,
                                                                  dt::MobilityNeedsMode::ProvidedByEvcc,
                                                                  dt::Pricing::NoPricing,
                                                              },
                                                              dt::BptChannel::Unified,
                                                              dt::GeneratorMode::GridFollowing};

        message_20::ServiceSelectionRequest req;
        req.header.session_id = session.get_id();
        req.header.timestamp = 1691411798;
        req.selected_energy_transfer_service.service_id = dt::ServiceCategory::MCS_BPT;
        req.selected_energy_transfer_service.parameter_set_id = 0;

        const auto res = d20::state::handle_request(req, session);

        THEN("ResponseCode: OK") {
            REQUIRE(res.response_code == dt::ResponseCode::OK);

            const auto selected_services = session.get_selected_services();
            REQUIRE(selected_services.selected_energy_service == dt::ServiceCategory::MCS_BPT);
            REQUIRE(selected_services.selected_control_mode == dt::ControlMode::Scheduled);
            REQUIRE(*std::get_if<dt::McsConnector>(&selected_services.selected_connector) == dt::McsConnector::Mcs);
            const auto bpt_channel = selected_services.selected_bpt_channel.has_value() and
                                     selected_services.selected_bpt_channel.value() == dt::BptChannel::Unified;
            REQUIRE(bpt_channel == true);
        }
    }

    // GIVEN("Bad case - FAILED_NoServiceRenegotiationSupported") {} // todo(sl): pause/resume not supported yet

    // GIVEN("Bad Case - sequence error") {} // todo(sl): not here

    // GIVEN("Bad Case - Performance Timeout") {} // todo(sl): not here

    // GIVEN("Bad Case - Sequence Timeout") {} // todo(sl): not here
}
