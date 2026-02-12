// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <everest/util/vector/fixed_vector.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace everest::lib::util;

TEST(FixedVectorTest, BasicInt) {
    fixed_vector<int, 10> vec;
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
    EXPECT_EQ(vec.at(1), 2);

    int count = 1;
    for (const auto& val : vec) {
        EXPECT_EQ(val, count++);
    }

    vec.clear();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);
}

TEST(FixedVectorTest, StringAndMove) {
    fixed_vector<std::string, 5> vec;
    vec.push_back("hello");
    std::string s = "world";
    vec.push_back(std::move(s));

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], "hello");
    EXPECT_EQ(vec[1], "world");
    // Note: The state of a moved-from string is valid but unspecified.
    // In many implementations it is empty, but we shouldn't rely on it.
    // EXPECT_TRUE(s.empty());

    vec.emplace_back(5, 'c');
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[2], "ccccc");
}

TEST(FixedVectorTest, Capacity) {
    fixed_vector<int, 3> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_THROW(vec.push_back(4), std::length_error);
    EXPECT_THROW(vec.emplace_back(5), std::length_error);
}

TEST(FixedVectorTest, AtThrows) {
    fixed_vector<int, 5> vec;
    vec.push_back(1);
    EXPECT_THROW(vec.at(1), std::out_of_range);
    const auto& cvec = vec;
    EXPECT_THROW(cvec.at(1), std::out_of_range);
}

TEST(FixedVectorTest, Erase) {
    fixed_vector<int, 10> vec;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    // erase first
    auto it = vec.erase(vec.begin());
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(vec.size(), 9);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[8], 9);

    // erase last
    it = vec.erase(vec.end() - 1);
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec.size(), 8);
    EXPECT_EQ(vec[7], 8);

    // erase middle
    it = vec.erase(vec.begin() + 3); // erase '4' from {1,2,3,4,5,6,7,8}
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(vec.size(), 7);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
    EXPECT_EQ(vec[3], 5);
}

TEST(FixedVectorTest, EraseRange) {
    fixed_vector<int, 10> vec;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    // erase range in the middle
    auto it = vec.erase(vec.begin() + 2, vec.begin() + 5); // erase 2, 3, 4
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(vec.size(), 7);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 1);
    EXPECT_EQ(vec[2], 5);
    EXPECT_EQ(vec[3], 6);
    EXPECT_EQ(vec[6], 9);
}

TEST(FixedVectorTest, MoveOnlyType) {
    fixed_vector<std::unique_ptr<int>, 5> vec;
    vec.emplace_back(std::make_unique<int>(1));
    vec.push_back(std::make_unique<int>(2));
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(*vec[0], 1);
    EXPECT_EQ(*vec[1], 2);

    vec.erase(vec.begin());
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(*vec[0], 2);
}

struct DestructorCheck {
    static int destructor_calls;
    DestructorCheck() = default;
    ~DestructorCheck() {
        destructor_calls++;
    }
};
int DestructorCheck::destructor_calls = 0;

TEST(FixedVectorTest, DestructorCalledOnClear) {
    DestructorCheck::destructor_calls = 0;
    fixed_vector<DestructorCheck, 5> vec;
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 0);
    vec.clear();
    EXPECT_EQ(DestructorCheck::destructor_calls, 3);
    vec.clear();
    EXPECT_EQ(DestructorCheck::destructor_calls, 3);
}

TEST(FixedVectorTest, DestructorCalledOnErase) {
    DestructorCheck::destructor_calls = 0;
    fixed_vector<DestructorCheck, 5> vec;
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 0);
    vec.erase(vec.begin());
    EXPECT_EQ(DestructorCheck::destructor_calls, 1);
    vec.erase(vec.begin(), vec.end());
    EXPECT_EQ(DestructorCheck::destructor_calls, 3);
}

TEST(FixedVectorTest, DestructorCalledOnDestruction) {
    DestructorCheck::destructor_calls = 0;
    {
        fixed_vector<DestructorCheck, 5> vec;
        vec.emplace_back();
        vec.emplace_back();
        EXPECT_EQ(DestructorCheck::destructor_calls, 0);
    }
    EXPECT_EQ(DestructorCheck::destructor_calls, 2);
}

TEST(FixedVectorTest, CopyConstructor) {
    fixed_vector<int, 5> original;
    original.push_back(1);
    original.push_back(2);

    fixed_vector<int, 5> copy = original;
    EXPECT_EQ(copy.size(), 2);
    EXPECT_EQ(copy[0], 1);
    EXPECT_EQ(copy[1], 2);

    // Ensure original is untouched
    EXPECT_EQ(original.size(), 2);
    EXPECT_EQ(original[0], 1);
}

TEST(FixedVectorTest, CopyAssignment) {
    fixed_vector<int, 5> original;
    original.push_back(1);
    original.push_back(2);

    fixed_vector<int, 5> copy;
    copy.push_back(99);
    copy = original;

    EXPECT_EQ(copy.size(), 2);
    EXPECT_EQ(copy[0], 1);
    EXPECT_EQ(copy[1], 2);

    // Ensure original is untouched
    EXPECT_EQ(original.size(), 2);
}

TEST(FixedVectorTest, CopyAssignmentEdgeCases) {
    // Case 1: Destination is larger than source
    fixed_vector<DestructorCheck, 5> dest1;
    dest1.emplace_back();
    dest1.emplace_back();
    dest1.emplace_back();

    fixed_vector<DestructorCheck, 5> source1;
    source1.emplace_back();
    source1.emplace_back();

    DestructorCheck::destructor_calls = 0;
    dest1 = source1;
    EXPECT_EQ(dest1.size(), 2);
    EXPECT_EQ(DestructorCheck::destructor_calls, 1); // One surplus element should have been destroyed

    // Case 2: Destination is smaller than source
    fixed_vector<int, 5> dest2;
    dest2.push_back(1);

    fixed_vector<int, 5> source2;
    source2.push_back(10);
    source2.push_back(20);

    dest2 = source2;
    EXPECT_EQ(dest2.size(), 2);
    EXPECT_EQ(dest2[0], 10);
    EXPECT_EQ(dest2[1], 20);

    // Case 3: Sizes are equal
    fixed_vector<int, 5> dest3;
    dest3.push_back(1);
    dest3.push_back(2);

    fixed_vector<int, 5> source3;
    source3.push_back(10);
    source3.push_back(20);

    dest3 = source3;
    EXPECT_EQ(dest3.size(), 2);
    EXPECT_EQ(dest3[0], 10);
    EXPECT_EQ(dest3[1], 20);

    // Case 4: Self-assignment
    fixed_vector<int, 5> self_assign;
    self_assign.push_back(123);
    self_assign = self_assign;
    EXPECT_EQ(self_assign.size(), 1);
    EXPECT_EQ(self_assign[0], 123);
}

TEST(FixedVectorTest, MoveConstructor) {
    fixed_vector<std::string, 5> original;
    original.push_back("a");
    original.push_back("b");

    fixed_vector<std::string, 5> moved = std::move(original);
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], "a");

    EXPECT_TRUE(original.empty());
}

TEST(FixedVectorTest, MoveAssignment) {
    fixed_vector<std::string, 5> original;
    original.push_back("a");
    original.push_back("b");

    fixed_vector<std::string, 5> moved;
    moved.push_back("c");
    moved = std::move(original);

    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], "a");

    EXPECT_TRUE(original.empty());
}

TEST(FixedVectorTest, ZeroCapacity) {
    fixed_vector<int, 0> vec;
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 0);
    EXPECT_THROW(vec.push_back(1), std::length_error);
}

TEST(FixedVectorTest, FrontAndBack) {
    fixed_vector<int, 3> vec;
    vec.push_back(10);
    vec.push_back(20);
    EXPECT_EQ(vec.front(), 10);
    EXPECT_EQ(vec.back(), 20);
    vec.front() = 11;
    EXPECT_EQ(vec[0], 11);

    const auto& cvec = vec;
    EXPECT_EQ(cvec.front(), 11);
    EXPECT_EQ(cvec.back(), 20);
}

TEST(FixedVectorTest, ReverseIteration) {
    fixed_vector<int, 3> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    int expected = 3;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        EXPECT_EQ(*it, expected--);
    }

    const auto& cvec = vec;
    expected = 3;
    for (auto it = cvec.rbegin(); it != cvec.rend(); ++it) {
        EXPECT_EQ(*it, expected--);
    }
}

TEST(FixedVectorTest, EraseEdgeCases) {
    fixed_vector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    // Erase empty range
    auto it = vec.erase(vec.begin() + 1, vec.begin() + 1);
    EXPECT_EQ(vec.size(), 4);
    EXPECT_EQ(*it, 2);

    // Erase to the end
    it = vec.erase(vec.begin() + 2, vec.end());
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);

    // Erase everything
    it = vec.erase(vec.begin(), vec.end());
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(it, vec.end());
}

struct ThrowsOnCopy {
    ThrowsOnCopy() = default;
    ThrowsOnCopy(const ThrowsOnCopy& /*other*/) {
        if (should_throw) {
            throw std::runtime_error("Throwing on copy");
        }
    }
    static bool should_throw;
};
bool ThrowsOnCopy::should_throw = false;

TEST(FixedVectorTest, ExceptionSafety) {
    fixed_vector<ThrowsOnCopy, 5> vec;
    vec.emplace_back();
    vec.emplace_back();
    EXPECT_EQ(vec.size(), 2);

    ThrowsOnCopy::should_throw = true;
    try {
        vec.push_back(ThrowsOnCopy{});
    } catch (const std::runtime_error&) {
        // exception caught
    }

    EXPECT_EQ(vec.size(), 2); // Strong exception guarantee: state is unchanged
    ThrowsOnCopy::should_throw = false;
}

TEST(FixedVectorTest, TryEmplaceBack) {
    fixed_vector<int, 3> vec;
    auto* elem1 = vec.try_emplace_back(10);
    ASSERT_NE(elem1, nullptr);
    EXPECT_EQ(*elem1, 10);
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0], 10);

    vec.try_emplace_back(20);
    vec.try_emplace_back(30);

    EXPECT_EQ(vec.size(), 3);

    // Vector is full
    auto* elem4 = vec.try_emplace_back(40);
    EXPECT_EQ(elem4, nullptr);
    EXPECT_EQ(vec.size(), 3);
}

struct ThrowsOnMoveConstruct {
    ThrowsOnMoveConstruct() = default;
    ThrowsOnMoveConstruct(ThrowsOnMoveConstruct&&) {
        if (should_throw) {
            throw std::runtime_error("Throwing on move construction");
        }
    }
    static bool should_throw;
};
bool ThrowsOnMoveConstruct::should_throw = false;

TEST(FixedVectorTest, TryEmplaceBackException) {
    fixed_vector<ThrowsOnMoveConstruct, 5> vec;
    vec.emplace_back(); // ensure there's at least one element for size check
    EXPECT_EQ(vec.size(), 1);

    ThrowsOnMoveConstruct::should_throw = true;
    auto val = ThrowsOnMoveConstruct{}; // create an rvalue that will be moved
    auto* elem = vec.try_emplace_back(std::move(val));
    EXPECT_EQ(elem, nullptr);
    EXPECT_EQ(vec.size(), 1); // should not have changed
    ThrowsOnMoveConstruct::should_throw = false;
}

struct ThrowsOnDefaultConstruct {
    ThrowsOnDefaultConstruct() {
        if (should_throw) {
            throw std::runtime_error("Throwing on default construction");
        }
    }
    static bool should_throw;
};
bool ThrowsOnDefaultConstruct::should_throw = false;

TEST(FixedVectorTest, TryEmplaceBackExceptionDefaultConstruct) {
    fixed_vector<ThrowsOnDefaultConstruct, 5> vec;
    vec.emplace_back(); // ensure there's at least one element for size check
    EXPECT_EQ(vec.size(), 1);

    ThrowsOnDefaultConstruct::should_throw = true;
    auto* elem = vec.try_emplace_back(); // default construct
    EXPECT_EQ(elem, nullptr);
    EXPECT_EQ(vec.size(), 1); // should not have changed
    ThrowsOnDefaultConstruct::should_throw = false;
}

TEST(FixedVectorTest, PopBack) {
    fixed_vector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.back(), 3);

    vec.pop_back();
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.back(), 2);

    vec.pop_back();
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.back(), 1);

    vec.pop_back();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);

    // Popping from an empty vector should be a no-op
    vec.pop_back();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);

    // Test with DestructorCheck
    DestructorCheck::destructor_calls = 0;
    fixed_vector<DestructorCheck, 5> vec_dc;
    vec_dc.emplace_back();
    vec_dc.emplace_back();
    vec_dc.emplace_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 0);

    vec_dc.pop_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 1);
    vec_dc.pop_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 2);
    vec_dc.pop_back();
    EXPECT_EQ(DestructorCheck::destructor_calls, 3);
    EXPECT_TRUE(vec_dc.empty());
}

TEST(FixedVectorTest, CapacityAndMaxSize) {
    fixed_vector<int, 5> vec;
    EXPECT_EQ(vec.capacity(), 5);
    EXPECT_EQ(vec.max_size(), 5);

    const fixed_vector<int, 0> zero_vec;
    EXPECT_EQ(zero_vec.capacity(), 0);
    EXPECT_EQ(zero_vec.max_size(), 0);
}

TEST(FixedVectorTest, ConstIteratorMethods) {
    fixed_vector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    const auto& cvec = vec;

    // Test cbegin() and cend()
    int expected_val = 1;
    for (auto it = cvec.cbegin(); it != cvec.cend(); ++it) {
        EXPECT_EQ(*it, expected_val++);
    }
    EXPECT_EQ(expected_val, 4); // Should have iterated 1, 2, 3

    // Test crbegin() and crend()
    expected_val = 3;
    for (auto it = cvec.crbegin(); it != cvec.crend(); ++it) {
        EXPECT_EQ(*it, expected_val--);
    }
    EXPECT_EQ(expected_val, 0); // Should have iterated 3, 2, 1

    // Test on empty vector
    fixed_vector<int, 5> empty_vec;
    const auto& cempty_vec = empty_vec;
    EXPECT_EQ(cempty_vec.cbegin(), cempty_vec.cend());
    EXPECT_EQ(cempty_vec.crbegin(), cempty_vec.crend());
}

TEST(FixedVectorTest, EraseInvalidRange) {
    fixed_vector<int, 5> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    // Test with first > last, which should do nothing
    auto original_size = vec.size();
    auto it = vec.erase(vec.begin() + 2, vec.begin() + 1);

    // Verify nothing happened
    EXPECT_EQ(vec.size(), original_size);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
    EXPECT_EQ(vec[3], 4);

    // The returned iterator should be the 'first' iterator passed in
    EXPECT_EQ(it, vec.begin() + 2);
    EXPECT_EQ(*it, 3);
}

TEST(FixedVectorTest, ComparisonOperators) {
    fixed_vector<int, 5> vec1;
    vec1.push_back(1);
    vec1.push_back(2);

    fixed_vector<int, 5> vec2;
    vec2.push_back(1);
    vec2.push_back(2);

    fixed_vector<int, 5> vec3;
    vec3.push_back(1);
    vec3.push_back(99);

    fixed_vector<int, 5> vec4;
    vec4.push_back(1);

    fixed_vector<int, 5> empty1;
    fixed_vector<int, 5> empty2;

    EXPECT_TRUE(vec1 == vec2);
    EXPECT_FALSE(vec1 != vec2);

    EXPECT_FALSE(vec1 == vec3);
    EXPECT_TRUE(vec1 != vec3);

    EXPECT_FALSE(vec1 == vec4);
    EXPECT_TRUE(vec1 != vec4);

    EXPECT_TRUE(empty1 == empty2);
    EXPECT_FALSE(empty1 != empty2);

    EXPECT_FALSE(vec1 == empty1);
    EXPECT_TRUE(vec1 != empty1);
}

struct ThrowsAt_Nth_Move {
    ThrowsAt_Nth_Move() = default;
    ThrowsAt_Nth_Move(const ThrowsAt_Nth_Move&) = default;
    ThrowsAt_Nth_Move& operator=(const ThrowsAt_Nth_Move&) = default;
    ThrowsAt_Nth_Move(const ThrowsAt_Nth_Move&&) {
        move_countdown -= 1;
        if (move_countdown == -1) {
            throw std::runtime_error("Throwing on move construction");
        }
    }
    ThrowsAt_Nth_Move& operator=(const ThrowsAt_Nth_Move&&) {
        move_countdown -= 1;
        if (move_countdown == -1) {
            throw std::runtime_error("Throwing on move construction");
        }
        return *this;
    }
    static int move_countdown;
};
int ThrowsAt_Nth_Move::move_countdown = -1;

TEST(FixedVectorTest, FailToMoveConstruct) {
    ThrowsAt_Nth_Move::move_countdown = 2;
    fixed_vector<ThrowsAt_Nth_Move, 5> vec;
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();

    auto moved_to = fixed_vector<ThrowsAt_Nth_Move, 5>(std::move(vec));

    EXPECT_EQ(moved_to.size(), 0);
    EXPECT_EQ(vec.size(), 4);
}

TEST(FixedVectorTest, FailToMoveAssign) {
    ThrowsAt_Nth_Move::move_countdown = 2;
    fixed_vector<ThrowsAt_Nth_Move, 5> vec;
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();
    vec.emplace_back();

    fixed_vector<ThrowsAt_Nth_Move, 5> moved_to;

    EXPECT_EQ(moved_to.size(), 0);

    moved_to = std::move(vec);

    EXPECT_EQ(vec.size(), 4);
}

TEST(FixedVectorTest, InitializerListConstructor) {
    // Basic construction
    fixed_vector<int, 5> vec1 = {1, 2, 3};
    EXPECT_EQ(vec1.size(), 3);
    EXPECT_EQ(vec1[0], 1);
    EXPECT_EQ(vec1[1], 2);
    EXPECT_EQ(vec1[2], 3);

    // Empty list
    fixed_vector<int, 5> vec2 = {};
    EXPECT_TRUE(vec2.empty());
    EXPECT_EQ(vec2.size(), 0);

    // Full capacity
    fixed_vector<int, 3> vec3 = {10, 20, 30};
    EXPECT_EQ(vec3.size(), 3);
    EXPECT_EQ(vec3[2], 30);

    // Exceeding capacity - use a lambda to avoid comma issues with the macro
    EXPECT_THROW(([] { fixed_vector<int, 2> vec4 = {1, 2, 3}; }()), std::length_error);

    // With strings
    fixed_vector<std::string, 4> vec5 = {"hello", "world"};
    EXPECT_EQ(vec5.size(), 2);
    EXPECT_EQ(vec5[0], "hello");
    EXPECT_EQ(vec5[1], "world");
}

struct ThrowsOnNthCopy {
    ThrowsOnNthCopy() = default;
    ThrowsOnNthCopy(const ThrowsOnNthCopy& /*other*/) {
        if (copy_countdown > 0) {
            copy_countdown--;
            if (copy_countdown == 0) {
                throw std::runtime_error("Throwing on nth copy");
            }
        }
    }
    static int copy_countdown;
};
int ThrowsOnNthCopy::copy_countdown = -1;

TEST(FixedVectorTest, InitializerListStrongException) {
    ThrowsOnNthCopy::copy_countdown = 2;

    fixed_vector<ThrowsOnNthCopy, 5>* vec_ptr = nullptr;
    try {
        vec_ptr = new fixed_vector<ThrowsOnNthCopy, 5>({ThrowsOnNthCopy{}, ThrowsOnNthCopy{}, ThrowsOnNthCopy{}});
        // This line should not be reached.
        FAIL() << "Exception was not thrown";
    } catch (const std::runtime_error& e) {
        // Exception caught as expected.
        // new should not have returned, so vec_ptr should still be null.
        EXPECT_EQ(vec_ptr, nullptr);
    } catch (...) {
        FAIL() << "Caught unexpected exception type";
    }

    // Ensure the pointer was not assigned.
    EXPECT_EQ(vec_ptr, nullptr);
    if (vec_ptr) {
        delete vec_ptr;
    }
}

TEST(FixedVectorTest, StdVectorConstructor) {
    // Test case 1: Construct from an empty std::vector
    std::vector<int> empty_std_vec = {};
    fixed_vector<int, 5> vec_from_empty(empty_std_vec);
    EXPECT_TRUE(vec_from_empty.empty());
    EXPECT_EQ(vec_from_empty.size(), 0);

    // Test case 2: Construct from a std::vector with elements (within capacity)
    std::vector<int> small_std_vec = {1, 2, 3};
    fixed_vector<int, 5> vec_from_small(small_std_vec);
    EXPECT_EQ(vec_from_small.size(), 3);
    EXPECT_EQ(vec_from_small[0], 1);
    EXPECT_EQ(vec_from_small[1], 2);
    EXPECT_EQ(vec_from_small[2], 3);

    // Test case 3: Construct from a std::vector with elements (exactly capacity)
    std::vector<int> full_std_vec = {10, 20, 30, 40, 50};
    fixed_vector<int, 5> vec_from_full(full_std_vec);
    EXPECT_EQ(vec_from_full.size(), 5);
    EXPECT_EQ(vec_from_full[0], 10);
    EXPECT_EQ(vec_from_full[4], 50);

    // Test case 4: Construct from a std::vector with elements exceeding capacity
    std::vector<int> large_std_vec = {1, 2, 3, 4, 5, 6};
    EXPECT_THROW(([large_std_vec] { fixed_vector<int, 5> vec_from_large(large_std_vec); }()), std::length_error);

    // Test case 5: Construct with std::string elements
    std::vector<std::string> string_std_vec = {"apple", "banana"};
    fixed_vector<std::string, 3> vec_from_strings(string_std_vec);
    EXPECT_EQ(vec_from_strings.size(), 2);
    EXPECT_EQ(vec_from_strings[0], "apple");
    EXPECT_EQ(vec_from_strings[1], "banana");
}
