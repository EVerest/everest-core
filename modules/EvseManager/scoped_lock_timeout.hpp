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
    explicit scoped_lock_timeout(mutex_type& __m, const std::string _description) :
        mutex(__m), description(_description) {
        if (not mutex.try_lock_for(std::chrono::seconds(10))) {
            std::stringstream ss;
            ss << "Mutex deadlock detected: Failed to lock " << description << " (Thread id "
               << std::this_thread::get_id() << "), mutex held by " << mutex.description;
            // EVLOG_AND_THROW(EverestTimeoutError(ss.str()));

            EVLOG_error << ss.str();
            request_backtrace(pthread_self());
            request_backtrace(mutex.p_id);
            sleep(240);
            exit(255);
        } else {
            locked = true;
            std::stringstream ss;
            ss << std::this_thread::get_id();
            mutex.description = description + " (thread_id: " + ss.str() + ")";
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
    const std::string description;

    mutex_type& mutex;
};
} // namespace Everest

#endif
