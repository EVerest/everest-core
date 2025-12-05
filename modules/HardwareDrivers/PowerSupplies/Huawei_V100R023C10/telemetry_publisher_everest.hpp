// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <nlohmann/json.hpp>
#include <telemetry.hpp>

#include <framework/ModuleAdapter.hpp>
#include <map>
#include <thread>

class TelemetryPublisherEverest : public fusion_charger::telemetry::TelemetryPublisherBase {
    std::function<void(const std::string&, const nlohmann::json&)> publish_func;
    std::string base_topic;

    struct SubtopicDataEntry {
        nlohmann::json data_entries;
        bool has_changes{false};
    };

    std::mutex subtopics_mutex;
    std::condition_variable subtopics_changed_cv;

    // key: subtopic, value: data entry
    std::unordered_map<std::string, SubtopicDataEntry> subtopics_data;

    std::thread publisher_thread;
    std::atomic<bool> stop_publisher_thread{false};

    void publisher_thread_func();

    template <typename T>
    void datapoint_changed_internal(const std::string& subtopic, const std::string& datapoint, T value) {
        std::lock_guard<std::mutex> lock(subtopics_mutex);
        if (subtopics_data.find(subtopic) != subtopics_data.end()) {

            subtopics_data[subtopic].data_entries[datapoint] = value;
            subtopics_data[subtopic].has_changes = true;

            subtopics_changed_cv.notify_one();
        }
    }

public:
    TelemetryPublisherEverest(std::function<void(const std::string&, const nlohmann::json&)> publish_func,
                              const std::string& base_topic);
    ~TelemetryPublisherEverest() override;

    void add_subtopic(const std::string& subtopic) override;
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                           const std::string& value) override;
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint, double value) override;
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint, bool value) override;
    bool datapoint_exists(const std::string& subtopic, const std::string& datapoint) override;
    void initialize_datapoint(const std::string& subtopic, const std::string& datapoint) override;
};
