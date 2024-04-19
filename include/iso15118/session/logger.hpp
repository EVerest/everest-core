// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <variant>

namespace iso15118::session {

namespace logging {

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

struct SimpleEvent {
    TimePoint time_point;
    std::string info;
};

enum class ExiMessageDirection {
    FROM_EV,
    TO_EV,
};
struct ExiMessageEvent {
    TimePoint time_point;
    uint16_t payload_type;
    uint8_t const* data;
    size_t len;
    ExiMessageDirection direction;
};

using Event = std::variant<SimpleEvent, ExiMessageEvent>;

using Callback = std::function<void(std::size_t id, const Event&)>;

void set_session_log_callback(const Callback&);

} // namespace logging

class SessionLogger {
public:
    SessionLogger(void*);
    void enter_state(const std::string& new_state);
    void event(const std::string& info) const;
    void exi(uint16_t payload_type, uint8_t const* data, size_t len, logging::ExiMessageDirection direction) const;

    void operator()(const std::string&) const;
    void operator()(const char* format, ...) const;

private:
    std::uintptr_t id;
    std::string last_state_name;
};

} // namespace iso15118::session
