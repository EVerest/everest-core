// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_MANAGER_REQ_HPP
#define UTILS_ERROR_MANAGER_REQ_HPP

#include <utils/error.hpp>

#include <list>
#include <map>
#include <memory>

namespace Everest {
namespace error {

struct ErrorDatabase;
struct ErrorTypeMap;

class ErrorManagerReq {
public:
    using SubscribeErrorFunc = std::function<void(const ErrorType&, const ErrorCallback&, const ErrorCallback&)>;

    ErrorManagerReq(std::shared_ptr<ErrorTypeMap> error_type_map, std::shared_ptr<ErrorDatabase> error_database,
                    std::list<ErrorType> allowed_error_types, SubscribeErrorFunc subscribe_error_func);

    void subscribe_error(const ErrorType& type, const ErrorCallback& callback, const ErrorCallback& clear_callback);
    void subscribe_all_errors(const ErrorCallback& callback, const ErrorCallback& clear_callback);

private:
    struct Subscription {
        Subscription(const ErrorType& type, const ErrorCallback& callback, const ErrorCallback& clear_callback);
        ErrorType type;
        ErrorCallback callback;
        ErrorCallback clear_callback;
    };

    std::map<ErrorType, std::list<Subscription>> error_subscriptions;

    void on_error_raised(const Error& error);
    void on_error_cleared(const Error& error);
    void on_error(const Error& error, const bool raised) const;

    std::shared_ptr<ErrorTypeMap> error_type_map;
    std::shared_ptr<ErrorDatabase> database;
    std::list<ErrorType> allowed_error_types;

    SubscribeErrorFunc subscribe_error_func;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_MANAGER_REQ_HPP
