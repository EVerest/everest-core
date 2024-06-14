// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_database_map.hpp>

#include <everest/logging.hpp>
#include <utils/error.hpp>
#include <utils/error/error_json.hpp>

#include <algorithm>

namespace Everest {
namespace error {

void ErrorDatabaseMap::add_error(ErrorPtr error) {
    std::lock_guard<std::mutex> lock(this->errors_mutex);
    if (this->errors.find(error->uuid) != this->errors.end()) {
        EVLOG_error << "Error with handle " << error->uuid.to_string() << " already exists in ErrorDatabaseMap.";
        return;
    }
    this->errors[error->uuid] = error;
}

std::list<ErrorPtr> ErrorDatabaseMap::get_errors(const std::list<ErrorFilter>& filters) const {
    std::lock_guard<std::mutex> lock(this->errors_mutex);
    return this->get_errors_no_mutex(filters);
}

std::list<ErrorPtr> ErrorDatabaseMap::get_errors_no_mutex(const std::list<ErrorFilter>& filters) const {
    BOOST_LOG_FUNCTION();

    std::list<ErrorPtr> result;
    std::transform(this->errors.begin(), this->errors.end(), std::back_inserter(result),
                   [](const std::pair<ErrorHandle, ErrorPtr>& entry) { return entry.second; });

    for (const ErrorFilter& filter : filters) {
        std::function<bool(const ErrorPtr&)> pred;
        switch (filter.get_filter_type()) {
        case FilterType::State: {
            pred = [](const ErrorPtr& error) { return false; };
            EVLOG_error << "ErrorDatabaseMap does not support StateFilter. Ignoring.";
        } break;
        case FilterType::Origin: {
            pred = [&filter](const ErrorPtr& error) { return error->origin != filter.get_origin_filter(); };
        } break;
        case FilterType::Type: {
            pred = [&filter](const ErrorPtr& error) { return error->type != filter.get_type_filter().value; };
        } break;
        case FilterType::Severity: {
            pred = [&filter](const ErrorPtr& error) {
                switch (filter.get_severity_filter()) {
                case SeverityFilter::LOW_GE: {
                    return error->severity < Severity::Low;
                } break;
                case SeverityFilter::MEDIUM_GE: {
                    return error->severity < Severity::Medium;
                } break;
                case SeverityFilter::HIGH_GE: {
                    return error->severity < Severity::High;
                } break;
                }
                EVLOG_error << "No known condition for provided enum of type SeverityFilter.";
                return false;
            };
        } break;
        case FilterType::TimePeriod: {
            pred = [&filter](const ErrorPtr& error) {
                return error->timestamp < filter.get_time_period_filter().from ||
                       error->timestamp > filter.get_time_period_filter().to;
            };
        } break;
        case FilterType::Handle: {
            pred = [&filter](const ErrorPtr& error) { return error->uuid != filter.get_handle_filter(); };
        } break;
        case FilterType::SubType: {
            pred = [&filter](const ErrorPtr& error) { return error->sub_type != filter.get_sub_type_filter().value; };
        } break;
        case FilterType::VendorId: {
            pred = [&filter](const ErrorPtr& error) { return error->vendor_id != filter.get_vendor_id_filter().value; };
        } break;
        default:
            EVLOG_error << "No known pred for provided enum of type FilterType. Ignoring.";
            return result;
        }
        result.remove_if(pred);
    }

    return result;
}

std::list<ErrorPtr> ErrorDatabaseMap::edit_errors(const std::list<ErrorFilter>& filters, EditErrorFunc edit_func) {
    std::lock_guard<std::mutex> lock(this->errors_mutex);
    std::list<ErrorPtr> result = this->get_errors_no_mutex(filters);
    for (const ErrorPtr& error : result) {
        edit_func(error);
    }
    return result;
}

std::list<ErrorPtr> ErrorDatabaseMap::remove_errors(const std::list<ErrorFilter>& filters) {
    BOOST_LOG_FUNCTION();
    EditErrorFunc remove_func = [this](const ErrorPtr& error) { this->errors.erase(error->uuid); };
    return this->edit_errors(filters, remove_func);
}

} // namespace error
} // namespace Everest
