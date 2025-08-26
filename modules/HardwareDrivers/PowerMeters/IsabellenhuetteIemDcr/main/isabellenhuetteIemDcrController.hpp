// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_CORE_MODULE_ISAIEMDCRCONTROLLER_H
#define EVEREST_CORE_MODULE_ISAIEMDCRCONTROLLER_H

#include "httpClientInterface.hpp"
#include <functional>
#include <generated/interfaces/powermeter/Implementation.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <utility>

namespace module::main {

class IsaIemDcrController {

public:
    struct SnapshotConfig {
        std::string timezone;
        bool timezone_handle_DST;
        int datetime_resync_interval;
        int resilience_initial_connection_retry_delay;
        int resilience_transaction_request_retries;
        int resilience_transaction_request_retry_delay;
        std::string CT;
        std::string CI;
        std::string TT_initial;
        bool US;
    };

    class IemDcrUnexpectedResponseException : public std::exception {
    public:
        const char* what() {
            return this->reason.c_str();
        }

        explicit IemDcrUnexpectedResponseException(std::string reason) : reason(std::move(reason)) {
        }

    private:
        std::string reason;
    };

    class UnexpectedIemDcrResponseBody : public IemDcrUnexpectedResponseException {
    public:
        UnexpectedIemDcrResponseBody(std::string endpoint, std::string error) :
            IemDcrUnexpectedResponseException(
                fmt::format("Received unexpected response body from endpoint {}: {}", endpoint, error)),
            endpoint(std::move(endpoint)),
            error(std::move(error)) {
        }

    private:
        std::string endpoint;
        std::string error;
    };

    class UnexpectedIemDcrResponseCode : public IemDcrUnexpectedResponseException {
    public:
        const std::string endpoint;
        const HttpResponse response;
        const std::string body;

        UnexpectedIemDcrResponseCode(const std::string& endpoint, unsigned int expected_code,
                                     const HttpResponse& response) :
            IemDcrUnexpectedResponseException(fmt::format(
                "Received unexpected response from endpoint '{}': {} (expected {}) {}", endpoint, response.status_code,
                expected_code, !response.body.empty() ? " - body: " + response.body : "")),
            endpoint(endpoint),
            response(response) {
        }
    };

    class ThreadSafeString {
    public:
        ThreadSafeString() : value("") {
        }

        void store(const std::string& new_value) {
            std::lock_guard<std::mutex> lock(mutex);
            value = new_value;
        }

        std::string load() const {
            std::lock_guard<std::mutex> lock(mutex);
            return value;
        }

    private:
        mutable std::mutex mutex;
        std::string value;
    };

    bool init();
    json get_gw();
    bool check_gw_is_empty();
    void post_gw();
    void post_tariff(std::string tariff_info);
    std::tuple<types::powermeter::Powermeter, std::string, bool> get_metervalue();
    std::string get_publickey(bool allow_cached_value);
    std::string get_datetime();
    void post_datetime();
    void refresh_datetime_if_required();
    void post_user(const types::powermeter::OCMFUserIdentificationStatus IS,
                   const std::optional<types::powermeter::OCMFIdentificationLevel> IL,
                   const std::vector<types::powermeter::OCMFIdentificationFlags>& IF,
                   const types::powermeter::OCMFIdentificationType& IT,
                   const std::optional<std::__cxx11::basic_string<char>>& ID,
                   const std::optional<std::__cxx11::basic_string<char>>& TT);
    types::units_signed::SignedMeterValue get_receipt();
    types::units_signed::SignedMeterValue get_transaction();
    void post_receipt(const std::string& TX);

    IsaIemDcrController() = delete;
    IsaIemDcrController(std::unique_ptr<HttpClientInterface> http_client, const SnapshotConfig& snap_config);

    template <typename Callable>
    static auto call_with_retry(Callable func, int number_of_retries, int retry_wait_in_milliseconds,
                                bool retry_on_http_client_error = true, bool retry_on_iemdcr_reponse_error = true)
        -> decltype(func()) {
        std::exception_ptr last_exception = nullptr;
        for (int attempt = 0; attempt < 1 + number_of_retries; ++attempt) {
            try {
                return func();
            } catch (HttpClientError& http_client_error) {
                last_exception = std::current_exception();
                if (!retry_on_http_client_error) {
                    std::rethrow_exception(last_exception);
                }
                EVLOG_warning << "HTTPClient request failed: " << http_client_error.what() << "; retry in "
                              << retry_wait_in_milliseconds << " milliseconds";
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_wait_in_milliseconds));
            } catch (IemDcrUnexpectedResponseException& iemdcr_error) {
                last_exception = std::current_exception();
                if (!retry_on_iemdcr_reponse_error) {
                    std::rethrow_exception(last_exception);
                }
                EVLOG_warning << "Unexpected IEM-DCR response: " << iemdcr_error.what() << "; retry in "
                              << retry_wait_in_milliseconds << " milliseconds";
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_wait_in_milliseconds));
            }
        }
        std::rethrow_exception(last_exception);
    }

private:
    const std::unique_ptr<HttpClientInterface> http_client;
    SnapshotConfig snapshot_config;
    std::string cached_public_key = "";
    std::chrono::minutes zone_time_offset;
    std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_datetime_sync;

    std::chrono::minutes helper_convert_timezone(std::string& timezone);
    bool helper_is_daylight_saving_time();
    std::string helper_get_current_datetime();
    std::string helper_remove_first_and_last_char(const std::string& input);
    bool helper_get_bool_from_OCMFUserIdentificationStatus(types::powermeter::OCMFUserIdentificationStatus IS);
    std::string
    helper_get_string_from_OCMFIdentificationLevel(std::optional<types::powermeter::OCMFIdentificationLevel> IL);
    std::string helper_get_string_from_OCMFIdentificationFlags(types::powermeter::OCMFIdentificationFlags id_flag);
    std::string helper_get_string_from_OCMFIdentificationType(types::powermeter::OCMFIdentificationType IT);
    types::units_signed::SignedMeterValue helper_get_signed_datatuple(const std::string& endpoint);
};

} // namespace module::main

#endif // EVEREST_CORE_MODULE_ISAIEMDCRCONTROLLER_H
