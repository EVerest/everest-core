// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_manager_req_global.hpp>

#include <everest/logging.hpp>
#include <utils/error/error_database.hpp>
#include <utils/error/error_json.hpp>
#include <utils/error/error_type_map.hpp>

namespace Everest {
namespace error {

ErrorManagerReqGlobal::ErrorManagerReqGlobal(std::shared_ptr<ErrorTypeMap> error_type_map_,
                                             std::shared_ptr<ErrorDatabase> error_database_,
                                             SubscribeGlobalAllErrorsFunc subscribe_global_all_errors_func_) :
    error_type_map(error_type_map_),
    database(error_database_),
    subscribe_global_all_errors_func(subscribe_global_all_errors_func_),
    subscriptions({}) {
    this->subscribe_global_all_errors_func([this](const Error& error) { this->on_error_raised(error); },
                                           [this](const Error& error) { this->on_error_cleared(error); });
}

void ErrorManagerReqGlobal::subscribe_global_all_errors(const ErrorCallback& callback,
                                                        const ErrorCallback& clear_callback) {
    Subscription sub(callback, clear_callback);
    subscriptions.push_back(sub);
}

ErrorManagerReqGlobal::Subscription::Subscription(const ErrorCallback& callback_,
                                                  const ErrorCallback& clear_callback_) :
    callback(callback_), clear_callback(clear_callback_) {
}

void ErrorManagerReqGlobal::on_error_raised(const Error& error) {
    if (!error_type_map->has(error.type)) {
        std::stringstream ss;
        ss << "Error type '" << error.type << "' is not defined, ignoring error";
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_error << ss.str();
        return;
    }
    std::list<ErrorPtr> errors =
        database->get_errors({ErrorFilter(TypeFilter(error.type)), ErrorFilter(SubTypeFilter(error.sub_type)),
                              ErrorFilter(OriginFilter(error.origin))});
    if (!errors.empty()) {
        std::stringstream ss;
        ss << "Error of type '" << error.type << "' and sub type '" << error.sub_type
           << "' is already raised, ignoring new error";
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_error << ss.str();
        return;
    }
    database->add_error(std::make_shared<Error>(error));
    errors = database->get_errors({ErrorFilter(TypeFilter(error.type)), ErrorFilter(SubTypeFilter(error.sub_type)),
                                   ErrorFilter(OriginFilter(error.origin))});
    if (errors.size() != 1) {
        EVLOG_error << "Error wasn't added, type: " << error.type << ", sub type: " << error.sub_type;
        return;
    }
    for (const Subscription& sub : subscriptions) {
        sub.callback(error);
    }
}

void ErrorManagerReqGlobal::on_error_cleared(const Error& error) {
    if (!error_type_map->has(error.type)) {
        std::stringstream ss;
        ss << "Error type '" << error.type << "' is not defined, ignoring error";
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_error << ss.str();
        return;
    }
    std::list<ErrorPtr> errors =
        database->get_errors({ErrorFilter(TypeFilter(error.type)), ErrorFilter(SubTypeFilter(error.sub_type)),
                              ErrorFilter(OriginFilter(error.origin))});
    if (errors.empty()) {
        std::stringstream ss;
        ss << "Error of type '" << error.type << "' and sub type '" << error.sub_type
           << "' is not raised, ignoring clear error";
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_error << ss.str();
        return;
    }
    std::list<ErrorPtr> res =
        database->remove_errors({ErrorFilter(TypeFilter(error.type)), ErrorFilter(SubTypeFilter(error.sub_type)),
                                 ErrorFilter(OriginFilter(error.origin))});
    if (res.size() > 1) {
        std::stringstream ss;
        ss << "More than one error is cleared, type: " << error.type << ", sub type: " << error.sub_type;
        for (const ErrorPtr& error : res) {
            ss << std::endl << "Error object: " << nlohmann::json(*error).dump(2);
        }
        EVLOG_error << ss.str();
        return;
    }
    if (res.empty()) {
        std::stringstream ss;
        ss << "Error wasn't removed, type: " << error.type << ", sub type: " << error.sub_type;
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_error << ss.str();
        return;
    }
    for (const Subscription& sub : subscriptions) {
        sub.clear_callback(error);
    }
}

} // namespace error
} // namespace Everest
