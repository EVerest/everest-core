// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_UTILS_FUNCTOR_HPP
#define FSM_UTILS_FUNCTOR_HPP

namespace fsm {
namespace utils {

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

} // namespace utils
} // namespace fsm

#endif // FSM_UTILS_FUNCTOR_HPP
