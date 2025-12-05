// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef LIBFSM__IMPL_COMMON_HPP
#define LIBFSM__IMPL_COMMON_HPP

namespace fsm::_impl {

template <template <typename, typename, typename> typename BaseStateType, typename DerivedStateType>
struct is_base_state_of {
    using BaseType = BaseStateType<typename DerivedStateType::EventType, typename DerivedStateType::ReturnType,
                                   typename DerivedStateType::AllocatorType>;
    static const bool value = std::is_base_of<BaseType, DerivedStateType>::value;
};

enum class FeedResultState {
    TRANSITION,
    UNHANDLED_EVENT,
    INTERNAL_ERROR,
    HAS_VALUE,
    NO_VALUE,
};

} // namespace fsm::_impl

#endif // LIBFSM__IMPL_COMMON_HPP
