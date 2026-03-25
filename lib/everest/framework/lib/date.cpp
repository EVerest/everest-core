// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest
#include <charconv>
#include <string_view>

#include <utils/date.hpp>

namespace Everest {
namespace Date {

std::string to_rfc3339(const std::chrono::time_point<date::utc_clock>& t) {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(t));
}

std::chrono::time_point<date::utc_clock> from_rfc3339_slow(const std::string& t) {
    std::istringstream infile{t};
    std::chrono::time_point<date::utc_clock> tp;
    infile >> date::parse("%FT%T", tp);
    return tp;
}

constexpr std::size_t typical_len = 24;

std::chrono::time_point<date::utc_clock> from_rfc3339(const std::string& t) {
    std::string_view input(t);
    // attempt a slow parse if input size doesn't match our expectations
    if (input.size() < typical_len) {
        return from_rfc3339_slow(t);
    }
    std::string_view y_str = input.substr(0, 4);
    std::string_view m_str = input.substr(5, 2);
    std::string_view d_str = input.substr(8, 2);
    std::string_view h_str = input.substr(11, 2);
    std::string_view M_str = input.substr(14, 2);
    std::string_view s_str = input.substr(17, 2);
    std::string_view ms_str = input.substr(20, 3);

    int y;
    {
        auto [ptr, ec] = std::from_chars(y_str.data(), y_str.data() + y_str.size(), y);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int m;
    {
        auto [ptr, ec] = std::from_chars(m_str.data(), m_str.data() + m_str.size(), m);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int d;
    {
        auto [ptr, ec] = std::from_chars(d_str.data(), d_str.data() + d_str.size(), d);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int h;

    {
        auto [ptr, ec] = std::from_chars(h_str.data(), h_str.data() + h_str.size(), h);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int M;
    {
        auto [ptr, ec] = std::from_chars(M_str.data(), M_str.data() + M_str.size(), M);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int s;
    {
        auto [ptr, ec] = std::from_chars(s_str.data(), s_str.data() + s_str.size(), s);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    int ms;
    {
        auto [ptr, ec] = std::from_chars(ms_str.data(), ms_str.data() + ms_str.size(), ms);
        if (ec != std::errc{}) {
            return from_rfc3339_slow(t);
        }
    }

    using namespace date;
    using namespace std::chrono;
    return utc_clock::from_sys(sys_days{year{y} / m / d} + hours{h} + minutes{M} + seconds{s} + milliseconds{ms});
}

} // namespace Date
} // namespace Everest
