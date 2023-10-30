// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONVERSIONS_HPP
#define UTILS_CONVERSIONS_HPP

#include <variant>

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

namespace detail {

template <typename FundamentalType> constexpr bool is_type_compatible(nlohmann::json::value_t json_type);

template <> constexpr inline bool is_type_compatible<std::nullptr_t>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::null;
}

template <> constexpr inline bool is_type_compatible<bool>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::boolean;
}

template <> constexpr inline bool is_type_compatible<int>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::number_integer ||
           json_type == nlohmann::json::value_t::number_unsigned;
}

template <> constexpr inline bool is_type_compatible<double>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::number_float;
}

template <> constexpr inline bool is_type_compatible<std::string>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::string;
}

template <> constexpr inline bool is_type_compatible<nlohmann::json::array_t>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::array;
}

template <> constexpr inline bool is_type_compatible<nlohmann::json::object_t>(nlohmann::json::value_t json_type) {
    return json_type == nlohmann::json::value_t::object;
}

template <typename T> bool json_to_variant_impl(T& to, const nlohmann::json& from) noexcept {
    return false;
}

template <typename VariantType, typename CurrentType, typename... Rest>
bool json_to_variant_impl(VariantType& to, const nlohmann::json& from) noexcept {
    if (is_type_compatible<CurrentType>(from.type())) {
        to = from.get<CurrentType>();
        return true;
    }

    return json_to_variant_impl<VariantType, Rest...>(to, from);
}

} // namespace detail

template <typename... Ts> static std::variant<Ts...> json_to_variant(const nlohmann::json& j) {
    std::variant<Ts...> var;

    if (detail::json_to_variant_impl<std::variant<Ts...>, Ts...>(var, j)) {
        return var;
    }

    throw std::runtime_error("The given json object doesn't contain any type, the std::variant is aware of");
}

template <typename T> nlohmann::json variant_to_json(T variant) {
    return std::visit([](auto&& arg) -> nlohmann::json { return arg; }, variant);
}

} // namespace Everest

#endif // UTILS_CONVERSIONS_HPP
