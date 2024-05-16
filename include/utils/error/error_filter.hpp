// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_FILTER_HPP
#define UTILS_ERROR_FILTER_HPP

#include <utils/error.hpp>
#include <utils/types.hpp>

#include <variant>

namespace Everest {
namespace error {

///
/// \brief This filter is used to filter errors by their state.
///
using StateFilter = State;
std::string state_filter_to_string(const StateFilter& s);
StateFilter string_to_state_filter(const std::string& s);

///
/// \brief This filter is used to filter errors by their origin.
///
using OriginFilter = ImplementationIdentifier;

///
/// \brief This filter is used to filter errors by their type.
///

struct TypeFilter {
    explicit TypeFilter(const ErrorType& value);
    ErrorType value;
};

///
/// \brief This filter is used to filter errors by their severity.
///
enum class SeverityFilter {
    LOW_GE,    ///< greater or equal LOW
    MEDIUM_GE, ///< greater or equal MEDIUM
    HIGH_GE    ///< greater or equal HIGHS
};
std::string severity_filter_to_string(const SeverityFilter& s);
SeverityFilter string_to_severity_filter(const std::string& s);

///
/// \brief This filter is used to filter errors by their time of occurrence.
///
struct TimePeriodFilter {
    Error::time_point from; // time point from which the filter should be applied
    Error::time_point to;   // time point to which the filter should be applied
};

///
/// \brief This filter is used to filter errors by their handle.
///        The handle is the error code.
using HandleFilter = ErrorHandle;

///
/// \brief This filter is used to filter errors by their sub type.
///
struct SubTypeFilter {
    explicit SubTypeFilter(const ErrorSubType& value);
    ErrorSubType value;
};

///
/// \brief This enum is used to identify the different filter types.
///
enum class FilterType {
    State = 1,
    Origin = 2,
    Type = 3,
    Severity = 4,
    TimePeriod = 5,
    Handle = 6,
    SubType = 7
};
std::string filter_type_to_string(const FilterType& f);
FilterType string_to_filter_type(const std::string& s);

class ErrorFilter {
public:
    using FilterVariant = std::variant<std::monostate, StateFilter, OriginFilter, TypeFilter, SeverityFilter,
                                       TimePeriodFilter, HandleFilter, SubTypeFilter>;
    ErrorFilter();
    explicit ErrorFilter(const FilterVariant& filter_);

    FilterType get_filter_type() const;

    StateFilter get_state_filter() const;
    OriginFilter get_origin_filter() const;
    TypeFilter get_type_filter() const;
    SeverityFilter get_severity_filter() const;
    TimePeriodFilter get_time_period_filter() const;
    HandleFilter get_handle_filter() const;
    SubTypeFilter get_sub_type_filter() const;

private:
    FilterVariant filter;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_FILTER_HPP
