// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_test_macros.hpp>

#include "helper.hpp"

#include <ieee2030/charger/v20/state/state_b.hpp>
#include <ieee2030/charger/v20/state/state_c.hpp>

using namespace ieee2030;

callback::Callbacks callbacks;

callback::HwSignal hw_signal;

SCENARIO("StateB state transitions") {

    callbacks.signal = []([[maybe_unused]] callback::Signal signal_) {};
    callbacks.hw_signal = [](callback::HwSignal hw_signal_) { hw_signal = hw_signal_; };

    hw_signal = callback::HwSignal();
    auto state_helper = FsmStateHelper(callbacks);
    auto ctx = state_helper.get_context();

    GIVEN("Handle Timeout") {
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        const auto result = fsm.feed(v20::Event::TIMEOUT);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateB);

            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::OFF);
        }
    }

    GIVEN("Handle stop event") {
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        state_helper.handle_event(events::StopCharging{true});
        const auto result = fsm.feed(v20::Event::EVENT);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateB);

            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::OFF);
        }
    }

    GIVEN("Handle stop event - false alarm") {
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        state_helper.handle_event(events::StopCharging{false});
        const auto result = fsm.feed(v20::Event::EVENT);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateB);

            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::ON);
        }
    }

    GIVEN("Handle first can message") {
        const auto message_100 = ieee2030::messages::EV100();
        const auto message_101 = ieee2030::messages::EV101();
        const auto message_102 = ieee2030::messages::EV102();
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        state_helper.handle_can_message(message_100, message_101, message_102);
        const auto result = fsm.feed(v20::Event::CAN_MESSAGE);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == true);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateC);
        }
    }

    GIVEN("Handle random HW Signal") {
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        state_helper.handle_event(events::ChargePermission{true});
        const auto result = fsm.feed(v20::Event::EVENT);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateB);

            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::ON);
        }
    }

    GIVEN("Handle random event") {
        fsm::v2::FSM<charger::v20::StateBase> fsm{ctx.create_state<charger::v20::state::StateB>()};
        state_helper.handle_event(events::CableCheckFinished{true});
        const auto result = fsm.feed(v20::Event::EVENT);

        THEN("Check state transititon") {
            REQUIRE(result.transitioned() == false);
            REQUIRE(fsm.get_current_state_id() == v20::StateID::StateB);

            REQUIRE(hw_signal.signal == callback::ChargerSequence::CS1);
            REQUIRE(hw_signal.status == callback::Status::ON);
        }
    }
}
