// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EXAMPLE_SUBSCRIBER_EXAMPLE_ERROR_FRAMEWORK_IMPL_HPP
#define EXAMPLE_SUBSCRIBER_EXAMPLE_ERROR_FRAMEWORK_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/example_error_framework/Implementation.hpp>

#include "../ExampleErrorSubscriber.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace example_subscriber {

struct Conf {};

class example_error_frameworkImpl : public example_error_frameworkImplBase {
public:
    example_error_frameworkImpl() = delete;
    example_error_frameworkImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<ExampleErrorSubscriber>& mod,
                                Conf& config) :
        example_error_frameworkImplBase(ev, "example_subscriber"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // no commands defined for this interface

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<ExampleErrorSubscriber>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    void check_conditions();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace example_subscriber
} // namespace module

#endif // EXAMPLE_SUBSCRIBER_EXAMPLE_ERROR_FRAMEWORK_IMPL_HPP
