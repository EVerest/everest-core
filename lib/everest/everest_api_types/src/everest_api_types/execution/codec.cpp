// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "execution/codec.hpp"
#include "execution/API.hpp"
#include "execution/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::execution {

std::string serialize(StopModulesResultEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(StartModulesResultEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModuleExecutionStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(StopModulesResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(StartModulesResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ExecutionStatusUpdateNotice const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, StopModulesResultEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StartModulesResultEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ModuleExecutionStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StopModulesResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StartModulesResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ExecutionStatusUpdateNotice const& val) {
    os << serialize(val);
    return os;
}

template <> StopModulesResultEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    StopModulesResultEnum obj = data;
    return obj;
}

template <> StartModulesResultEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    StartModulesResultEnum obj = data;
    return obj;
}

template <> ModuleExecutionStatusEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ModuleExecutionStatusEnum obj = data;
    return obj;
}

template <> StopModulesResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    StopModulesResult obj = data;
    return obj;
}

template <> StartModulesResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    StartModulesResult obj = data;
    return obj;
}

template <> ExecutionStatusUpdateNotice deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ExecutionStatusUpdateNotice obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::execution
