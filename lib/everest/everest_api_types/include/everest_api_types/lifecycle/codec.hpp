// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::lifecycle {

std::string serialize(StopModulesResultEnum val) noexcept;
std::string serialize(StartModulesResultEnum val) noexcept;
std::string serialize(ModuleExecutionStatusEnum val) noexcept;
std::string serialize(StopModulesResult const& val) noexcept;
std::string serialize(StartModulesResult const& val) noexcept;
std::string serialize(ExecutionStatusUpdateNotice const& val) noexcept;

std::ostream& operator<<(std::ostream& os, StopModulesResultEnum const& val);
std::ostream& operator<<(std::ostream& os, StartModulesResultEnum const& val);
std::ostream& operator<<(std::ostream& os, ModuleExecutionStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, StopModulesResult const& val);
std::ostream& operator<<(std::ostream& os, StartModulesResult const& val);
std::ostream& operator<<(std::ostream& os, ExecutionStatusUpdateNotice const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::lifecycle
