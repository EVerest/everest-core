// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM_BUFFER_HPP
#define LIBFSM_BUFFER_HPP

#include <cstddef>
#include <cstdint>

namespace fsm::buffer {
template <size_t MaxSimpleStateSize, size_t MaxCompoundStateSize, size_t MaxNestingLevel> struct SwapBuffer {
    static const size_t MAX_SIMPLE_STATE_SIZE = MaxSimpleStateSize;
    static const size_t MAX_COMPOUND_STATE_SIZE = MaxCompoundStateSize;
    static const size_t MAX_NESTING_LEVEL = MaxNestingLevel;

    struct SimpleStateBuffer {
        alignas(std::max_align_t) uint8_t a[MaxSimpleStateSize];
        alignas(std::max_align_t) uint8_t b[MaxSimpleStateSize];
        bool a_is_next{true};
    };

    SimpleStateBuffer simple{};

    struct CompoundStateBuffer {
        alignas(std::max_align_t) uint8_t a[MaxCompoundStateSize];
        alignas(std::max_align_t) uint8_t b[MaxCompoundStateSize];
        bool a_is_next{true};
    };

    CompoundStateBuffer compound[MAX_NESTING_LEVEL]{};
};

} // namespace fsm::buffer

#endif // LIBFSM_BUFFER_HPP
