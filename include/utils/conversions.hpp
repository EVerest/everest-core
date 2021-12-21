// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONVERSIONS_HPP
#define UTILS_CONVERSIONS_HPP

#include <nlohmann/json.hpp>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

template <class R, class V> R convertTo(V);

template <> json convertTo<json>(Result retval);
template <> Result convertTo<Result>(json data);
template <> json convertTo<json>(Parameters params);
template <> Parameters convertTo<Parameters>(json data);
template <> json convertTo<json>(Value value);
template <> Value convertTo<Value>(json data);

} // namespace Everest

#endif // UTILS_CONVERSIONS_HPP
