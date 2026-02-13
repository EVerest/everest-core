// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>

namespace ieee2030::charger {

namespace callback {

enum class Signal {
    START_CABLE_CHECK,
    CHARGE_LOOP_STARTED,
    CHARGE_LOOP_FINISHED,
};

enum class ChargerSequence {
    CS1,
    CS2,
};

enum class Status : bool {
    OFF = false,
    ON = true
};

struct HwSignal {
    ChargerSequence signal;
    Status status;
};

struct Callbacks {
    std::function<void(Signal)> signal;
    std::function<void(const HwSignal&)> hw_signal;
};

} // namespace callback

class Callback {
public:
    Callback(callback::Callbacks);

    void signal(callback::Signal) const;
    void hw_signal(const callback::HwSignal&) const;

private:
    callback::Callbacks callbacks;
};

} // namespace ieee2030::charger