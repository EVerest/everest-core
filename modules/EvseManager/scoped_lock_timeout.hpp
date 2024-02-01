// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SCOPED_LOCK_TIMEOUT
#define SCOPED_LOCK_TIMEOUT

#include "everest/exceptions.hpp"
#include "everest/logging.hpp"
#include <mutex>

/*
 Simple helper class for scoped lock with timeout
*/
namespace Everest {

class timed_mutex_traceable : public std::timed_mutex {
public:
    std::string description;
};

template <typename mutex_type> class scoped_lock_timeout {
public:
    explicit scoped_lock_timeout(mutex_type& __m, const std::string _description) :
        mutex(__m), description(_description) {
        if (not mutex.try_lock_for(std::chrono::seconds(120))) {
            std::stringstream ss;
            ss << "Mutex deadlock detected: Failed to lock " << description << " (Thread id "
               << std::this_thread::get_id << "), mutex held by " << mutex.description;
            EVLOG_AND_THROW(EverestTimeoutError(ss.str()));
        } else {
            locked = true;
            std::stringstream ss;
            ss << std::this_thread::get_id;
            mutex.description = description + " (thread_id: " + ss.str() + ")";
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
