// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_CORE_MODULE_HTTPCLIENT_H
#define EVEREST_CORE_MODULE_HTTPCLIENT_H

#include "fmt/format.h"
#include "httpClientInterface.hpp"
#include <curl/curl.h>
#include <everest/logging.hpp>
#include <regex>
#include <stdexcept>
#include <string>

namespace module::main {

class HttpClient : public HttpClientInterface {

public:
    HttpClient() = delete;

    HttpClient(const std::string& host_arg, int port_arg) {
        // initialize libcurl - this is safe to do multiple times, if there are multiple HttpClients
        // Note: This is only thread-safe after libcurl 7.84.0, but we use 8.4.0, so it should be fine
        curl_global_init(CURL_GLOBAL_DEFAULT);
        // These are saved in the client to avoid making the controller pass them at every call
        host = host_arg;
        port = port_arg;
    }
    ~HttpClient() override {
        // release the libcurl resources - this must be done once for every call to curl_global_init().
        // Note: This is only thread-safe after libcurl 7.84.0, but we use 8.4.0, so it should be fine
        curl_global_cleanup();
    }

    [[nodiscard]] HttpResponse get(const std::string& path) const override;
    [[nodiscard]] HttpResponse post(const std::string& path, const std::string& body) const override;

private:
    std::string host;
    int port;

    [[nodiscard]] CURL* create_curl_handle_and_setup_url(const std::string& path) const;
    HttpResponse perform_request(CURL* connection, const std::string& request_body, const char* method_name,
                                 const std::string& path) const;
};

} // namespace module::main

#endif // EVEREST_CORE_MODULE_HTTPCLIENT_H
