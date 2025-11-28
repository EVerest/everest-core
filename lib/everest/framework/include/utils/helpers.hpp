// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <limits>

#include <utils/types.hpp>

namespace Everest::helpers {
template <typename T, typename U> T constexpr clamp_to(U len) {
    return (len <= std::numeric_limits<T>::max()) ? static_cast<T>(len) : std::numeric_limits<T>::max();
}
} // namespace Everest::helpers
