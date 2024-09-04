// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>

#include <iso15118/d20/limits.hpp>

namespace iso15118::d20 {
class CableCheckFinished {
public:
    explicit CableCheckFinished(bool success_) : success(success_) {
    }

    operator bool() const {
        return success;
    }

private:
    bool success;
};

struct PresentVoltageCurrent {
    float voltage;
    float current;
};

class AuthorizationResponse {
public:
    explicit AuthorizationResponse(bool authorized_) : authorized(authorized_) {
    }

    operator bool() const {
        return authorized;
    }

private:
    bool authorized;
};

class StopCharging {
public:
    explicit StopCharging(bool stop_) : stop(stop_) {
    }

    operator bool() const {
        return stop;
    }

private:
    bool stop;
};

using ControlEvent =
    std::variant<CableCheckFinished, PresentVoltageCurrent, AuthorizationResponse, StopCharging, DcTransferLimits>;

} // namespace iso15118::d20
