// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM_FSM_HPP
#define LIBFSM_FSM_HPP

#include <array>
#include <memory>
#include <vector>

#include "_impl/common.hpp"
#include "_impl/state_allocator.hpp"

#include "states.hpp"

namespace fsm {

// FIXME (aw): we should treat internal unhandled events as errors because this doesn't make sense by design!
enum class HandleEventResult {
    SUCCESS,
    UNHANDLED,
    INTERNAL_ERROR,
};

// TODO (aw): would be good to know, if references or pointers can be passed as well
template <typename ResultType> class FeedResult {
private:
    using InternalState = _impl::FeedResultState;

public:
    FeedResult(InternalState state_) : state(state_){};
    FeedResult(ResultType value_) : value(value_), state(InternalState::HAS_VALUE){};

    bool has_value() const {
        return (state == InternalState::HAS_VALUE);
    }

    bool internal_error() const {
        return (state == InternalState::INTERNAL_ERROR);
    }

    bool transition() const {
        return (state == InternalState::TRANSITION);
    }

    bool unhandled_event() const {
        return (state == InternalState::UNHANDLED_EVENT);
    }

    ResultType& operator*() {
        return value;
    }

    ResultType* operator->() {
        return &value;
    }

private:
    ResultType value;
    InternalState state;
};

template <typename EventType, typename ReturnType, typename AllocatorBufferType = void> class FSM;

template <typename EventType, typename ReturnType> class FSM<EventType, ReturnType, void> {
public:
    using StateAllocatorType = states::StateAllocator<>;
    using SimpleStateType = states::SimpleStateBase<EventType, ReturnType, StateAllocatorType>;
    using CompoundStateType = states::CompoundStateBase<EventType, ReturnType, StateAllocatorType>;

    FSM() = default;
    FSM(const FSM& other) = delete;
    FSM(FSM&& other) = delete;
    FSM& operator=(const FSM& other) = delete;
    FSM& operator=(FSM&& other) = delete;
    ~FSM() = default;

    template <typename StateType, typename... Args> void reset(Args&&... args) {
        reset();

        state_allocator.make_ready_for_allocation();
        state_allocator.create_simple<StateType>(std::forward<Args>(args)...);
        current_state.reset(state_allocator.pull_simple_state<SimpleStateType>());
        current_state->enter();
    }

    HandleEventResult handle_event(EventType ev) {
        if (current_state == nullptr) {
            return HandleEventResult::INTERNAL_ERROR;
        }

        auto next_nesting_level_to_handle = compound_stack.size();
        auto state_allocator_wrapper = StateAllocatorType(state_allocator);

        state_allocator.make_ready_for_allocation();

        auto result = current_state->handle_event(state_allocator_wrapper, ev);

        while (result.is_pass_on() && next_nesting_level_to_handle != 0) {
            next_nesting_level_to_handle--;
            state_allocator.make_ready_for_allocation();
            result = compound_stack[next_nesting_level_to_handle]->handle_event(state_allocator_wrapper, ev);
        }

        if (result.is_pass_on()) {
            if (state_allocator.has_staged_states()) {
                state_allocator.release_staged_states<SimpleStateType, CompoundStateType>();
            }
            return HandleEventResult::UNHANDLED;
        } else if (result.is_handled_internally()) {
            if (state_allocator.has_staged_states()) {
                state_allocator.release_staged_states<SimpleStateType, CompoundStateType>();
            }
            return HandleEventResult::SUCCESS;
        } else if (result.is_allocation_error()) {
            state_allocator.release_staged_states<SimpleStateType, CompoundStateType>();
            return HandleEventResult::INTERNAL_ERROR;
        }

        const auto handled_at_nesting_level = next_nesting_level_to_handle;

        // fall-though: event has been handled, clear current state and all states up to the handled level

        // note: this will change current_nesting_level
        reset(handled_at_nesting_level, true);

        auto const compound_state = state_allocator.pull_compound_state<CompoundStateType>();

        if (compound_state != nullptr) {
            compound_stack.emplace_back(compound_state);
            compound_state->enter();
        }

        auto const next_state = state_allocator.pull_simple_state<SimpleStateType>();
        if (next_state != nullptr) {
            next_state->enter();
            current_state.reset(next_state);

            return HandleEventResult::SUCCESS;
        }

        // this should never happen - i.e. when we come here that would mean, someone managed to return NEW_STATE from
        // the handle_event callback but didn't set the next state
        return HandleEventResult::INTERNAL_ERROR;
    }

    FeedResult<ReturnType> feed() {
        using FeedResultState = _impl::FeedResultState;
        if (current_state == nullptr) {
            return FeedResultState::INTERNAL_ERROR;
        }

        const auto result = current_state->callback();

        if (result.is_event) {
            switch (handle_event(result.event)) {
            case HandleEventResult::SUCCESS:
                return FeedResultState::TRANSITION;
            case HandleEventResult::UNHANDLED:
                return FeedResultState::UNHANDLED_EVENT;
            default:
                // NOTE: everything else should be an internal error
                return FeedResultState::INTERNAL_ERROR;
            }
        } else if (result.is_value_set) {
            return result.value;
        } else {
            return FeedResultState::NO_VALUE;
        }
    }

private:
    void reset(size_t up_to_nested_level = 0, bool execute_leave = false) {
        // leave and destroy everything allocated
        if (current_state) {
            if (execute_leave) {
                current_state->leave();
            }
            current_state.reset();
        }

        while (compound_stack.size() > up_to_nested_level) {
            if (execute_leave) {
                compound_stack.back()->leave();
            }
            compound_stack.pop_back();
        }
    }

    std::unique_ptr<SimpleStateType> current_state{nullptr};
    std::vector<std::unique_ptr<CompoundStateType>> compound_stack{};

    _impl::DynamicStateAllocator state_allocator;
};

template <typename EventType, typename ReturnType, typename AllocatorBufferType> class FSM {
public:
    using StateAllocatorType = states::StateAllocator<AllocatorBufferType>;
    using SimpleStateType = states::SimpleStateBase<EventType, ReturnType, StateAllocatorType>;
    using CompoundStateType = states::CompoundStateBase<EventType, ReturnType, StateAllocatorType>;

    FSM(AllocatorBufferType& buffer) :
        state_allocator(buffer){

        };

    FSM(const FSM& other) = delete;
    FSM(FSM&& other) = delete;
    FSM& operator=(const FSM& other) = delete;
    FSM& operator=(FSM&& other) = delete;
    ~FSM() {
        state_allocator.template release_staged_states<SimpleStateType, CompoundStateType>();
        reset();
    }

    template <typename StateType, typename... Args> void reset(Args&&... args) {
        reset();

        state_allocator.make_ready_for_nesting_level(0);
        state_allocator.template create_simple<StateType>(std::forward<Args>(args)...);
        current_state = state_allocator.template pull_simple_state<SimpleStateType>();
        current_state->enter();
    }

    HandleEventResult handle_event(EventType ev) {
        if (current_state == nullptr) {
            return HandleEventResult::INTERNAL_ERROR;
        }

        auto next_nesting_level_to_handle = current_nesting_level;
        auto state_allocator_wrapper = StateAllocatorType(state_allocator);

        state_allocator.make_ready_for_nesting_level(next_nesting_level_to_handle);

        auto result = current_state->handle_event(state_allocator_wrapper, ev);

        while (result.is_pass_on() && next_nesting_level_to_handle != 0) {
            next_nesting_level_to_handle--;
            state_allocator.make_ready_for_nesting_level(next_nesting_level_to_handle);
            result = compound_states[next_nesting_level_to_handle]->handle_event(state_allocator_wrapper, ev);
        }

        if (result.is_pass_on()) {
            if (state_allocator.has_staged_states()) {
                state_allocator.template release_staged_states<SimpleStateType, CompoundStateType>();
            }
            return HandleEventResult::UNHANDLED;
        } else if (result.is_handled_internally()) {
            if (state_allocator.has_staged_states()) {
                state_allocator.template release_staged_states<SimpleStateType, CompoundStateType>();
            }
            return HandleEventResult::SUCCESS;
        } else if (result.is_allocation_error()) {
            state_allocator.template release_staged_states<SimpleStateType, CompoundStateType>();
            return HandleEventResult::INTERNAL_ERROR;
        }

        const auto handled_at_nesting_level = next_nesting_level_to_handle;

        // fall-though: event has been handled, clear current state and all states up to the handled level

        // note: this will change current_nesting_level
        reset(handled_at_nesting_level, true);

        const auto compound_state = state_allocator.template pull_compound_state<CompoundStateType>();

        if (compound_state != nullptr) {
            // compound state has been set
            current_nesting_level = handled_at_nesting_level + 1;
            compound_states[handled_at_nesting_level] = compound_state;

            compound_state->enter();
        }

        const auto next_state = state_allocator.template pull_simple_state<SimpleStateType>();
        if (next_state != nullptr) {
            next_state->enter();
            current_state = next_state;

            return HandleEventResult::SUCCESS;
        }

        // this should never happen - i.e. when we come here that would mean, someone managed to return NEW_STATE from
        // the handle_event callback but didn't set the next state
        return HandleEventResult::INTERNAL_ERROR;
    }

    FeedResult<ReturnType> feed() {
        using FeedResultState = _impl::FeedResultState;
        if (current_state == nullptr) {
            return FeedResultState::INTERNAL_ERROR;
        }

        const auto result = current_state->callback();

        if (result.is_event) {
            switch (handle_event(result.event)) {
            case HandleEventResult::SUCCESS:
                return FeedResultState::TRANSITION;
            case HandleEventResult::UNHANDLED:
                return FeedResultState::UNHANDLED_EVENT;
            default:
                // NOTE: everything else should be an internal error
                return FeedResultState::INTERNAL_ERROR;
            }
        } else if (result.is_value_set) {
            return result.value;
        } else {
            return FeedResultState::NO_VALUE;
        }
    }

private:
    void reset(size_t up_to_nested_level = 0, bool execute_leave = false) {
        // leave and destroy everything allocated
        if (current_state) {
            if (execute_leave) {
                current_state->leave();
            }
            current_state->~SimpleStateBase();
            current_state = nullptr;
        }

        while (current_nesting_level > up_to_nested_level) {
            auto& compound_state = compound_states[current_nesting_level - 1];
            if (execute_leave) {
                compound_state->leave();
            }
            compound_state->~CompoundStateBase();
            compound_state = nullptr;
            current_nesting_level--;
        }
    }

    SimpleStateType* current_state{nullptr};
    std::array<CompoundStateType*, AllocatorBufferType::MAX_NESTING_LEVEL> compound_states{};
    size_t current_nesting_level{0};

    _impl::StateAllocator<AllocatorBufferType> state_allocator;
};

} // namespace fsm

#endif // LIBFSM_FSM_HPP
