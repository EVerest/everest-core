// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp_generic/ocppImpl.hpp>

namespace {
using namespace module::ocpp_generic;

TEST(MonitorList, insert) {
    // check that duplicates are avoided
    ocppImpl::MonitorList list;

    ocppImpl::MonitorListEntry entryA{{"Component"}, {"Variable"}};
    ocppImpl::MonitorListEntry entryB{{"Component"}, {"Variable", "Instance"}};

    auto result = list.insert(entryA);
    EXPECT_TRUE(std::get<bool>(result));
    result = list.insert(entryB);
    EXPECT_TRUE(std::get<bool>(result));
    ASSERT_EQ(list.size(), 2);

    // attempt to add same object again
    result = list.insert(entryB);
    EXPECT_FALSE(std::get<bool>(result));
    ASSERT_EQ(list.size(), 2);

    // attempt to add new object with same values
    ocppImpl::MonitorListEntry entryC{{"Component"}, {"Variable"}};
    result = list.insert(entryC);
    EXPECT_FALSE(std::get<bool>(result));
    ASSERT_EQ(list.size(), 2);
}

} // namespace
