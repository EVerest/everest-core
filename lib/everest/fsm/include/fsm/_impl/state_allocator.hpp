// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM__IMPL_STATE_ALLOCATOR_HPP
#define LIBFSM__IMPL_STATE_ALLOCATOR_HPP

namespace fsm::_impl {

template <typename SwapAllocatorBufferType> class StateAllocator;

class DynamicStateAllocator {
public:
    enum class InternalState {
        READY_FOR_CREATION,
        FAILED_MULTIPLE_SIMPLE_CREATE,
        FAILED_MULTIPLE_COMPOUND_CREATE,
    };

    DynamicStateAllocator() = default;
    DynamicStateAllocator(const DynamicStateAllocator& other) = delete;
    DynamicStateAllocator(DynamicStateAllocator&& other) = delete;
    DynamicStateAllocator& operator=(const DynamicStateAllocator& other) = delete;
    DynamicStateAllocator& operator=(DynamicStateAllocator&& other) = delete;

    template <typename StateType, typename... Args> bool create_compound(Args&&... args) {
        if (state != InternalState::READY_FOR_CREATION) {
            return false;
        }

        if (compound_state) {
            state = InternalState::FAILED_MULTIPLE_COMPOUND_CREATE;
            return false;
        };

        compound_state = new StateType(std::forward<Args>(args)...);

        return true;
    }

    template <typename StateType, typename... Args> bool create_simple(Args&&... args) {
        if (state != InternalState::READY_FOR_CREATION) {
            return false;
        }

        if (simple_state) {
            state = InternalState::FAILED_MULTIPLE_SIMPLE_CREATE;
            return false;
        }

        simple_state = new StateType(std::forward<Args>(args)...);
        return true;
    }

    template <typename SimpleStateType> auto pull_simple_state() {
        auto retval = reinterpret_cast<SimpleStateType*>(simple_state);
        if (retval) {
            simple_state = nullptr;
        }

        return retval;
    }

    template <typename CompoundStateType> auto pull_compound_state() {
        auto retval = reinterpret_cast<CompoundStateType*>(compound_state);
        if (retval) {
            compound_state = nullptr;
        }

        return retval;
    }

    void make_ready_for_allocation() {
        state = InternalState::READY_FOR_CREATION;
    }

    bool has_staged_states() {
        return (simple_state != nullptr) || (compound_state != nullptr);
    }

    template <typename SimpleStateType, typename CompoundStateType> void release_staged_states() {
        if (simple_state != nullptr) {
            reinterpret_cast<SimpleStateType*>(simple_state)->~SimpleStateType();
        }

        if (compound_state != nullptr) {
            reinterpret_cast<CompoundStateType*>(compound_state)->~CompoundStateType();
        }
    }

private:
    void* compound_state{nullptr};
    void* simple_state{nullptr};

    InternalState state{InternalState::READY_FOR_CREATION};
};

template <typename SwapBufferType> class StateAllocator {
public:
    enum class InternalState {
        READY_FOR_CREATION,
        FAILED_COMPOUND_OVERFLOW,
        FAILED_MULTIPLE_SIMPLE_CREATE,
        FAILED_MULTIPLE_COMPOUND_CREATE,
    };

    StateAllocator(SwapBufferType& buffer_) : buffer(buffer_){};
    StateAllocator(const StateAllocator& other) = delete;
    StateAllocator(StateAllocator&& other) = delete;
    StateAllocator& operator=(const StateAllocator& other) = delete;
    StateAllocator& operator=(StateAllocator&& other) = delete;

    template <typename StateType, typename... Args> bool create_compound(Args&&... args) {
        static_assert(sizeof(StateType) <= buffer.MAX_COMPOUND_STATE_SIZE,
                      "Buffer too small for the supplied compound state type");
        // TODO (aw): move this prolog into a helper function
        if (state != InternalState::READY_FOR_CREATION) {
            return false;
        }

        if (current_nested_level == SwapBufferType::MAX_NESTING_LEVEL) {
            state = InternalState::FAILED_COMPOUND_OVERFLOW;
            // FIXME (aw): overflow
            return false;
        }

        if (compound_state) {
            state = InternalState::FAILED_MULTIPLE_COMPOUND_CREATE;
            return false;
        };

        auto& next_compound = buffer.compound[current_nested_level];
        auto next_buffer = next_compound.a_is_next ? next_compound.a : next_compound.b;
        next_compound.a_is_next = !next_compound.a_is_next;

        compound_state = new (next_buffer) StateType(std::forward<Args>(args)...);

        return true;
    }

    template <typename StateType, typename... Args> bool create_simple(Args&&... args) {
        static_assert(sizeof(StateType) <= buffer.MAX_SIMPLE_STATE_SIZE,
                      "Buffer too small for the supplied simple state type");
        // TODO (aw): move this prolog into a helper function
        if (state != InternalState::READY_FOR_CREATION) {
            return false;
        }

        if (simple_state) {
            state = InternalState::FAILED_MULTIPLE_SIMPLE_CREATE;
            return false;
        }

        auto next_buffer = (buffer.simple.a_is_next) ? buffer.simple.a : buffer.simple.b;
        buffer.simple.a_is_next = !buffer.simple.a_is_next;

        simple_state = new (next_buffer) StateType(std::forward<Args>(args)...);
        return true;
    }

    template <typename SimpleStateType> auto pull_simple_state() {
        auto retval = reinterpret_cast<SimpleStateType*>(simple_state);
        if (retval) {
            simple_state = nullptr;
        }

        return retval;
    }

    template <typename CompoundStateType> auto pull_compound_state() {
        auto retval = reinterpret_cast<CompoundStateType*>(compound_state);
        if (retval) {
            compound_state = nullptr;
        }

        return retval;
    }

    void make_ready_for_nesting_level(size_t level) {
        state = InternalState::READY_FOR_CREATION;
        current_nested_level = level;
    }

    bool has_staged_states() {
        return (simple_state != nullptr) || (compound_state != nullptr);
    }

    template <typename SimpleStateType, typename CompoundStateType> void release_staged_states() {
        if (simple_state != nullptr) {
            reinterpret_cast<SimpleStateType*>(simple_state)->~SimpleStateType();
        }

        if (compound_state != nullptr) {
            reinterpret_cast<CompoundStateType*>(compound_state)->~CompoundStateType();
        }
    }

private:
    SwapBufferType& buffer;

    void* compound_state{nullptr};
    void* simple_state{nullptr};

    InternalState state{InternalState::READY_FOR_CREATION};

    size_t current_nested_level{0};
};

} // namespace fsm::_impl

#endif // LIBFSM__IMPL_STATE_ALLOCATOR_HPP
