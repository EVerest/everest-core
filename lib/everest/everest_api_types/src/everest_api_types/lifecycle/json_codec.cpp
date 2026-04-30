// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "lifecycle/json_codec.hpp"
#include "lifecycle/API.hpp"
#include "lifecycle/codec.hpp"

#include "nlohmann/json.hpp"
#include <stdexcept>

namespace everest::lib::API::V1_0::types::lifecycle {

void to_json(json& j, StopModulesResultEnum const& k) noexcept {
    switch (k) {
    case StopModulesResultEnum::Stopping:
        j = "Stopping";
        return;
    case StopModulesResultEnum::NoModulesToStop:
        j = "NoModulesToStop";
        return;
    case StopModulesResultEnum::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::lifecycle::StopModulesResultEnum";
}

void from_json(const json& j, StopModulesResultEnum& k) {
    std::string s = j;
    if (s == "Stopping") {
        k = StopModulesResultEnum::Stopping;
        return;
    }
    if (s == "NoModulesToStop") {
        k = StopModulesResultEnum::NoModulesToStop;
        return;
    }
    if (s == "Rejected") {
        k = StopModulesResultEnum::Rejected;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::lifecycle::StopModulesResultEnum");
}

void to_json(json& j, StartModulesResultEnum const& k) noexcept {
    switch (k) {
    case StartModulesResultEnum::Starting:
        j = "Starting";
        return;
    case StartModulesResultEnum::Restarting:
        j = "Restarting";
        return;
    case StartModulesResultEnum::NoConfigToStart:
        j = "NoConfigToStart";
        return;
    case StartModulesResultEnum::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::lifecycle::StartModulesResultEnum";
}

void from_json(const json& j, StartModulesResultEnum& k) {
    std::string s = j;
    if (s == "Starting") {
        k = StartModulesResultEnum::Starting;
        return;
    }
    if (s == "Restarting") {
        k = StartModulesResultEnum::Restarting;
        return;
    }
    if (s == "NoConfigToStart") {
        k = StartModulesResultEnum::NoConfigToStart;
        return;
    }
    if (s == "Rejected") {
        k = StartModulesResultEnum::Rejected;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::lifecycle::StartModulesResultEnum");
}

void to_json(json& j, ModuleExecutionStatusEnum const& k) noexcept {
    switch (k) {
    case ModuleExecutionStatusEnum::Running:
        j = "Running";
        return;
    case ModuleExecutionStatusEnum::NotRunning:
        j = "NotRunning";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::lifecycle::ExecutionStatusEnum";
}

void from_json(const json& j, ModuleExecutionStatusEnum& k) {
    std::string s = j;
    if (s == "Running") {
        k = ModuleExecutionStatusEnum::Running;
        return;
    }
    if (s == "NotRunning") {
        k = ModuleExecutionStatusEnum::NotRunning;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::lifecycle::ExecutionStatusEnum");
}

void to_json(json& j, StopModulesResult const& k) noexcept {
    j = json{{"status", k.status}};
}

void from_json(const json& j, StopModulesResult& k) {
    k.status = j.at("status");
}

void to_json(json& j, StartModulesResult const& k) noexcept {
    j = json{{"status", k.status}};
}

void from_json(const json& j, StartModulesResult& k) {
    k.status = j.at("status");
}

void to_json(json& j, ExecutionStatusUpdateNotice const& k) noexcept {
    j = json{{"tstamp", k.tstamp}, {"status", k.status}, {"configuration_api_available", k.configuration_api_available}};
}

void from_json(const json& j, ExecutionStatusUpdateNotice& k) {
    k.tstamp = j.at("tstamp");
    k.status = j.at("status");
    k.configuration_api_available = j.at("configuration_api_available");
}

} // namespace everest::lib::API::V1_0::types::lifecycle
