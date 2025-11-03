#ifndef FSM_FSM_HPP
#define FSM_FSM_HPP

#include <iostream>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace fsm {

template <typename FunctionType, typename StorageType> class StaticFunctorWrapper;

template <typename R, typename... ArgTypes, typename StorageType>
class StaticFunctorWrapper<R(ArgTypes...), StorageType> {
public:
    StaticFunctorWrapper() = default;
    template <typename T> StaticFunctorWrapper(T lambda) {
        static_assert(sizeof(T) <= sizeof(buf),
                      "Size of passed StorageType is to small for storing the passed functor object");
        // FIXME (aw): we would need to call the destructor by hand, but we restrict usage of this class to cases, where
        // this is not neccessary
        ::new (&buf) T(std::move(lambda));
        fn_ptr = fn_tramp<T>;
    }
    template <typename T> StaticFunctorWrapper& operator=(T lambda) {
        static_assert(sizeof(T) <= sizeof(buf),
                      "Size of passed StorageType is to small for storing the passed functor object");
        // FIXME (aw): we would need to call the destructor by hand, but we restrict usage of this class to cases, where
        // this is not neccessary
        ::new (&buf) T(std::move(lambda));
        fn_ptr = fn_tramp<T>;
        return *this;
    }

    R operator()(ArgTypes... args) {
        return (*fn_ptr)(&buf, args...);
    }

    StorageType buf;

    operator bool() {
        return fn_ptr != nullptr;
    }

private:
    R (*fn_ptr)(void* lambda, ArgTypes...){nullptr};
    template <typename T> static R fn_tramp(void* lambda, ArgTypes... args) {
        return (*static_cast<T*>(lambda))(args...);
    }
};

template <typename FunctionType>
using StaticThisFunctor = StaticFunctorWrapper<FunctionType, std::aligned_storage_t<sizeof(void*), alignof(void*)>>;

template <typename EventBaseType,
          typename EventStorageType = std::aligned_storage_t<sizeof(EventBaseType), alignof(EventBaseType)>>
struct EventInfo {
    using BaseType = EventBaseType;
    using StorageType = EventStorageType;
};

// TODO (aw): could we have a function template syntax here, that is R(EventBase) ?
template <typename R, typename EventInfoType> class TransitionWrapper {
public:
    using EventBaseType = typename EventInfoType::BaseType;

    // TODO (aw): properly handle reference values
    void set(R value) {
        ret_val = &value;
        wrap_type = WrapType::InstRetVal;
    }

    void set(R (*fn)()) {
        fn_ptr = fn;
        wrap_type = WrapType::FnWithoutEvent;
    }

    template <typename ED, R (*fn)(const ED&)> void set(const EventBaseType& event) {
        ::new (&buffer) ED(event);
        fn_v_ptr = &fn_tramp<ED, fn>;
        wrap_type = WrapType::FnWithEvent;
    }

    template <typename ED, R (*fn)(const ED&, void*)> void set(const EventBaseType& event, void* ctx) {
        ::new (&buffer) ED(event);
        fn_v_v_ptr = &ctx_fn_tramp<ED, fn>;
        this->_ctx = ctx;
        wrap_type = WrapType::CtxFnWithEvent;
    }

    void set(R (*fn)(void*), void* ctx) {
        fn_v_ptr = fn;
        this->_ctx = ctx;
        wrap_type = WrapType::CtxFnWithoutEvent;
    }

    template <typename T, R (T::*mem_fn)()> void set(T* inst) {
        fn_v_ptr = &mem_fn_ne_tramp<T, mem_fn>;
        this->_inst = inst;
        wrap_type = WrapType::MemFnWithoutEvent;
    }

    template <typename ED, typename T, R (T::*mem_fn)(const ED&)> void set(const EventBaseType& event, T* inst) {
        ::new (&buffer) ED(event);
        fn_v_v_ptr = &mem_fn_tramp<ED, T, mem_fn>;
        this->_inst = inst;
        wrap_type = WrapType::MemFnWithEvent;
    }

    void reset() {
        wrap_type = WrapType::None;
    }

    operator bool() {
        return wrap_type != WrapType::None;
    }

    R operator()() {
        switch (wrap_type) {
        case WrapType::FnWithoutEvent:
            return (*fn_ptr)();
        case WrapType::FnWithEvent:
            return (*fn_v_ptr)(&buffer);
        case WrapType::CtxFnWithEvent:
            return (*fn_v_v_ptr)(&buffer, _ctx);
        case WrapType::CtxFnWithoutEvent:
            return (*fn_v_ptr)(_ctx);
        case WrapType::MemFnWithoutEvent:
            return (*fn_v_ptr)(_inst);
        case WrapType::MemFnWithEvent:
            return (*fn_v_v_ptr)(_inst, &buffer);
        case WrapType::InstRetVal:
            return *ret_val;
        default:
            // FIXME (aw): how to forbid that?
            return *ret_val;
        }
    }

private:
    enum class WrapType : uint8_t
    {
        None,
        FnWithEvent,
        FnWithoutEvent,
        CtxFnWithEvent,
        CtxFnWithoutEvent,
        MemFnWithEvent,
        MemFnWithoutEvent,
        InstRetVal
    };

    template <typename ED, R (*fn)(const ED&)> static R fn_tramp(void* buf) {
        return (*fn)(*reinterpret_cast<ED*>(buf));
    }

    template <typename ED, R (*fn)(const ED&, void*)> static R ctx_fn_tramp(void* buf, void* ctx) {
        return (*fn)(*reinterpret_cast<ED*>(buf), ctx);
    }

    template <typename ED, typename T, R (T::*mem_fn)(const ED&)> static R mem_fn_tramp(void* class_inst, void* buf) {
        auto cast_class_inst = static_cast<T*>(class_inst);
        return (cast_class_inst->*mem_fn)(*reinterpret_cast<ED*>(buf));
    }

    template <typename T, R (T::*mem_fn)()> static R mem_fn_ne_tramp(void* class_inst) {
        auto cast_class_inst = static_cast<T*>(class_inst);
        return (cast_class_inst->*mem_fn)();
    }

    WrapType wrap_type{WrapType::None};

    R (*fn_ptr)(){nullptr};
    R (*fn_v_ptr)(void*){nullptr};
    R (*fn_v_v_ptr)(void*, void*){nullptr};

    void* _inst;
    void* _ctx;

    // TODO (aw): how to proper handle return value types?
    std::remove_reference_t<R>* ret_val{nullptr};
    typename EventInfoType::StorageType buffer;
};

template <typename EventBaseType> class FSMContext {
public:
    template <typename T> FSMContext(T lambda) : submit_event_cb(lambda) {
    }
    virtual void wait() = 0;

    // wait_for should return true if timeout successful
    virtual bool wait_for(int timeout_ms) = 0;
    virtual bool got_cancelled() = 0;
    bool submit_event(const EventBaseType& event) {
        return submit_event_cb(event);
    }

    virtual ~FSMContext() = default;

private:
    StaticThisFunctor<bool(const EventBaseType& event)> submit_event_cb;
    virtual void cancel() = 0;
    virtual void reset() = 0;
};

template <typename EventInfoType, typename StateIDType> struct StateHandle {
    StateHandle(const StateIDType& id) : id(id){};
    using TransitionWrapperType = TransitionWrapper<StateHandle&, EventInfoType>;
    using EventBaseType = typename EventInfoType::BaseType;
    using TransitionTableType = StaticThisFunctor<void(const EventBaseType&, TransitionWrapperType&)>;
    using FSMContextType = FSMContext<EventBaseType>;
    using StateFunctionType = StaticThisFunctor<void(FSMContextType&)>;

    TransitionTableType transitions;
    StateFunctionType state_fun;
    const StateIDType id;
};

template <typename StateHandleType, typename PushPullLockType, typename ThreadType, typename ContextType,
          void (*LogFunction)(const std::string&) = nullptr>
class BasicController {
public:
    enum class InternalEvent
    {
        NewTransition,
        Break
    };

    BasicController() :
        fsm_context([this](const typename StateHandleType::EventBaseType& event) { return submit_event(event); }) {
    }

    bool submit_event(const typename StateHandleType::EventBaseType& event, bool blocking = false) {
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
            fsm_context.cancel();
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
            fsm_context.cancel();
        }

        push_lck.commit();

        loop_thread.join();
    }

private:
    void loop() {
        while (true) {
            // run the state function

            if (cur_state->state_fun) {
                cur_state->state_fun(fsm_context);
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
            fsm_context.reset();
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

    ContextType fsm_context;

    ThreadType loop_thread;

    PushPullLockType internal_change;
    InternalState internal_state;
};

} // namespace fsm

#endif // FSM_FSM_HPP
