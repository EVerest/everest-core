// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_SYNC_HPP
#define FSM_SYNC_HPP

#include <string>

#include <fsm/fsm.hpp>
#include <fsm/utils/Functor.hpp>

namespace fsm {
namespace sync {

template <typename EventBaseType> class FSMContext;

template <typename EventBaseType> class FSMInitContext {
public:
    virtual void set_callback(utils::StaticThisFunctor<void(FSMContext<EventBaseType>&)> fn, bool single_shot,
                              int interval) = 0;
};

template <typename EventBaseType> class FSMContext : public FSMInitContext<EventBaseType> {
public:
    virtual bool submit_event(const EventBaseType& ev) = 0;
    virtual void cancel_callback() = 0;
    virtual void repeat_callback() = 0;
    virtual ~FSMContext() = default; // FIXME (aw): is this necessary when only pass it as a reference?
};

template <typename EventInfoType, typename StateIDType> struct StateHandle {
    StateHandle(const StateIDType& id) : id(id){};
    using TransitionWrapperType = TransitionWrapper<StateHandle&, EventInfoType>;
    using EventBaseType = typename EventInfoType::BaseType;
    using FSMInitContextType = FSMInitContext<EventBaseType>;
    using FSMContextType = FSMContext<EventBaseType>;

    using TransitionTableType = utils::StaticThisFunctor<void(const EventBaseType&, TransitionWrapperType&)>;

    using EntryFunctionType = utils::StaticThisFunctor<void(FSMInitContextType&)>;
    using ExitFunctionType = utils::StaticThisFunctor<void()>;

    TransitionTableType transitions;
    EntryFunctionType entry{};
    ExitFunctionType exit{};

    // Callback should be set somehow from the context
    const StateIDType id;
};

template <typename ControllerType, typename EventBaseType, typename RepeatableType>
class FSMContextImpl : public FSMContext<EventBaseType> {
public:
    FSMContextImpl(ControllerType& ctrl) : ctrl(ctrl){};
    void set_callback(utils::StaticThisFunctor<void(FSMContext<EventBaseType>&)> fn, bool single_shot,
                      int interval) override final {
        cur_callback = fn;
        state = single_shot ? InternalState::SingleCallbackQueued : InternalState::RecurringCallback;
        this->interval = RepeatableType(interval);
    }
    bool submit_event(const EventBaseType& ev) override final {
        return ctrl.submit_event(ev);
    }
    void cancel_callback() override final {
        state = InternalState::NoCallback;
    }
    void repeat_callback() override final {
        if (state == InternalState::NoCallback) {
            return;
        }
    }

    // interface for SyncController, checking the callback status and such ...

    // FIXME (aw): ticks_to_next_run needs to be called before run() and the value must be passed
    // although this is a private api, is it possible to make the api more fail-safe?
    int ticks_to_next_run() {
        if (state == InternalState::NoCallback || state == InternalState::SingleCallbackExeced) {
            // there is nothing to do
            return -1;
        }

        return interval.ticks_left();
    }

    bool run(int ticks_left) {
        if (ticks_left != 0) {
            return false;
        }

        // if we come here, we need to be in SingleCallbackQueued or RecurringCallback
        // and the callback should be run
        // NOTE (aw): this part is critical, because cur_callback might modify ourself
        if (state == InternalState::SingleCallbackQueued) {
            state = InternalState::SingleCallbackExeced;
        } else { // must be RecurringCallback
            interval.repeat_from_last();
        }

        cur_callback(*this);

        return true;
    }

private:
    enum class InternalState : uint8_t
    {
        NoCallback,
        SingleCallbackQueued,
        SingleCallbackExeced,
        RecurringCallback
    };

    InternalState state{InternalState::NoCallback};

    utils::StaticThisFunctor<void(FSMContext<EventBaseType>&)> cur_callback{};
    bool single_shot{true};

    RepeatableType interval;

    // for submitting event
    ControllerType& ctrl;
};

template <typename StateHandleType, typename RepeatableType, void (*LogFunction)(const std::string&) = nullptr>
class BasicController {
    using EventBaseType = typename StateHandleType::EventBaseType;

public:
    BasicController() : state_context(*this) {
    }
    // setup starting state and call only the entry function, assuming
    // there will be no transitions setup during initial state
    // FIXME (aw): is this a valid assumption?
    void reset(StateHandleType& initial_state) {
        log("Starting with state: " + initial_state.id.name);
        cur_state = &initial_state;
        cur_state->entry(state_context);
    }

    // returns the time in milliseconds, when this feed should be called again
    // if zero, it should be called immediately again
    // if negative, there is just nothing to do
    int feed() {
        // run current callback if timer elapsed
        int ticks_left = state_context.ticks_to_next_run();
        if (state_context.run(ticks_left)) {
            // in case the callback sent events
            run_transitions();

            return state_context.ticks_to_next_run();
        }

        return ticks_left;
    }

    bool submit_event(const EventBaseType& event) {
        // check transition table of current set, return false if not possible
        cur_state->transitions(event, cur_trans);

        return run_transitions();
    }

private:
    // return true, if at least one transition was done
    bool run_transitions() {
        if (!cur_trans) {
            return false;
        }

        // while transition set?
        // FIXME (aw): this might need rethinking, do we want to return during each transition?
        while (cur_trans) {
            const std::string old_state_name = cur_state->id.name;

            // 1. run exit of current state
            if (cur_state->exit) {
                cur_state->exit();
            }

            // 2. run transition and set new state
            cur_state = &cur_trans();
            cur_trans.reset();

            // 3. run entry of new state
            if (cur_state->entry) {
                cur_state->entry(state_context);
            }

            log("Went from " + old_state_name + " to " + cur_state->id.name);
        }

        return true;
    }

    // FIXME (aw): does this get optimized away?
    void log(const std::string& msg) {
        if (LogFunction != nullptr) {
            LogFunction(msg);
        }
    }

    FSMContextImpl<BasicController, EventBaseType, RepeatableType> state_context;

    StateHandleType* cur_state{nullptr};
    typename StateHandleType::TransitionWrapperType cur_trans;
};

} // namespace sync
} // namespace fsm

#endif // FSM_SYNC_CRTL
