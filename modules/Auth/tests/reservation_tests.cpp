// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <utils/date.hpp>

#define private public
// Make 'ReservationEVSEs.h privates public to test a helper function 'get_all_possible_orders'.
#include "ReservationEVSEs.h"
#undef private

#include "ReservationHandler.hpp"

namespace module {
const static std::string TOKEN_1 = "RFID_1";
const static std::string TOKEN_2 = "RFID_2";
const static std::string TOKEN_3 = "RFID_3";
const static std::string TOKEN_4 = "RFID_4";
const static std::string TOKEN_5 = "RFID_5";
const static std::string TOKEN_6 = "RFID_6";
const static std::string TOKEN_7 = "RFID_7";
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

class ReservationEVSETest : public ::testing::Test {
private:
    uint32_t reservation_id = 0;

protected:
    types::reservation::Reservation
    create_reservation_no_evseid(const types::evse_manager::ConnectorTypeEnum connector_type) {
        return types::reservation::Reservation{
            static_cast<int32_t>(reservation_id++), "TOKEN_" + std::to_string(reservation_id),
            Everest::Date::to_rfc3339((date::utc_clock::now()) + std::chrono::hours(1)), std::nullopt, connector_type};
    }
};

TEST_F(ReservationEVSETest, scenario01) {
    using namespace types::reservation;
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
}

TEST_F(ReservationEVSETest, scenario02) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, scenario03) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
}

TEST_F(ReservationEVSETest, scenario04) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(4, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(4, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(7, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(7, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
}

TEST_F(ReservationEVSETest, scenario05) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, scenario06) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, scenario07) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::sType3);

    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
}

TEST_F(ReservationEVSETest, scenario08) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(r.make_reservation(std::nullopt,
                                    create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
}

TEST_F(ReservationEVSETest, scenario09) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, scenario10) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(5, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(5, 1, types::evse_manager::ConnectorTypeEnum::Unknown);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Other3Ph)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, scenario11) {
    ReservationEVSEs r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_TRUE(r.make_reservation(std::nullopt,
                                   create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::Unknown)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cType2)));
    EXPECT_TRUE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
    EXPECT_FALSE(
        r.make_reservation(std::nullopt, create_reservation_no_evseid(types::evse_manager::ConnectorTypeEnum::cCCS2)));
}

TEST_F(ReservationEVSETest, get_all_possible_orders) {
    using namespace types::evse_manager;
    ReservationEVSEs r;
    std::vector<ConnectorTypeEnum> connectors;
    connectors.push_back(ConnectorTypeEnum::cCCS2);

    std::vector<std::vector<ConnectorTypeEnum>> result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result, std::vector<std::vector<ConnectorTypeEnum>>({{ConnectorTypeEnum::cCCS2}}));

    connectors.push_back(ConnectorTypeEnum::cCCS2);
    result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result,
              std::vector<std::vector<ConnectorTypeEnum>>({{ConnectorTypeEnum::cCCS2, ConnectorTypeEnum::cCCS2}}));

    connectors.push_back(ConnectorTypeEnum::cCCS1);
    result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result, std::vector<std::vector<ConnectorTypeEnum>>(
                          {{ConnectorTypeEnum::cCCS2, ConnectorTypeEnum::cCCS2, ConnectorTypeEnum::cCCS1},
                           {ConnectorTypeEnum::cCCS2, ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::cCCS2},
                           {ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::cCCS2, ConnectorTypeEnum::cCCS2}}));
}

TEST_F(ReservationEVSETest, get_all_possible_orders2) {
    using namespace types::evse_manager;
    ReservationEVSEs r;
    std::vector<ConnectorTypeEnum> connectors;
    connectors.push_back(ConnectorTypeEnum::cType1);

    std::vector<std::vector<ConnectorTypeEnum>> result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result, std::vector<std::vector<ConnectorTypeEnum>>({{ConnectorTypeEnum::cType1}}));

    connectors.push_back(ConnectorTypeEnum::Pan);
    result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result,
              std::vector<std::vector<ConnectorTypeEnum>>({{ConnectorTypeEnum::cType1, ConnectorTypeEnum::Pan},
                                                           {ConnectorTypeEnum::Pan, ConnectorTypeEnum::cType1}}));

    connectors.push_back(ConnectorTypeEnum::cCCS1);
    result = r.get_all_possible_orders(connectors);
    EXPECT_EQ(result, std::vector<std::vector<ConnectorTypeEnum>>(
                          {{ConnectorTypeEnum::cType1, ConnectorTypeEnum::Pan, ConnectorTypeEnum::cCCS1},
                           {ConnectorTypeEnum::Pan, ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::cType1},
                           {ConnectorTypeEnum::Pan, ConnectorTypeEnum::cType1, ConnectorTypeEnum::cCCS1},
                           {ConnectorTypeEnum::cType1, ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::Pan},
                           {ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::Pan, ConnectorTypeEnum::cType1},
                           {ConnectorTypeEnum::cCCS1, ConnectorTypeEnum::cType1, ConnectorTypeEnum::Pan}}));
}

} // namespace module
