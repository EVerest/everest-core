// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_JSON_HPP
#define UTILS_ERROR_JSON_HPP

#include <utils/error.hpp>

namespace Everest {
namespace error {

Error json_to_error(const json& j);
json error_to_json(const Error& e);

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_JSON_HPP
