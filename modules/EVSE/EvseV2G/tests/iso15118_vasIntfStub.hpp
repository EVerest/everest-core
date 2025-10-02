// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ISO15118_VASINTFSTUB_H_
#define ISO15118_VASINTFSTUB_H_

#include "ModuleAdapterStub.hpp"
#include <generated/interfaces/ISO15118_vas/Interface.hpp>

namespace module::stub {

class iso15118_vasIntfStub : public ISO15118_vasIntf {
public:
    explicit iso15118_vasIntfStub(ModuleAdapterStub& adapter) :
        ISO15118_vasIntf(&adapter, Requirement{"", 0}, "EvseV2G", std::nullopt) {
    }
};

} // namespace module::stub

#endif // ISO15118_VASINTFSTUB_H_
