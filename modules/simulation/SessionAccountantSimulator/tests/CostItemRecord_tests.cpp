/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "SingleEvseAccountant/CostItemRecord.hpp"

#include <chrono>
#include <ostream>

#include <gtest/gtest.h>

TEST(CostItemRecord, constructor) {
    SessionAccountant::CostItemRecord item_record{};

    std::ostringstream buffer{};

    for (auto item : *(item_record.get_items())) {
        buffer << item;
    }

    std::string str_repr = buffer.str();
    std::string str_repr_expected("");

    EXPECT_TRUE(item_record.get_items()->empty());
    EXPECT_EQ(str_repr, str_repr_expected);
}

TEST(CostItemRecord, addIdleItem) {
    SessionAccountant::CostItemRecord item_record{};

    SessionAccountant::tstamp_t tpoint{};
    {
        using namespace std::chrono;
        auto ymd = date::sys_days{date::January / 2 / 2025} + 12h + 0min + 0s;
        tpoint = date::clock_cast<date::utc_clock>(ymd);
    }

    item_record.add_idle_item_as_first_item(tpoint);

    std::ostringstream buffer{};

    for (auto item : *(item_record.get_items())) {
        buffer << item;
    }

    std::string str_repr = buffer.str();
    std::string str_repr_expected(
        "2025-01-02 12:00:00.000000000 -> 2025-01-02 12:00:00.000000000: 0 Wh -> 0 Wh; 'IdleTime'");

    auto items = item_record.get_items();
    auto& first = items->at(0);

    EXPECT_EQ(items->size(), 1);
    EXPECT_EQ(first.time_point_to, first.time_point_from);
    EXPECT_EQ(first.category, SessionAccountant::ItemCategory::IdleTime);
    EXPECT_EQ(str_repr, str_repr_expected);
}

TEST(CostItemRecord, addIdleItemPlusTimeUpdate) {
    SessionAccountant::CostItemRecord item_record{};

    SessionAccountant::tstamp_t tpoint_1{};
    SessionAccountant::tstamp_t tpoint_2{};
    {
        using namespace std::chrono;
        auto ymd_1 = date::sys_days{date::January / 2 / 2025} + 12h + 0min + 0s;
        tpoint_1 = date::clock_cast<date::utc_clock>(ymd_1);
        auto ymd_2 = date::sys_days{date::January / 2 / 2025} + 12h + 15min + 0s;
        tpoint_2 = date::clock_cast<date::utc_clock>(ymd_2);
    }

    item_record.add_idle_item_as_first_item(tpoint_1);
    item_record.update_idle_cost_item(tpoint_2);

    std::ostringstream buffer{};

    for (auto item : *(item_record.get_items())) {
        buffer << item;
    }

    std::string str_repr = buffer.str();
    std::string str_repr_expected(
        "2025-01-02 12:00:00.000000000 -> 2025-01-02 12:15:00.000000000: 0 Wh -> 0 Wh; 'IdleTime'");

    auto items = item_record.get_items();
    auto& first = items->at(0);

    EXPECT_EQ(items->size(), 1);
    EXPECT_NE(first.time_point_to, first.time_point_from);
    EXPECT_EQ(first.category, SessionAccountant::ItemCategory::IdleTime);
    EXPECT_EQ(str_repr, str_repr_expected);
}
