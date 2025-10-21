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

template <int Prec, class T1, class T2>
constexpr auto almost_eq(T1 lhs, T2 rhs) ->
    typename std::enable_if<std::is_floating_point<T1>::value and std::is_floating_point<T2>::value, bool>::type {
    using CommonType = typename std::common_type<T1, T2>::type;
    constexpr auto range = range_limit<CommonType>(Prec);

    const auto common_lhs = static_cast<CommonType>(lhs);
    const auto common_rhs = static_cast<CommonType>(rhs);

    return common_lhs > common_rhs - range and common_lhs < common_rhs + range;
}

template <int Prec, class T> constexpr bool almost_eq(std::optional<T> const& lhs, std::optional<T> const& rhs) {
    if (lhs.has_value() and rhs.has_value()) {
        return almost_eq<Prec, T>(lhs.value(), rhs.value());
    }
    if (not lhs.has_value() and not rhs.has_value()) {
        return true;
    }
    return false;
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
