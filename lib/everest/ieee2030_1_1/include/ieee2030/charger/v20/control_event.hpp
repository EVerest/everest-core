// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>

namespace ieee2030::charger::events {

class CS1 {
public:
    explicit CS1(bool status_) : status(status_) {
    }

    operator bool() const {
        return status;
    }

private:
    bool status;
};

class CS2 {
public:
    explicit CS2(bool status_) : status(status_) {
    }

    operator bool() const {
        return status;
    }

private:
    bool status;
};

class ProximityDetection {
public:
    explicit ProximityDetection(bool status_) : status(status_) {
    }

    operator bool() const {
        return status;
    }

private:
    bool status;
};

class ChargePermission {
public:
    explicit ChargePermission(bool status_) : status(status_) {
    }

    operator bool() const {
        return status;
    }

private:
    bool status;
};

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

struct PresentVoltageCurrent {
    float voltage;
    float current;
};

struct AvailableVoltageCurrent {
    float voltage;
    float current;
};

enum class IsolationStatus {
    Invalid,
    Valid,
    Fault,
};

enum class Error {
    Test,
};

using Event = std::variant<CS1, CS2, ProximityDetection, ChargePermission, CableCheckFinished, StopCharging,
                           PresentVoltageCurrent, AvailableVoltageCurrent, IsolationStatus, Error>;
} // namespace ieee2030::charger::events
