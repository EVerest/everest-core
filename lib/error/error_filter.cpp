// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_filter.hpp>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

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
    EVLOG_error << "No known string conversion for provided enum of type SeverityFilter. Defaulting to HIGH_GE";
    return "HIGH_GE";
}

SeverityFilter string_to_severity_filter(const std::string& s) {
    if (s == "LOW_GE") {
        return SeverityFilter::LOW_GE;
    } else if (s == "MEDIUM_GE") {
        return SeverityFilter::MEDIUM_GE;
    } else if (s == "HIGH_GE") {
        return SeverityFilter::HIGH_GE;
    }
    EVLOG_error << "Provided string " << s
                << " could not be converted to enum of type SeverityFilter. Defaulting to HIGH_GE";
    return SeverityFilter::HIGH_GE;
}

TypeFilter::TypeFilter(const ErrorType& value_) : value(value_) {
}

SubTypeFilter::SubTypeFilter(const ErrorSubType& value_) : value(value_) {
}

VendorIdFilter::VendorIdFilter(const std::string& value_) : value(value_) {
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
    case FilterType::SubType:
        return "SubType";
    case FilterType::VendorId:
        return "VendorId";
    }
    EVLOG_error << "No known string conversion for provided enum of type FilterType. Defaulting to Type";
    return "Type";
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
    } else if (s == "SubType") {
        return FilterType::SubType;
    } else if (s == "VendorId") {
        return FilterType::VendorId;
    }
    EVLOG_error << "Provided string " << s << " could not be converted to enum of type FilterType. Deafulting to Type.";
    return FilterType::Type;
}

ErrorFilter::ErrorFilter() = default;

ErrorFilter::ErrorFilter(const FilterVariant& filter_) : filter(filter_) {
}

FilterType ErrorFilter::get_filter_type() const {
    if (filter.index() == 0) {
        EVLOG_error << "Filter type is not set. Defaulting to 'FilterType::State'.";
        return FilterType::State;
    }
    return static_cast<FilterType>(filter.index());
}

StateFilter ErrorFilter::get_state_filter() const {
    if (this->get_filter_type() != FilterType::State) {
        EVLOG_error << "Filter type is not StateFilter. Defaulting to 'StateFilter::Active'.";
        return StateFilter::Active;
    }
    return std::get<StateFilter>(filter);
}

OriginFilter ErrorFilter::get_origin_filter() const {
    if (this->get_filter_type() != FilterType::Origin) {
        EVLOG_error << "Filter type is not OriginFilter. Defaulting to "
                       "'OriginFilter::ImplementationIdentifier(\"no-module-id-provided\", "
                       "\"no-implementation-id-provided\")'.";
        return OriginFilter(ImplementationIdentifier("no-module-id-provided", "no-implementation-id-provided"));
    }
    return std::get<OriginFilter>(filter);
}

TypeFilter ErrorFilter::get_type_filter() const {
    if (this->get_filter_type() != FilterType::Type) {
        EVLOG_error << "Filter type is not TypeFilter. Defaulting to 'TypeFilter(\"no-type-provided\")'.";
        return TypeFilter("no-type-provided");
    }
    return std::get<TypeFilter>(filter);
}

SeverityFilter ErrorFilter::get_severity_filter() const {
    if (this->get_filter_type() != FilterType::Severity) {
        EVLOG_error << "Filter type is not SeverityFilter. Defaulting to 'SeverityFilter::HIGH_GE'.";
        return SeverityFilter::HIGH_GE;
    }
    return std::get<SeverityFilter>(filter);
}

TimePeriodFilter ErrorFilter::get_time_period_filter() const {
    if (this->get_filter_type() != FilterType::TimePeriod) {
        EVLOG_error << "Filter type is not TimePeriodFilter. Defaulting to 'TimePeriodFilter{Error::time_point(), "
                       "Error::time_point()}'.";
        return TimePeriodFilter{Error::time_point(), Error::time_point()};
    }
    return std::get<TimePeriodFilter>(filter);
}

HandleFilter ErrorFilter::get_handle_filter() const {
    if (this->get_filter_type() != FilterType::Handle) {
        EVLOG_error << "Filter type is not HandleFilter. Defaulting to 'HandleFilter()'.";
        return HandleFilter();
    }
    return std::get<HandleFilter>(filter);
}

SubTypeFilter ErrorFilter::get_sub_type_filter() const {
    if (this->get_filter_type() != FilterType::SubType) {
        EVLOG_error << "Filter type is not SubTypeFilter. Defaulting to 'SubTypeFilter(\"no-sub-type-provided\")'.";
        return SubTypeFilter("no-sub-type-provided");
    }
    return std::get<SubTypeFilter>(filter);
}

VendorIdFilter ErrorFilter::get_vendor_id_filter() const {
    if (this->get_filter_type() != FilterType::VendorId) {
        throw EverestBaseLogicError("Filter type is not VendorIdFilter.");
    }
    return std::get<VendorIdFilter>(filter);
}

} // namespace error
} // namespace Everest
