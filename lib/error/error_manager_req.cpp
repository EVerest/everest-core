// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_manager_req.hpp>

#include <utils/error.hpp>
#include <utils/error/error_database.hpp>
#include <utils/error/error_exceptions.hpp>
#include <utils/error/error_filter.hpp>
#include <utils/error/error_type_map.hpp>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

namespace Everest {
namespace error {

ErrorManagerReq::ErrorManagerReq(std::shared_ptr<ErrorTypeMap> error_type_map_,
                                 std::shared_ptr<ErrorDatabase> error_database_,
                                 std::list<ErrorType> allowed_error_types_, SubscribeErrorFunc subscribe_error_func_) :
    error_type_map(error_type_map_),
    database(error_database_),
    allowed_error_types(allowed_error_types_),
    subscribe_error_func(subscribe_error_func_) {
    ErrorCallback on_raise = [this](const Error& error) { this->on_error_raised(error); };
    ErrorCallback on_clear = [this](const Error& error) { this->on_error_cleared(error); };
    for (const ErrorType& type : allowed_error_types) {
        subscribe_error_func(type, on_raise, on_clear);
        error_subscriptions[type] = {};
    }
}

ErrorManagerReq::Subscription::Subscription(const ErrorType& type_, const ErrorCallback& callback_,
                                            const ErrorCallback& clear_callback_) :
    type(type_), callback(callback_), clear_callback(clear_callback_) {
}

void ErrorManagerReq::subscribe_error(const ErrorType& type, const ErrorCallback& callback,
                                      const ErrorCallback& clear_callback) {
    Subscription sub(type, callback, clear_callback);
    error_subscriptions.at(type).push_back(sub);
}

void ErrorManagerReq::subscribe_all_errors(const ErrorCallback& callback, const ErrorCallback& clear_callback) {
    for (const ErrorType& type : allowed_error_types) {
        Subscription sub(type, callback, clear_callback);
        error_subscriptions.at(type).push_back(sub);
    }
}

void ErrorManagerReq::on_error_raised(const Error& error) {
    if (std::find(allowed_error_types.begin(), allowed_error_types.end(), error.type) == allowed_error_types.end()) {
        throw EverestBaseLogicError("Error can't be raised by module_id " + error.origin.module_id +
                                    " and implemenetation_id " + error.origin.implementation_id);
    }
    if (!error_type_map->has(error.type)) {
        throw EverestBaseLogicError("Error type '" + error.type + "' is not defined");
    }
    database->add_error(std::make_shared<Error>(error));
    on_error(error, true);
}

void ErrorManagerReq::on_error_cleared(const Error& error) {
    std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(error.type)), ErrorFilter(SubTypeFilter(error.sub_type))};
    std::list<ErrorPtr> res = database->remove_errors(filters);
    if (res.size() < 1) {
        throw EverestBaseLogicError("Error wasn't raised, type: " + error.type + ", sub_type: " + error.sub_type);
    }
    if (res.size() > 1) {
        throw EverestBaseLogicError("More than one error is cleared, type: " + error.type +
                                    ", sub_type: " + error.sub_type);
    }

    on_error(error, false);
}

void ErrorManagerReq::on_error(const Error& error, const bool raised) const {
    for (const Subscription& sub : error_subscriptions.at(error.type)) {
        if (raised) {
            sub.callback(error);
        } else {
            sub.clear_callback(error);
        }
    }
}

} // namespace error
} // namespace Everest
