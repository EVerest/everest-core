// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/event/fd_event_handler.hpp>

namespace everest::lib::io::event {

generic_fd_event_client_impl::generic_fd_event_client_impl(action const& send_one, action const& receive_one,
                                                           action const& reset_client, error_status const& get_error) :
    m_send_one(send_one), m_receive_one(receive_one), m_reset_client(reset_client), m_get_error(get_error) {
    m_event_handler = std::make_unique<event::fd_event_handler>();
}

generic_fd_event_client_impl::~generic_fd_event_client_impl() = default;

int generic_fd_event_client_impl::get_poll_fd() {
    return m_event_handler->get_poll_fd();
}

sync_status generic_fd_event_client_impl::sync() {
    return sync_impl(-1);
}

sync_status generic_fd_event_client_impl::sync_impl(int timeout_ms) {
    auto result = m_event_handler->poll(std::chrono::milliseconds(timeout_ms));

    // The error handler must be called after all event handlers have run.
    // Removing handlers during error processing is likely to result in
    // inconsistent state and and segmentations fault.
    return result ? sync_status::ok : sync_status::timeout;
}

bool generic_fd_event_client_impl::setup_error_event_handler() {
    return m_event_handler->register_event_handler(&m_error_status_event_fd, [this](auto) {
        if (on_error()) {
            error_handler();
            call_error_handler(m_error);
            return sync_status::error;
        } else if (clear_error_pending()) {
            clear_error_handler(m_error);
        }
        return sync_status::ok;
    });
}

void generic_fd_event_client_impl::setup_io_event_handler(int fd) {
    using namespace everest::lib::io::event;
    m_event_handler->register_event_handler(fd,
                                            [this, fd](auto events) {
                                                auto success = true;
                                                if (events.count(poll_events::error)) {
                                                    auto error = m_get_error();
                                                    if (error) {
                                                        set_error_status_and_notify(error);
                                                    }
                                                    success = false;
                                                }
                                                if (events.count(poll_events::hungup)) {
                                                    success = false;
                                                }
                                                if (success && events.count(poll_events::read)) {
                                                    success = rx_handler();
                                                }
                                                if (success && events.count(poll_events::write)) {
                                                    success = tx_handler(fd);
                                                }
                                            },
                                            {poll_events::read});
    m_event_handler->register_event_handler(&m_io_event_fd, [this, fd](auto) {
        m_event_handler->modify_event_handler(fd, {poll_events::write}, event_modification::add);
    });
}

void generic_fd_event_client_impl::set_error_handler(cb_error const& handler) {
    m_error = handler;
}

bool generic_fd_event_client_impl::unregister_source(int fd) {
    return m_event_handler->remove_event_handler(fd);
}

bool generic_fd_event_client_impl::set_error_status_and_notify(int error_code) {
    auto result = set_error_status(error_code);
    m_error_status_event_fd.notify();
    return result;
}

bool generic_fd_event_client_impl::rx_handler() {
    auto status = m_receive_one();
    auto error_code = status == action_status::success ? 0 : m_get_error();
    auto result = set_error_status_and_notify(error_code);
    return result;
}

bool generic_fd_event_client_impl::tx_handler(int fd) {
    // We send one message only, even if more data is queued.
    // This prevents the kernel buffer from filling up
    auto status = m_send_one();
    switch (status) {
    case action_status::empty: {
        // if there are no more message we no longer listen to writeable events
        // otherwise we wait for the socket to become writeable again.
        m_event_handler->modify_event_handler(fd, {event::poll_events::write}, event::event_modification::remove);
        return true;
    }
    case action_status::fail: {
        auto error_code = m_get_error();
        set_error_status_and_notify(error_code);
        return false;
    }
    case action_status::success: {
        set_error_status_and_notify(0);
        return true;
    }
    }
    return true;
}

void generic_fd_event_client_impl::error_handler() {
    m_reset_client();
}

} // namespace everest::lib::io::event
