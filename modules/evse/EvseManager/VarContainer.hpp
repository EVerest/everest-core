// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef VARCONTAINER_HPP
#define VARCONTAINER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

/*
 Simple helper class for a thread safe single producer / single consumer pattern
 with a queue size of one.
*/

template <class T> class VarContainer {
public:
    T& operator=(T d) {
        {
            std::scoped_lock lock(data_mutex);
            data = d;
            unread_data = true;
        }
        condvar.notify_one();
        return data;
    };

    void clear() {
        std::scoped_lock lock(data_mutex);
        unread_data = false;
    };

    bool wait_for(T& d, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(data_mutex);

        if (condvar.wait_for(lock, timeout, [this] { return unread_data; })) {
            unread_data = false;
            d = data;
            return true;
        } else {
            // Timeout occured in wait_for
            return false;
        }
    };

private:
    T data;
    std::condition_variable condvar;
    std::mutex data_mutex;
    bool unread_data{false};
};

#endif
