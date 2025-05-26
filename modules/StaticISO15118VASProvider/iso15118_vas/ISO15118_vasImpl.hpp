// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ISO15118_VAS_ISO15118_VAS_IMPL_HPP
#define ISO15118_VAS_ISO15118_VAS_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ISO15118_vas/Implementation.hpp>

#include "../StaticISO15118VASProvider.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace iso15118_vas {

struct Conf {};

class ISO15118_vasImpl : public ISO15118_vasImplBase {
public:
    ISO15118_vasImpl() = delete;
    ISO15118_vasImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<StaticISO15118VASProvider>& mod,
                     Conf& config) :
        ISO15118_vasImplBase(ev, "iso15118_vas"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual std::vector<types::iso15118_vas::ParameterSet> handle_get_service_parameters(int& service_id) override;
    virtual void
    handle_selected_services(std::vector<types::iso15118_vas::SelectedService>& selected_services) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    std::map<uint16_t, std::vector<types::iso15118_vas::ParameterSet>> value_added_services;
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<StaticISO15118VASProvider>& mod;
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

} // namespace iso15118_vas
} // namespace module

#endif // ISO15118_VAS_ISO15118_VAS_IMPL_HPP
