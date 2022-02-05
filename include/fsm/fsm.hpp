// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef FSM_FSM_HPP
#define FSM_FSM_HPP

#include <memory>

namespace fsm {

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

protected:
    enum class WrapType : uint8_t
    {
        None,
        FnWithEvent,
        FnWithoutEvent,
        CtxFnWithEvent,
        CtxFnWithoutEvent,
        MemFnWithEvent,
        MemFnWithoutEvent,
        InstRetVal,
        Internal
    };

    WrapType wrap_type{WrapType::None};

private:
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

        R (*fn_ptr)(){nullptr};
    R (*fn_v_ptr)(void*){nullptr};
    R (*fn_v_v_ptr)(void*, void*){nullptr};

    void* _inst;
    void* _ctx;

    // TODO (aw): how to proper handle return value types?
    std::remove_reference_t<R>* ret_val{nullptr};
    typename EventInfoType::StorageType buffer;
};

} // namespace fsm

#endif // FSM_FSM_HPP
