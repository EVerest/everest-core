// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_EMPTY_IMPL_HPP
#define MAIN_EMPTY_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/empty/Implementation.hpp>

#include "../ManagerLifecycleSimulator.hpp"

namespace module {
namespace main {

struct Conf {};

class emptyImpl : public emptyImplBase {
public:
    emptyImpl() = delete;
    emptyImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<ManagerLifecycleSimulator>& mod, Conf& config) :
        emptyImplBase(ev, "main"), mod(mod), config(config){};

private:
    const Everest::PtrContainer<ManagerLifecycleSimulator>& mod;
    const Conf& config;
    bool blocked = false;

    virtual void init() override;
    virtual void ready() override;
    virtual void shutdown() override;
};

} // namespace main
} // namespace module

#endif // MAIN_EMPTY_IMPL_HPP
