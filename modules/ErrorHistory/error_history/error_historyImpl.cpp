// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "error_historyImpl.hpp"

#include "../ErrorDatabaseSqlite.hpp"

#include <filesystem>
#include <utils/error/error_exceptions.hpp>

namespace fs = std::filesystem;
namespace module {
namespace error_history {

void error_historyImpl::init() {
    this->db = std::make_shared<ErrorDatabaseSqlite>(this->config.database_path);

    Everest::error::StateFilter state_filter(Everest::error::State::Active);
    Everest::error::ErrorFilter error_filter(state_filter);
    this->db->edit_errors(
        {error_filter}, [](Everest::error::ErrorPtr error) { error->state = Everest::error::State::ClearedByReboot; });
    subscribe_global_all_errors(
        [this](const Everest::error::Error& error) { this->handle_global_all_errors(error); },
        [this](const Everest::error::Error& error) { this->handle_global_all_errors_cleared(error); });
}

void error_historyImpl::ready() {
}

Everest::error::StateFilter convert_state_filter(types::error_history::State filter) {
    switch (filter) {
    case types::error_history::State::Active:
        return Everest::error::StateFilter::Active;
    case types::error_history::State::ClearedByModule:
        return Everest::error::StateFilter::ClearedByModule;
    case types::error_history::State::ClearedByReboot:
        return Everest::error::StateFilter::ClearedByReboot;
    }
    throw std::out_of_range("No known enum conversion from enum type types::error_history::State to enum type "
                            "Everest::error::StateFilter");
}

Everest::error::SeverityFilter convert_severity_filter(types::error_history::SeverityFilter filter) {
    switch (filter) {
    case types::error_history::SeverityFilter::LOW_GE:
        return Everest::error::SeverityFilter::LOW_GE;
    case types::error_history::SeverityFilter::MEDIUM_GE:
        return Everest::error::SeverityFilter::MEDIUM_GE;
    case types::error_history::SeverityFilter::HIGH_GE:
        return Everest::error::SeverityFilter::HIGH_GE;
    }
    throw std::out_of_range("No known enum conversion from enum type types::error_history::SeverityFilter to enum type "
                            "Everest::error::SeverityFilter");
}

std::vector<types::error_history::ErrorObject>
error_historyImpl::handle_get_errors(types::error_history::FilterArguments& filters) {
    std::list<Everest::error::ErrorFilter> error_filters;
    if (filters.state_filter.has_value()) {
        Everest::error::StateFilter state_filter = convert_state_filter(filters.state_filter.value());
        error_filters.push_back(Everest::error::ErrorFilter(state_filter));
    }
    if (filters.origin_filter.has_value()) {
        Everest::error::OriginFilter origin_filter(filters.origin_filter.value().module_id,
                                                   filters.origin_filter.value().implementation_id);
        error_filters.push_back(Everest::error::ErrorFilter(origin_filter));
    }
    if (filters.type_filter.has_value()) {
        Everest::error::TypeFilter type_filter(filters.type_filter.value());
        error_filters.push_back(Everest::error::ErrorFilter(type_filter));
    }
    if (filters.severity_filter.has_value()) {
        Everest::error::SeverityFilter severity_filter = convert_severity_filter(filters.severity_filter.value());
        error_filters.push_back(Everest::error::ErrorFilter(severity_filter));
    }
    if (filters.timeperiod_filter.has_value()) {
        Everest::error::TimePeriodFilter timeperiod_filter;
        timeperiod_filter.from = Everest::Date::from_rfc3339(filters.timeperiod_filter.value().timestamp_from);
        timeperiod_filter.to = Everest::Date::from_rfc3339(filters.timeperiod_filter.value().timestamp_to);
        error_filters.push_back(Everest::error::ErrorFilter(timeperiod_filter));
    }
    if (filters.handle_filter.has_value()) {
        Everest::error::HandleFilter handle_filter(filters.handle_filter.value());
        error_filters.push_back(Everest::error::ErrorFilter(handle_filter));
    }
    std::list<Everest::error::ErrorPtr> errors = this->db->get_errors(error_filters);
    std::vector<types::error_history::ErrorObject> result;
    std::transform(errors.begin(), errors.end(), std::back_inserter(result), [](Everest::error::ErrorPtr error) {
        types::error_history::ErrorObject error_object;
        error_object.uuid = error->uuid.to_string();
        error_object.timestamp = Everest::Date::to_rfc3339(error->timestamp);
        std::string string_state = Everest::error::state_to_string(error->state);
        error_object.state = types::error_history::string_to_state(string_state);
        std::string string_severity = Everest::error::severity_to_string(error->severity);
        error_object.severity = types::error_history::string_to_severity(string_severity);
        error_object.type = error->type;
        error_object.origin.module_id = error->from.module_id;
        error_object.origin.implementation_id = error->from.implementation_id;
        error_object.message = error->message;
        error_object.description = error->description;
        return error_object;
    });
    return result;
}

void error_historyImpl::handle_global_all_errors(const Everest::error::Error& error) {
    Everest::error::ErrorPtr error_ptr = std::make_shared<Everest::error::Error>(error);
    this->db->add_error(error_ptr); // LTODO check if error is already in db -> write test case
}

void error_historyImpl::handle_global_all_errors_cleared(const Everest::error::Error& error) {
    Everest::error::HandleFilter handle_filter(error.uuid);
    Everest::error::ErrorFilter error_filter(handle_filter);
    int edited_errors =
        this->db
            ->edit_errors({error_filter},
                          [](Everest::error::ErrorPtr error) { error->state = Everest::error::State::ClearedByModule; })
            .size();
    if (edited_errors == 0) {
        throw Everest::EverestBaseLogicError("ErrorHistory: Error with uuid " + error.uuid.to_string() +
                                             " not found in database.");
    } else if (edited_errors > 1) {
        throw Everest::EverestBaseLogicError("ErrorHistory: Multiple errors with uuid " + error.uuid.to_string() +
                                             " found in database.");
    }
}

} // namespace error_history
} // namespace module
