// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "httpClient.hpp"
#include <fmt/core.h>
#include <stdexcept>

namespace module::main {
const char* CONTENT_TYPE_HEADER = "Content-Type: application/json";

struct PayloadInTransit {
    const std::string& data;
    size_t position;
};

// Callback for receiving data, saves it into a string
static size_t receive_data(char* ptr, size_t size, size_t nmemb, std::string* received_data) {
    received_data->append(ptr, size * nmemb);
    return size * nmemb;
}

// Callback for sending data, fetches it from a string
static size_t send_data(char* buffer, size_t size, size_t nitems, struct PayloadInTransit* payload) {
    if (payload->position >= payload->data.length()) {
        // Returning 0 signals to libcurl that we have no more data to send
        return 0;
    }

    // Send up to size*nitems bytes of data
    size_t payload_remaining_bytes = payload->data.length() - payload->position;
    size_t num_bytes_to_send = std::min(size * nitems, payload_remaining_bytes);
    std::memcpy(buffer, payload->data.c_str() + payload->position, num_bytes_to_send);
    payload->position += num_bytes_to_send;
    return num_bytes_to_send;
}

static HttpClientError client_error(const std::string& host, unsigned int port, const char* method,
                                    const std::string& path, const std::string& message) {
    return HttpClientError(fmt::format("HTTP client error on {} {}:{}{} : {} ", method, host, port, path, message));
}

static void setup_connection(CURL* connection, struct PayloadInTransit& request_payload, std::string& response_body,
                             curl_slist*& headers) {
    // Override the Content-Type header
    headers = curl_slist_append(nullptr, CONTENT_TYPE_HEADER);
    if (curl_easy_setopt(connection, CURLOPT_HTTPHEADER, headers) != CURLE_OK) {
        throw std::runtime_error(
            "libcurl signals that HTTP is unsupported. Your build or linkage might be misconfigured.");
    }

    // Set up callbacks for reading and writing
    curl_easy_setopt(connection, CURLOPT_WRITEFUNCTION, receive_data);
    curl_easy_setopt(connection, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(connection, CURLOPT_READFUNCTION, send_data);
    curl_easy_setopt(connection, CURLOPT_READDATA, &request_payload);

    // Misc. settings come here
    curl_easy_setopt(connection, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(connection, CURLOPT_CONNECTTIMEOUT, 2);
    if (curl_easy_setopt(connection, CURLOPT_FOLLOWLOCATION, 0) != CURLE_OK) {
        throw std::runtime_error(
            "libcurl signals that HTTP is unsupported. Your build or linkage might be misconfigured.");
    }
}

// Note: method_name and path are only there for the error message
HttpResponse HttpClient::perform_request(CURL* connection, const std::string& request_body, const char* method_name,
                                         const std::string& path) const {
    // give curl a buffer to write its error messages to
    char curl_error_message[CURL_ERROR_SIZE] = {};
    curl_easy_setopt(connection, CURLOPT_ERRORBUFFER, curl_error_message);

    // set up the connection options
    std::string response_body;
    struct PayloadInTransit request_payload {
        request_body, 0
    };
    struct curl_slist* headers;
    setup_connection(connection, request_payload, response_body, headers);

    // perform the request
    CURLcode res = curl_easy_perform(connection);

    // remember to free the headers list...
    curl_slist_free_all(headers);
    // check the result of the request and return
    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(connection, CURLINFO_RESPONSE_CODE, &response_code);
        return HttpResponse{(unsigned int)response_code, std::move(response_body)};
    } else {
        throw client_error(this->host, this->port, method_name, path, std::string(curl_error_message));
    }
}

CURL* HttpClient::create_curl_handle_and_setup_url(const std::string& path) const {
    CURL* connection = curl_easy_init();
    if (!connection) {
        throw std::runtime_error("Could not create a CURL handle: curl_easy_init() returned null");
    }
    const char* protocol = "http";
    if (curl_easy_setopt(connection, CURLOPT_URL,
                         fmt::format("{}://{}:{}{}", protocol, this->host, this->port, path).c_str()) != CURLE_OK) {
        throw std::runtime_error("Could not set CURLOPT_URL, likely ran out of memory");
    }
    if (curl_easy_setopt(connection, CURLOPT_PROTOCOLS_STR, protocol) != CURLE_OK) {
        throw std::runtime_error(std::string("Could not set supported protocol to ") + protocol +
                                 ", is it enabled in libcurl?");
    }
    return connection;
}

HttpResponse HttpClient::get(const std::string& path) const {
    CURL* connection = this->create_curl_handle_and_setup_url(path);

    if (curl_easy_setopt(connection, CURLOPT_HTTPGET, 1) != CURLE_OK) {
        curl_easy_cleanup(connection);
        throw std::runtime_error(
            "libcurl signals that HTTP is unsupported. Your build or linkage might be misconfigured.");
    }

    // perform_request() does not cleanup the connection on its own.
    // We do the cleanup here, and make sure to rethrow any exception that might've occurred.
    try {
        HttpResponse response = perform_request(connection, "", "GET", path);
        curl_easy_cleanup(connection);
        return response;
    } catch (std::exception& e) {
        curl_easy_cleanup(connection);
        throw;
    }
}

HttpResponse HttpClient::post(const std::string& path, const std::string& body) const {
    CURL* connection = this->create_curl_handle_and_setup_url(path);

    if (curl_easy_setopt(connection, CURLOPT_POST, 1) != CURLE_OK) {
        curl_easy_cleanup(connection);
        throw std::runtime_error(
            "libcurl signals that HTTP is unsupported. Your build or linkage might be misconfigured.");
    }

    // perform_request() does not cleanup the connection on its own.
    // We do the cleanup here, and make sure to rethrow any exception that might've occurred.
    try {
        HttpResponse response = perform_request(connection, body, "POST", path);
        curl_easy_cleanup(connection);
        return response;
    } catch (std::exception& e) {
        curl_easy_cleanup(connection);
        throw;
    }
}
} // namespace module::main
