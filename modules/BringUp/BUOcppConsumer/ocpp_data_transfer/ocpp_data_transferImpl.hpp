// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP_DATA_TRANSFER_OCPP_DATA_TRANSFER_IMPL_HPP
#define OCPP_DATA_TRANSFER_OCPP_DATA_TRANSFER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ocpp_data_transfer/Implementation.hpp>

#include "../BUOcppConsumer.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace ocpp_data_transfer {

struct Conf {};

class ocpp_data_transferImpl : public ocpp_data_transferImplBase {
public:
    ocpp_data_transferImpl() = delete;
    ocpp_data_transferImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<BUOcppConsumer>& mod, Conf& config) :
        ocpp_data_transferImplBase(ev, "ocpp_data_transfer"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::ocpp::DataTransferResponse handle_data_transfer(types::ocpp::DataTransferRequest& request) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<BUOcppConsumer>& mod;
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

} // namespace ocpp_data_transfer
} // namespace module

#endif // OCPP_DATA_TRANSFER_OCPP_DATA_TRANSFER_IMPL_HPP
