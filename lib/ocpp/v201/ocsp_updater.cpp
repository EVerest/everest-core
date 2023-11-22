// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <chrono>
#include <condition_variable>
#include <thread>

#include <everest/logging.hpp>

#include <ocpp/v201/charge_point.hpp>
#include <ocpp/v201/messages/GetCertificateStatus.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/ocsp_updater.hpp>

namespace ocpp::v201 {

OcspUpdater::OcspUpdater(std::shared_ptr<EvseSecurity> evse_security, cert_status_func get_cert_status_from_csms,
                         std::chrono::seconds ocsp_cache_update_interval,
                         std::chrono::seconds ocsp_cache_update_retry_interval) :
    evse_security(std::move(evse_security)),
    get_cert_status_from_csms(std::move(get_cert_status_from_csms)),
    update_deadline(std::chrono::steady_clock::now()),
    ocsp_cache_update_interval(ocsp_cache_update_interval),
    ocsp_cache_update_retry_interval(ocsp_cache_update_retry_interval),
    running(false) {
}

void OcspUpdater::start() {
    std::unique_lock lock(this->update_ocsp_cache_lock);
    this->running = true;
    // Create the updater thread - we are holding the lock, so it will only start after we leave this function.
    // Because the deadline is in the past, it will immediately attempt an update
    this->updater_thread = std::thread([this] { this->updater_thread_loop(); });
}

void OcspUpdater::stop() {
    std::unique_lock lock(this->update_ocsp_cache_lock);
    if (this->running) {
        this->running = false;
        // Wake up the updater thread
        this->explicit_update_trigger.notify_one();
        lock.unlock();
        // wait for updater thread to exit
        this->updater_thread.join();
    }
}

void OcspUpdater::trigger_ocsp_cache_update() {
    std::unique_lock lock(this->update_ocsp_cache_lock);
    if (!this->running) {
        throw std::logic_error("Called trigger_ocsp_cache_update, but the OcspUpdater is not running.");
    }
    // Move the deadline to "now" so the updater thread doesn't think this is a spurious wakeup
    this->update_deadline = std::chrono::steady_clock::now();
    // Wake up the updater thread
    this->explicit_update_trigger.notify_one();
}

void OcspUpdater::updater_thread_loop() {
    std::unique_lock lock(this->update_ocsp_cache_lock);
    while (true) {
        auto current_deadline = this->update_deadline;
        // Wait until the last known deadline expires, or until we're woken up by the trigger
        // This is the only place where we release this->update_ocsp_cache_lock
        this->explicit_update_trigger.wait_until(lock, current_deadline);
        // If the running variable is disabled, we should exit
        if (!this->running) {
            break;
        }
        // Check that the current deadline has expired - this controls for spurious wakeups
        if (std::chrono::steady_clock::now() <= this->update_deadline) {
            continue;
        }

        // Perform the OCPP cache update
        try {
            this->execute_ocsp_update();
            // Successful update, set the deadline at a week from now and go back to sleep
            this->update_deadline = std::chrono::steady_clock::now() + this->ocsp_cache_update_interval;
        } catch (OcspUpdateFailedException& e) {
            // Unsuccessful update
            if (e.allows_retry()) {
                // Can be retried - go to sleep for a short time then retry
                EVLOG_warning << "libocpp: OCSP status update failed: " << e.what() << ", will retry.";
                this->update_deadline = std::chrono::steady_clock::now() + this->ocsp_cache_update_retry_interval;
            } else {
                // Cannot be retried - rethrow the exception. This will terminate the updater thread.
                EVLOG_error << "libocpp FATAL: OCSP status update failed: " << e.what();
                throw;
            }
        } catch (UnexpectedMessageTypeFromCSMS& e) {
            EVLOG_warning << "libocpp: " << e.what() << ", will retry.";
            this->update_deadline = std::chrono::steady_clock::now() + this->ocsp_cache_update_retry_interval;
        }
    }
}

void OcspUpdater::execute_ocsp_update() {
    auto ocsp_request_list = this->evse_security->get_ocsp_request_data();
    std::vector<std::pair<ocpp::CertificateHashDataType, std::string>> ocsp_responses;

    EVLOG_info << "libocpp: Updating OCSP cache on " << ocsp_request_list.size() << " certificates";

    for (auto& ocsp_request : ocsp_request_list) {
        GetCertificateStatusRequest request;
        switch (ocsp_request.hashAlgorithm) {
        case HashAlgorithmEnumType::SHA256:
            request.ocspRequestData.hashAlgorithm = ocpp::v201::HashAlgorithmEnum::SHA256;
            break;
        case HashAlgorithmEnumType::SHA384:
            request.ocspRequestData.hashAlgorithm = ocpp::v201::HashAlgorithmEnum::SHA384;
            break;
        case HashAlgorithmEnumType::SHA512:
            request.ocspRequestData.hashAlgorithm = ocpp::v201::HashAlgorithmEnum::SHA512;
            break;
        }
        request.ocspRequestData.issuerKeyHash = ocsp_request.issuerKeyHash;
        request.ocspRequestData.issuerNameHash = ocsp_request.issuerNameHash;
        request.ocspRequestData.serialNumber = ocsp_request.serialNumber;
        request.ocspRequestData.responderURL = ocsp_request.responderUrl;

        const auto response = this->get_cert_status_from_csms(request);

        if (response.status != GetCertificateStatusEnum::Accepted) {
            std::string error_msg = (response.statusInfo.has_value()) ? response.statusInfo.value().reasonCode.get()
                                                                      : "(No status info provided)";
            throw OcspUpdateFailedException(std::string("CSMS rejected certificate status update: ") + error_msg, true);
        }

        if (!response.ocspResult.has_value()) {
            throw OcspUpdateFailedException(
                std::string("CSMS sent an Accepted GetCertificateStatusResponse with no ocspResult"), true);
        }

        ocpp::CertificateHashDataType hash_data;
        hash_data.hashAlgorithm = ocsp_request.hashAlgorithm;
        hash_data.issuerNameHash = ocsp_request.issuerNameHash;
        hash_data.issuerKeyHash = ocsp_request.issuerKeyHash;
        hash_data.serialNumber = ocsp_request.serialNumber;
        ocsp_responses.emplace_back(std::pair(hash_data, response.ocspResult.value()));
    }

    for (auto& response : ocsp_responses) {
        this->evse_security->update_ocsp_cache(response.first, response.second);
    }

    EVLOG_info << "libocpp: Done updating OCSP cache";
}

} // namespace ocpp::v201
