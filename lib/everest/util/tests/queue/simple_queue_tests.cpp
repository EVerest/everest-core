// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <everest/util/queue/simple_queue.hpp>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

using namespace everest::lib::util;

// =================================================================
// 2. Test Fixture Setup
// =================================================================

template <typename T> class SimpleQueueTest : public ::testing::Test {
protected:
    simple_queue<T> queue;
};

// Typed Test Suite for standard types
using QueueTypes = ::testing::Types<int, std::string>;
TYPED_TEST_SUITE(SimpleQueueTest, QueueTypes);

// =================================================================
// A. Basic Functionality Tests (FIFO & Empty Checks)
// =================================================================

TYPED_TEST(SimpleQueueTest, InitialStateIsEmpty) {
    ASSERT_TRUE(this->queue.empty());
    ASSERT_EQ(this->queue.size(), 0);
    ASSERT_FALSE(this->queue.pop().has_value());
}

TYPED_TEST(SimpleQueueTest, PushAndEmptyCheck) {
    TypeParam value;

    // Use if constexpr to initialize value correctly
    if constexpr (std::is_same_v<TypeParam, int>) {
        value = 10;
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        value = "Test_10";
    } else {
        return;
    }

    this->queue.push(value);

    ASSERT_FALSE(this->queue.empty());
    ASSERT_EQ(this->queue.size(), 1);
}

TYPED_TEST(SimpleQueueTest, PushPopAndEmptyCheck) {
    TypeParam expected_value;

    // Use if constexpr to initialize value correctly
    if constexpr (std::is_same_v<TypeParam, int>) {
        expected_value = 42;
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        expected_value = "Test_42";
    } else {
        return;
    }

    this->queue.push(expected_value);

    std::optional<TypeParam> result = this->queue.pop();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), expected_value);
    ASSERT_TRUE(this->queue.empty());
    ASSERT_EQ(this->queue.size(), 0);
}

TYPED_TEST(SimpleQueueTest, MultiplePushAndPopOrder) {
    const int count = 3;

    // Push elements (0, 1, 2)
    for (int i = 0; i < count; ++i) {
        if constexpr (std::is_same_v<TypeParam, int>) {
            this->queue.push(i);
        } else {
            this->queue.push(std::to_string(i));
        }
    }

    // Pop elements and verify FIFO order (0, 1, 2)
    for (int i = 0; i < count; ++i) {
        std::optional<TypeParam> result = this->queue.pop();
        TypeParam expected_value;
        if constexpr (std::is_same_v<TypeParam, int>) {
            expected_value = i;
        } else {
            expected_value = std::to_string(i);
        }

        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), expected_value) << "Element popped out of FIFO order.";
    }

    ASSERT_TRUE(this->queue.empty());
}

// =================================================================
// B. Reference Tests (front() and back())
// =================================================================

TYPED_TEST(SimpleQueueTest, FrontAndBackReferences) {
    TypeParam val1, val2;

    if constexpr (std::is_same_v<TypeParam, int>) {
        val1 = 100;
        val2 = 200;
    } else if constexpr (std::is_same_v<TypeParam, std::string>) {
        val1 = "Front";
        val2 = "Back";
    } else {
        return;
    }

    this->queue.push(val1);
    this->queue.push(val2);

    // Verify front()
    ASSERT_EQ(this->queue.front(), val1);

    // Verify back()
    ASSERT_EQ(this->queue.back(), val2);

    // After pop, front should change
    this->queue.pop();
    ASSERT_EQ(this->queue.front(), val2);
    ASSERT_EQ(this->queue.back(), val2);
}

// =================================================================
// C. Move-Only Type Compatibility Test (Verifying the pop() fix)
// =================================================================

// Test suite for std::unique_ptr<int> (a move-only type)
class SimpleQueueMoveOnlyTest : public ::testing::Test {
protected:
    simple_queue<std::unique_ptr<int>> queue;
};

TEST_F(SimpleQueueMoveOnlyTest, PushAndPopMoveOnlyType) {
    const int value1 = 10;
    const int value2 = 20;

    // Push: Requires the r-value push overload
    this->queue.push(std::make_unique<int>(value1));
    this->queue.push(std::make_unique<int>(value2));

    ASSERT_EQ(this->queue.size(), 2);

    // Pop: Requires the fixed move-based pop()
    std::optional<std::unique_ptr<int>> opt_result1 = this->queue.pop();

    // Verify the value was retrieved
    ASSERT_TRUE(opt_result1.has_value());
    ASSERT_NE(opt_result1.value(), nullptr);
    ASSERT_EQ(*opt_result1.value(), value1);

    // Pop the second item
    std::optional<std::unique_ptr<int>> opt_result2 = this->queue.pop();
    ASSERT_TRUE(opt_result2.has_value());
    ASSERT_EQ(*opt_result2.value(), value2);

    ASSERT_TRUE(this->queue.empty());
}
