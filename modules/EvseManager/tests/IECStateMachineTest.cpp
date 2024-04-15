// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "evse_board_supportIntfStub.hpp"
#include <EventQueue.hpp>
#include <IECStateMachine.hpp>
#include <backtrace.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>

namespace {

using BspEvent = types::board_support_common::BspEvent;
using Event = types::board_support_common::Event;
using Reason = types::evse_board_support::Reason;

using namespace std::chrono_literals;

struct BspStub : public module::stub::ModuleAdapterStub {
    typedef Result (BspStub::*bsp_fn)(Parameters p);
    ValueCallback event_cb;
    std::map<std::string, bsp_fn> _bsp;

    BspStub() : module::stub::ModuleAdapterStub() {
        _bsp["allow_power_on"] = &BspStub::call_allow_power_on;
        _bsp["enable"] = &BspStub::call_enable;
        _bsp["pwm_off"] = &BspStub::call_pwm_off;
        _bsp["pwm_on"] = &BspStub::call_pwm_on;
    }

    // ------------------------------------------------------------------------
    // test interface (calls from BSP)

    void raise_event(Event event) {
        if (event_cb != nullptr) {
            BspEvent bsp_event;
            bsp_event.event = event;
            event_cb(bsp_event);
        }
    };

    // ------------------------------------------------------------------------
    // BSP calls

    virtual Result call_allow_power_on(Parameters p) {
        std::cout << "call_allow_power_on(" << p << ")" << std::endl;
        return std::nullopt;
    }

    virtual Result call_enable(Parameters p) {
        std::cout << "call_enable(" << p << ")" << std::endl;
        return std::nullopt;
    }

    virtual Result call_pwm_off(Parameters p) {
        std::cout << "call_pwm_off(" << p << ")" << std::endl;
        return std::nullopt;
    }

    virtual Result call_pwm_on(Parameters p) {
        std::cout << "call_pwm_on(" << p << ")" << std::endl;
        return std::nullopt;
    }

    // ------------------------------------------------------------------------
    // internal interface
    virtual void subscribe_fn(const Requirement&, const std::string& fn, ValueCallback cb) override {
        std::printf("subscribe_fn(%s)\n", fn.c_str());
        event_cb = cb;
    }

    virtual Result call_fn(const Requirement& req, const std::string& fn, Parameters p) override {
        if (auto itt = _bsp.find(fn); itt == _bsp.end()) {
            std::cout << "<missing> call_fn(" << fn << "," << p << ")" << std::endl;
            return std::nullopt;
        } else {
            return std::invoke(itt->second, this, p);
        }
    }
};

TEST(IECStateMachine, init) {
    std::unique_ptr<evse_board_supportIntf> bsp_if = std::make_unique<module::stub::evse_board_supportIntfStub>();
    module::IECStateMachine state_machine(std::move(bsp_if));
}

#if 0
// test to demonstrate the output from backtrace

struct BspStubTimeout : public BspStub {
    Result call_allow_power_on(Parameters p) {
        std::cout << "call_allow_power_on(" << p << ")" << std::endl;
        std::cout << "call_allow_power_on: sleep (" << std::this_thread::get_id() << ")" << std::endl;
        std::this_thread::sleep_for(200s);
        std::cout << "call_allow_power_on: finished" << std::endl;
        return std::nullopt;
    }
};

TEST(IECStateMachine, init_subscribe) {
#ifdef EVEREST_USE_BACKTRACES
    Everest::install_backtrace_handler();
#endif

    BspStubTimeout bsp;
    std::unique_ptr<evse_board_supportIntf> bsp_if = std::make_unique<module::stub::evse_board_supportIntfStub>(bsp);
    module::IECStateMachine state_machine(std::move(bsp_if));

    state_machine.enable(true);
    state_machine.allow_power_on(true, Reason::FullPowerCharging);
    bsp.raise_event(Event::A);
    bsp.raise_event(Event::B);
    bsp.raise_event(Event::PowerOn);
    bsp.raise_event(Event::C);
    std::cout << "main: sleep (" << std::this_thread::get_id() << ")" << std::endl;
    std::this_thread::sleep_for(300s);
}
#endif

struct BspStubDeadlock : public BspStub {
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> call_pwm_off_times;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> call_allow_power_on_times;
    int count = 0;
    enum class events_t : std::uint8_t {
        call_allow_power_on,
        call_enable,
        call_pwm_off,
        call_pwm_on,
        signal_lock,
        sleeping,
    };
    module::EventQueue<events_t> events;

    std::string to_string(module::EventQueue<events_t>::events_t e) {
        std::string result;
        for (const auto& i : e) {
            result += std::to_string(static_cast<std::uint8_t>(i));
            result += " ";
        }
        return result;
    }

    bool contains(module::EventQueue<events_t>::events_t e, events_t event) {
        for (const auto& i : e) {
            if (i == event) {
                return true;
            }
        }
        return false;
    }

    auto wait() {
        return events.wait();
    }

    virtual Result call_allow_power_on(Parameters p) {
        std::cout << "call_allow_power_on(" << p << ")" << std::endl;
        call_allow_power_on_times.push_back(std::chrono::steady_clock::now());
        events.push(events_t::call_allow_power_on);
        return std::nullopt;
    }

    virtual Result call_enable(Parameters p) {
        std::cout << "call_enable(" << p << ")" << std::endl;
        events.push(events_t::call_enable);
        return std::nullopt;
    }

    virtual Result call_pwm_off(Parameters p) {
        std::cout << "call_pwm_off(" << p << ")" << std::endl;
        call_pwm_off_times.push_back(std::chrono::steady_clock::now());
        if (count == 2) {
            std::cout << "sleeping ..." << std::endl;
            events.push(events_t::sleeping);
            std::this_thread::sleep_for(7s);
        }
        count++;
        events.push(events_t::call_pwm_off);
        return std::nullopt;
    }

    virtual Result call_pwm_on(Parameters p) {
        std::cout << "call_pwm_on(" << p << ")" << std::endl;
        events.push(events_t::call_pwm_on);
        return std::nullopt;
    }

    auto elapsed() const {
        auto last_call_pwm_off_time = call_pwm_off_times.back();
        auto last_call_allow_power_on_time = call_allow_power_on_times.back();
        return std::chrono::duration_cast<std::chrono::milliseconds>(last_call_allow_power_on_time -
                                                                     last_call_pwm_off_time)
            .count();
    }
};

TEST(IECStateMachine, deadlock_test) {
    GTEST_SKIP() << "relies on thread timing so occasionally fails";

    /*
     * a deadlock was caused by timeout_state_c1 timing out and
     * trying to raise an event while state_machine() was running
     * and wanting to stop the timer
     * 61851-1 state C starts the timer (from x2)
     *
     * check the timer runs for 6 seconds
     */
#ifdef EVEREST_USE_BACKTRACES
    Everest::install_backtrace_handler();
#endif

    BspStubDeadlock bsp;
    std::unique_ptr<evse_board_supportIntf> bsp_if = std::make_unique<module::stub::evse_board_supportIntfStub>(bsp);
    module::IECStateMachine state_machine(std::move(bsp_if));

    std::uint8_t signal_lock_count = 0;

    state_machine.signal_lock.connect([&signal_lock_count, &bsp]() {
        signal_lock_count++;
        std::cout << "signal_lock" << std::endl;
        // bsp.events.push(BspStubDeadlock::events_t::signal_lock);
    });

    state_machine.enable(true);
    auto actions = bsp.wait();
    ASSERT_EQ(actions.size(), 1);
    ASSERT_EQ(actions[0], BspStubDeadlock::events_t::call_enable);

    bsp.raise_event(Event::A);
    actions = bsp.wait();
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_pwm_off));

    bsp.raise_event(Event::B);
    actions = bsp.wait();
    // std::cout << bsp.to_string(actions) << std::endl;
    // call_allow_power_on and/or signal_lock
    // ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_allow_power_on));

    state_machine.set_pwm(0.5);
    actions = bsp.wait();
    // std::cout << bsp.to_string(actions) << std::endl;
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_pwm_on));

    state_machine.allow_power_on(true, types::evse_board_support::Reason::FullPowerCharging);
    std::this_thread::sleep_for(1s);
    // actions = bsp.wait();
    // ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::signal_lock));

    bsp.raise_event(Event::C);
    actions = bsp.wait();
    // std::cout << bsp.to_string(actions) << std::endl;
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_allow_power_on));

    state_machine.set_pwm_off();
    // expect state_machine to run once and then again when the
    // timer expires

    std::this_thread::sleep_for(8s);
    auto elapsed = bsp.elapsed();
    std::cout << "Elapsed: " << elapsed << std::endl;
    EXPECT_GT(elapsed, 6000);
    EXPECT_LT(elapsed, 6500);
}

TEST(IECStateMachine, deadlock_fix) {
    GTEST_SKIP() << "relies on thread timing so occasionally fails";

    /*
     * a deadlock was caused by timeout_state_c1 timing out and
     * trying to raise an event while state_machine() was running
     * and wanting to stop the timer
     * 61851-1 state C starts the timer (from X2)
     */
#ifdef EVEREST_USE_BACKTRACES
    Everest::install_backtrace_handler();
#endif

    BspStubDeadlock bsp;
    std::unique_ptr<evse_board_supportIntf> bsp_if = std::make_unique<module::stub::evse_board_supportIntfStub>(bsp);
    module::IECStateMachine state_machine(std::move(bsp_if));

    std::uint8_t signal_lock_count = 0;

    state_machine.signal_lock.connect([&signal_lock_count, &bsp]() {
        signal_lock_count++;
        std::cout << "signal_lock" << std::endl;
        // bsp.events.push(BspStubDeadlock::events_t::signal_lock);
    });

    state_machine.enable(true);
    auto actions = bsp.wait();
    ASSERT_EQ(actions.size(), 1);
    ASSERT_EQ(actions[0], BspStubDeadlock::events_t::call_enable);

    bsp.raise_event(Event::A);
    actions = bsp.wait();
    // std::cout << bsp.to_string(actions) << std::endl;
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_pwm_off));

    bsp.raise_event(Event::B);
    actions = bsp.wait();
    // call_allow_power_on and/or signal_lock
    // ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_allow_power_on));

    state_machine.set_pwm(0.5);
    actions = bsp.wait();
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_pwm_on));

    state_machine.allow_power_on(true, types::evse_board_support::Reason::FullPowerCharging);
    std::this_thread::sleep_for(1s);

    bsp.raise_event(Event::C);
    actions = bsp.wait();
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_allow_power_on));

    state_machine.set_pwm_off();
    // expect state_machine to run once and then again when the
    // timer expires

    actions = bsp.wait();
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::call_pwm_off));

    // this would previously trigger the deadlock
    bsp.raise_event(Event::A);
    actions = bsp.wait();
    // std::cout << bsp.to_string(actions) << std::endl;
    ASSERT_TRUE(bsp.contains(actions, BspStubDeadlock::events_t::sleeping));

    std::this_thread::sleep_for(10s);
    // if there is a deadlock the test won't finish
}

} // namespace
