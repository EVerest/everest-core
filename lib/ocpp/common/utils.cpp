// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/algorithm/string/predicate.hpp>

#include <ocpp/common/utils.hpp>

namespace ocpp {

bool iequals(const std::string& lhs, const std::string rhs) {
    return boost::algorithm::iequals(lhs, rhs);
}

} // namespace ocpp
