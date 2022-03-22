// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef AUTH_VALIDATOR_AUTH_TOKEN_VALIDATOR_IMPL_HPP
#define AUTH_VALIDATOR_AUTH_TOKEN_VALIDATOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include <generated/auth_token_validator/Implementation.hpp>
#include "../OCPP.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace auth_validator {

struct Conf {};

class auth_token_validatorImpl : public auth_token_validatorImplBase {
public:
    auth_token_validatorImpl() = delete;
    auth_token_validatorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<OCPP>& mod, Conf& config) :
        auth_token_validatorImplBase(ev, "auth_validator"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual Object handle_validate_token(std::string& token) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<OCPP>& mod;
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

} // namespace auth_validator
} // namespace module

#endif // AUTH_VALIDATOR_AUTH_TOKEN_VALIDATOR_IMPL_HPP
