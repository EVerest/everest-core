// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <limits>

#include <LpcUseCaseHandler.hpp>

namespace module {

// Readable SKI constants for multi-EG tests. Values are 40-character lowercase
// hex strings — valid by the allowlist format check and distinct between each
// other so EG-filtering logic can be exercised.
static const std::string SKI_A(40, 'a');
static const std::string SKI_B(40, 'b');

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
        control_service::SubscribeUseCaseEventsResponse response;
        response.mutable_use_case_event()->set_event("DataUpdateHeartbeat");
        h.handle_event(response);
    }

    // Send a heartbeat event tagged with a specific SKI (used for multi-EG tests).
    void send_heartbeat_from(LpcUseCaseHandler& h, const std::string& ski) {
        control_service::SubscribeUseCaseEventsResponse response;
        response.set_remote_ski(ski);
        response.mutable_use_case_event()->set_event("DataUpdateHeartbeat");
        h.handle_event(response);
    }

    // Send an arbitrary event string from a specific SKI.
    void send_event_from(LpcUseCaseHandler& h, const std::string& ski, const std::string& event_name) {
        control_service::SubscribeUseCaseEventsResponse response;
        response.set_remote_ski(ski);
        response.mutable_use_case_event()->set_event(event_name);
        h.handle_event(response);
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
// Unlimited/autonomous requires heartbeat before accepting limits [LPC-918/919/920]
// ===========================================================================

TEST_F(LpcStateMachineTest, unlimited_autonomous_ignores_active_limit_without_heartbeat) {
    auto h = make_handler();
    h->start();

    // Init timeout → Unlimited/autonomous.
    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // Send an active limit without a preceding heartbeat — must be ignored.
    send_active_limit(*h, ACTIVE_LIMIT);
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
}

TEST_F(LpcStateMachineTest, unlimited_autonomous_ignores_deactivated_limit_without_heartbeat) {
    auto h = make_handler();
    h->start();

    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // Send a deactivated limit without a preceding heartbeat — must be ignored.
    send_deactivated_limit(*h);
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);
}

TEST_F(LpcStateMachineTest, unlimited_autonomous_transitions_to_limited_with_heartbeat_and_active_limit) {
    auto h = make_handler();
    h->start();

    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // Heartbeat then active limit — must transition to Limited [LPC-919].
    send_heartbeat(*h);
    send_active_limit(*h, ACTIVE_LIMIT);
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), ACTIVE_LIMIT);
}

TEST_F(LpcStateMachineTest,
       unlimited_autonomous_transitions_to_unlimited_controlled_with_heartbeat_and_deactivated_limit) {
    auto h = make_handler();
    h->start();

    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // Heartbeat then deactivated limit — must transition to Unlimited/controlled [LPC-920].
    send_heartbeat(*h);
    send_deactivated_limit(*h);
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedControlled);
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

// ===========================================================================
// Phase 2 — [LPC-001] Active Power Consumption Limit value validator
// ===========================================================================

TEST(LpcLimitValueValidatorTest, AcceptsZero) {
    common_types::LoadLimit limit;
    limit.set_value(0.0);
    EXPECT_TRUE(LpcUseCaseHandler::limit_value_is_valid(limit));
}

TEST(LpcLimitValueValidatorTest, AcceptsPositive) {
    common_types::LoadLimit limit;
    limit.set_value(4200.0);
    EXPECT_TRUE(LpcUseCaseHandler::limit_value_is_valid(limit));
}

TEST(LpcLimitValueValidatorTest, RejectsNegative) {
    common_types::LoadLimit limit;
    limit.set_value(-100.0);
    EXPECT_FALSE(LpcUseCaseHandler::limit_value_is_valid(limit));
}

TEST(LpcLimitValueValidatorTest, RejectsNegativeEpsilon) {
    common_types::LoadLimit limit;
    limit.set_value(-std::numeric_limits<double>::min());
    EXPECT_FALSE(LpcUseCaseHandler::limit_value_is_valid(limit));
}

// ===========================================================================
// Phase 3 — [LPC-022] FailsafeDurationMinimum [2h, 24h] range validator
// ===========================================================================

TEST(LpcFailsafeDurationValidatorTest, AcceptsTwoHours) {
    EXPECT_TRUE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::hours(2)));
}

TEST(LpcFailsafeDurationValidatorTest, AcceptsTwentyFourHours) {
    EXPECT_TRUE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::hours(24)));
}

TEST(LpcFailsafeDurationValidatorTest, AcceptsFifteenHours) {
    EXPECT_TRUE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::hours(15)));
}

TEST(LpcFailsafeDurationValidatorTest, RejectsOneHourFiftyNineMinutes) {
    EXPECT_FALSE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::hours(1) + std::chrono::minutes(59)));
}

TEST(LpcFailsafeDurationValidatorTest, RejectsTwentyFourHoursOneSecond) {
    EXPECT_FALSE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::hours(24) + std::chrono::seconds(1)));
}

TEST(LpcFailsafeDurationValidatorTest, RejectsZero) {
    EXPECT_FALSE(LpcUseCaseHandler::failsafe_duration_is_valid(std::chrono::nanoseconds::zero()));
}

// ===========================================================================
// Phase 4 — [LPC-001] FailsafeConsumptionActivePowerLimit value validator
// ===========================================================================

TEST(LpcFailsafeLimitValidatorTest, AcceptsZero) {
    EXPECT_TRUE(LpcUseCaseHandler::failsafe_limit_is_valid(0.0));
}

TEST(LpcFailsafeLimitValidatorTest, AcceptsPositive) {
    EXPECT_TRUE(LpcUseCaseHandler::failsafe_limit_is_valid(4200.0));
}

TEST(LpcFailsafeLimitValidatorTest, RejectsNegative) {
    EXPECT_FALSE(LpcUseCaseHandler::failsafe_limit_is_valid(-100.0));
}

// ===========================================================================
// Active-EMS SKI connection — single-EG semantics across multiple discovered peers
// ===========================================================================

TEST_F(LpcStateMachineTest, first_data_event_connects_active_ems_ski) {
    auto h = make_handler();
    h->start();
    send_heartbeat_from(*h, SKI_A);
    send_active_limit(*h, ACTIVE_LIMIT); // (direct API; reuses internal last_heartbeat)
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), ACTIVE_LIMIT);
}

TEST_F(LpcStateMachineTest, heartbeat_from_second_ski_is_ignored_while_other_connected) {
    auto h = make_handler();
    h->start();
    send_heartbeat_from(*h, SKI_A);
    send_active_limit(*h, ACTIVE_LIMIT);
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);

    // Advance to the edge of the 120s heartbeat timeout. Only SKI-A's heartbeat
    // counts; SKI-B's heartbeat is ignored because SKI-A is the connected EG.
    advance(std::chrono::seconds(119));
    send_heartbeat_from(*h, SKI_B);
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);

    // Advance past the 120s boundary — SKI-B's ignored heartbeat did NOT update the
    // last_heartbeat timestamp, so the handler enters Failsafe.
    advance(std::chrono::seconds(2));
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Failsafe);
}

TEST_F(LpcStateMachineTest, failsafe_entry_clears_active_ems_ski) {
    auto h = make_handler();
    enter_failsafe(*h); // already connects SKI-A internally via send_heartbeat in the helper

    // After Failsafe entry, SKI-B's heartbeat should now be accepted — a new connection
    // forms on this first data event.
    send_heartbeat_from(*h, SKI_B);
    send_active_limit(*h, ACTIVE_LIMIT);
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), ACTIVE_LIMIT);
}

TEST_F(LpcStateMachineTest, unlimited_autonomous_entry_clears_active_ems_ski) {
    auto h = make_handler();
    h->start();

    // Init → UnlimitedAutonomous via 120s timeout [LPC-906]. No EG connected at Init.
    advance(std::chrono::seconds(121));
    h->run_state_machine();
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::UnlimitedAutonomous);

    // No EG was connected (empty), so entry is a no-op for the connection — but the
    // code path should still not crash. Subsequently, SKI-B's heartbeat connects cleanly.
    send_heartbeat_from(*h, SKI_B);
    send_active_limit(*h, ACTIVE_LIMIT);
    h->run_state_machine();
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
}

TEST_F(LpcStateMachineTest, use_case_support_update_from_non_active_ski_is_ignored_after_connection) {
    auto h = make_handler();
    h->start();

    // Connect SKI-A via data event.
    send_heartbeat_from(*h, SKI_A);
    send_active_limit(*h, ACTIVE_LIMIT);
    ASSERT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);

    const auto watts_before = last_limit_watts();

    // UseCaseSupportUpdate from SKI-B must NOT trigger configure_use_case / start_heartbeat.
    // We can't easily assert the negative on the stub (no stub in this test), but we can
    // assert that the handler state is unchanged (no new heartbeat timestamp, no crash).
    send_event_from(*h, SKI_B, "UseCaseSupportUpdate");
    EXPECT_EQ(h->get_state(), LpcUseCaseHandler::State::Limited);
    EXPECT_DOUBLE_EQ(last_limit_watts(), watts_before);
}

} // namespace module

// ---------------------------------------------------------------------------
// UseCaseEventReader tests
// ---------------------------------------------------------------------------
#include <UseCaseEventReader.hpp>
#include <chrono>
#include <grpcpp/grpcpp.h>
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
