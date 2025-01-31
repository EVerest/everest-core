// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "conversions.hpp"

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/error/error_json.hpp>

namespace EverestJs {

Everest::json convertToJson(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsNull() || value.IsUndefined()) {
        return Everest::json(nullptr);
    } else if (value.IsString()) {
        return Everest::json(std::string(value.As<Napi::String>()));
    } else if (value.IsNumber()) {
        int64_t intNumber = value.As<Napi::Number>();
        double floatNumber = value.As<Napi::Number>();
        if (floatNumber == intNumber)
            return Everest::json(intNumber);
        return Everest::json(floatNumber);
    } else if (value.IsBoolean()) {
        return Everest::json(bool(value.As<Napi::Boolean>()));
    } else if (value.IsArray()) {
        auto j = Everest::json::array();
        Napi::Array array = value.As<Napi::Array>();
        for (uint64_t i = 0; i < array.Length(); i++) {
            Napi::Value entry = Napi::Value(array[i]);
            j[i] = convertToJson(entry);
        }
        return j;
    } else if (value.IsObject() && value.Type() == napi_object) {
        auto j = Everest::json({});
        Napi::Object obj = value.As<Napi::Object>();
        Napi::Array keys = obj.GetPropertyNames();
        for (uint64_t i = 0; i < keys.Length(); i++) {
            Napi::Value key = keys[i];
            if (key.IsString()) {
                std::string k = key.As<Napi::String>();
                Napi::Value v = Napi::Value(obj[k]);
                j[k] = convertToJson(v);
            } else {
                EVTHROW(EVEXCEPTION(Everest::EverestApiError,
                                    "Javascript type of object key can not be converted to Everest::json: ",
                                    napi_valuetype_strings[key.Type()]));
            }
        }
        return j;
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError, "Javascript type can not be converted to Everest::json: ",
                        napi_valuetype_strings[value.Type()]));
}

Everest::json convertToConfigMap(const Everest::json& json_config) {
    json config_map;
    for (auto& entry : json_config.items()) {
        config_map[entry.key()] = entry.value().at("value");
    }
    return config_map;
}

Everest::TelemetryMap convertToTelemetryMap(const Napi::Object& obj) {
    BOOST_LOG_FUNCTION();
    Everest::TelemetryMap telemetry;
    Napi::Array keys = obj.GetPropertyNames();
    for (uint64_t i = 0; i < keys.Length(); i++) {
        Napi::Value key = keys[i];
        if (key.IsString()) {
            std::string k = key.As<Napi::String>();
            Napi::Value value = Napi::Value(obj[k]);
            if (value.IsString()) {
                telemetry[k] = std::string(value.As<Napi::String>());
            } else if (value.IsNumber()) {
                int intNumber = value.As<Napi::Number>();
                double floatNumber = value.As<Napi::Number>();
                if (floatNumber == intNumber) {
                    telemetry[k] = intNumber;
                } else {
                    telemetry[k] = floatNumber;
                }
            } else if (value.IsBoolean()) {
                telemetry[k] = bool(value.As<Napi::Boolean>());
            }
        }
    }
    return telemetry;
}

Napi::Value convertToNapiValue(const Napi::Env& env, const json& value) {
    BOOST_LOG_FUNCTION();

    if (value.is_null()) {
        return env.Null();
    } else if (value.is_string()) {
        return Napi::String::New(env, std::string(value));
    } else if (value.is_number_integer()) {
        return Napi::Number::New(env, int64_t(value));
    } else if (value.is_number_float()) {
        return Napi::Number::New(env, double(value));
    } else if (value.is_boolean()) {
        return Napi::Boolean::New(env, bool(value));
    } else if (value.is_array()) {
        Napi::Array v = Napi::Array::New(env);
        for (uint64_t i = 0; i < value.size(); i++) {
            v.Set(i, convertToNapiValue(env, value[i]));
        }
        return v;
    } else if (value.is_object()) {
        Napi::Object v = Napi::Object::New(env);
        for (auto& el : value.items()) {
            v.Set(el.key(), convertToNapiValue(env, el.value()));
        }
        return v;
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError, "Javascript type can not be converted to Napi::Value: ", value));
}

Everest::error::Error convertToError(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    Everest::json j = convertToJson(value);
    return j.get<Everest::error::Error>();
}

Everest::error::ErrorType convertToErrorType(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsString()) {
        return Everest::error::ErrorType(std::string(value.As<Napi::String>()));
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError, "Javascript type can not be converted to Everest::error::ErrorType: ",
                        napi_valuetype_strings[value.Type()]));
}

Everest::error::ErrorSubType convertToErrorSubType(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsString()) {
        return Everest::error::ErrorSubType(std::string(value.As<Napi::String>()));
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError,
                        "Javascript type can not be converted to Everest::error::ErrorSubType: ",
                        napi_valuetype_strings[value.Type()]));
}

Everest::error::Severity convertToErrorSeverity(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsString()) {
        return Everest::error::string_to_severity(std::string(value.As<Napi::String>()));
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError, "Javascript type can not be converted to Everest::error::Severity: ",
                        napi_valuetype_strings[value.Type()]));
}

Everest::error::State convertToErrorState(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsString()) {
        return Everest::error::string_to_state(std::string(value.As<Napi::String>()));
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError, "Javascript type can not be converted to Everest::error::State: ",
                        napi_valuetype_strings[value.Type()]));
}

Napi::Value convertToNapiValue(const Napi::Env& env, const Everest::error::Error& error) {
    BOOST_LOG_FUNCTION();

    json j(error);
    Napi::Value res = convertToNapiValue(env, j);
    return res;
}

bool isSingleErrorStateCondition(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsArray()) {
        return false;
    }
    return true;
}

Everest::error::ErrorStateMonitor::StateCondition convertToErrorStateCondition(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsObject()) {
        Napi::Object obj = value.As<Napi::Object>();
        Napi::Value type = obj.Get("type");
        Napi::Value sub_type = obj.Get("sub_type");
        Napi::Value active = obj.Get("active");
        return Everest::error::ErrorStateMonitor::StateCondition(
            convertToErrorType(type), convertToErrorSubType(sub_type), bool(active.As<Napi::Boolean>()));
    }
    EVTHROW(EVEXCEPTION(Everest::EverestApiError,
                        "Javascript type can not be converted to Everest::error::ErrorStateMonitor::StateCondition: ",
                        napi_valuetype_strings[value.Type()]));
}

std::list<Everest::error::ErrorStateMonitor::StateCondition>
convertToErrorStateConditionList(const Napi::Value& value) {
    BOOST_LOG_FUNCTION();

    if (value.IsArray()) {
        std::list<Everest::error::ErrorStateMonitor::StateCondition> conditions;
        Napi::Array array = value.As<Napi::Array>();
        for (uint64_t i = 0; i < array.Length(); i++) {
            Napi::Value entry = Napi::Value(array[i]);
            conditions.push_back(convertToErrorStateCondition(entry));
        }
        return conditions;
    }
    EVTHROW(EVEXCEPTION(
        Everest::EverestApiError,
        "Javascript type can not be converted to std::list<Everest::error::ErrorStateMonitor::StateCondition>: ",
        napi_valuetype_strings[value.Type()]));
}

} // namespace EverestJs
