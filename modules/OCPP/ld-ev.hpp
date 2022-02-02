// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef LD_EV_HPP
#define LD_EV_HPP

//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.3
//

#include <framework/ModuleAdapter.hpp>
#include <framework/everest.hpp>

#include <everest/logging.hpp>

namespace module {

// helper class for invoking private functions on module
struct LdEverest {
    static void init(ModuleConfigs module_configs, const ModuleInfo& info);
    static void ready();
};

} // namespace module

#endif // LD_EV_HPP
