// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/session/logger.hpp>

#include <iso15118/detail/helper.hpp>

static iso15118::session::logging::Callback session_log_callback{nullptr};

namespace iso15118::session {

SessionLogger::SessionLogger(void* id_) : id(reinterpret_cast<std::uintptr_t>(id_)){};

void SessionLogger::event(const std::string& info) const {
    logging::SimpleEvent event{std::chrono::system_clock::now(), info};
    session_log_callback(this->id, std::move(event));
}

void SessionLogger::exi(uint16_t payload_type, uint8_t const* data, size_t len,
                        logging::ExiMessageDirection direction) const {
    logging::ExiMessageEvent event{
        std::chrono::system_clock::now(), payload_type, data, len, direction,
    };

    session_log_callback(this->id, std::move(event));
}

void SessionLogger::enter_state(const std::string& new_state) {
    if (last_state_name.size()) {
        this->operator()("Transition (%s -> %s)", last_state_name.c_str(), new_state.c_str());
    } else {
        this->operator()("Transition (entered %s)", new_state.c_str());
    }

    last_state_name = std::move(new_state);
}

void SessionLogger::operator()(const std::string& info) const {
    event(info);
}

void SessionLogger::operator()(const char* format, ...) const {
    static constexpr auto MAX_FMT_LOG_BUFSIZE = 1024;
    char msg_buf[MAX_FMT_LOG_BUFSIZE];

    va_list args;
    va_start(args, format);

    vsnprintf(msg_buf, MAX_FMT_LOG_BUFSIZE, format, args);

    va_end(args);

    event(msg_buf);
}

namespace logging {
void set_session_log_callback(const logging::Callback& callback) {
    session_log_callback = callback;
}
} // namespace logging

} // namespace iso15118::session
