// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/date.hpp>

#include <generated/interfaces/kvs/Interface.hpp>

#define private public
// Make 'ReservationHandler.hpp privates public to test a helper function 'get_all_possible_orders'.
#include "ReservationHandler.hpp"
#undef private

using testing::_;
using testing::MockFunction;
using testing::Return;
using testing::SaveArg;

using namespace types::reservation;

namespace module {
class ReservationHandlerTest : public ::testing::Test {
private:
    uint32_t reservation_id = 0;

protected:
    Reservation create_reservation(const types::evse_manager::ConnectorTypeEnum connector_type) {
        return Reservation{static_cast<int32_t>(reservation_id),
                           "TOKEN_" + std::to_string(reservation_id++),
                           Everest::Date::to_rfc3339((date::utc_clock::now()) + std::chrono::hours(1)),
                           std::nullopt,
                           std::nullopt,
                           connector_type};
    }

    void add_connector(const int32_t evse_id, const uint32_t connector_id,
                       const types::evse_manager::ConnectorTypeEnum type,
                       std::map<int32_t, std::unique_ptr<EVSEContext>>& evses) {
        if (evses.count(evse_id) > 0) {
            evses[evse_id]->connectors.push_back(Connector{static_cast<int>(connector_id), type});
        } else {
            evses[evse_id] = std::make_unique<EVSEContext>(evse_id, connector_id, type);
        }
    }

    kvsIntf kvs;
    std::map<int32_t, std::unique_ptr<EVSEContext>> evses;
    ReservationHandler r{evses, "reservation_kvs", &kvs};
};

TEST_F(ReservationHandlerTest, global_reservation_scenario_01) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's, one with cCCS2, two with cCCS2 and cType2.
    // Three cCCS2 reservations should be accepted.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_02) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's, one with cCCS2, two with cCCS2 and cType2.
    // One cCCS2 and one cType2 reservation should be accepted, but another reservation can not be made. Because if
    // there would be two cCCS2 and one cType2, it is possible that first two cCCS2 type cars arrive, charge at the
    // two combined charging stations and when the cType2 car arrives, there is no charging station with this connector
    // available anymore.

    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_03) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's, one with cCCS2, two with cCCS2 and cType2.
    // Two cType2 reservations should be accepted, but a third reservation is not accepted, because it is not guaranteed
    // that in all circumstances a charger is available (for example cCCS2 goes to evse 2, cType2 goes to connector
    // 1 and the second cType2 arrives but no charger is available anymore).
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_04) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's, one with cCCS2, two with cCCS2 and cType2.
    // A cCCS2 and cType2 reservation should be accepted, because it does not matter in which order they arrive, there
    // is always an evse available for the other one. But a cType2 as third reservation is not possible. Imagine the
    // first car that arrives is cCCS2 and charges at evse 4 or 7, the second car can only put it at 4 or 7, then
    // the third car that arrives (cType2) does not have an EVSE for his type available.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(4, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(4, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(7, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(7, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_05) {
    // Test global reservations (not bound to specific evse id). 4 EVSE's, three with cCCS2, one with cCCS2 and cType2.
    // When a cCCS2 reservation is made, cType2 can not make a reservation anymore, because it is possible that when
    // the cCCS2 car first arrives, there is no EVSE available for the cType2 car anymore (evse 2).
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(3, 5, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_06) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's, two with cCCS2, one with cCCS2 and cType2.
    // Only one cType2 reservation can be made and nothing else, also no cCCS2 reservation (because when the cCCS2 car
    // arives first and puts it on connector 2, the cType2 that arrives second does not have an EVSE available anymore).
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_07) {
    // Test global reservations (not bound to specific evse id). 1 EVSE only. Unknown is accepted, a type that is not
    // available is rejected.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::sType3, evses);

    // There is no cType2 connector on this evse.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Rejected);
    // Unknown is accepted
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_08) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's with all cCCS2 and cType2 connectors.
    // Unknown and cCCS2 reservations are accepted, max 3 in total.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_09) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's with all cCCS2 and cType2 connectors.
    // Unknown, cType2 and cCCS2 reservations are accepted, max 3 in total.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_10) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's with all two 'Unknown' connectors.
    // Three reservations are accepted in total, it does not matter what connector types they have.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::Unknown, evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::Unknown, evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::Unknown, evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::Unknown, evses);
    add_connector(5, 0, types::evse_manager::ConnectorTypeEnum::Unknown, evses);
    add_connector(5, 1, types::evse_manager::ConnectorTypeEnum::Unknown, evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Other3Ph)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_11) {
    // Test global reservations (not bound to specific evse id). 3 EVSE's with only one cCCS2 connector each.
    // In total three reservations are accepted with the correct type (cCCS2 or Unknown).
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_12) {
    // Test global reservations (not bound to specific evse id). One EVSE with cCCS2 and cType2, one with cType2 and
    // cTesla, one with cTesla and cCCS2.
    // Only two reservations can be accepted, for the third there is no guarantee there is always place to charge in all
    // orders of arrival of the different cars.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cTesla, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cTesla, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType1)),
              ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cTesla)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, get_all_possible_orders) {
    using namespace types::evse_manager;
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

TEST_F(ReservationHandlerTest, get_all_possible_orders2) {
    using namespace types::evse_manager;
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

TEST_F(ReservationHandlerTest, specific_evse_scenario_01) {
    // Test reservations for a specific evse.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // On EVSE1, there is no cCCS1 type connector.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS1)),
              ReservationResult::Rejected);
    // But there is a cCCS2 type connector, accept reservation.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // Another reservation on cCCS1 type connector will return Occupied, as there already is a reservation.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // But on another connector, the reservation can be made.
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // But only when there is not already a reservation for that specific connector.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, specific_evse_scenario_02) {
    // Test reservations for a specific evse.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // No cCCS2 type on evse 1 (only on 0, but that one is not reserved here).
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Rejected);
    // But it has a cType2 connector.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    // There already is a reservation for this EVSE, so 'Occupied' is returned.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // But on the other EVSE, the reservation can be made.
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // And now it is already reserved, so a second can not be made.
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_01) {
    // Test global reservation (not bound to specific EVSE) combined with reservation for a specific EVSE.
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);

    // Global reservation for cType2, this can be EVSE 0 or 1.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    // Specific reservation for EVSE 1, the global reservation can still charge on EVSE 0.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // There already is a reservation for EVSE 1.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // Specific reservation for EVSE 2, the global reservation can still charge on EVSE 0.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // Specific reservation for EVSE 0, but if this would be accepted, the global reservation can not charge anymore, so
    // this is denied.
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // EVSE 1 is already occupied with a reservation.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // Global reservation, can not be made because then the first reservation can not charge anymore.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // Same for a cCCS2 global reservation.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_02) {
    // Test global reservation (not bound to specific EVSE) combined with reservation for a specific EVSE.
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);

    // Global reservation for cType2, this can charge on EVSE 0 or 1.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    // Specific reservation for EVSE 1, the global reservation still has an EVSE left to charge on.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // EVSE 1 is already reserved.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // Another global reservation. This can not be made, because the first global reservation would not have been able
    // to charge in that case.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // But a global reservation for cCCS2 is possible. Because as EVSE 1 is reserved, there is only one option left for
    // this reservation, which is EVSE 2, and the first reservation can then still charge on EVSE 0.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // But another global reservation is not possible anymore.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // As well as any other specific reservation.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_03) {
    // Test global reservation (not bound to specific EVSE) combined with reservation for a specific EVSE.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    // Make a reservation for EVSE 2.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    // EVSE 2 already has a reservation.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // Make a reservation for EVSE 1.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    // A global reservation is possible, because EVSE 0 is still not reserved and has cCCS2.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // But another global reservation is not possible, because there are not enough cCCS2 connectors.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);
    // And cType2 is also not possible, because it can arrive before the first global reservation and then put the car
    // at EVSE 0, and then there will be no place for the car that did the first global reservation.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    // But a specific reservation for EVSE 3 is possible, because the first global reservation can then still charge
    // at EVSE 0.
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, check_charging_possible_global_specific_reservations_scenario_01) {
    // Do some specific reservations and check if charging is possible when a car arrives.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    // Charging is possible on all EVSE's (except for the not existing one of course).
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(2));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    EXPECT_FALSE(r.is_charging_possible(4));
    // But after a reservation on EVSE 2, charging is not possible on that EVSE anymore.
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    // Now EVSE 1 is also occupied, charging will also not be possible there.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    // A global reservation have been made for a cCCS2 charger. The only still available is the one on EVSE 0. That one
    // must be available for the reservation at all times.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    // So that makes charging on EVSE 0 not possible.
    EXPECT_FALSE(r.is_charging_possible(0));
    // But the car can charge at EVSE 3 (as EVSE 0 is then still available for the global reservation).
    EXPECT_TRUE(r.is_charging_possible(3));
    // And now all reservations are made, no new car can make a reservation or charge anymore.
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_FALSE(r.is_charging_possible(3));
}

TEST_F(ReservationHandlerTest, check_charging_possible_global_specific_reservations_scenario_02) {
    // Do some specific reservations and check if charging is possible when a car arrives.
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);

    // Charging is possible on all EVSE's.
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    // After a global reservation for cType2, charging is still possible on all EVSE's, is there are two cType2
    // connectors, so when one car charges, there is still a cType2 available.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    // A reservation for EVSE 1 is made. Now the global reservation only has the possibility to charge on EVSE 0.
    // So on that EVSE, charging is not possible anymore. And of course also not on EVSE 1 as that one is reserved.
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    // Another global reservation makes charging impossible on all EVSE's.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_FALSE(r.is_charging_possible(2));
}

TEST_F(ReservationHandlerTest, is_evse_reserved) {
    // Check if a specific EVSE is reserved.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_FALSE(r.is_evse_reserved(1));

    // After a global reservation, no specific EVSE is still reserved.
    r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2));

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_FALSE(r.is_evse_reserved(1));

    // But after a specific reservation, the EVSE of that reservation is reserved.
    r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2));

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_TRUE(r.is_evse_reserved(1));
}

TEST_F(ReservationHandlerTest, change_availability_scenario_01) {
    // Change availability of an EVSE and check if reservations are cancelled.
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // Four global reservations are made.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);

    // Set an evse to not available, this will call the cancel reservation callback for the last reserved reservation
    // id
    EXPECT_CALL(reservation_callback_mock, Call(_, 3, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));

    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);
    EXPECT_FALSE(evse_id.has_value());

    // Setting an evse to faulted will cancel the next reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));

    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);
    EXPECT_FALSE(evse_id.has_value());

    // Set evse to available again. This will not call a cancelled callback. And setting one to unavailable will also
    // not cause the cancelled callback to be called because there is still one evse available.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, ReservationEndReason::Cancelled)).Times(0);

    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::ERROR_CLEARED);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);

    this->evses[2]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[2]->connectors.at(1).get_state(), 2, 1);

    // If we set even one more evse to unavailable (or actually, to faulted), this will cancel the next (or actually
    // previous) reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 1, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));

    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(1).get_state(), 0, 1);
    EXPECT_FALSE(evse_id.has_value());
}

TEST_F(ReservationHandlerTest, change_availability_scenario_02) {
    // Change availability of an EVSE and check if reservations are cancelled. This time, global and specific EVSE
    // reservations mixed.
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);

    // Set an evse to not available, this will call the cancel reservation callback for the reservation of that evse id.
    EXPECT_CALL(reservation_callback_mock, Call(_, 0, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 1);

    // Setting an evse to faulted will cancel the next reservation (last made), this will be a 'global' reservation as
    // there is no evse specific reservation made.
    EXPECT_CALL(reservation_callback_mock, Call(_, 3, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));
    this->evses[2]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[2]->connectors.at(1).get_state(), 2, 1);
    EXPECT_FALSE(evse_id.has_value());

    // Set one more evse to unavailable, this will cancel the next reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(1).get_state(), 0, 1);

    // r.set_evse_state(ConnectorState::FAULTED, 0);
    EXPECT_FALSE(evse_id.has_value());

    // Set the last evse to unavailable will cancel the reservation of that specific evse.
    EXPECT_CALL(reservation_callback_mock, Call(_, 1, ReservationEndReason::Cancelled)).WillOnce(SaveArg<0>(&evse_id));

    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 3);
}

TEST_F(ReservationHandlerTest, reservation_evse_unavailable) {
    // Set evse unavailable and check if a reservation can not be made in that case. Global reservations.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[1]->connectors.at(0).get_state(), 1, 0);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);
    // r.set_evse_state(ConnectorState::UNAVAILABLE, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);

    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[0]->connectors.at(0).get_state(), 0, 0);
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[0]->connectors.at(0).get_state(), 0, 1);
    this->evses[2]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[2]->connectors.at(0).get_state(), 2, 0);
    this->evses[2]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[2]->connectors.at(0).get_state(), 2, 1);
    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Unavailable);
}

TEST_F(ReservationHandlerTest, reservation_specific_evse_unavailable) {
    // Set an EVSE to unavailable and check if that specific EVSE can not be reserved anymore.

    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Unavailable);
}

TEST_F(ReservationHandlerTest, reservation_specific_evse_faulted) {
    // Set an EVSE to faulted and check if that specific EVSE can not be reserved anymore.

    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // Evse state is faulted, should return faulted.
    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(0).get_state(), 0, 0);
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(1).get_state(), 0, 1);

    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Faulted);

    // Connectors are faulted, should return faulted.
    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(0).get_state(), 1, 0);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_evse_faulted) {
    // Set EVSE's to faulted and check if no global reservations can made for that EVSE.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(0).get_state(), 1, 0);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);

    // One EVSE is faulted and there are only two cCCS2 connectors left. So only two global reservations for cCCS2 can
    // be made.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);

    // Everything is faulted now, a reservation is not possible anymore.
    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(0).get_state(), 0, 0);
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[0]->connectors.at(1).get_state(), 0, 1);
    this->evses[2]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[2]->connectors.at(0).get_state(), 2, 0);
    this->evses[2]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[2]->connectors.at(1).get_state(), 2, 1);
    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);

    // All EVSE's are faulted, so 'Faulted' is returned.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_evse_unavailable_and_faulted) {
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // Set evse to faulted.
    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(0).get_state(), 1, 0);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Occupied);

    // Set all other evse's to unavailable, but not faulted.
    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[0]->connectors.at(0).get_state(), 0, 0);
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[0]->connectors.at(1).get_state(), 0, 1);
    this->evses[2]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[2]->connectors.at(0).get_state(), 2, 0);
    this->evses[2]->connectors.at(1).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[2]->connectors.at(1).get_state(), 2, 1);
    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    r.on_connector_state_changed(this->evses[3]->connectors.at(0).get_state(), 3, 1);

    // At least one evse is faulted, so 'faulted' is returned.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_connector_all_faulted) {
    // Set all connectors to 'Faulted', no reservation can be made and the function will return 'Faulted'.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(3, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    this->evses[0]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    this->evses[3]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    this->evses[3]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_connector_unavailable) {
    // Set specific connectors to 'Unavailable' and try to make reservations.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS1, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType1, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    this->evses[0]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::DISABLE);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);

    // There is a reservation already made, so this will return 'occupied'.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS1)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType1)),
              ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, reservation_in_the_past) {
    // Try to create a reservation in the past, this should be rejected.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() - std::chrono::hours(2));
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Rejected);
}

TEST_F(ReservationHandlerTest, reservation_timer) {
    // Test the reservation timer: after the time has expired, the reservation should be cancelled.
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, ReservationEndReason::Expired)).WillOnce(SaveArg<0>(&evse_id));
    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::seconds(1));
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    sleep(1);
    EXPECT_FALSE(evse_id.has_value());

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, ReservationEndReason::Expired)).WillOnce(SaveArg<0>(&evse_id));
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::seconds(1));
    EXPECT_EQ(r.make_reservation(0, reservation), ReservationResult::Accepted);
    sleep(1);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 0);
}

TEST_F(ReservationHandlerTest, cancel_reservation) {
    // Cancel reservation and check if a new reservation can be made after an old one is cancelled.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);

    std::pair<bool, std::optional<uint32_t>> reservation_cancelled_check_value;

    // There was no reservation with id 5.
    reservation_cancelled_check_value = {false, std::nullopt};
    EXPECT_EQ(r.cancel_reservation(5, false, ReservationEndReason::Cancelled), reservation_cancelled_check_value);

    // There was a reservation with id 1, it had no evse id (global reservation).
    reservation_cancelled_check_value = {true, std::nullopt};
    EXPECT_EQ(r.cancel_reservation(1, false, ReservationEndReason::Cancelled), reservation_cancelled_check_value);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);

    // There was a reservation with id 3, it was made for evse id 1.
    reservation_cancelled_check_value = {true, 1};
    EXPECT_EQ(r.cancel_reservation(3, false, ReservationEndReason::Cancelled), reservation_cancelled_check_value);
}

TEST_F(ReservationHandlerTest, overwrite_reservation) {
    // If a reservation is made and another one is made with the same reservation id, it should be overwritten.
    // The old reservation will then be cancelled and the new one is made.
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    add_connector(5, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(5, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, ReservationEndReason::Cancelled)).Times(0);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_EQ(r.make_reservation(5, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(5, reservation), ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, matches_reserved_identifier) {
    // Check if token or parent token matches with a reservation.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation.parent_id_token = "PARENT_TOKEN_0";
    Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation2.parent_id_token = "PARENT_TOKEN_2";
    Reservation reservation3 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation3.parent_id_token = "PARENT_TOKEN_3";
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, reservation3), ReservationResult::Accepted);

    // Id token is correct and evse id as well.
    EXPECT_EQ(r.matches_reserved_identifier(reservation.id_token, std::nullopt, std::nullopt), 0);
    // Id token is correct and evse id as well, parent token is not but that is ignored since the normal token is ok.
    EXPECT_EQ(r.matches_reserved_identifier(reservation.id_token, std::nullopt, "WRONG_PARENT_TOKEN"), 0);
    // Token is wrong.
    EXPECT_EQ(r.matches_reserved_identifier("WRONG_TOKEN", std::nullopt, std::nullopt), std::nullopt);
    // Evse id reservation does not have parent token, do not search in global reservation.
    EXPECT_EQ(r.matches_reserved_identifier(reservation.id_token, 1, std::nullopt), std::nullopt);
    // Evse id is wrong.
    EXPECT_EQ(r.matches_reserved_identifier(reservation2.id_token, 2, std::nullopt), std::nullopt);
    // Token is wrong but parent token is correct.
    EXPECT_EQ(r.matches_reserved_identifier("WRONG_TOKEN", std::nullopt, "PARENT_TOKEN_0"), 0);
    // Token is wrong and parent token as well.
    EXPECT_EQ(r.matches_reserved_identifier("WRONG_TOKEN", std::nullopt, "WRONG_PARENT_TOKEN"), std::nullopt);
    // Evse id is correct and token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(reservation2.id_token, 1, std::nullopt), 1);
    // Evse id is correct but token is wrong.
    EXPECT_EQ(r.matches_reserved_identifier("TOKEN_NOK", 1, std::nullopt), std::nullopt);
    // Evse id is wrong and token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(reservation2.id_token, 2, std::nullopt), std::nullopt);
    // Evse id is correct, token is wrong but parent token is correct.
    EXPECT_EQ(r.matches_reserved_identifier("TOKEN_NOK", 1, "PARENT_TOKEN_2"), 1);
    // Evse id is correct, token is wrong and parent token as well.
    EXPECT_EQ(r.matches_reserved_identifier("TOKEN_NOK", 1, "PARENT_TOKEN_NOK"), std::nullopt);
}

TEST_F(ReservationHandlerTest, has_reservation_parent_id) {
    // Check if the reservation has a parent id token.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation.parent_id_token = "PARENT_TOKEN_0";
    Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation2.parent_id_token = "PARENT_TOKEN_2";
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), ReservationResult::Accepted);

    // Id token is correct and evse id as well.
    EXPECT_TRUE(r.has_reservation_parent_id(std::nullopt));
    EXPECT_TRUE(r.has_reservation_parent_id(1));
    EXPECT_TRUE(r.has_reservation_parent_id(0));
    // Evse id does not exist.
    EXPECT_FALSE(r.has_reservation_parent_id(2));
}

TEST_F(ReservationHandlerTest, has_reservation_parent_id_no_parent_token) {
    // Check if the reservation has a parent id token.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), ReservationResult::Accepted);

    // No parent id tokens
    EXPECT_FALSE(r.has_reservation_parent_id(std::nullopt));
    EXPECT_FALSE(r.has_reservation_parent_id(1));
    EXPECT_FALSE(r.has_reservation_parent_id(0));
    // Evse id does not exist.
    EXPECT_FALSE(r.has_reservation_parent_id(2));
}

TEST_F(ReservationHandlerTest, has_reservation_parent_id_evse_reservation_parent_token) {
    // Check if the reservation has a parent id token.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation2.parent_id_token = "PARENT_TOKEN_2";
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), ReservationResult::Accepted);

    // Only evse id 1 reservation has parent id token.
    EXPECT_FALSE(r.has_reservation_parent_id(std::nullopt));
    EXPECT_TRUE(r.has_reservation_parent_id(1));
    // So evse id 0 has not.
    EXPECT_FALSE(r.has_reservation_parent_id(0));
    // Evse id does not exist.
    EXPECT_FALSE(r.has_reservation_parent_id(2));
}

TEST_F(ReservationHandlerTest, has_reservation_parent_id_global_reservation_parent_token) {
    // Check if the reservation has a parent id token.
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation.parent_id_token = "PARENT_TOKEN_0";
    Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), ReservationResult::Accepted);

    // Only global reservation has parent id token. Reservation on evse id 1 has none.
    EXPECT_TRUE(r.has_reservation_parent_id(std::nullopt));
    EXPECT_FALSE(r.has_reservation_parent_id(1));
    // No reservation for evse id 0, but global reservation has parent id token.
    EXPECT_TRUE(r.has_reservation_parent_id(0));
    // Evse id does not exist.
    EXPECT_FALSE(r.has_reservation_parent_id(2));
}

TEST_F(ReservationHandlerTest, on_reservation_used) {
    // A reservation is made and later used, so the reservation should be removed and the EVSE available again.

    // Register a callback, which should not be called.
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    EXPECT_CALL(reservation_callback_mock, Call(_, _, _)).Times(3);

    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);

    r.on_reservation_used(1);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              ReservationResult::Occupied);

    r.on_reservation_used(0);
    r.on_reservation_used(2);
    r.on_reservation_used(3);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, store_load_reservations) {
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());

    r.init();

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);

    EXPECT_EQ(r.evse_reservations.size(), 1);
    EXPECT_EQ(r.global_reservations.size(), 1);
    EXPECT_EQ(r.reservation_id_to_reservation_timeout_timer_map.size(), 2);

    r.evse_reservations.clear();
    r.global_reservations.clear();
    r.reservation_id_to_reservation_timeout_timer_map.clear();

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());
    EXPECT_TRUE(r.reservation_id_to_reservation_timeout_timer_map.empty());

    r.init();

    EXPECT_EQ(r.evse_reservations.size(), 1);
    EXPECT_EQ(r.global_reservations.size(), 1);
    EXPECT_EQ(r.reservation_id_to_reservation_timeout_timer_map.size(), 2);
}

TEST_F(ReservationHandlerTest, store_load_reservations_connector_unavailable) {
    add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);
    add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2, this->evses);
    add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2, this->evses);

    // Register a callback, which should not be called.
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    EXPECT_CALL(reservation_callback_mock, Call(_, _, _)).Times(1);

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());

    r.init();

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              ReservationResult::Accepted);

    EXPECT_EQ(r.evse_reservations.size(), 1);
    EXPECT_EQ(r.global_reservations.size(), 1);
    EXPECT_EQ(r.reservation_id_to_reservation_timeout_timer_map.size(), 2);

    r.evse_reservations.clear();
    r.global_reservations.clear();
    r.reservation_id_to_reservation_timeout_timer_map.clear();

    EXPECT_TRUE(r.evse_reservations.empty());
    EXPECT_TRUE(r.global_reservations.empty());
    EXPECT_TRUE(r.reservation_id_to_reservation_timeout_timer_map.empty());

    this->evses[1]->connectors.at(0).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(0).get_state(), 3, 0);
    this->evses[1]->connectors.at(1).submit_event(ConnectorEvent::FAULTED);
    r.on_connector_state_changed(this->evses[1]->connectors.at(1).get_state(), 1, 1);

    r.init();

    EXPECT_EQ(r.evse_reservations.size(), 0);
    EXPECT_EQ(r.global_reservations.size(), 1);
    EXPECT_EQ(r.reservation_id_to_reservation_timeout_timer_map.size(), 1);
}

} // namespace module
