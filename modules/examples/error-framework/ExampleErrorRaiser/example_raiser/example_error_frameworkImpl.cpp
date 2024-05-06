// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "example_error_frameworkImpl.hpp"

#include <utils/error/error_factory.hpp>
#include <utils/error/error_state_monitor.hpp>

#include <vector>

using Error = Everest::error::Error;
using Condition = Everest::error::ErrorStateMonitor::StateCondition;
namespace module {
namespace example_raiser {

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
    // for (int i = 0; i < conditions.size(); i++) {
    //    if (this->error_state_monitor->is_condition_satisfied(conditions.at(i))) {
    //        EVLOG_info << "Condition " << i << " satisfied";
    //    } else {
    //        EVLOG_info << "Condition " << i << " not satisfied";
    //    }
    // }
}

void example_error_frameworkImpl::init() {
}

void example_error_frameworkImpl::ready() {
    Error error_a = this->error_factory->create_error(
        "example/ExampleErrorA",
        "some custom sub_type",
        "This error is raised to test the error handling"
    );
    raise_error(error_a);
    check_conditions();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clear_error("example/ExampleErrorA", "some custom sub_type");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Error error_b = this->error_factory->create_error(
        "example/ExampleErrorB",
        "some custom sub_type",
        "This error is raised to test the error handling",
        Everest::error::Severity::High
    );
    std::this_thread::sleep_for(std::chrono::seconds(1));
    raise_error(error_b);
    check_conditions();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clear_error("example/ExampleErrorB", "some custom sub_type");

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Error error_c = this->error_factory->create_error(
        "example/ExampleErrorC",
        "some custom sub_type",
        "This error is raised to test the error handling",
        Everest::error::Severity::Medium
    );
    raise_error(error_c);
    check_conditions();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Error error_d = this->error_factory->create_error(
        "example/ExampleErrorD",
        "some custom sub_type",
        "This error is raised to test the error handling",
        Everest::error::Severity::Medium
    );
    raise_error(error_d);
    check_conditions();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    clear_all_errors_of_impl();
    check_conditions();

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

} // namespace example_raiser
} // namespace module
