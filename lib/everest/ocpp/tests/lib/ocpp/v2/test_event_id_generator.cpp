// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp/v2/event_id_generator.hpp>

#include <thread>
#include <unordered_set>
#include <vector>

using ocpp::v2::EventIdGenerator;

TEST(EventIdGeneratorTest, starts_at_zero_and_monotonic) {
    EventIdGenerator gen;
    EXPECT_EQ(gen.next(), 0);
    EXPECT_EQ(gen.next(), 1);
    EXPECT_EQ(gen.next(), 2);
}

TEST(EventIdGeneratorTest, ids_are_unique_under_concurrency) {
    EventIdGenerator gen;
    constexpr int threads = 8;
    constexpr int per_thread = 1000;

    std::vector<std::vector<std::int32_t>> buckets(threads);
    std::vector<std::thread> workers;
    workers.reserve(threads);
    for (int t = 0; t < threads; ++t) {
        workers.emplace_back([&, t]() {
            buckets[t].reserve(per_thread);
            for (int i = 0; i < per_thread; ++i) {
                buckets[t].push_back(gen.next());
            }
        });
    }
    for (auto& w : workers) {
        w.join();
    }

    std::unordered_set<std::int32_t> seen;
    for (const auto& b : buckets) {
        for (auto id : b) {
            EXPECT_TRUE(seen.insert(id).second) << "duplicate id: " << id;
        }
    }
    EXPECT_EQ(seen.size(), static_cast<std::size_t>(threads * per_thread));
}
