/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LD_EV_HPP
#define LD_EV_HPP

//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include <framework/ModuleAdapter.hpp>
#include <framework/everest.hpp>

#include <everest/logging.hpp>

namespace module {

// helper class for invoking private functions on module
struct LdEverest {
    static void init(ModuleConfigs module_configs);
    static void ready();
};

} // namespace module

#endif // LD_EV_HPP
