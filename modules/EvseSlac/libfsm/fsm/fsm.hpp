// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM_FSM_HPP
#define LIBFSM_FSM_HPP

#include <memory>
#include <vector>

namespace fsm {

namespace _detail {

enum class StateFlavour {
    SIMPLE,
    COMPOUND,
};

template <typename T, typename = void> struct add_ref_for_non_trivial {
    typedef const T& type;
};

template <typename T>
struct add_ref_for_non_trivial<T, std::enable_if_t<std::is_enum<T>::value || std::is_fundamental<T>::value>> {
    typedef T type;
};

template <typename T> using add_ref_for_non_trivial_t = typename add_ref_for_non_trivial<T>::type;
} // namespace _detail

// FIXME (aw): these constants should probably seperated from the real return value ...
constexpr int IMMEDIATELY_INVOKE_CALLBACK = 0;
constexpr int DO_NOT_CALL_ME_AGAIN = -1;
constexpr int EVENT_UNHANDLED = -2;
constexpr int EVENT_HANDLED_INTERNALLY = -3;

template <typename EventType> struct CallbackResult {
    CallbackResult(int value_) : value(value_){};
    CallbackResult(EventType event_) : event(event_), is_event(true){};

    int value{0};
    EventType event;
    bool is_event{false};
};

template <typename EventType> class NextState;

template <typename EventType> struct BaseState {
    virtual int enter() {
        return IMMEDIATELY_INVOKE_CALLBACK;
    };
    virtual NextState<EventType> handle_event(_detail::add_ref_for_non_trivial_t<EventType>) = 0;
    virtual CallbackResult<EventType> callback() {
        return DO_NOT_CALL_ME_AGAIN;
    };
    virtual void leave(){};
    virtual ~BaseState() = default;
};

template <typename EventType> struct CompoundState : public BaseState<EventType> {

private:
    using BaseState<EventType>::callback;
};

template <typename StateBaseType, typename ContextType> struct StateWithContext : public StateBaseType {
    StateWithContext(ContextType& context) : ctx(context){};

protected:
    template <typename StateType, typename... Args> auto create_with_context(Args&&... args) {
        return std::make_unique<StateType>(ctx, std::forward<Args>(args)...);
    }
    ContextType& ctx;
};

// forward declaration, so NextState can use it as friend
template <typename EventType> class FSM;

enum class NextStateOption {
    RESTART,
    PASS_ON,
    HANDLED_INTERNALLY,
};

template <typename EventType> class NextState {
public:
    NextState(std::unique_ptr<CompoundState<EventType>>&& compound_, std::unique_ptr<BaseState<EventType>>&& child_) :
        compound(std::move(compound_)), child(std::move(child_)){};
    NextState(std::unique_ptr<BaseState<EventType>>&& child_) : child(std::move(child_)){};
    template <typename DerivedStateType>
    NextState(std::unique_ptr<DerivedStateType>&& child_) : child(std::move(child_)){};
    NextState(NextStateOption option_) : option(option_) {
        if (option == NextStateOption::PASS_ON) {
            pass_on = true;
        }
    };

private:
    NextStateOption option{NextStateOption::PASS_ON};

    bool pass_on{false};

    std::unique_ptr<CompoundState<EventType>> compound{nullptr};
    std::unique_ptr<BaseState<EventType>> child{nullptr};

    template <typename FSMEventType> friend class FSM;
};

template <typename EventType> class FSM {
public:
    using BaseType = BaseState<EventType>;
    using CompoundType = CompoundState<EventType>;
    using NextStateType = NextState<EventType>;
    using CallbackResultType = CallbackResult<EventType>;

    // FIXME (aw): should we support compound states as initial states?
    template <typename StateType, typename... Args> int reset(Args&&... args) {
        state_stack.clear();
        state_stack.emplace_back(std::make_unique<StateType>(std::forward<Args>(args)...));
        return state_stack.back()->enter();
    }

    int feed_event(_detail::add_ref_for_non_trivial_t<EventType> ev) {
        return dispatch_event(ev);
    }

    // NOTE (aw): the feed method will run as long as there are new internal events
    int feed() {
        auto result = state_stack.back()->callback();

        if (result.is_event) {
            return dispatch_event(result.event);
        } else {
            return result.value;
        }
    }

private:
    // dispatch_event will either:
    // - change the state (enter will be called at the end)
    // - handle the event internally (same as what the callback function could return)
    // - no handle the event at all -> previous feed return value should keep it's meaning ?
    int dispatch_event(_detail::add_ref_for_non_trivial_t<EventType> ev) {
        auto current_state_it = state_stack.rbegin();

        auto next_state = NextStateType(NextStateOption::PASS_ON);

        while (current_state_it != state_stack.rend() && next_state.pass_on) {
            next_state = (*current_state_it)->handle_event(ev);
            std::advance(current_state_it, 1);
        }

        if (next_state.pass_on == true) {
            // no one handled that!
            return EVENT_UNHANDLED;
        }

        if (next_state.option == NextStateOption::HANDLED_INTERNALLY) {
            // the event just got ignored
            return EVENT_HANDLED_INTERNALLY;
        }

        for (auto child_to_leave_it = state_stack.rbegin(); child_to_leave_it != current_state_it;) {
            (*child_to_leave_it)->leave();
            child_to_leave_it++;
            state_stack.pop_back();
        }

        if (next_state.compound) {
            state_stack.emplace_back(std::move(next_state.compound));
            state_stack.back()->enter();
        }

        if (next_state.child) {
            state_stack.emplace_back(std::move(next_state.child));
            // FIXME (aw): if enter returns IMMEDIATELY_INVOKE_CALLBACK, we could call it directly
            //             but would then also have to dispatch the events again and so on ...
            return state_stack.back()->enter();
        }

        // FIXME (aw): should never happen, restructure code to get rid of this line
        return 0;
    }

    std::vector<std::unique_ptr<BaseType>> state_stack;
};

} // namespace fsm

#endif // LIBFSM_FSM_HPP
