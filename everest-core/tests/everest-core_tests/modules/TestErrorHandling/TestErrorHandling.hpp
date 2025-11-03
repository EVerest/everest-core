// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TEST_ERROR_HANDLING_HPP
#define TEST_ERROR_HANDLING_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/test_error_handling/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/test_error_raiser/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class TestErrorHandling : public Everest::ModuleBase {
public:
    TestErrorHandling() = delete;
    TestErrorHandling(const ModuleInfo& info, std::unique_ptr<test_error_handlingImplBase> p_main,
                      std::unique_ptr<test_error_raiserIntf> r_error_raiser, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_error_raiser(std::move(r_error_raiser)), config(config){};

    const std::unique_ptr<test_error_handlingImplBase> p_main;
    const std::unique_ptr<test_error_raiserIntf> r_error_raiser;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // TEST_ERROR_HANDLING_HPP
