// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "StartupMonitor.hpp"
#include <everest/logging.hpp>

#include <memory>

namespace module {

bool StartupMonitor::check_ready() {
    bool result{false};
    if (ready_set) {
        result = ready_set->size() >= n_managers;
    }
    return result;
}

bool StartupMonitor::set_total(std::uint8_t total) {
    bool result{true};
    {
        std::lock_guard lock(mutex);
        if (!ready_set) {
            n_managers = total;
            if (total == 0) {
                managers_ready = true;
            } else {
                managers_ready = false;
                ready_set = std::make_unique<ready_t>();
            }
        } else {
            // already set
            EVLOG_error << "Invalid attempt to set number of EVSE managers";
            result = false;
        }
    }
    if (total == 0) {
        cv.notify_all();
    }
    return result;
}

void StartupMonitor::wait_ready() {
    std::unique_lock lock(mutex);
    cv.wait(lock, [this] { return this->managers_ready; });
}

bool StartupMonitor::notify_ready(const std::string& evse_manager_id) {
    bool result{true};
    bool notify{false};
    {
        std::lock_guard lock(mutex);
        if (ready_set) {
            ready_set->insert(evse_manager_id);
            notify = StartupMonitor::check_ready();
            if (notify) {
                managers_ready = true;
                n_managers = 0;
                ready_set->clear(); // reclaim memory
            }
        } else {
            result = false;
            if (managers_ready) {
                EVLOG_warning << "EVSE manager ready after complete";
            } else {
                EVLOG_error << "EVSE manager ready before total number set";
            }
        }
    }
    if (notify) {
        cv.notify_all();
    }
    return result;
}

} // namespace module
