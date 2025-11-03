// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <framework/everest.hpp>

#include <napi.h>

#include <utils/conversions.hpp>
#include <utils/types.hpp>

#include <utils/error.hpp>
#include <utils/error/error_state_monitor.hpp>

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
Everest::json convertToConfigMap(const Everest::json& json_config);
Everest::TelemetryMap convertToTelemetryMap(const Napi::Object& obj);
Napi::Value convertToNapiValue(const Napi::Env& env, const Everest::json& value);

// Error related
Everest::error::Error convertToError(const Napi::Value& value);
Everest::error::ErrorType convertToErrorType(const Napi::Value& value);
Everest::error::ErrorSubType convertToErrorSubType(const Napi::Value& value);
Everest::error::Severity convertToErrorSeverity(const Napi::Value& value);
Everest::error::State convertToErrorState(const Napi::Value& value);
Napi::Value convertToNapiValue(const Napi::Env& env, const Everest::error::Error& error);
// ErrorStateCondition related
bool isSingleErrorStateCondition(const Napi::Value& value);
Everest::error::ErrorStateMonitor::StateCondition convertToErrorStateCondition(const Napi::Value& value);
std::list<Everest::error::ErrorStateMonitor::StateCondition> convertToErrorStateConditionList(const Napi::Value& value);

} // namespace EverestJs

#endif // CONVERSIONS_HPP
