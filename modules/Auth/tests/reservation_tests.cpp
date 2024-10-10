// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <utils/date.hpp>

#include "ReservationHandler.hpp"

namespace module {
const static std::string TOKEN_1 = "RFID_1";
const static std::string TOKEN_2 = "RFID_2";
const static std::string TOKEN_3 = "RFID_3";
const static std::string PARENT_ID_TOKEN = "PARENT_RFID";

TEST(ReservationHandler, single_reservation) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    reservations.init_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    reservations.init_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Accepted);
}

TEST(ReservationHandler, single_reservation_type_unknown) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::Unknown;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Accepted);
}

TEST(ReservationHandler, single_reservation_type_wrong) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cTesla;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Unavailable);
}

TEST(ReservationHandler, too_many_global_reservations) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Accepted);

    types::reservation::Reservation reservation2;
    reservation2.reservation_id = 2;
    reservation2.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation2.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation2.id_token = TOKEN_2;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation2, 1),
              types::reservation::ReservationResult::Occupied);

    types::reservation::Reservation reservation3;
    reservation3.reservation_id = 3;
    reservation3.connector_type = types::evse_manager::ConnectorTypeEnum::cType2;
    reservation3.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation3.id_token = TOKEN_3;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation3, 1),
              types::reservation::ReservationResult::Occupied);
}

TEST(ReservationHandler, too_many_evse_specific_reservations) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(0, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Accepted);

    types::reservation::Reservation reservation2;
    reservation2.reservation_id = 2;
    reservation2.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation2.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation2.id_token = TOKEN_2;

    EXPECT_EQ(reservations.reserve(0, ConnectorState::AVAILABLE, true, reservation2, 1),
              types::reservation::ReservationResult::Occupied);

    types::reservation::Reservation reservation3;
    reservation3.reservation_id = 3;
    reservation3.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation3.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation3.id_token = TOKEN_3;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation3, 1),
              types::reservation::ReservationResult::Occupied);
}

TEST(ReservationHandler, different_number_of_connector_types) {
    ReservationHandler reservations;
    reservations.init_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    reservations.init_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservations.init_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    reservations.init_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    types::reservation::Reservation reservation;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation.id_token = TOKEN_1;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation, 1),
              types::reservation::ReservationResult::Accepted);

    types::reservation::Reservation reservation2;
    reservation2.reservation_id = 2;
    reservation2.connector_type = types::evse_manager::ConnectorTypeEnum::cType2;
    reservation2.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation2.id_token = TOKEN_2;

    EXPECT_EQ(reservations.reserve(0, ConnectorState::AVAILABLE, true, reservation2, 1),
              types::reservation::ReservationResult::Accepted);

    types::reservation::Reservation reservation3;
    reservation3.reservation_id = 3;
    reservation3.connector_type = types::evse_manager::ConnectorTypeEnum::cType2;
    reservation3.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation3.id_token = TOKEN_3;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation3, 1),
              types::reservation::ReservationResult::Occupied);

    types::reservation::Reservation reservation4;
    reservation3.reservation_id = 4;
    reservation3.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation3.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));
    reservation3.id_token = TOKEN_3;

    EXPECT_EQ(reservations.reserve(std::nullopt, ConnectorState::AVAILABLE, true, reservation4, 1),
              types::reservation::ReservationResult::Accepted);
}

TEST(ReservationEVSE, different_number_of_connector_types) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
}

TEST(ReservationEVSE, different_number_of_connector_types2) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
}

TEST(ReservationEVSE, different_number_of_connector_types3) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
}

TEST(ReservationEVSE, different_number_of_connector_types4) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cCCS2));
    EXPECT_TRUE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
    EXPECT_FALSE(r.make_reservation(std::nullopt, types::evse_manager::ConnectorTypeEnum::cType2));
}

} // namespace module
