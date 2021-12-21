// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef FSM_UTILS_IDENTIFIABLE_HPP
#define FSM_UTILS_IDENTIFIABLE_HPP

#include <utility>

namespace fsm {
namespace utils {

template <typename IDType> struct IdentifiableBase {
    IdentifiableBase(IDType id) : id(id) {
    }
    const IDType id;
};

template <typename IDType, IDType ID, typename T = void> struct Identifiable : IdentifiableBase<IDType> {
    Identifiable(T&& data) : IdentifiableBase<IDType>(ID), data(std::forward<T>(data)) {
    }

    Identifiable(const IdentifiableBase<IDType>& ev) :
        IdentifiableBase<IDType>(ID), data(static_cast<const Identifiable<IDType, ID, T>&>(ev).data) {
    }

    T data;
};

template <typename IDType, IDType ID> struct Identifiable<IDType, ID, void> : IdentifiableBase<IDType> {
    Identifiable() : IdentifiableBase<IDType>(ID) {
    }

    Identifiable(const IdentifiableBase<IDType>& ev) : IdentifiableBase<IDType>(ID) {
    }
};

template <typename IDType> struct IdentifiableTypeFactory {
    using Base = IdentifiableBase<IDType>;
    template <IDType ID, typename T = void> using Derived = Identifiable<IDType, ID, T>;
};

} // namespace utils
} // namespace fsm

#endif // FSM_UTILS_IDENTIFIABLE_HPP
