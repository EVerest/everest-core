// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef FSM_SYNC_HPP
#define FSM_SYNC_HPP

#include <string>

#include <fsm/fsm.hpp>
#include <fsm/utils/Functor.hpp>

namespace fsm {
namespace sync {

class FSMCommonContext {
public:
    virtual void set_next_timeout(int interval, bool repeat = false) = 0;
};

template <typename EventBaseType> class FSMInitContext : virtual public FSMCommonContext {
public:
    // FIXME (aw): internal events can't be forwarded directly
    // and would need to be stored, but we don't have their type
    // ..., only possible if we template this submit function
    // const EventBaseType& ev() {
    //     return event;
    // }

protected:
    // const EventBaseType* event;
};

template <typename EventBaseType> class FSMContext : virtual public FSMCommonContext {
public:
    virtual bool submit_event(const EventBaseType& ev) = 0;
    virtual void disable_timeout() = 0;
    virtual ~FSMContext() = default; // FIXME (aw): is this necessary when only pass it as a reference?
};

template <typename R, typename EventInfoType> class SyncTransitionWrapper : public TransitionWrapper<R, EventInfoType> {
public:
    using FSMContextType = FSMContext<typename EventInfoType::BaseType>;
    explicit SyncTransitionWrapper(FSMContextType& state_ctx) : state_ctx(state_ctx){};

    FSMContextType& set_internal() {
        // FIXME (aw): howto shorten this type?
        this->wrap_type = TransitionWrapper<R, EventInfoType>::WrapType::Internal;
        return state_ctx;
    }

    bool is_internal() {
        return (this->wrap_type == TransitionWrapper<R, EventInfoType>::WrapType::Internal);
    }

private:
    FSMContextType& state_ctx;
};

template <typename EventInfoType, typename StateIDType> struct StateHandle {
    StateHandle(const StateIDType& id) : id(id){};
    using TransitionWrapperType = SyncTransitionWrapper<StateHandle&, EventInfoType>;
    using EventBaseType = typename EventInfoType::BaseType;
    using FSMContextType = FSMContext<EventBaseType>;
    using FSMInitContextType = FSMInitContext<EventBaseType>;
    using TransitionTableType = utils::StaticThisFunctor<void(const EventBaseType&, TransitionWrapperType&)>;

    using EntryFunctionType = utils::StaticThisFunctor<void(FSMInitContextType&)>;
    using HandlerFunctionType = utils::StaticThisFunctor<void(FSMContextType&)>;
    using ExitFunctionType = utils::StaticThisFunctor<void()>;

    TransitionTableType transitions;
    EntryFunctionType entry{};
    HandlerFunctionType handler{};
    ExitFunctionType exit{};

    // Callback should be set somehow from the context
    const StateIDType id;
};

template <typename StateHandleType, typename RepeatableType, void (*LogFunction)(const std::string&) = nullptr>
class BasicController {
    using EventBaseType = typename StateHandleType::EventBaseType;

public:
    BasicController() : state_context(*this), cur_trans(state_context) {
    }
    // setup starting state and call only the entry function, assuming
    // there will be no transitions setup during initial state
    // FIXME (aw): is this a valid assumption?
    void reset(StateHandleType& initial_state) {
        log("Starting with state: " + initial_state.id.name);
        cur_state = &initial_state;
        if (cur_state->entry) {
            cur_state->entry(state_context);
        }
        call_state = cur_state->handler ? HandlerCallState::InitialCallQueued : HandlerCallState::NoCall;
    }

    // returns the time in milliseconds, when this feed should be called again
    // if zero, it should be called immediately again
    // if negative, there is just nothing to do
    int feed() {
        switch (call_state) {
        case HandlerCallState::NoCall:
        case HandlerCallState::SingleCallDone:
            return -1;
        }

        // this needs to be SingleCallQueued or RecurringCall
        // so we have to check for the timeout
        auto ticks_left = call_timeout.ticks_left();
        if (ticks_left != 0) {
            return ticks_left;
        }

        if (call_state == HandlerCallState::SingleCallQueued) {
            call_state = HandlerCallState::SingleCallDone;
        } else {
            // must be RecurringCallback
            call_timeout.repeat_from_last();
        }

        cur_state->handler(state_context);

        // if we end up here, a handler has been call and might
        // triggered a new transition, therefor we need to check them

        if (run_transition()) {
            // now we have either NoCall because there
            // is no handler, or we immediately need to run the initial
            // handler
            return (call_state == HandlerCallState::NoCall) ? -1 : 0;
        }

        // we call a handler but no transition, we only need to be called back again for the case, that we're in
        if (call_state == HandlerCallState::SingleCallQueued || call_state == HandlerCallState::RecurringCall) {
            return call_timeout.ticks_left();
        }

        return -1;
    }

    bool submit_event(const EventBaseType& event) {
        // check transition table of current set, return false if not possible
        cur_state->transitions(event, cur_trans);

        return run_transition();
    }

    StateHandleType* current_state() {
        return cur_state;
    }

private:
    // return true, if at least one transition was done
    bool run_transition() {
        if (!cur_trans) {
            return false;
        }

        // handle "internal" transitions
        if (cur_trans.is_internal()) {
            // internal handler has been called already in the
            // transitions block
            log("Internal transition on " + cur_state->id.name);
            cur_trans.reset();
            return true;
        }

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

        call_state = (cur_state->handler) ? HandlerCallState::InitialCallQueued : HandlerCallState::NoCall;

        log("Went from " + old_state_name + " to " + cur_state->id.name);

        return true;
    }

    // FIXME (aw): does this get optimized away?
    void log(const std::string& msg) {
        if (LogFunction != nullptr) {
            LogFunction(msg);
        }
    }

    class FSMContextImpl : public FSMInitContext<EventBaseType>, public FSMContext<EventBaseType> {
    public:
        explicit FSMContextImpl(BasicController& ctrl) : ctrl(ctrl){};
        void set_next_timeout(int interval, bool repeat) override final {
            ctrl.call_timeout = RepeatableType(interval);
            ctrl.call_state = repeat ? HandlerCallState::RecurringCall : HandlerCallState::SingleCallQueued;
        }
        bool submit_event(const EventBaseType& ev) override final {
            auto& cur_trans = ctrl.cur_trans;
            if (cur_trans && !cur_trans.is_internal()) {
                ctrl.log("Recursive event submit detected!");
            }

            ctrl.cur_state->transitions(ev, cur_trans);
            // FIXME (aw): should we disable calling set_next_timeout after submitting an event?
            return cur_trans;
        }

        void disable_timeout() override final {
            ctrl.call_state = HandlerCallState::NoCall;
        }

        void set_event_ptr(const EventBaseType* event_ptr) {
            this->event = event_ptr;
        }

    private:
        BasicController& ctrl;
    };

    RepeatableType call_timeout;

    enum class HandlerCallState : uint8_t
    {
        NoCall,
        InitialCallQueued,
        InitialCallDone,
        SingleCallQueued,
        SingleCallDone,
        RecurringCall
    };

    HandlerCallState call_state{HandlerCallState::NoCall};

    FSMContextImpl state_context;

    StateHandleType* cur_state{nullptr};
    typename StateHandleType::TransitionWrapperType cur_trans;
};

} // namespace sync
} // namespace fsm

#endif // FSM_SYNC_CRTL
