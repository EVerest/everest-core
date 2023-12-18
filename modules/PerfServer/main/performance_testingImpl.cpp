// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "performance_testingImpl.hpp"

namespace module {
namespace main {

void performance_testingImpl::init() {
}

void performance_testingImpl::ready() {
}

types::performance_testing::PerformanceTracing
performance_testingImpl::handle_simple_command(types::performance_testing::PerformanceTracing& value) {
    types::performance_testing::PerformanceTimestamp t;
    t.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    t.name = "Reply sent";
    value.timestamps.push_back(t);
    return value;
}

void performance_testingImpl::handle_start_publishing_var(int& nr_of_pubs, int& sleep_time) {
    std::thread([this, nr_of_pubs, sleep_time]() {
        for (int i; i < nr_of_pubs; i++) {
            types::performance_testing::PerformanceTimestamp ts;
            ts.name = "Var sent";
            ts.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
            types::performance_testing::PerformanceTracing trace;
            trace.timestamps.push_back(ts);
            publish_simple_var(trace);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
    }).detach();
}

void performance_testingImpl::handle_start_publishing_external_mqtt(int& nr_of_pubs, int& sleep_time) {
    std::thread([this, nr_of_pubs, sleep_time]() {
        for (int i; i < nr_of_pubs; i++) {
            auto ts = Everest::Date::to_rfc3339(date::utc_clock::now());
            mod->mqtt.publish("everest_external/performance_testing/timestamp", ts);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
    }).detach();
}

} // namespace main
} // namespace module
