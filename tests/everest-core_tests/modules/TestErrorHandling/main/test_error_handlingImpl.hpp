// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_TEST_ERROR_HANDLING_IMPL_HPP
#define MAIN_TEST_ERROR_HANDLING_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/test_error_handling/Implementation.hpp>

#include "../TestErrorHandling.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class test_error_handlingImpl : public test_error_handlingImplBase {
public:
    test_error_handlingImpl() = delete;
    test_error_handlingImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<TestErrorHandling>& mod,
                            Conf& config) :
        test_error_handlingImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_clear_error(std::string& type, std::string& sub_type) override;
    virtual void handle_clear_all_errors() override;
    virtual void handle_raise_error(std::string& type, std::string& sub_type, std::string& message,
                                    std::string& severity) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<TestErrorHandling>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_TEST_ERROR_HANDLING_IMPL_HPP
