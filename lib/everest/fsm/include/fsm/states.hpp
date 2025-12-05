// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM_STATES_HPP
#define LIBFSM_STATES_HPP

#include "_impl/state_allocator.hpp"

namespace fsm::states {

// forward declaration so it can be made friend of states::HandleEventResult
template <typename SwapBufferType> class StateAllocator;

class HandleEventResult {
private:
    enum class InternalState {
        NEW_STATE,
        HANDLED_INTERNALY,
        ALLOCATION_ERROR,
        PASS_ON,
    };

    constexpr HandleEventResult(InternalState state_) : state(state_){};

    InternalState state;

    template <typename SwapBufferType> friend class StateAllocator;

public:
    bool is_new_state() const {
        return InternalState::NEW_STATE == state;
    }

    bool is_allocation_error() const {
        return InternalState::ALLOCATION_ERROR == state;
    }

    bool is_pass_on() const {
        return InternalState::PASS_ON == state;
    }

    bool is_handled_internally() const {
        return InternalState::HANDLED_INTERNALY == state;
    }
};

template <typename EventType, typename ReturnType> struct CallbackResult {
    CallbackResult() = default;
    CallbackResult(ReturnType value_) : value(value_), is_value_set(true){};
    CallbackResult(EventType event_) : event(event_), is_event(true){};

    ReturnType value{0};
    EventType event;
    bool is_event{false};
    bool is_value_set{false};
};

template <typename EventType_, typename ReturnType_, typename StateAllocatorType> struct SimpleStateBase {
    using EventType = EventType_;
    using ReturnType = ReturnType_;
    using AllocatorType = StateAllocatorType;

    using CallbackReturnType = CallbackResult<EventType_, ReturnType>;
    using HandleEventReturnType = HandleEventResult;

    virtual void enter(){};
    virtual HandleEventReturnType handle_event(AllocatorType&, EventType) = 0;
    virtual CallbackReturnType callback() {
        return {};
    };
    virtual void leave(){};
    virtual ~SimpleStateBase() = default;
};

template <typename EventType_, typename ReturnType_, typename StateAllocatorType> struct CompoundStateBase {
    using EventType = EventType_;
    using ReturnType = ReturnType_;
    using AllocatorType = StateAllocatorType;

    using HandleEventReturnType = HandleEventResult;

    virtual void enter(){};
    virtual HandleEventReturnType handle_event(AllocatorType&, EventType) = 0;
    virtual void leave(){};
    virtual ~CompoundStateBase() = default;
};

template <typename StateType, typename ContextType> struct StateWithContext : public StateType {
    StateWithContext(ContextType& context) : ctx(context){};

protected:
    ContextType& ctx;
};

template <typename SwapAllocatorBufferType = void> class StateAllocator;

template <> class StateAllocator<void> {
public:
    StateAllocator(_impl::DynamicStateAllocator& allocator_) : allocator(allocator_){};

    template <typename StateType, typename... Args> void create_compound(Args&&... args) {
        static_assert(_impl::is_base_state_of<CompoundStateBase, StateType>::value,
                      "StateType needs to be derived from CompoundStateBase");
        allocator.create_compound<StateType>(std::forward<Args>(args)...);
    }

    template <typename StateType, typename... Args> HandleEventResult create_simple(Args&&... args) {
        static_assert(_impl::is_base_state_of<SimpleStateBase, StateType>::value,
                      "StateType needs to be derived from SimpleStateBase");

        using State = HandleEventResult::InternalState;
        const auto success = allocator.create_simple<StateType>(std::forward<Args>(args)...);
        return success ? State::NEW_STATE : State::ALLOCATION_ERROR;
    }

    // NOTE (aw): this could also be a non-static function, which checks if any states have been set
    static constexpr HandleEventResult PASS_ON{HandleEventResult::InternalState::PASS_ON};
    static constexpr HandleEventResult HANDLED_INTERNALLY{HandleEventResult::InternalState::HANDLED_INTERNALY};

private:
    _impl::DynamicStateAllocator& allocator;
};

template <typename SwapBufferType> class StateAllocator {
public:
    StateAllocator(_impl::StateAllocator<SwapBufferType>& allocator_) : allocator(allocator_){};

    template <typename StateType, typename... Args> void create_compound(Args&&... args) {
        static_assert(_impl::is_base_state_of<CompoundStateBase, StateType>::value,
                      "StateType needs to be derived from CompoundStateBase");
        allocator.template create_compound<StateType>(std::forward<Args>(args)...);
    }

    template <typename StateType, typename... Args> HandleEventResult create_simple(Args&&... args) {
        static_assert(_impl::is_base_state_of<SimpleStateBase, StateType>::value,
                      "StateType needs to be derived from SimpleStateBase");

        using State = HandleEventResult::InternalState;
        const auto success = allocator.template create_simple<StateType>(std::forward<Args>(args)...);
        return success ? State::NEW_STATE : State::ALLOCATION_ERROR;
    }

    // NOTE (aw): this could also be a non-static function, which checks if any states have been set
    static constexpr HandleEventResult PASS_ON{HandleEventResult::InternalState::PASS_ON};
    static constexpr HandleEventResult HANDLED_INTERNALLY{HandleEventResult::InternalState::HANDLED_INTERNALY};

private:
    _impl::StateAllocator<SwapBufferType>& allocator;
};

} // namespace fsm::states

#endif // LIBFSM_STATES_HPP
