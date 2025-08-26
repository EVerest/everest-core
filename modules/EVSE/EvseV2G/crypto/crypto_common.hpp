// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef CRTYPTO_COMMON_HPP_
#define CRTYPTO_COMMON_HPP_

#include <openssl_util.hpp>

#include <cstddef>

namespace crypto {

using verify_result_t = openssl::verify_result_t;

constexpr std::size_t MAX_EXI_SIZE = 8192;

} // namespace crypto

#endif // CRTYPTO_COMMON_HPP_
