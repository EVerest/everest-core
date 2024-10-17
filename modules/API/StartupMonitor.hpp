// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef STARTUPMONITOR_HPP
#define STARTUPMONITOR_HPP

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <set>
#include <string>

namespace module {

/**
 * \brief collect ready responses from all EVSE managers
 *
 * Provides a mechanism for API code to wait for all EVSE managers to be ready.
 * Every EVSE manager is expected to set a `ready` variable to true. This class
 * collects the IDs of EVSE managers to check that the expected number are
 * ready before allowing API calls to proceed.
 *
 * \note an EVSE manager is not expected to set `ready` more than once, however
 *       this class manages this so that the `ready` is only counted once.
 */
class StartupMonitor {
private:
    using ready_t = std::set<std::string>;

    std::condition_variable cv;
    std::mutex mutex;

protected:
    std::unique_ptr<ready_t> ready_set; //!< set of received ready responses
    std::uint16_t n_managers{0};        //!< total number of EVSE managers
    bool managers_ready{false};         //!< all EVSE managers are ready

    /**
     * \brief check whether all ready responses have been received
     * \returns true when the ready set contains at least n_managers responses
     */
    bool check_ready();

public:
    /**
     * \brief set the total number of EVSE managers
     * \param[in] total the number of EVSE managers
     * \returns false if the total has already been set
     */
    bool set_total(std::uint8_t total);

    /**
     * \brief wait for all EVSE managers to be ready
     */
    void wait_ready();

    /**
     * \brief notify that a specific EVSE manager is ready
     * \param[in] evse_manager_id the ID of the EVSE manager
     * \returns false if the total has not been set
     * \note notify_ready() may be called multiple times with the same evse_manager_id
     */
    bool notify_ready(const std::string& evse_manager_id);
};

} // namespace module

#endif // STARTUPMONITOR_HPP
