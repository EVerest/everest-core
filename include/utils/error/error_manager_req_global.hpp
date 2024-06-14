// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_MANAGER_REQ_GLOBAL_HPP
#define UTILS_ERROR_MANAGER_REQ_GLOBAL_HPP

#include <utils/error.hpp>

#include <list>
#include <map>
#include <memory>

namespace Everest {
namespace error {

struct ErrorDatabase;
struct ErrorTypeMap;

class ErrorManagerReqGlobal {
public:
    using SubscribeGlobalAllErrorsFunc = std::function<void(const ErrorCallback&, const ErrorCallback&)>;

    ErrorManagerReqGlobal(std::shared_ptr<ErrorTypeMap> error_type_map, std::shared_ptr<ErrorDatabase> error_database,
                          SubscribeGlobalAllErrorsFunc subscribe_global_all_errors_func);

    void subscribe_global_all_errors(const ErrorCallback& callback, const ErrorCallback& clear_callback);

private:
    struct Subscription {
        Subscription(const ErrorCallback& callback, const ErrorCallback& clear_callback);
        ErrorCallback callback;
        ErrorCallback clear_callback;
    };

    std::list<Subscription> subscriptions;

    void on_error_raised(const Error& error);
    void on_error_cleared(const Error& error);

    SubscribeGlobalAllErrorsFunc subscribe_global_all_errors_func;

    std::shared_ptr<ErrorTypeMap> error_type_map;
    std::shared_ptr<ErrorDatabase> database;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_MANAGER_REQ_GLOBAL_HPP
