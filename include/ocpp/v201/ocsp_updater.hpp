// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_OCSP_UPDATER_HPP
#define OCPP_OCSP_UPDATER_HPP

#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <ocpp/common/call_types.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/v201/messages/GetCertificateStatus.hpp>

namespace ocpp::v201 {

class OcspUpdateFailedException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return this->reason.c_str();
    }

    explicit OcspUpdateFailedException(std::string reason, bool can_be_retried) :
        reason(std::move(reason)), _allows_retry(can_be_retried) {
    }

    [[nodiscard]] bool allows_retry() const {
        return _allows_retry;
    }

private:
    std::string reason;
    const bool _allows_retry;
};

typedef std::function<GetCertificateStatusResponse(GetCertificateStatusRequest)> cert_status_func;

// Forward declarations to avoid include loops
class ChargePoint;
class UnexpectedMessageTypeFromCSMS;

class OcspUpdater {
public:
    OcspUpdater() = delete;
    OcspUpdater(std::shared_ptr<EvseSecurity> evse_security, cert_status_func get_cert_status_from_csms,
                std::chrono::seconds ocsp_cache_update_interval = std::chrono::hours(167),
                std::chrono::seconds ocsp_cache_update_retry_interval = std::chrono::hours(24));

    void start();
    void stop();

    // Wake up the updater thread and tell it to update
    // Used e.g. when a new charging station cert was just installed
    void trigger_ocsp_cache_update();

private:
    // Updater thread responsible for executing the updates
    std::thread updater_thread;

    // This mutex guards access to everything below it, INCLUDING explicit_update_trigger
    // - The updater thread always holds the lock, except when it's waiting on explicit_update_trigger
    // - The lib needs to hold the lock to notify the explicit_update_trigger (this guarantees it wakes up the worker)
    std::mutex update_ocsp_cache_lock;
    // Condition variable used to wake up the updater thread
    std::condition_variable explicit_update_trigger;
    // Deadline by which libocpp must automatically trigger an OCSP cache update
    std::chrono::time_point<std::chrono::steady_clock> update_deadline;
    std::shared_ptr<EvseSecurity> evse_security;
    // Set this when starting and stopping the updater thread
    bool running;

    // This function captures a pointer to a ChargePoint, which has to remain valid.
    // The OcspUpdater class is part of the ChargePoint, and thus it cannot outlive the ChargePoint.
    cert_status_func get_cert_status_from_csms;

    // Timing constants
    const std::chrono::seconds ocsp_cache_update_interval;
    const std::chrono::seconds ocsp_cache_update_retry_interval;

    // Running loop of the OCSP updater thread
    void updater_thread_loop();
    // Helper method, only called within updater_thread_loop().
    void execute_ocsp_update();
};

} // namespace ocpp::v201

#endif // OCPP_OCSP_UPDATER_HPP
