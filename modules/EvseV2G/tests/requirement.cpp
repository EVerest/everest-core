// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <string>

#include "utils/types.hpp"

Requirement::Requirement(const std::string& requirement_id_, size_t index_) {
}
bool Requirement::operator<(const Requirement& rhs) const {
    return true;
}
