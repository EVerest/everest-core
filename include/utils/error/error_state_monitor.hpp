// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_STATE_MONITOR_HPP
#define UTILS_ERROR_STATE_MONITOR_HPP

#include <list>

#include <utils/error.hpp>

namespace Everest {
namespace error {

struct ErrorDatabase;

///
/// \brief The StateMonitor class is used to monitor the state of multiple error types
/// \details The StateMonitor class is used to monitor the state of multiple error types. It can be used to check if a
///          certain error type is active or if a certain condition is satisfied.
///
class ErrorStateMonitor {
public:
    ///
    /// \brief The StateCondition struct represents a single condition that can be checked by the StateMonitor
    ///
    struct StateCondition {
        StateCondition(ErrorType type, ErrorSubType sub_type, bool active);
        ErrorType type;
        ErrorSubType sub_type;
        bool active;
    };

    ///
    /// \brief StateMonitor constructor
    /// \param error_database The error database to monitor
    ///
    explicit ErrorStateMonitor(std::shared_ptr<ErrorDatabase> error_database);

    ///
    /// \brief is_error_active checks if a certain combination of error type and sub_type is active
    /// \param type The error type to check
    /// \param sub_type The error sub type to check
    /// \return True if the error type is active, false otherwise
    ///
    bool is_error_active(const ErrorType& type, const ErrorSubType& sub_type) const;

    ///
    /// \brief is_condition_satisfied checks if a certain condition is satisfied
    /// \param condition The condition to check
    /// \return True if the condition is satisfied, false otherwise
    ///
    bool is_condition_satisfied(const StateCondition& condition) const;

    ///
    /// \brief is_condition_satisfied checks if a certain list of conditions is satisfied
    /// \param condition The list of conditions to check
    /// \return True if all conditions are satisfied, false otherwise
    ///
    bool is_condition_satisfied(const std::list<StateCondition>& condition) const;

private:
    std::shared_ptr<ErrorDatabase> database;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_STATE_MONITOR_HPP
