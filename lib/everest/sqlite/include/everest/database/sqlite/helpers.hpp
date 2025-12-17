// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <limits>

namespace everest::db::sqlite {
template <typename T, typename U> T constexpr clamp_to(U len) {
    return (len <= std::numeric_limits<T>::max()) ? static_cast<T>(len) : std::numeric_limits<T>::max();
}
} // namespace everest::db::sqlite
