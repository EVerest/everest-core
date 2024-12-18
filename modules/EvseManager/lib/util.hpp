// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

namespace ev::util {

template <typename Matcher, typename... Args> auto find_first_match(Matcher&& matcher, Args&&... sets) {
    typename Matcher::return_type match;
    // the following line makes use of short-circuiting in boolean expression
    (((match = matcher(std::forward<Args>(sets))) || ...));
    return match;
}

} // namespace ev::util