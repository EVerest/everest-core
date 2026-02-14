// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utility>
#include <utils/error/error_manager_impl.hpp>

#include <utils/error.hpp>
#include <utils/error/error_database.hpp>
#include <utils/error/error_json.hpp>
#include <utils/error/error_type_map.hpp>

#include <everest/logging.hpp>

#include <list>
#include <memory>

namespace Everest {
namespace error {

ErrorManagerImpl::ErrorManagerImpl(std::shared_ptr<ErrorTypeMap> error_type_map_,
                                   std::shared_ptr<ErrorDatabase> error_database_,
                                   std::list<ErrorType> allowed_error_types_,
                                   ErrorManagerImpl::PublishErrorFunc publish_raised_error_,
                                   ErrorManagerImpl::PublishErrorFunc publish_cleared_error_,
                                   const bool validate_error_types_) :
    error_type_map(error_type_map_),
    database(error_database_),
    allowed_error_types(std::move(allowed_error_types_)),
    publish_raised_error(std::move(publish_raised_error_)),
    publish_cleared_error(std::move(publish_cleared_error_)),
    validate_error_types(validate_error_types_) {
    if (validate_error_types) {
        for (const ErrorType& type : allowed_error_types) {
            if (!error_type_map->has(type)) {
                EVLOG_error << "Error type '" << type << "' in allowed_error_types is not defined, ignored.";
            }
        }
    }
}

void ErrorManagerImpl::raise_error(const Error& error) {
    if (validate_error_types) {
        if (std::find(allowed_error_types.begin(), allowed_error_types.end(), error.type) ==
            allowed_error_types.end()) {
            std::stringstream ss;
            ss << "Error type " << error.type << " is not allowed to be raised. Ignoring.";
            ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
            EVLOG_error << ss.str();
            return;
        }
    }
    const std::lock_guard<std::mutex> lock(mutex);
    if (!can_be_raised(error.type, error.sub_type)) {
        std::stringstream ss;
        ss << "Error can't be raised, because type " << error.type << ", sub_type " << error.sub_type
           << " is already active.";
        ss << std::endl << "Error object: " << nlohmann::json(error).dump(2);
        EVLOG_debug << ss.str();
        return;
    }
    database->add_error(std::make_shared<Error>(error));
    this->publish_raised_error(error);
    EVLOG_error << "Error raised, type: " << error.type << ", sub_type: " << error.sub_type
                << ", message: " << error.message;
}

std::list<ErrorPtr> ErrorManagerImpl::clear_error(const ErrorType& type) {
    const ErrorSubType sub_type("");
    return clear_error(type, sub_type);
}

std::list<ErrorPtr> ErrorManagerImpl::clear_error(const ErrorType& type, const ErrorSubType& sub_type) {
    const std::lock_guard<std::mutex> lock(mutex);
    if (!can_be_cleared(type, sub_type)) {
        EVLOG_debug << "Error can't be cleared, because type " << type << ", sub_type " << sub_type
                    << " is not active.";
        return {};
    }
    const std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(type)), ErrorFilter(SubTypeFilter(sub_type))};
    std::list<ErrorPtr> res = database->remove_errors(filters);
    if (res.size() > 1) {
        std::stringstream ss;
        ss << "There are more than one matching error, this is not valid:" << std::endl;
        for (const ErrorPtr& error : res) {
            ss << nlohmann::json(*error).dump(2) << std::endl;
        }
        EVLOG_error << ss.str();
        return {};
    }
    const ErrorPtr error = res.front();
    error->state = State::ClearedByModule;
    this->publish_cleared_error(*error);
    EVLOG_info << "Cleared error of type " << type << " with sub_type " << sub_type;
    return res;
}

std::list<ErrorPtr> ErrorManagerImpl::clear_all_errors() {
    const std::lock_guard<std::mutex> lock(mutex);
    const std::list<ErrorFilter> filters = {};
    std::list<ErrorPtr> res = database->remove_errors(filters);
    if (res.empty()) {
        EVLOG_debug << "No errors can be cleared, because no errors are active";
        return res;
    }
    std::stringstream ss;
    ss << "Cleared " << res.size() << " errors:" << std::endl;
    for (const ErrorPtr& error : res) {
        error->state = State::ClearedByModule;
        this->publish_cleared_error(*error);
        ss << "  - type: " << error->type << ", sub_type: " << error->sub_type << std::endl;
    }
    EVLOG_info << ss.str();
    return res;
}

std::list<ErrorPtr> ErrorManagerImpl::clear_all_errors(const ErrorType& error_type) {
    const std::lock_guard<std::mutex> lock(mutex);
    if (!can_be_cleared(error_type)) {
        EVLOG_debug << "Errors can't be cleared, because type " << error_type << " is not active.";
        return {};
    }
    const std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(error_type))};
    std::list<ErrorPtr> res = database->remove_errors(filters);
    std::stringstream ss;
    ss << "Cleared " << res.size() << " errors of type " << error_type << " with sub_types:" << std::endl;
    for (const ErrorPtr& error : res) {
        error->state = State::ClearedByModule;
        this->publish_cleared_error(*error);
        ss << "  - " << error->sub_type << std::endl;
    }
    EVLOG_info << ss.str();
    return res;
}

bool ErrorManagerImpl::can_be_raised(const ErrorType& type, const ErrorSubType& sub_type) const {
    const std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(type)), ErrorFilter(SubTypeFilter(sub_type))};
    return database->get_errors(filters).empty();
}

bool ErrorManagerImpl::can_be_cleared(const ErrorType& type, const ErrorSubType& sub_type) const {
    const std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(type)), ErrorFilter(SubTypeFilter(sub_type))};
    return !database->get_errors(filters).empty();
}

bool ErrorManagerImpl::can_be_cleared(const ErrorType& type) const {
    const std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(type))};
    return !database->get_errors(filters).empty();
}

} // namespace error
} // namespace Everest
