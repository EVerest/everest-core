// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/date.hpp>

#define private public
// Make 'ReservationHandler.hpp privates public to test a helper function 'get_all_possible_orders'.
#include "ReservationHandler.hpp"
#undef private

using testing::_;
using testing::MockFunction;
using testing::Return;
using testing::SaveArg;

// TODO mz tests for 'unknown'

namespace module {
class ReservationHandlerTest : public ::testing::Test {
private:
    uint32_t reservation_id = 0;

protected:
    types::reservation::Reservation create_reservation(const types::evse_manager::ConnectorTypeEnum connector_type) {
        return types::reservation::Reservation{
            static_cast<int32_t>(reservation_id), "TOKEN_" + std::to_string(reservation_id++),
            Everest::Date::to_rfc3339((date::utc_clock::now()) + std::chrono::hours(1)), std::nullopt, connector_type};
    }
};

TEST_F(ReservationHandlerTest, global_reservation_scenario_01) {
    using namespace types::reservation;
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_02) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_03) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_04) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(4, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(4, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(7, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(7, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_05) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 5, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_06) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_07) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::sType3);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_08) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_09) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_10) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(5, 0, types::evse_manager::ConnectorTypeEnum::Unknown);
    r.add_connector(5, 1, types::evse_manager::ConnectorTypeEnum::Unknown);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Other3Ph)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_11) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::Unknown)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_scenario_12) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cTesla);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cTesla);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType1)),
              types::reservation::ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cTesla)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, get_all_possible_orders) {
    using namespace types::evse_manager;
    ReservationHandler r;
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
    ReservationHandler r;
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
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS1)),
              types::reservation::ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, specific_evse_scenario_02) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Rejected);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_01) {
    ReservationHandler r;
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_02) {
    ReservationHandler r;
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, global_reservation_specific_evse_combination_scenario_03) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
}

TEST_F(ReservationHandlerTest, check_charging_possible_global_specific_reservations_scenario_01) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(2));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    EXPECT_FALSE(r.is_charging_possible(4));
    EXPECT_EQ(r.make_reservation(2, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(3));
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(3));
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_FALSE(r.is_charging_possible(2));
    EXPECT_FALSE(r.is_charging_possible(3));
}

TEST_F(ReservationHandlerTest, check_charging_possible_global_specific_reservations_scenario_02) {
    ReservationHandler r;
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);

    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_TRUE(r.is_charging_possible(0));
    EXPECT_TRUE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_TRUE(r.is_charging_possible(2));
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_FALSE(r.is_charging_possible(0));
    EXPECT_FALSE(r.is_charging_possible(1));
    EXPECT_FALSE(r.is_charging_possible(2));
}

TEST_F(ReservationHandlerTest, is_evse_reserved) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_FALSE(r.is_evse_reserved(1));

    r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2));

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_FALSE(r.is_evse_reserved(1));

    r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2));

    EXPECT_FALSE(r.is_evse_reserved(0));
    EXPECT_TRUE(r.is_evse_reserved(1));
}

TEST_F(ReservationHandlerTest, change_availability_scenario_01) {
    ReservationHandler r;
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const types::reservation::ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);

    // Set an evse to not available, this will call the cancel reservation callback for the last reserved reservation
    // id
    EXPECT_CALL(reservation_callback_mock, Call(_, 3, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, false, 1);
    EXPECT_FALSE(evse_id.has_value());

    // Setting an evse to faulted will cancel the next reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, true, 3);
    EXPECT_FALSE(evse_id.has_value());

    // Set evse to available again. This will not call a cancelled callback. And setting one to unavailable will also
    // not cause the cancelled callback to be called because there is still one evse available.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, types::reservation::ReservationEndReason::Cancelled)).Times(0);

    r.set_evse_available(true, false, 3);
    r.set_evse_available(false, false, 2);

    // If we set even one more evse to unavailable (or actually, to faulted), this will cancel the next (or actually
    // previous) reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 1, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, true, 0);
    EXPECT_FALSE(evse_id.has_value());
}

TEST_F(ReservationHandlerTest, change_availability_scenario_02) {
    ReservationHandler r;
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const types::reservation::ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(3, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);

    // Set an evse to not available, this will call the cancel reservation callback for the reservation of that evse id.
    EXPECT_CALL(reservation_callback_mock, Call(_, 0, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, false, 1);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 1);

    // Setting an evse to faulted will cancel the next reservation (last made), this will be a 'global' reservation as
    // there is no evse specific reservation made.
    EXPECT_CALL(reservation_callback_mock, Call(_, 3, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, true, 2);
    EXPECT_FALSE(evse_id.has_value());

    // Set one more evse to unavailable, this will cancel the next reservation.
    EXPECT_CALL(reservation_callback_mock, Call(_, 2, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, true, 0);
    EXPECT_FALSE(evse_id.has_value());

    // Set the last evse to unavailable will cancel the reservation of that specific evse.
    EXPECT_CALL(reservation_callback_mock, Call(_, 1, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    r.set_evse_available(false, true, 3);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 3);
}

TEST_F(ReservationHandlerTest, reservation_evse_unavailable) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    r.set_evse_available(false, false, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);

    r.set_evse_available(false, false, 0);
    r.set_evse_available(false, false, 2);
    r.set_evse_available(false, false, 3);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Unavailable);
}

TEST_F(ReservationHandlerTest, reservation_specific_evse_unavailable) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    r.set_evse_available(false, false, 1);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Unavailable);
}

TEST_F(ReservationHandlerTest, reservation_specific_evse_faulted) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    // Available is false and faulted is true, should return faulted.
    r.set_evse_available(false, true, 0);

    EXPECT_EQ(r.make_reservation(0, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Faulted);

    // Available is true and faulted is true, should return faulted.
    r.set_evse_available(true, true, 1);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_evse_faulted) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    r.set_evse_available(false, true, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);

    r.set_evse_available(true, true, 0);
    r.set_evse_available(false, true, 2);
    r.set_evse_available(false, true, 3);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_evse_unavailable_and_faulted) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    // Set evse to faulted.
    r.set_evse_available(false, true, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Occupied);

    // Set all other evse's to unavailable, but not faulted.
    r.set_evse_available(false, false, 0);
    r.set_evse_available(false, false, 2);
    r.set_evse_available(false, false, 3);

    // At least one evse is faulted, so 'faulted' is returned.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_connector_all_faulted) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(3, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(3, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    r.set_connector_available(false, true, 0, 0);
    r.set_connector_available(false, true, 0, 1);
    r.set_connector_available(true, true, 3, 0);
    r.set_connector_available(false, true, 3, 1);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Faulted);
}

TEST_F(ReservationHandlerTest, reservation_connector_unavailable) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS1);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType1);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    r.set_connector_available(false, false, 0, 0);
    r.set_connector_available(false, false, 1, 0);
    r.set_connector_available(false, true, 1, 1);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);

    // There is a reservation already made, so this will return 'occupied'.
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS1)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType1)),
              types::reservation::ReservationResult::Occupied);
}

TEST_F(ReservationHandlerTest, reservation_in_the_past) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    types::reservation::Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() - std::chrono::hours(2));
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), types::reservation::ReservationResult::Rejected);
}

TEST_F(ReservationHandlerTest, reservation_timer) {
    ReservationHandler r;
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const types::reservation::ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, types::reservation::ReservationEndReason::Expired))
        .WillOnce(SaveArg<0>(&evse_id));
    types::reservation::Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::seconds(1));
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), types::reservation::ReservationResult::Accepted);
    sleep(1);
    EXPECT_FALSE(evse_id.has_value());

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, types::reservation::ReservationEndReason::Expired))
        .WillOnce(SaveArg<0>(&evse_id));
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::seconds(1));
    EXPECT_EQ(r.make_reservation(0, reservation), types::reservation::ReservationResult::Accepted);
    sleep(1);
    ASSERT_TRUE(evse_id.has_value());
    EXPECT_EQ(evse_id.value(), 0);
}

TEST_F(ReservationHandlerTest, cancel_reservation) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);

    EXPECT_EQ(r.cancel_reservation(5, false, types::reservation::ReservationEndReason::Cancelled), std::nullopt);

    EXPECT_EQ(r.cancel_reservation(1, false, types::reservation::ReservationEndReason::Cancelled), std::nullopt);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.cancel_reservation(3, false, types::reservation::ReservationEndReason::Cancelled), 1);
}

TEST_F(ReservationHandlerTest, overwrite_reservation) {
    ReservationHandler r;
    std::optional<uint32_t> evse_id;
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const types::reservation::ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    r.add_connector(5, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(5, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_CALL(reservation_callback_mock, Call(_, 0, types::reservation::ReservationEndReason::Cancelled))
        .WillOnce(SaveArg<0>(&evse_id));

    types::reservation::Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    EXPECT_EQ(r.make_reservation(5, reservation), types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(5, reservation), types::reservation::ReservationResult::Accepted);

    EXPECT_EQ(evse_id, 5);
}

TEST_F(ReservationHandlerTest, matches_reserved_identifier) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(2, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(2, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation.parent_id_token = "PARENT_TOKEN_0";
    types::reservation::Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation2.parent_id_token = "PARENT_TOKEN_2";
    types::reservation::Reservation reservation3 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation3.parent_id_token = "PARENT_TOKEN_3";
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(2, reservation3), types::reservation::ReservationResult::Accepted);

    // Id token is correct and evse id as well.
    EXPECT_EQ(r.matches_reserved_identifier(std::nullopt, reservation.id_token, std::nullopt), 0);
    // Id token is correct and evse id as well, parent token is not but that is ignored since the normal token is ok.
    EXPECT_EQ(r.matches_reserved_identifier(std::nullopt, reservation.id_token, "WRONG_PARENT_TOKEN"), 0);
    // Token is wrong.
    EXPECT_EQ(r.matches_reserved_identifier(std::nullopt, "WRONG_TOKEN", std::nullopt), std::nullopt);
    // Evse id is wrong, but id token is in a global reservation.
    EXPECT_EQ(r.matches_reserved_identifier(1, reservation.id_token, std::nullopt), 0);
    // Evse id is wrong.
    EXPECT_EQ(r.matches_reserved_identifier(2, reservation2.id_token, std::nullopt), std::nullopt);
    // Token is wrong but parent token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(std::nullopt, "WRONG_TOKEN", "PARENT_TOKEN_0"), 0);
    // Token is wrong and parent token as well.
    EXPECT_EQ(r.matches_reserved_identifier(std::nullopt, "WRONG_TOKEN", "WRONG_PARENT_TOKEN"), std::nullopt);
    // Evse id is correct and token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(1, reservation2.id_token, std::nullopt), 1);
    // Evse id is correct but token is wrong.
    EXPECT_EQ(r.matches_reserved_identifier(1, "TOKEN_NOK", std::nullopt), std::nullopt);
    // Evse id is wrong and token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(2, reservation2.id_token, std::nullopt), std::nullopt);
    // Evse id is correct, token is wrong but parent token is correct.
    EXPECT_EQ(r.matches_reserved_identifier(1, "TOKEN_NOK", "PARENT_TOKEN_2"), 1);
    // Evse id is correct, token is wrong and parent token as well.
    EXPECT_EQ(r.matches_reserved_identifier(1, "TOKEN_NOK", "PARENT_TOKEN_NOK"), std::nullopt);
}

TEST_F(ReservationHandlerTest, has_reservation_parent_id) {
    ReservationHandler r;
    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    types::reservation::Reservation reservation = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation.parent_id_token = "PARENT_TOKEN_0";
    types::reservation::Reservation reservation2 = create_reservation(types::evse_manager::ConnectorTypeEnum::cType2);
    reservation2.parent_id_token = "PARENT_TOKEN_2";
    EXPECT_EQ(r.make_reservation(std::nullopt, reservation), types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(1, reservation2), types::reservation::ReservationResult::Accepted);

    // Id token is correct and evse id as well.
    EXPECT_TRUE(r.has_reservation_parent_id(std::nullopt));
    EXPECT_TRUE(r.has_reservation_parent_id(1));
    EXPECT_FALSE(r.has_reservation_parent_id(0));
    EXPECT_FALSE(r.has_reservation_parent_id(2));
}

TEST_F(ReservationHandlerTest, on_reservation_used) {
    ReservationHandler r;

    // Register a callback, which should not be called.
    MockFunction<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                      const types::reservation::ReservationEndReason reason)>
        reservation_callback_mock;

    r.register_reservation_cancelled_callback(reservation_callback_mock.AsStdFunction());

    EXPECT_CALL(reservation_callback_mock, Call(_, _, _)).Times(0);

    r.add_connector(0, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(0, 1, types::evse_manager::ConnectorTypeEnum::cType2);
    r.add_connector(1, 0, types::evse_manager::ConnectorTypeEnum::cCCS2);
    r.add_connector(1, 1, types::evse_manager::ConnectorTypeEnum::cType2);

    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);

    r.on_reservation_used(1);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);
    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cType2)),
              types::reservation::ReservationResult::Occupied);

    r.on_reservation_used(0);
    r.on_reservation_used(2);
    r.on_reservation_used(3);

    EXPECT_EQ(r.make_reservation(1, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
    EXPECT_EQ(r.make_reservation(std::nullopt, create_reservation(types::evse_manager::ConnectorTypeEnum::cCCS2)),
              types::reservation::ReservationResult::Accepted);
}

// TODO mz test with cars arriving, removing reservation and parent tokens

} // namespace module
