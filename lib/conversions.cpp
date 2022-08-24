// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <boost/current_function.hpp>
#include <fmt/core.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/conversions.hpp>

namespace Everest {

template <> json convertTo<json>(Result retval) {
    BOOST_LOG_FUNCTION();
    if (retval == boost::none) {
        return json(nullptr);
    } else {
        auto ret_any = retval.get();
        if (ret_any.type() == typeid(boost::blank)) {
            return json(nullptr);
        } else if (ret_any.type() == typeid(std::string)) {
            return boost::any_cast<std::string>(ret_any);
        } else if (ret_any.type() == typeid(double)) {
            return boost::any_cast<double>(ret_any);
        } else if (ret_any.type() == typeid(int)) {
            return boost::any_cast<int>(ret_any);
        } else if (ret_any.type() == typeid(bool)) {
            return boost::any_cast<bool>(ret_any);
        } else if (ret_any.type() == typeid(Array)) {
            return boost::any_cast<Array>(ret_any);
        } else if (ret_any.type() == typeid(Object)) {
            return boost::any_cast<Object>(ret_any);
        } else {
            EVLOG_AND_THROW(EverestApiError(fmt::format("Wrong C++ type: {}", ret_any.type().name()))); // FIXME
        }
    }
}

template <> Result convertTo<Result>(json data, json type_hints) {
    BOOST_LOG_FUNCTION();
    Result retval;
    if (data.is_string()) {
        retval = data.get<std::string>();
    } else if (data.is_number_float()) {
        // result must always be double
        retval = data.get<double>();
    } else if (data.is_number_integer()) {
        bool is_double = false;
        if (type_hints.is_array()) {
            // consult type hints if it's really an integer or double:
            for (auto type_hint : type_hints) {
                if (type_hint == "number") {
                    is_double = true;
                }
            }
        } else {
            if (type_hints.get<std::string>() == "number") {
                is_double = true;
            }
        }

        if (is_double) {
            // parameter is actually a double, not an integer
            retval = data.get<double>();
        } else {
            retval = data.get<int>();
        }
    } else if (data.is_boolean()) {
        retval = data.get<bool>();
    } else if (data.is_array()) {
        retval = data.get<Array>();
    } else if (data.is_object()) {
        retval = data.get<Object>();
    } else if (data.is_null()) {
        retval = boost::blank();
    } else {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Wrong JSON type: {}", data.type_name()))); // FIXME
    }
    return retval;
}

template <> json convertTo<json>(Parameters params) {
    BOOST_LOG_FUNCTION();
    json j = json({});
    for (auto const& param : params) {
        if (param.second.type() == typeid(boost::blank)) {
            j[param.first] = json(nullptr);
        } else if (param.second.type() == typeid(std::string)) {
            j[param.first] = boost::any_cast<std::string>(param.second);
        } else if (param.second.type() == typeid(double)) {
            j[param.first] = boost::any_cast<double>(param.second);
        } else if (param.second.type() == typeid(int)) {
            j[param.first] = boost::any_cast<int>(param.second);
        } else if (param.second.type() == typeid(bool)) {
            j[param.first] = boost::any_cast<bool>(param.second);
        } else if (param.second.type() == typeid(Array)) {
            j[param.first] = boost::any_cast<Array>(param.second);
        } else if (param.second.type() == typeid(Object)) {
            j[param.first] = boost::any_cast<Object>(param.second);
        } else {
            EVLOG_AND_THROW(EverestApiError(
                fmt::format("Wrong C++ type: {} for entry: {}", param.second.type().name(), param.first))); // FIXME
        }
    }
    return j;
}

template <> Parameters convertTo<Parameters>(json data, json type_hints) {
    BOOST_LOG_FUNCTION();
    Parameters params;
    for (auto const& arg : data.items()) {
        json value = arg.value();
        if (value.is_string()) {
            params[arg.key()] = value.get<std::string>();
        } else if (value.is_number_float()) {
            // consult type hints if it's really an integer:
            if (type_hints.at(arg.key()).at("type") == "number") {
                // parameter is actually a double, not an integer
                params[arg.key()] = value.get<double>();
            } else {
                params[arg.key()] = value.get<int>();
            }
        } else if (value.is_number_integer()) {
            // consult type hints if it's really an integer:
            if (type_hints.at(arg.key()).at("type") == "number") {
                // parameter is actually a double, not an integer
                params[arg.key()] = value.get<double>();
            } else {
                params[arg.key()] = value.get<int>();
            }
        } else if (value.is_boolean()) {
            params[arg.key()] = value.get<bool>();
        } else if (value.is_array()) {
            params[arg.key()] = value.get<Array>();
        } else if (value.is_object()) {
            params[arg.key()] = value.get<Object>();
        } else if (value.is_null()) {
            params[arg.key()] = boost::blank();
        } else {
            EVLOG_AND_THROW(EverestApiError(fmt::format("Wrong JSON type: {} for entry: {}", value.type_name(),
                                                        arg.key()))); // FIXME
        }
    }
    return params;
}

template <> json convertTo<json>(Value value) {
    BOOST_LOG_FUNCTION();
    if (value.type() == typeid(boost::blank)) {
        return json(nullptr);
    } else if (value.type() == typeid(std::string)) {
        return boost::any_cast<std::string>(value);
    } else if (value.type() == typeid(double)) {
        return boost::any_cast<double>(value);
    } else if (value.type() == typeid(int)) {
        return boost::any_cast<int>(value);
    } else if (value.type() == typeid(bool)) {
        return boost::any_cast<bool>(value);
    } else if (value.type() == typeid(Array)) {
        return boost::any_cast<Array>(value);
    } else if (value.type() == typeid(Object)) {
        return boost::any_cast<Object>(value);
    } else {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Wrong C++ type: {}", value.type().name()))); // FIXME
    }
}

template <> Value convertTo<Value>(json data, const std::string& type_hint) {
    BOOST_LOG_FUNCTION();
    if (data.is_string()) {
        return data.get<std::string>();
    } else if (data.is_number_float()) {
        // consult type hints if it's really an integer:
        if (type_hint == "number") {
            // parameter is actually a double, not an integer
            return data.get<double>();
        } else {
            return data.get<int>();
        }
    } else if (data.is_number_integer()) {
        // consult type hints if it's really an integer:
        if (type_hint == "number") {
            // parameter is actually a double, not an integer
            return data.get<double>();
        } else {
            return data.get<int>();
        }
    } else if (data.is_boolean()) {
        return data.get<bool>();
    } else if (data.is_array()) {
        return data.get<Array>();
    } else if (data.is_object()) {
        return data.get<Object>();
    } else if (data.is_null()) {
        return boost::blank();
    } else {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Wrong JSON type: {}", data.type_name()))); // FIXME
    }
}

} // namespace Everest
