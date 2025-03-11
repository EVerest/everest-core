// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_CORE_MODULE_HTTP_CLIENT_INTERFACE_H
#define EVEREST_CORE_MODULE_HTTP_CLIENT_INTERFACE_H

#include <string>

namespace module::main {

class HttpClientError : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return this->reason.c_str();
    }
    explicit HttpClientError(std::string msg) {
        this->reason = std::move(msg);
    }
    explicit HttpClientError(const char* msg) {
        this->reason = std::string(msg);
    }

private:
    std::string reason;
};

struct HttpResponse {
    unsigned int status_code;
    std::string body;
};

struct HttpClientInterface {

    virtual ~HttpClientInterface() = default;

    [[nodiscard]] virtual HttpResponse get(const std::string& path) const = 0;
    [[nodiscard]] virtual HttpResponse post(const std::string& path, const std::string& body) const = 0;
};

} // namespace module::main

#endif // EVEREST_CORE_MODULE_HTTP_CLIENT_INTERFACE_H
