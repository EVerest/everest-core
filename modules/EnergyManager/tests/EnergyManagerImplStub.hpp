// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ENERGYMANAGERSTUB_H_
#define ENERGYMANAGERSTUB_H_

#include <generated/interfaces/energy_manager/Implementation.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

struct energy_managerImplStub : public energy_managerImplBase {
    energy_managerImplStub() : energy_managerImplBase(nullptr, "manager") {
    }
    virtual void init() {
    }
    virtual void ready() {
    }
};

} // namespace module::stub

#endif
