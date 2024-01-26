// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_exceptions.hpp>
#include <utils/error/error_filter.hpp>

#include <everest/exceptions.hpp>

namespace Everest {
namespace error {

std::string state_filter_to_string(const StateFilter& f) {
    return state_to_string(f);
}

StateFilter string_to_state_filter(const std::string& s) {
    return string_to_state(s);
}

std::string severity_filter_to_string(const SeverityFilter& f) {
    switch (f) {
    case SeverityFilter::LOW_GE:
        return "LOW_GE";
    case SeverityFilter::MEDIUM_GE:
        return "MEDIUM_GE";
    case SeverityFilter::HIGH_GE:
        return "HIGH_GE";
    }
    throw std::out_of_range("No known string conversion for provided enum of type SeverityFilter.");
}

SeverityFilter string_to_severity_filter(const std::string& s) {
    if (s == "LOW_GE") {
        return SeverityFilter::LOW_GE;
    } else if (s == "MEDIUM_GE") {
        return SeverityFilter::MEDIUM_GE;
    } else if (s == "HIGH_GE") {
        return SeverityFilter::HIGH_GE;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type SeverityFilter.");
}

std::string filter_type_to_string(const FilterType& f) {
    switch (f) {
    case FilterType::State:
        return "State";
    case FilterType::Origin:
        return "Origin";
    case FilterType::Type:
        return "Type";
    case FilterType::Severity:
        return "Severity";
    case FilterType::TimePeriod:
        return "TimePeriod";
    case FilterType::Handle:
        return "Handle";
    }
    throw std::out_of_range("No known string conversion for provided enum of type FilterType.");
}

FilterType string_to_filter_type(const std::string& s) {
    if (s == "State") {
        return FilterType::State;
    } else if (s == "Origin") {
        return FilterType::Origin;
    } else if (s == "Type") {
        return FilterType::Type;
    } else if (s == "Severity") {
        return FilterType::Severity;
    } else if (s == "TimePeriod") {
        return FilterType::TimePeriod;
    } else if (s == "Handle") {
        return FilterType::Handle;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type FilterType.");
}

ErrorFilter::ErrorFilter() = default;

ErrorFilter::ErrorFilter(const FilterVariant& filter_) : filter(filter_) {
}

FilterType ErrorFilter::get_filter_type() const {
    if (filter.index() == 0) {
        throw EverestBaseLogicError("Filter type is not set.");
    }
    return static_cast<FilterType>(filter.index());
}

StateFilter ErrorFilter::get_state_filter() const {
    if (this->get_filter_type() != FilterType::State) {
        throw EverestBaseLogicError("Filter type is not StateFilter.");
    }
    return std::get<StateFilter>(filter);
}

OriginFilter ErrorFilter::get_origin_filter() const {
    if (this->get_filter_type() != FilterType::Origin) {
        throw EverestBaseLogicError("Filter type is not OriginFilter.");
    }
    return std::get<OriginFilter>(filter);
}

TypeFilter ErrorFilter::get_type_filter() const {
    if (this->get_filter_type() != FilterType::Type) {
        throw EverestBaseLogicError("Filter type is not TypeFilter.");
    }
    return std::get<TypeFilter>(filter);
}

SeverityFilter ErrorFilter::get_severity_filter() const {
    if (this->get_filter_type() != FilterType::Severity) {
        throw EverestBaseLogicError("Filter type is not SeverityFilter.");
    }
    return std::get<SeverityFilter>(filter);
}

TimePeriodFilter ErrorFilter::get_time_period_filter() const {
    if (this->get_filter_type() != FilterType::TimePeriod) {
        throw EverestBaseLogicError("Filter type is not TimePeriodFilter.");
    }
    return std::get<TimePeriodFilter>(filter);
}

HandleFilter ErrorFilter::get_handle_filter() const {
    if (this->get_filter_type() != FilterType::Handle) {
        throw EverestBaseLogicError("Filter type is not HandleFilter.");
    }
    return std::get<HandleFilter>(filter);
}

} // namespace error
} // namespace Everest
