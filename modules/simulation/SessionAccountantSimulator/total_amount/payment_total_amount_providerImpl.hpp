/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */
#ifndef TOTAL_AMOUNT_PAYMENT_TOTAL_AMOUNT_PROVIDER_IMPL_HPP
#define TOTAL_AMOUNT_PAYMENT_TOTAL_AMOUNT_PROVIDER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/payment_total_amount_provider/Implementation.hpp>

#include "../SessionAccountantSimulator.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace total_amount {

struct Conf {};

class payment_total_amount_providerImpl : public payment_total_amount_providerImplBase {
public:
    payment_total_amount_providerImpl() = delete;
    payment_total_amount_providerImpl(Everest::ModuleAdapter* ev,
                                      const Everest::PtrContainer<SessionAccountantSimulator>& mod, Conf& config) :
        payment_total_amount_providerImplBase(ev, "total_amount"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual bool
    handle_cmd_announce_preauthorization(types::payment::BankingPreauthorization& preauthorization) override;
    virtual bool handle_cmd_withdraw_preauthorization(types::authorization::IdToken& id_token) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<SessionAccountantSimulator>& mod;
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

} // namespace total_amount
} // namespace module

#endif // TOTAL_AMOUNT_PAYMENT_TOTAL_AMOUNT_PROVIDER_IMPL_HPP
