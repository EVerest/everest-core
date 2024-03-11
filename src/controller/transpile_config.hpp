// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <nlohmann/json.hpp>

#include <ryml.hpp>
#include <ryml_std.hpp>

c4::yml::Tree transpile_config(nlohmann::json config_json);
