// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <chrono>
#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <functional>
#include <memory>
#include <set>

namespace everest::lib::io::event {

/**
 * @enum poll_events
 * @brief Collection of types of polling events
 */
enum class poll_events {
    read = 0,
    priority = 1,
    write = 2,
    error = 3,
    hungup = 4,
};

/**
 * @enum event_modification
 * @brief Possible modifications to the \p poll_events list of an event handler
 */
enum class event_modification {
    add,
    remove,
    replace,
};

// forward declaration
class EventHandlerMap;
class event_fd;
class timer_fd;
class generic_fd_event_client_impl;
/**
 * fd_event_handler implements a general event handling mechanism based on file descriptors
 * and <a href="https://man7.org/linux/man-pages/man7/epoll.7.html">epoll</a>. Any file descriptor can
 * be registered together with a list of the events of interest and a callback.
 * This class provides itself a filedescriptor that can be added to other event handlers. This way
 * concerns may be separated and event handlers nested.
 */
class fd_event_handler {
public:
    /**
     * @var event_list
     * @brief A disjoint list of events.
     */
    using event_list = std::set<poll_events>;

    /**
     * @var event_handler_type
     * @brief Prototype of the callbacks that can be registered for event handling
     */
    using event_handler_type = std::function<void(event_list const& event)>;

    /**
     * @brief fd_event_event is default constructed
     */
    fd_event_handler();
    /**
     * @brief On destruction all event handling is stopped and resources are cleaned up.
     * The file descriptor returned by \ref get_poll_fd is invalidated at this point.
     */
    ~fd_event_handler();

    /**
     * @name Event registration
     * Register events or special objects for event handling
     * @{
     */

    /**
     * @brief Register an arbitrary file descriptor for event handling
     * @details The file descriptor will be monitored for all listed events. The handler is called when any of the
     * events in the list occur. The actual list of events will be passed as argument to the handler. It is the
     * users responsibility to read from the file descriptor as necessary to acknowledge the event handling. Otherwise
     * the same event will fire again.
     * @param[in] fd The file descriptor to be monitored
     * @param[in] handler Callback for the handling of the events on \p fd
     * @param[in] events The list of events to me monitored of \p fd
     * @return True on success, false otherwise
     */
    bool register_event_handler(int fd, event_handler_type const& handler, event_list const& events);

    /**
     * @brief Register an \ref event_fd for event handling
     * @details Reading from the event happens internally to acknowledge event handling.
     * If manual handling is necessary use \ref event_fd filedescriptor directly
     * @param[inout] obj The object to be registerd for event handling
     * @param[in] handler Callback for handling the events on \p obj
     * @return True on success, false otherwise
     */
    bool register_event_handler(event_fd* obj, event_handler_type const& handler);
    /**
     * @brief Register an \ref timer_fd for event handling
     * @details Reading from the event happens internally to acknowledge event handling.
     * If manual handling is necessary use \ref timer_fd filedecriptor directly
     * @param[in] obj The object to be registerd for event handling
     * @param[in] handler Callback for handling the events on \p obj
     * @return True on success, false otherwise
     */
    bool register_event_handler(timer_fd* obj, event_handler_type const& handler);

    /**
     * @brief Register a client implementing \ref fd_event_sync_interface for event handling
     * @details On notification from the file descriptor of the client, its sync method is called
     * If manual handling is necessary use \ref fd_event_client filedescriptor directly
     * @param[in] obj The object to be registerd for event handling
     * @return True on success, false otherwise
     */
    bool register_event_handler(fd_event_sync_interface* obj);

    /**
     * @brief Register client evens of \ref fd_event_register_interface for event handling
     * @param[in] obj The object that needs to register its events registerd for event handling
     * @return True on success, false otherwise
     */
    bool register_event_handler(fd_event_register_interface* obj);

    /**
     * @brief Unregister client events of \ref fd_event_register_interface from event handling
     * @param[in] obj The object that needs to unregister its events registerd from
     * event handling
     * @return True on success, false otherwise
     */
    bool unregister_event_handler(fd_event_register_interface* obj);

    /**
     * @brief Unregister object implementing \ref fd_event_sync_interface from event handling
     * @param[in] obj The object to be removed from event handling
     * @return True on success, false otherwise
     */

    bool unregister_event_handler(fd_event_sync_interface* obj);

    /**
     * @brief Unregister timer_fd from event handling
     * @param[in] obj The timer to be removed
     * @return True on success, false otherwise
     */
    bool unregister_event_handler(timer_fd* obj);

    /**
     * @brief Unregister event_fd from event handling
     * @param[in] obj The event to be removed
     * @return True on success, false otherwise
     */
    bool unregister_event_handler(event_fd* obj);

    /**
     * @brief Register an \ref fd_event_handler for event handling
     * @details This will call \p poll on the event_handler whenever any of its handles events
     need attention.
     * @param[in] obj The fd_event_handler to be registerd for event handling
     * @return True on success, false otherwise
     */
    bool register_event_handler(fd_event_handler* obj);

    /**
     * @brief Modify the handling of an already registered file descriptor
     * @details This allows to change the handled events on a file descriptor. The most obvious
     * usecase is to add modify the handling of writability (EPOLLOUT).
     * You want to be notified if there is something to write only.
     * @param[in] fd The file descriptor to perform the modification on
     * @param[in] events List of events, that should be changed
     * @param[in] change The kind of modification to perform
     * @return Description
     */
    bool modify_event_handler(int fd, event_list const& events, event_modification change);

    /**
     * @brief Stop monitoring of events on the file descriptor.
     * @param[in] fd The file descriptor
     * @return True on success, false otherwise
     */
    bool remove_event_handler(int fd);
    /**
     * @}
     */

    /**
     * @brief Wait for any of the registered events to occur
     * @param[in] timeout Maximum time to wait for any event
     * @return false in case of timeout, true otherwise
     */
    template <class Rep, class Period> bool poll(std::chrono::duration<Rep, Period> timeout) {
        return poll_impl(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    }

    /**
     * @brief Wait for any of the registered events to occur
     * @details This function polls the internal file descriptor. Blocks until an event occurs.
     */
    void poll();

    /**
     * @brief Get the event handlers own file descriptor.
     * @details If any of the registered file descriptors have an update for any event,
     * this filedescriptors notifies a read event (POLLIN/EPOLLIN).
     * @return The file descriptor of the event handler.
     */
    int get_poll_fd();

private:
    /// Wait with timeout for any of the registered events to occur
    /**
     * \param timeout_ms Maximum time in ms to wait for any event
     * \return false in case of timeout, true otherwise
     */
    bool poll_impl(int timeout_ms);
    std::unique_ptr<EventHandlerMap> m_handlers{nullptr};
};

} // namespace everest::lib::io::event
