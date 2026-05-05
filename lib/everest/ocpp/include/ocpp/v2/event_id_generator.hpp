// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <cstdint>

namespace ocpp::v2 {

/// Monotonic source for `EventData.eventId` values.
///
/// A single instance is owned by `ChargePoint` and shared via
/// `FunctionalBlockContext` so that all NotifyEvent emitters
/// (MonitoringUpdater, Availability::availability_state_notify_event_req,
/// future ones) draw from one namespace and never collide.
class EventIdGenerator {
public:
    EventIdGenerator() = default;

    EventIdGenerator(const EventIdGenerator&) = delete;
    EventIdGenerator& operator=(const EventIdGenerator&) = delete;

    /// Returns the next eventId. Thread-safe.
    std::int32_t next() {
        return counter.fetch_add(1, std::memory_order_relaxed);
    }

private:
    std::atomic<std::int32_t> counter{0};
};

} // namespace ocpp::v2
