// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>

// forward declare
class SessionLog;

class SessionLogger {
public:
    SessionLogger(std::filesystem::path output_dir);
    ~SessionLogger();

private:
    std::filesystem::path output_dir;
    std::map<std::uintptr_t, std::unique_ptr<SessionLog>> logs;
};
