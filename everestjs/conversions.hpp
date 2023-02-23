// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <framework/everest.hpp>

#include <napi.h>

#include <utils/conversions.hpp>
#include <utils/types.hpp>

namespace EverestJs {

static const char* const napi_valuetype_strings[] = {
    "undefined", //
    "null",      //
    "boolean",   //
    "number",    //
    "string",    //
    "symbol",    //
    "object",    //
    "function",  //
    "external",  //
    "bigint",    //
};

Everest::json convertToJson(const Napi::Value& value);
Everest::TelemetryMap convertToTelemetryMap(const Napi::Object& obj);
Napi::Value convertToNapiValue(const Napi::Env& env, const Everest::json& value);

} // namespace EverestJs

#endif // CONVERSIONS_HPP
