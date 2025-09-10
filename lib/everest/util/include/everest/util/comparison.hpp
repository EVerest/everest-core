// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include <cmath>
#include <optional>
#include <type_traits>

namespace everest::lib::util {

template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
constexpr T range_limit(int digits_of_precision) {
    constexpr auto factor = 0.1;
    if (digits_of_precision <= 0) {
        return 1;
    }
    if (digits_of_precision == 1) {
        return factor;
    }
    return factor * range_limit<T>(digits_of_precision - 1);
}

template <int TPrec, class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
constexpr bool almost_eq(float lhs, float rhs) {
    constexpr T range = range_limit<T>(TPrec);
    return lhs > rhs - range and lhs < rhs + range;
}

template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
bool in_noise_range(T val_a, T val_b, T noise_level) {
    return (std::fabs(val_a - val_b) <= noise_level);
}

template <class T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
std::optional<T> min_optional(std::optional<T> const& a, std::optional<T> const& b) {
    return std::min(a, b, [](std::optional<T> const& aa, std::optional<T> const& bb) {
        return aa.value_or(std::numeric_limits<T>::max()) < bb.value_or(std::numeric_limits<T>::max());
    });
}

template <class T> T min_optional(T a, std::optional<T> const& b) {
    return min_optional(std::make_optional(a), b).value();
}

template <class T> T min_optional(std::optional<T> const& a, T b) {
    return min_optional(a, std::make_optional(b)).value();
}

} // namespace everest::lib::util
