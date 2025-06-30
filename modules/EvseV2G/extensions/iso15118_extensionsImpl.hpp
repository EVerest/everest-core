// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EXTENSIONS_ISO15118_EXTENSIONS_IMPL_HPP
#define EXTENSIONS_ISO15118_EXTENSIONS_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/iso15118_extensions/Implementation.hpp>

#include "../EvseV2G.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include "v2g.hpp"
extern struct v2g_context* v2g_ctx;
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace extensions {

struct Conf {};

class iso15118_extensionsImpl : public iso15118_extensionsImplBase {
public:
    iso15118_extensionsImpl() = delete;
    iso15118_extensionsImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseV2G>& mod, Conf& config) :
        iso15118_extensionsImplBase(ev, "extensions"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void
    handle_set_get_certificate_response(types::iso15118::ResponseExiStreamStatus& certificate_response) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseV2G>& mod;
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

} // namespace extensions
} // namespace module

#endif // EXTENSIONS_ISO15118_EXTENSIONS_IMPL_HPP
