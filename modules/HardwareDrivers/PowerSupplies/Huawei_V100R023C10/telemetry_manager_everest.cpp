// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "telemetry_manager_everest.hpp"

TelementryManagerEverest::TelementryManagerEverest(
    std::function<void(const std::string&, const nlohmann::json&)> publish_func, const std::string& base_topic) :
    publish_func(publish_func), base_topic(base_topic) {
    publisher_thread = std::thread(&TelementryManagerEverest::publisher_thread_func, this);
}

TelementryManagerEverest::~TelementryManagerEverest() {
    stop_publisher_thread = true;
    if (publisher_thread.joinable()) {
        subtopics_changed_cv.notify_all();
        publisher_thread.join();
    }
}

void TelementryManagerEverest::add_subtopic(const std::string& subtopic) {
    if (subtopics_data.find(subtopic) == subtopics_data.end()) {
        SubtopicDataEntry entry;
        subtopics_data[subtopic] = entry;
    }
}

void TelementryManagerEverest::datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                                                 const std::string& value) {
    datapoint_changed_internal(subtopic, datapoint, value);
}

void TelementryManagerEverest::datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                                                 double value) {
    datapoint_changed_internal(subtopic, datapoint, value);
}

void TelementryManagerEverest::datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                                                 bool value) {
    datapoint_changed_internal(subtopic, datapoint, value);
}

void TelementryManagerEverest::publisher_thread_func() {
    for (;;) {
        // await changes
        {
            std::unique_lock<std::mutex> lock(subtopics_mutex);
            subtopics_changed_cv.wait(lock, [this]() {
                if (stop_publisher_thread) {
                    return true;
                }
                for (const auto& [subtopic, data_entry] : subtopics_data) {
                    if (data_entry.has_changes) {
                        return true;
                    }
                }
                return false;
            });

            if (stop_publisher_thread) {
                return;
            }
        }

        // wait for a few ms to batch changes
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // publish changed subtopics
        {
            std::lock_guard<std::mutex> lock(subtopics_mutex);
            for (auto& [subtopic, subtopic_data_entry] : subtopics_data) {
                if (subtopic_data_entry.has_changes) {
                    // publish data_entry.data_entries to telemetry system
                    publish_func(base_topic + "/" + subtopic, subtopic_data_entry.data_entries);

                    // reset change flag
                    subtopic_data_entry.has_changes = false;
                }
            }
        }
    }
}

bool TelementryManagerEverest::datapoint_exists(const std::string& subtopic, const std::string& datapoint) {
    if (subtopics_data.find(subtopic) != subtopics_data.end()) {
        std::lock_guard<std::mutex> lock(subtopics_mutex);
        return subtopics_data[subtopic].data_entries.find(datapoint) != subtopics_data[subtopic].data_entries.end();
    }

    return false;
}

void TelementryManagerEverest::initialize_datapoint(const std::string& subtopic, const std::string& datapoint) {
    if (subtopics_data.find(subtopic) != subtopics_data.end()) {
        std::lock_guard<std::mutex> lock(subtopics_mutex);
        if (subtopics_data[subtopic].data_entries.find(datapoint) == subtopics_data[subtopic].data_entries.end()) {
            subtopics_data[subtopic].data_entries[datapoint] = nullptr;
            subtopics_data[subtopic].has_changes = true;
            subtopics_changed_cv.notify_one();
        }
    }
}
