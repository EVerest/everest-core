// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "exampleImpl.hpp"
#include <thread>

namespace module {
namespace example_multiple {

void exampleImpl::init() {
}

void exampleImpl::ready() {
    publish_max_current(0, 10);
    publish_max_current(1, 20);

    const auto error_object0 =
        this->error_factory.at(0)->create_error("example/ExampleErrorA", "", "index0", Everest::error::Severity::High);
    const auto error_object1 =
        this->error_factory.at(1)->create_error("example/ExampleErrorA", "", "index1", Everest::error::Severity::High);
    this->raise_error(0, error_object0);
    this->raise_error(1, error_object1);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    this->clear_error(1, error_object1.type);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    this->clear_error(0, error_object0.type);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    this->raise_error(0, error_object0);
    this->raise_error(1, error_object1);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    this->clear_all_errors_of_impl(0);
    this->clear_all_errors_of_impl(1);
}

bool exampleImpl::handle_uses_something(std::size_t index, std::string& key) {
    EVLOG_info << "got something, index: " << index << " key: " << key;
    // your code for cmd uses_something goes here
    return true;
}

} // namespace example_multiple
} // namespace module
