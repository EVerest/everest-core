// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_FORMATTER_HPP
#define UTILS_FORMATTER_HPP

#include <atomic>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <string>

template <> struct fmt::formatter<nlohmann::json> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();

        if (it != end && *it != '}') {
            fmt::throw_format_error("Invalid format");
        }

        return it;
    }

    auto format(const nlohmann::json& j, fmt::format_context& ctx) const -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "{}", j.dump());
    }
};

template <> struct fmt::formatter<std::atomic_bool> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();

        if (it != end && *it != '}') {
            fmt::throw_format_error("Invalid format");
        }

        return it;
    }

    auto format(const std::atomic_bool& a, fmt::format_context& ctx) const -> fmt::format_context::iterator {
        if (a) {
            return fmt::format_to(ctx.out(), "true");
        }
        return fmt::format_to(ctx.out(), "false");
    }
};

template <typename T> struct fmt::formatter<std::atomic<T>> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();

        if (it != end && *it != '}') {
            fmt::throw_format_error("Invalid format");
        }

        return it;
    }

    auto format(const std::atomic<T>& a, fmt::format_context& ctx) const -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "{}", a.load());
    }
};

#endif // UTILS_FORMATTER_HPP
