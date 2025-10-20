// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <chrono>
#include <everest/io/utilities/stop_watch.hpp>
#include <iostream>

namespace everest::lib::io::utilities {

stop_watch::stop_watch(std::string const& id) : m_id(id) {
    m_start_time = clock::now();
}

stop_watch::tp stop_watch::reset() {
    m_start_time = clock::now();
    return m_start_time;
}

stop_watch::us stop_watch::stop() {
    m_end_time = clock::now();
    auto dura = std::chrono::duration_cast<us>(m_end_time - m_start_time);
    return dura;
}

stop_watch::us stop_watch::lap() const {
    auto current = clock::now();
    auto dura = std::chrono::duration_cast<us>(current - m_start_time);
    return dura;
}

stop_watch::~stop_watch() {
    if (not m_id.empty()) {
        auto dura = stop();
        std::cout << "StopWatch ( " << m_id << " ) duration " << dura.count() << "us" << std::endl;
    }
}

} // namespace everest::lib::io::utilities
