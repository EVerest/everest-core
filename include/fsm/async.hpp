// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_ASYNC_HPP
#define FSM_ASYNC_HPP

#include <string>

#include <fsm/fsm.hpp>
#include <fsm/utils/Functor.hpp>

namespace fsm {
namespace async {

template <typename EventBaseType> class FSMContext {
public:
    virtual void wait() = 0;

    // wait_for should return true if timeout successful
    virtual bool wait_for(int timeout_ms) = 0;
    virtual bool got_cancelled() = 0;
    virtual bool submit_event(const EventBaseType& event) = 0;

    virtual ~FSMContext() = default;
};

template <typename ControllerType, typename EventBaseType> class FSMContextCtrl : public FSMContext<EventBaseType> {
public:
    FSMContextCtrl(ControllerType& ctrl) : ctrl(ctrl){};

    bool submit_event(const EventBaseType& event) override final {
        return ctrl.submit_event(event);
    }

    virtual void cancel() = 0;
    virtual void reset() = 0;

private:
    ControllerType& ctrl;
};

template <typename EventInfoType, typename StateIDType> struct StateHandle {
    StateHandle(const StateIDType& id) : id(id){};
    using TransitionWrapperType = TransitionWrapper<StateHandle&, EventInfoType>;
    using EventBaseType = typename EventInfoType::BaseType;
    using FSMContextType = FSMContext<EventBaseType>;

    using TransitionTableType = utils::StaticThisFunctor<void(const EventBaseType&, TransitionWrapperType&)>;

    using StateFunctionType = utils::StaticThisFunctor<void(FSMContextType&)>;

    TransitionTableType transitions;
    StateFunctionType state_fun;
    const StateIDType id;
};

template <typename StateHandleType, typename PushPullLockType, typename ThreadType,
          template <typename, typename> class ContextImplType, void (*LogFunction)(const std::string&) = nullptr>
class BasicController {
    using EventBaseType = typename StateHandleType::EventBaseType;

public:
    enum class InternalEvent
    {
        NewTransition,
        Break
    };

    BasicController() : state_context(*this) {
    }

    bool submit_event(const EventBaseType& event, bool blocking = false) {
        // prevent multiple invocation
        auto push_lck = internal_change.get_push_lock();

        if (internal_state == InternalState::NewTransition) {
            // ongoing transition
            log("Already in transition");
            if (!blocking) {
                return false;
            }

            log("Blocking submission - waiting for pull");

            push_lck.wait_for_pull();
        }

        if (internal_state == InternalState::Break) {
            log("Skipping, because ongoing break");
            return false;
        }

        // set the new transition
        cur_state->transitions(event, cur_trans);

        // check if the transition exists
        if (!cur_trans) {
            log("Transition does not exist for the current state");
            return false;
        }

        internal_state = InternalState::NewTransition;

        log("Setup new transition");

        if (cur_state->state_fun) {
            state_context.cancel();
        }

        push_lck.commit();

        return true;
    }

    void run(StateHandleType& initial_state) {
        log("Starting with state: " + initial_state.id.name);
        internal_state = InternalState::Idle;
        cur_state = &initial_state;
        loop_thread.template spawn<BasicController, &BasicController::loop>(this);
    }

    void stop() {
        auto push_lck = internal_change.get_push_lock();

        internal_state = InternalState::Break;

        if (cur_state->state_fun) {
            state_context.cancel();
        }

        push_lck.commit();

        loop_thread.join();
    }

private:
    void loop() {
        while (true) {
            // run the state function

            if (cur_state->state_fun) {
                cur_state->state_fun(state_context);
            }

            // wait for the next transition
            auto pull_lck = internal_change.wait_for_pull_lock();

            if (internal_state == InternalState::Break) {
                // FIXME (aw): handle proper reset?
                break;
            }

            const std::string old_state_name = cur_state->id.name;

            cur_state = &cur_trans();

            log("Went from " + old_state_name + " to " + cur_state->id.name);

            internal_state = InternalState::Idle;
            cur_trans.reset();
            state_context.reset();
        }
    }

    // FIXME (aw): does this get optimized away?
    void log(const std::string& msg) {
        if (LogFunction != nullptr) {
            LogFunction(msg);
        }
    }

    enum class InternalState
    {
        Idle,
        NewTransition,
        Break
    };

    StateHandleType* cur_state{nullptr};
    typename StateHandleType::TransitionWrapperType cur_trans;

    ContextImplType<BasicController, EventBaseType> state_context;

    ThreadType loop_thread;

    PushPullLockType internal_change;
    InternalState internal_state{InternalState::Idle};
};

} // namespace async
} // namespace fsm

#endif // FSM_ASYNC_HPP
