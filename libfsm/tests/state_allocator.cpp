// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <fsm/_impl/state_allocator.hpp>
#include <fsm/buffer.hpp>

SCENARIO("Testing buffer allocator") {
    GIVEN("A state allocator with a defined swap buffer") {
        using BufferType = fsm::buffer::SwapBuffer<64, 64, 3>;
        using StateAllocatorType = fsm::_impl::StateAllocator<BufferType>;

        BufferType buffer;

        REQUIRE(buffer.MAX_SIMPLE_STATE_SIZE == 64);

        StateAllocatorType state_allocator{buffer};

        WHEN("The state_allocator is vanilla") {
            THEN("It shouldn not have any staged states") {
                REQUIRE(state_allocator.has_staged_states() == false);

                auto simple_object = state_allocator.pull_simple_state<double>();
                REQUIRE(simple_object == nullptr);

                auto compound_object = state_allocator.pull_compound_state<int>();
                REQUIRE(compound_object == nullptr);
            }
        }

        WHEN("Made ready for allocation and added a simple and compound object") {
            state_allocator.make_ready_for_nesting_level(0);
            const int SOME_INT_CONST = 42;
            const double SOME_DOUBLE_CONST = 3.14;
            auto compound_allocation_result = state_allocator.create_compound<int>(SOME_INT_CONST);
            auto simple_allocation_result = state_allocator.create_simple<double>(SOME_DOUBLE_CONST);

            REQUIRE(compound_allocation_result == true);
            REQUIRE(simple_allocation_result == true);

            THEN("The allocator should have staged states") {
                REQUIRE(state_allocator.has_staged_states() == true);
            }

            THEN("The created objects should yield the correct values") {
                auto simple_object = state_allocator.pull_simple_state<double>();
                REQUIRE(state_allocator.has_staged_states() == true);

                auto compound_object = state_allocator.pull_compound_state<int>();
                REQUIRE(state_allocator.has_staged_states() == false);

                REQUIRE(*simple_object == SOME_DOUBLE_CONST);
                REQUIRE(*compound_object == SOME_INT_CONST);
            }

            THEN("Creating a simple object again should fail") {
                simple_allocation_result = state_allocator.create_simple<double>(SOME_DOUBLE_CONST);
                REQUIRE(simple_allocation_result == false);
            }
        }
    }
}
