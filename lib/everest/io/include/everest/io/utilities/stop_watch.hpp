// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <chrono>
#include <string>

namespace everest::lib::io::utilities {
/**
 * A simple utility class for time measurements. It's based on RAII principle.
 */
class stop_watch {
public:
    /**
     * @var clock
     * @brief Convenience definition
     */
    using clock = std::chrono::high_resolution_clock;

    /**
     * @var tp
     * @brief convenience definition
     */
    using tp = clock::time_point;

    /**
     * @var us
     * @brief Convenience definition
     */
    using us = std::chrono::microseconds;

    /**
     * @brief Create a stop_watch with start time now
     * @param[in] id Give the stop_watch an id.
     */
    stop_watch(std::string const& id = "");
    /**
     * @brief If the \p id is not empty a message with the last
     * measurement will be printed on destruction.
     * @details Duration is bewenn start_time and stop_time
     */
    ~stop_watch();

    /**
     * @brief Reset the start_time to now
     * @return The new start_time
     */
    tp reset();

    /**
     * @brief Set the end_time to now
     * @return The duration between start_time and end_time in microseconds
     */
    us stop();

    /**
     * @brief Get the current duration
     * @return The duration between start_time and now in microseconds
     */
    us lap() const;

private:
    tp m_start_time;
    tp m_end_time;
    std::string m_id;
};

} // namespace everest::lib::io::utilities
