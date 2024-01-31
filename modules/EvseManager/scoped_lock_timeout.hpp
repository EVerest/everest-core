// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SCOPED_LOCK_TIMEOUT
#define SCOPED_LOCK_TIMEOUT

#include "everest/exceptions.hpp"
#include "everest/logging.hpp"
#include <mutex>
#include <signal.h>

#include "backtrace.hpp"

/*
 Simple helper class for scoped lock with timeout
*/
namespace Everest {

class timed_mutex_traceable : public std::timed_mutex {
public:
    std::string description;
    pthread_t p_id;
};

template <typename mutex_type> class scoped_lock_timeout {
public:
    explicit scoped_lock_timeout(mutex_type& __m, const std::string& description) : mutex(__m) {
        if (not mutex.try_lock_for(std::chrono::seconds(120))) {
            request_backtrace(pthread_self());
            request_backtrace(mutex.p_id);
            // Give some time for other timeouts to report their state and backtraces
            std::this_thread::sleep_for(std::chrono::seconds(10));

            std::string different_thread;
            if (mutex.p_id not_eq pthread_self()) {
                different_thread = " from a different thread.";
            } else {
                different_thread = " from the same thread";
            }

            EVLOG_AND_THROW(EverestTimeoutError("Mutex deadlock detected: Failed to lock " + description +
                                                ", mutex held by " + mutex.description + different_thread));
        } else {
            locked = true;
            mutex.description = description;
            mutex.p_id = pthread_self();
        }
    }

    ~scoped_lock_timeout() {
        if (locked) {
            mutex.unlock();
        }
    }

    scoped_lock_timeout(const scoped_lock_timeout&) = delete;
    scoped_lock_timeout& operator=(const scoped_lock_timeout&) = delete;

private:
    bool locked{false};
    mutex_type& mutex;
};
} // namespace Everest

#endif
