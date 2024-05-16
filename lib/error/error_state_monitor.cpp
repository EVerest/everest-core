// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_state_monitor.hpp>

#include <utils/error/error_database.hpp>
#include <utils/error/error_filter.hpp>

#include <everest/logging.hpp>

namespace Everest {
namespace error {

ErrorStateMonitor::StateCondition::StateCondition(ErrorType type_, ErrorSubType sub_type_, bool active_) :
    type(type_), sub_type(sub_type_), active(active_) {
}

ErrorStateMonitor::ErrorStateMonitor(std::shared_ptr<ErrorDatabase> error_database_) : database(error_database_) {
}

bool ErrorStateMonitor::is_error_active(const ErrorType& type, const ErrorSubType& sub_type) const {
    std::list<ErrorFilter> filters = {ErrorFilter(TypeFilter(type)), ErrorFilter(SubTypeFilter(sub_type))};
    return database->get_errors(filters).size() > 0;
}

bool ErrorStateMonitor::is_condition_satisfied(const StateCondition& condition) const {
    bool res = is_error_active(condition.type, condition.sub_type);
    return res == condition.active;
}

bool ErrorStateMonitor::is_condition_satisfied(const std::list<StateCondition>& condition) const {
    for (const auto& cond : condition) {
        if (!is_condition_satisfied(cond)) {
            return false;
        }
    }
    return true;
}

} // namespace error
} // namespace Everest
