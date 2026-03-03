// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <LpcUseCaseHandler.hpp>

namespace module {

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class LpcStateMachineTest : public ::testing::Test {
protected:
    using TP = std::chrono::time_point<std::chrono::steady_clock>;

    // Start at an arbitrary non-min() time so that min()-sentinel comparisons work correctly.
    TP fake_time{TP::min() + std::chrono::hours(24)};

    static constexpr double FAILSAFE_LIMIT = 1000.0;
    static constexpr double MAX_NOMINAL_POWER = 22000.0;
    static constexpr double ACTIVE_LIMIT = 5000.0;

    std::vector<types::energy::ExternalLimits> applied_limits;

    std::unique_ptr<LpcUseCaseHandler> make_handler() {
        eebus::EEBusCallbacks callbacks;
        callbacks.update_limits_callback = [this](const types::energy::ExternalLimits& lim) {
            applied_limits.push_back(lim);
        };
        return std::make_unique<LpcUseCaseHandler>(FAILSAFE_LIMIT, MAX_NOMINAL_POWER, callbacks,
                                                   [this] { return fake_time; });
    }

    void advance(std::chrono::seconds s) {
        fake_time += s;
    }

    void advance(std::chrono::hours h) {
        fake_time += h;
    }

    // Send a heartbeat event (drives handle_data_update_heartbeat + run_state_machine).
    void send_heartbeat(LpcUseCaseHandler& h) {
        control_service::UseCaseEvent event;
        event.set_event("DataUpdateHeartbeat");
        h.handle_event(event);
    }

    // Drive a DataUpdateLimit event; the handler will try to read from its stub, which is null,
    // so we inject the limit directly via process_received_limit and then re-run the state machine.
    void send_active_limit(LpcUseCaseHandler& h, double watts) {
        common_types::LoadLimit limit;
        limit.set_is_active(true);
        limit.set_value(watts);
        limit.set_duration_nanoseconds(0); // no duration
        limit.set_delete_duration(true);
        h.process_received_limit(limit);
    }

    void send_deactivated_limit(LpcUseCaseHandler& h) {
        common_types::LoadLimit limit;
        limit.set_is_active(false);
        h.process_received_limit(limit);
    }

    // ---------- helpers to inspect the last applied limit ----------

    bool last_limit_is_unlimited() const {
        if (applied_limits.empty())
            return false;
        const auto& last = applied_limits.back();
        if (last.schedule_import.empty())
            return false;
        return !last.schedule_import[0].limits_to_root.total_power_W.has_value();
    }

    double last_limit_watts() const {
        if (applied_limits.empty())
            return -1.0;
        const auto& last = applied_limits.back();
        if (last.schedule_import.empty())
            return -1.0;
        if (!last.schedule_import[0].limits_to_root.total_power_W.has_value())
            return -1.0;
        return last.schedule_import[0].limits_to_root.total_power_W->value;
    }

    size_t limit_callback_count() const {
        return applied_limits.size();
    }

    // Reach Failsafe from Limited (heartbeat timeout path).
    void enter_failsafe(LpcUseCaseHandler& h) {
        h.start();
        send_heartbeat(h);
        send_active_limit(h, ACTIVE_LIMIT);
        advance(std::chrono::seconds(121)); // heartbeat timeout
        h.run_state_machine();
        ASSERT_EQ(h.get_state(), LpcUseCaseHandler::State::Failsafe);
    }
};

// ===========================================================================
// Bug 2 — Init state must immediately apply the failsafe limit [LPC-901/1]
// ===========================================================================

TEST_F(LpcStateMachineTest, applies_failsafe_limit_immediately_on_startup) {
    auto h = make_handler();
    h->start();

    ASSERT_GT(limit_callback_count(), 0u) << "Expected a limit callback at startup";
    EXPECT_DOUBLE_EQ(last_limit_watts(), FAILSAFE_LIMIT);
}

// ===========================================================================
// Bug 3 — Init must NOT transition to Failsafe on heartbeat timeout;
//          it must transition to Unlimited/autonomous after 120 s [LPC-906]
// ===========================================================================

TEST_F(LpcStateMachineTest, init_transitions_to_unlimited_autonomous_not_failsafe_after_120s) {
    auto h = make_handler();
    h->start();

    // Advance just past the 120 s Init timeout without any EG communication.
    advance(std::chrono::seconds(121));
    h->run_state_machine();

    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
    EXPECT_TRUE(last_limit_is_unlimited());
}

// ===========================================================================
// Bug 1 — Heartbeat timeout must be 120 s, not 60 s [LPC-911 / LPC-912]
// ===========================================================================

TEST_F(LpcStateMachineTest, does_not_enter_failsafe_before_120s_heartbeat_timeout) {
    auto h = make_handler();
    h->start();

    // Get to Limited state: heartbeat then active limit.
    send_heartbeat(*h);
    send_active_limit(*h, ACTIVE_LIMIT);
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);

    // At 119 s since last heartbeat — must NOT enter Failsafe.
    advance(std::chrono::seconds(119));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), ACTIVE_LIMIT);
}

TEST_F(LpcStateMachineTest, enters_failsafe_exactly_after_120s_heartbeat_timeout) {
    auto h = make_handler();
    h->start();

    send_heartbeat(*h);
    send_active_limit(*h, ACTIVE_LIMIT);
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);

    // At 121 s since last heartbeat — must enter Failsafe.
    advance(std::chrono::seconds(121));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);
    EXPECT_DOUBLE_EQ(last_limit_watts(), FAILSAFE_LIMIT);
}

// Same requirement from Unlimited/controlled (LPC-911).
TEST_F(LpcStateMachineTest, unlimited_controlled_enters_failsafe_after_120s_heartbeat_timeout) {
    auto h = make_handler();
    h->start();

    send_heartbeat(*h);
    send_deactivated_limit(*h);
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedControlled);

    advance(std::chrono::seconds(121));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);
    EXPECT_DOUBLE_EQ(last_limit_watts(), FAILSAFE_LIMIT);
}

// ===========================================================================
// Bug 4 — Unlimited/autonomous must NOT transition to Failsafe on heartbeat
//          timeout; it has no such outgoing edge in the spec state diagram.
// ===========================================================================

TEST_F(LpcStateMachineTest, unlimited_autonomous_stays_unlimited_when_heartbeat_absent) {
    auto h = make_handler();
    h->start();

    // Let Init timeout → Unlimited/autonomous.
    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // Advance well past any heartbeat timeout — state must not change to Failsafe.
    advance(std::chrono::seconds(300));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
    EXPECT_TRUE(last_limit_is_unlimited());
}

// ===========================================================================
// Bug 5 — Failsafe exit conditions must be independent [LPC-921 / LPC-922]
// ===========================================================================

// LPC-922: exit after Failsafe Duration Minimum expires (no heartbeat required).
TEST_F(LpcStateMachineTest, failsafe_exits_to_unlimited_autonomous_after_duration_minimum) {
    auto h = make_handler();
    enter_failsafe(*h);

    // Advance past the default 2-hour Failsafe Duration Minimum.
    advance(std::chrono::hours(2) + std::chrono::seconds(1));
    h->run_state_machine();

    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
    EXPECT_TRUE(last_limit_is_unlimited());
}

// LPC-922: must NOT exit before the 2-hour minimum even if heartbeat arrives.
TEST_F(LpcStateMachineTest, failsafe_does_not_exit_before_duration_minimum_expires) {
    auto h = make_handler();
    enter_failsafe(*h);

    // Just under 2 hours — no exit yet.
    advance(std::chrono::hours(2) - std::chrono::seconds(1));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);
}

// LPC-921: exit when heartbeat received but no limit write within 120 s.
TEST_F(LpcStateMachineTest, failsafe_exits_when_heartbeat_received_but_no_limit_within_120s) {
    auto h = make_handler();
    enter_failsafe(*h);

    // EG reconnects and sends a heartbeat, but does not send a new limit.
    advance(std::chrono::seconds(10));
    send_heartbeat(*h); // first post-failsafe heartbeat
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);

    // 119 s after that heartbeat — still in Failsafe.
    advance(std::chrono::seconds(119));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);

    // 121 s after that heartbeat — must exit to Unlimited/autonomous [LPC-921].
    advance(std::chrono::seconds(2));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
    EXPECT_TRUE(last_limit_is_unlimited());
}

// LPC-916 / LPC-918 / LPC-919: receiving heartbeat + limit in Failsafe transitions normally.
TEST_F(LpcStateMachineTest, failsafe_exits_to_limited_when_heartbeat_and_active_limit_received) {
    auto h = make_handler();
    enter_failsafe(*h);

    advance(std::chrono::seconds(10));
    send_heartbeat(*h);
    send_active_limit(*h, ACTIVE_LIMIT);

    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), ACTIVE_LIMIT);
}

TEST_F(LpcStateMachineTest, failsafe_exits_to_unlimited_controlled_when_heartbeat_and_deactivated_limit_received) {
    auto h = make_handler();
    enter_failsafe(*h);

    advance(std::chrono::seconds(10));
    send_heartbeat(*h);
    send_deactivated_limit(*h);

    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedControlled);
    EXPECT_TRUE(last_limit_is_unlimited());
}

} // namespace module

// ---------------------------------------------------------------------------
// UseCaseEventReader tests
// ---------------------------------------------------------------------------
#include <UseCaseEventReader.hpp>
#include <grpcpp/grpcpp.h>
#include <chrono>
#include <thread>

TEST(UseCaseEventReaderTest, DestructorBlocksUntilOnDoneFires) {
    auto reader = std::make_unique<UseCaseEventReader>(nullptr, nullptr, nullptr);

    // Mark the reader as started (stub is null, so no gRPC call is made)
    reader->start(common_types::EntityAddress{}, control_service::UseCase{});

    // Capture raw pointer so the lambda remains valid after reader.reset() nulls the unique_ptr
    UseCaseEventReader* raw = reader.get();

    // Simulate gRPC calling OnDone asynchronously after a short delay
    std::thread done_thread([raw]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        raw->OnDone(grpc::Status::CANCELLED);
    });

    auto start = std::chrono::steady_clock::now();
    reader.reset(); // destructor must block until OnDone fires
    auto elapsed = std::chrono::steady_clock::now() - start;

    done_thread.join();

    EXPECT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 40);
}

TEST(UseCaseEventReaderTest, DestructorReturnsImmediatelyIfStartNeverCalled) {
    auto reader = std::make_unique<UseCaseEventReader>(nullptr, nullptr, nullptr);

    auto start = std::chrono::steady_clock::now();
    reader.reset(); // must NOT block (start() was never called)
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 100);
}

TEST(UseCaseEventReaderTest, DestructorReturnsImmediatelyIfOnDoneAlreadyFired) {
    auto reader = std::make_unique<UseCaseEventReader>(nullptr, nullptr, nullptr);

    // OnDone fires before destructor
    reader->OnDone(grpc::Status::CANCELLED);

    auto start = std::chrono::steady_clock::now();
    reader.reset();
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(), 50);
}
