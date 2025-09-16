// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest/io/event/fd_event_sync_interface.hpp"
#include <everest/io/event/event_fd.hpp>
#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/event/unique_fd.hpp>

#include <cstdio>
#include <fcntl.h>
#include <map>
#include <vector>

#include <sys/epoll.h>
#include <sys/poll.h>

namespace everest::lib::io::event {

namespace {
uint32_t poll_event_to_bitmask(poll_events e) {
    switch (e) {
    case poll_events::read:
        return EPOLLIN;
    case poll_events::priority:
        return EPOLLPRI;
    case poll_events::write:
        return EPOLLOUT;
    case poll_events::error:
        return EPOLLERR;
    case poll_events::hungup:
        return EPOLLHUP;
    }
    return 0;
}

std::set<poll_events> bitmask_to_poll_events(uint32_t bitmask) {
    std::set<poll_events> result;
    if (bitmask & EPOLLIN) {
        result.insert(poll_events::read);
    }
    if (bitmask & EPOLLPRI) {
        result.insert(poll_events::priority);
    }
    if (bitmask & EPOLLOUT) {
        result.insert(poll_events::write);
    }
    if (bitmask & EPOLLERR) {
        result.insert(poll_events::error);
    }
    if (bitmask & EPOLLHUP) {
        result.insert(poll_events::hungup);
    }
    return result;
}

uint32_t sum_events(std::set<poll_events> const& events) {
    uint32_t result = 0;
    for (auto e : events) {
        result = result | poll_event_to_bitmask(e);
    }
    return result;
}
} // namespace

class EventHandlerMap {
public:
    EventHandlerMap() : m_epoll_fd(epoll_create1(0)) {
        if (not m_epoll_fd.is_fd()) {
            ::perror("epoll_create");
        }
    }

    bool add(int fd, fd_event_handler::event_handler_type handler, fd_event_handler::event_list const& events) {
        epoll_event event;
        event.events = sum_events(events);
        event.data.fd = fd;
        auto result = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event) == 0;
        if (result) {
            m_handlers[fd] = std::move(handler);
            m_pollfd_params[fd] = event;
            m_pollfds.push_back(event);
        }
        return result;
    }

    bool remove(int fd) {
        auto result = false;
        if (m_pollfd_params.count(fd) > 0 or m_handlers.count(fd) > 0) {
            m_pollfd_params.erase(fd);
            m_handlers.erase(fd);
            m_pollfds.resize(m_pollfds.size() - 1);
            result = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == 0;
        }
        return result;
    }

    bool modify_remove(int fd, fd_event_handler::event_list const& events) {
        auto action = [](uint32_t current, fd_event_handler::event_list const& change) {
            auto raw_change = sum_events(change);
            auto result = current & (~raw_change);
            return result;
        };
        return modify(fd, events, action);
    }

    bool modify_add(int fd, fd_event_handler::event_list const& events) {
        auto action = [](uint32_t current, fd_event_handler::event_list const& change) {
            auto raw_change = sum_events(change);
            auto result = current | raw_change;
            return result;
        };
        return modify(fd, events, action);
    }

    bool modify_replace(int fd, fd_event_handler::event_list const& events) {
        auto action = [](uint32_t, fd_event_handler::event_list const& change) {
            auto raw_change = sum_events(change);
            auto result = raw_change;
            return result;
        };
        return modify(fd, events, action);
    }

    const auto& get(int fd) const {
        return m_handlers.at(fd);
    }

    bool exists(int fd) const {
        return m_handlers.count(fd);
    }

    auto& get_pollfds() {
        return m_pollfds;
    }

    int get_epoll_fd() {
        return static_cast<int>(m_epoll_fd);
    }

private:
    bool modify(int fd, fd_event_handler::event_list const& events,
                std::function<uint32_t(uint32_t, fd_event_handler::event_list const&)> const& event_action) {
        auto result = false;
        if (m_pollfd_params.count(fd)) {
            auto& event = m_pollfd_params.at(fd);
            auto backup = event.events;
            event.events = event_action(backup, events);
            result = epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &event) == 0;
            if (not result) {
                event.events = backup;
            }
        }
        return result;
    }

    std::vector<epoll_event> m_pollfds;
    std::map<int, fd_event_handler::event_handler_type> m_handlers;
    std::map<int, epoll_event> m_pollfd_params;
    unique_fd m_epoll_fd;
};

fd_event_handler::~fd_event_handler() = default;

fd_event_handler::fd_event_handler() {
    m_handlers = std::make_unique<EventHandlerMap>();
}

bool fd_event_handler::register_event_handler(int fd, event_handler_type const& handler, event_list const& events) {
    if (fd == -1 or not handler or m_handlers->exists(fd)) {
        return false;
    }

    m_handlers->add(fd, handler, events);

    return true;
}

bool fd_event_handler::register_event_handler(event_fd* fd, event_handler_type const& handler) {
    if (not fd) {
        return false;
    }
    auto raw = fd->get_raw_fd();
    return register_event_handler(raw,
                                  [handler, fd](event_list const& e) {
                                      fd->read();
                                      handler(e);
                                  },
                                  {poll_events::read});
}

bool fd_event_handler::register_event_handler(timer_fd* fd, event_handler_type const& handler) {
    if (not fd) {
        return false;
    }
    auto raw = fd->get_raw_fd();
    return register_event_handler(raw,
                                  [handler, fd](event_list const& e) {
                                      fd->read();
                                      handler(e);
                                  },
                                  {poll_events::read});
}

bool fd_event_handler::register_event_handler(fd_event_sync_interface* obj) {
    if (not obj) {
        return false;
    }
    auto raw = obj->get_poll_fd();
    return register_event_handler(raw, [obj](event_list const&) { obj->sync(); }, {poll_events::read});
}

bool fd_event_handler::register_event_handler(fd_event_register_interface* obj) {
    if (not obj) {
        return false;
    }
    return obj->register_events(*this);
}

bool fd_event_handler::unregister_event_handler(fd_event_register_interface* obj) {
    if (not obj) {
        return false;
    }
    return obj->unregister_events(*this);
}

bool fd_event_handler::unregister_event_handler(fd_event_sync_interface* obj) {
    if (not obj) {
        return false;
    }
    return remove_event_handler(obj->get_poll_fd());
}

bool fd_event_handler::unregister_event_handler(timer_fd* obj) {
    if (not obj) {
        return false;
    }
    return remove_event_handler(obj->get_raw_fd());
}

bool fd_event_handler::unregister_event_handler(event_fd* obj) {
    if (not obj) {
        return false;
    }
    return remove_event_handler(obj->get_raw_fd());
}

bool fd_event_handler::register_event_handler(fd_event_handler* obj) {
    if (not obj or obj == this) {
        return false;
    }
    auto raw = obj->get_poll_fd();
    return register_event_handler(raw, [obj](event_list const&) { obj->poll(); }, {poll_events::read});
}

bool fd_event_handler::modify_event_handler(int fd, event_list const& events, event_modification change) {
    if (fd == -1) {
        return false;
    }
    switch (change) {
    case event_modification::add:
        return m_handlers->modify_add(fd, events);
    case event_modification::remove:
        return m_handlers->modify_remove(fd, events);
    case event_modification::replace:
        return m_handlers->modify_replace(fd, events);
    default:
        return false;
    }
}

bool fd_event_handler::remove_event_handler(int fd) {
    if (not fd) {
        return false;
    }
    return m_handlers->remove(fd);
}

void fd_event_handler::poll() {
    poll_impl(-1);
}

bool fd_event_handler::poll_impl(int timeout_ms) {
    auto& pollfds = m_handlers->get_pollfds();
    auto status = ::epoll_wait(m_handlers->get_epoll_fd(), pollfds.data(), pollfds.size(), timeout_ms);

    if (status > 0) {
        for (int i = 0; i < status; ++i) {
            auto& item = pollfds[i];
            m_handlers->get(item.data.fd)(bitmask_to_poll_events(item.events));
        }
        return true;
    }
    return false;
}

int fd_event_handler::get_poll_fd() {
    return m_handlers->get_epoll_fd();
}

} // namespace everest::lib::io::event
