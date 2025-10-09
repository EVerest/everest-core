// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#include <functional>
#include <type_traits>

namespace everest::lib::io::utilities {

/**
 * @brief Primary template for the trait to check for the existence of a member function 'setup'.
 * @tparam T The type to check.
 */
template <typename T, typename V = void> struct has_member_setup : std::false_type {};

/**
 * @brief Specialization of has_member_setup.
 * This checks for existence and accessibility of a member function 'setup', accepting any signature.
 * @tparam T The type to check.
 */
template <typename T> struct has_member_setup<T, std::void_t<decltype(&T::setup)>> : std::true_type {};

/**
 * @brief Primary template for the trait to check if a type T has a member function
 * 'connect(std::function<void(bool, int)> const&)' with any return type.
 * @tparam T The type to check.
 */
template <typename T, typename V = void> struct has_member_connect : std::false_type {};

/**
 * @brief Specialization of has_member_connect.
 * This checks the existence of a member function connect with 'std::function<void(bool, int)>' as parameter.
 * @tparam T The type to check.
 */
template <typename T>
struct has_member_connect<
    T, std::void_t<decltype(std::declval<T>().connect(std::declval<std::function<void(bool, int)> const&>()))>>
    : std::true_type {};

/**
 * @brief Defines the policy trait for an asynchronous event client.
 * A type T satisfies this policy if it has:
 * 1. An accessible member 'setup' (any signature).
 * 2. A callable member function 'connect(const std::function<void(bool, int)>&)' (any return type).
 * @tparam T The type to check.
 */
template <typename T>
struct event_client_async_policy
    : std::integral_constant<bool, has_member_setup<T>::value && has_member_connect<T>::value> {};

/**
 * @brief Convenience variable template for the event_client_async_policy trait's value.
 * @tparam T The type to check.
 */
template <typename T> inline constexpr bool event_client_async_policy_v = event_client_async_policy<T>::value;

} // namespace everest::lib::io::utilities
