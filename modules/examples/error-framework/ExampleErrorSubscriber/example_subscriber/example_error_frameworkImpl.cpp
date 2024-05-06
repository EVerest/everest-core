// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "example_error_frameworkImpl.hpp"

#include <utils/error/error_factory.hpp>
#include <utils/error/error_state_monitor.hpp>

#include <vector>

using Condition = Everest::error::ErrorStateMonitor::StateCondition;

namespace module {
namespace example_subscriber {

std::list<Condition> condition_0 = {
    Condition("example/ExampleErrorA", "some custom sub_type", true),
    Condition("example/ExampleErrorB", "some custom sub_type", false),
    Condition("example/ExampleErrorC", "some custom sub_type", false),
    Condition("example/ExampleErrorD", "some custom sub_type", false)
};

std::list<Condition> condition_1 = {
    Condition("example/ExampleErrorA", "some custom sub_type", false),
    Condition("example/ExampleErrorB", "some custom sub_type", true),
    Condition("example/ExampleErrorC", "some custom sub_type", false),
    Condition("example/ExampleErrorD", "some custom sub_type", false)
};

std::list<Condition> condition_2 = {
    Condition("example/ExampleErrorA", "some custom sub_type", false),
    Condition("example/ExampleErrorB", "some custom sub_type", false),
    Condition("example/ExampleErrorC", "some custom sub_type", true),
    Condition("example/ExampleErrorD", "some custom sub_type", true)
};

std::list<Condition> condition_3 = {
    Condition("example/ExampleErrorA", "some custom sub_type", false),
    Condition("example/ExampleErrorB", "some custom sub_type", false),
    Condition("example/ExampleErrorC", "some custom sub_type", false),
    Condition("example/ExampleErrorD", "some custom sub_type", false)
};

std::vector<std::list<Condition>> conditions = {
    condition_0,
    condition_1,
    condition_2,
    condition_3
};

void example_error_frameworkImpl::check_conditions() {
    for (int i = 0; i < conditions.size(); i++) {
        if (this->mod->r_example_raiser->error_state_monitor->is_condition_satisfied(conditions.at(i))) {
            EVLOG_info << "Condition " << i << " satisfied";
        } else {
            EVLOG_info << "Condition " << i << " not satisfied";
        }
    }
}

void example_error_frameworkImpl::init() {
    Everest::error::ErrorCallback error_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received error: " << error.type;
        check_conditions();
    };
    Everest::error::ErrorCallback error_cleared_callback = [this](const Everest::error::Error& error) {
        EVLOG_info << "received error cleared: " << error.type;
        check_conditions();
    };
    mod->r_example_raiser->subscribe_error("example/ExampleErrorA", error_callback, error_cleared_callback);
    mod->r_example_raiser->subscribe_all_errors(error_callback, error_cleared_callback);
}

void example_error_frameworkImpl::ready() {
    check_conditions();
}

} // namespace example_subscriber
} // namespace module
