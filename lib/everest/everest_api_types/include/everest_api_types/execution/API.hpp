// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <string>

namespace everest::lib::API::V1_0::types::execution {

enum class StopModulesResultEnum {
    Stopping,
    NoModulesToStop,
    Rejected,
};

enum class StartModulesResultEnum {
    Starting,
    Restarting,
    NoConfigToStart,
    Rejected,
};

enum class ModuleExecutionStatusEnum {
    Running,
    NotRunning,
};

struct StopModulesResult {
    StopModulesResultEnum status;
};

struct StartModulesResult {
    StartModulesResultEnum status;
};

struct ExecutionStatusUpdateNotice {
    std::string tstamp{};
    ModuleExecutionStatusEnum status;
    bool configuration_api_available;
};

} // namespace everest::lib::API::V1_0::types::execution
