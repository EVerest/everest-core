// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include <ieee2030/charger/session/callback.hpp>

using namespace ieee2030::charger;

callback::Signal signal;
callback::HwSignal hw_signal;

SCENARIO("Testing callback") {
    GIVEN("Signal") {

        callback::Callbacks callbacks_functions;

        callbacks_functions.signal = [](callback::Signal signal_) { signal = signal_; };

        auto callback = Callback(callbacks_functions);

        callback.signal(callback::Signal::CHARGE_LOOP_STARTED);

        THEN("Signal should be CHARGE_LOOP_STARTED") {
            REQUIRE(signal == callback::Signal::CHARGE_LOOP_STARTED);
        }
    }

    GIVEN("HwSignal") {

        callback::Callbacks callbacks_functions;

        callbacks_functions.hw_signal = [](callback::HwSignal hw_signal_) { hw_signal = hw_signal_; };

        auto callback = Callback(callbacks_functions);

        callback.hw_signal({callback::ChargerSequence::CS1, callback::Status::ON});

        THEN("Hw Signal should be enabled CS1") {
            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::ON);
        }
    }
}