// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_CORE_MODULE_LEMDCBM400600_H
#define EVEREST_CORE_MODULE_LEMDCBM400600_H

#include "http_client_interface.hpp"
#include "lem_dcbm_time_sync_helper.hpp"
#include <functional>
#include <generated/interfaces/powermeter/Implementation.hpp>
#include <string>
#include <thread>
#include <utility>

namespace module::main {

class LemDCBM400600Controller {

public:
    struct Conf {
        // number of retries to connect to powermeter at initialization
        const int init_number_of_http_retries;
        // wait time before each retry during powermeter at initialization
        const int init_retry_wait_in_milliseconds;
        // number of retries for failed requests (due to HTTP or device errors) to start or stop a transaction
        const int transaction_number_of_http_retries;
        // wait time before each retry for transaction start/stop requests
        const int transaction_retry_wait_in_milliseconds;
        // The cable loss compensation level to use. This allows compensating the measurements of the DCBM with a
        // resistance.
        const int cable_id;
        // Used for a unique transaction tariff designation
        const int tariff_id;
    };

    class DCBMUnexpectedResponseException : public std::exception {
    public:
        const char* what() {
            return this->reason.c_str();
        }

        explicit DCBMUnexpectedResponseException(std::string reason) : reason(std::move(reason)) {
        }

    private:
        std::string reason;
    };

    class UnexpectedDCBMResponseBody : public DCBMUnexpectedResponseException {
    public:
        UnexpectedDCBMResponseBody(std::string endpoint, std::string error) :
            DCBMUnexpectedResponseException(
                fmt::format("Received unexpected response body from endpoint {}: {}", endpoint, error)),
            endpoint(std::move(endpoint)),
            error(std::move(error)) {
        }

    private:
        std::string endpoint;
        std::string error;
    };

    class UnexpectedDCBMResponseCode : public DCBMUnexpectedResponseException {
    public:
        const std::string endpoint;
        const HttpResponse response;
        const std::string body;

        UnexpectedDCBMResponseCode(const std::string& endpoint, unsigned int expected_code,
                                   const HttpResponse& response) :
            DCBMUnexpectedResponseException(fmt::format(
                "Received unexpected response from endpoint '{}': {} (expected {}) {}", endpoint, response.status_code,
                expected_code, !response.body.empty() ? " - body: " + response.body : "")),
            endpoint(endpoint),
            response(response) {
        }
    };

private:
    const std::unique_ptr<HttpClientInterface> http_client;
    std::string meter_id;
    Conf config;
    std::unique_ptr<LemDCBMTimeSyncHelper> time_sync_helper;

    void fetch_meter_id_from_device();
    void request_device_to_start_transaction(const types::powermeter::TransactionReq& value);
    void request_device_to_stop_transaction(const std::string& transaction_id);
    std::string fetch_ocmf_result(const std::string& transaction_id);
    void convert_livemeasure_to_powermeter(const std::string& livemeasure, types::powermeter::Powermeter& powermeter);
    static std::string transaction_start_request_to_dcbm_payload(const types::powermeter::TransactionReq& request,
                                                                 const int cable_id, const int tariff_id);
    static std::pair<std::string, std::string> get_transaction_stop_time_bounds();

    template <typename Callable>
    static auto call_with_retry(Callable func, int number_of_retries, int retry_wait_in_milliseconds,
                                bool retry_on_http_client_error = true, bool retry_on_dcbm_reponse_error = true)
        -> decltype(func()) {
        std::exception_ptr lastException = nullptr;
        for (int attempt = 0; attempt < 1 + number_of_retries; ++attempt) {
            try {
                return func();
            } catch (HttpClientError& http_client_error) {
                lastException = std::current_exception();
                if (!retry_on_http_client_error) {
                    std::rethrow_exception(lastException);
                }
                EVLOG_warning << "HTTPClient request failed: " << http_client_error.what() << "; retry in "
                              << retry_wait_in_milliseconds << " milliseconds";
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_wait_in_milliseconds));
            } catch (DCBMUnexpectedResponseException& dcbm_error) {
                lastException = std::current_exception();
                if (!retry_on_dcbm_reponse_error) {
                    std::rethrow_exception(lastException);
                }
                EVLOG_warning << "Unexpected DCBM response: " << dcbm_error.what() << "; retry in "
                              << retry_wait_in_milliseconds << " milliseconds";
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_wait_in_milliseconds));
            }
        }
        std::rethrow_exception(lastException);
    }

public:
    LemDCBM400600Controller() = delete;

    explicit LemDCBM400600Controller(std::unique_ptr<HttpClientInterface> http_client,
                                     std::unique_ptr<LemDCBMTimeSyncHelper> time_sync_helper, const Conf& config) :
        http_client(std::move(http_client)), time_sync_helper(std::move(time_sync_helper)), config(config) {
    }

    void init();
    types::powermeter::TransactionStartResponse start_transaction(const types::powermeter::TransactionReq& value);
    types::powermeter::TransactionStopResponse stop_transaction(const std::string& transaction_id);
    types::powermeter::Powermeter get_powermeter();
};

} // namespace module::main

#endif // EVEREST_CORE_MODULE_LEMDCBM400600_H
