// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <type_traits>

/**
 * @file util.hpp
 * @brief Mathematical and Optional utility functions for the EVerest framework.
 */

namespace everest::lib::util {

// --- Floating Point Utilities ---

/**
 * @brief Calculates a decimal precision threshold (10^-n).
 * * This function is used to generate an epsilon value based on a human-readable
 * number of decimal digits. For example, a precision of 3 returns 0.001.
 * * @tparam T The floating point type (float, double, long double).
 * @param digits_of_precision The number of decimal places to consider.
 * @return constexpr T The calculated threshold value.
 */
template <class T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
constexpr T range_limit(int digits_of_precision) {
    T result = static_cast<T>(1.0);
    for (int i = 0; i < digits_of_precision; ++i) {
        result /= static_cast<T>(10.0);
    }
    return result;
}

/**
 * @brief Compares two floating point numbers for equality within a decimal precision.
 * * Uses a fixed epsilon (absolute difference) approach. This is ideal for
 * physical values (like Amps or Volts) where the scale of the numbers is
 * relatively consistent and known.
 * @detail std::abs is not guarantted to be constexpr in C++17, using the tenery operate instead.
 * * @tparam Prec The number of decimal digits of precision to use for comparison.
 * @tparam T1, T2 Floating point types of the input values.
 * @param lhs Left hand side value.
 * @param rhs Right hand side value.
 * @return true if the absolute difference is less than 10^-Prec.
 */
template <int Prec, class T1, class T2,
          std::enable_if_t<std::is_floating_point_v<T1> && std::is_floating_point_v<T2>>* = nullptr>
constexpr bool almost_eq(T1 lhs, T2 rhs) {
    using Common = std::common_type_t<T1, T2>;
    const auto val_lhs = static_cast<Common>(lhs);
    const auto val_rhs = static_cast<Common>(rhs);
    const auto diff = (val_lhs > val_rhs) ? (val_lhs - val_rhs) : (val_rhs - val_lhs);
    return diff < range_limit<Common>(Prec);
}
/**
 * @brief Compares two std::optional floating point values for equality.
 * * Logic follows these rules:
 * 1. If both contain values, performs almost_eq on the underlying values.
 * 2. If both are empty, they are considered equal (true).
 * 3. If only one is empty, they are not equal (false).
 * * @tparam Prec Decimal digits of precision.
 * @param lhs Optional value A.
 * @param rhs Optional value B.
 */
template <int Prec, class T> constexpr bool almost_eq(std::optional<T> const& lhs, std::optional<T> const& rhs) {
    if (lhs.has_value() and rhs.has_value()) {
        return almost_eq<Prec>(lhs.value(), rhs.value());
    }
    return lhs.has_value() == rhs.has_value();
}

/**
 * @brief Determines if the difference between two values is within a specific noise level.
 * * Useful for filtering out jitter in sensor readings or small fluctuations
 * that should not trigger logic changes.
 * * @param val_a First value.
 * @param val_b Second value.
 * @param noise_level The maximum allowed difference to be considered "noise".
 * @return true if |val_a - val_b| <= noise_level.
 */
template <class T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
bool in_noise_range(T val_a, T val_b, T noise_level) {
    return std::abs(val_a - val_b) <= noise_level;
}

// --- Optional Math Utilities (Minimum) ---

/**
 * @brief Finds the minimum between two std::optional values.
 * * In this context, an empty std::optional is treated as "no limit" or "infinity".
 * * @param a First optional value.
 * @param b Second optional value.
 * @return The smaller of the two if both exist, otherwise the existing value.
 */
template <class T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
std::optional<T> min_optional(std::optional<T> const& a, std::optional<T> const& b) {
    if (not a.has_value()) {
        return b;
    }
    if (not b.has_value()) {
        return a;
    }
    return std::min(a.value(), b.value());
}

template <class T> T min_optional(T a, std::optional<T> const& b) {
    if (b.has_value() and b.value() < a) {
        return b.value();
    }
    return a;
}

template <class T> T min_optional(std::optional<T> const& a, T b) {
    if (a.has_value() and a.value() < b) {
        return a.value();
    }
    return b;
}

// --- Optional Math Utilities (Maximum) ---

/**
 * @brief Finds the maximum between two std::optional values.
 * * In this context, an empty std::optional is treated as "negative infinity".
 * * @param a First optional value.
 * @param b Second optional value.
 * @return The larger of the two if both exist, otherwise the existing value.
 */
template <class T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
std::optional<T> max_optional(std::optional<T> const& a, std::optional<T> const& b) {
    if (not a.has_value()) {
        return b;
    }
    if (not b.has_value()) {
        return a;
    }
    return std::max(a.value(), b.value());
}

template <class T> T max_optional(T a, std::optional<T> const& b) {
    if (b.has_value() and b.value() > a) {
        return b.value();
    }
    return a;
}

template <class T> T max_optional(std::optional<T> const& a, T b) {
    if (a.has_value() and a.value() > b) {
        return a.value();
    }
    return b;
}

// --- Optional Math Utilities (Clamping) ---

/**
 * @brief Clamps a value between an optional minimum and an optional maximum.
 * * If a bound is empty, it is ignored (e.g., if min is empty, only the max is applied).
 * * @param v The value to clamp.
 * @param min The lower bound constraint.
 * @param max The upper bound constraint.
 * @return The clamped value.
 */
template <class T> T clamp_optional(T v, std::optional<T> const& min, std::optional<T> const& max) {
    T result = v;
    if (min.has_value() and result < min.value()) {
        result = min.value();
    }
    if (max.has_value() and result > max.value()) {
        result = max.value();
    }
    return result;
}

} // namespace everest::lib::util
