// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <boost/current_function.hpp>

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
        if (ret_any.type() == typeid(std::string)) {
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
            EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG C++ TYPE: ", ret_any.type().name())); // FIXME
        }
    }
}

template <> Result convertTo<Result>(json data) {
    BOOST_LOG_FUNCTION();
    Result retval;
    if (data.is_string()) {
        retval = data.get<std::string>();
    } else if (data.is_number_float()) {
        retval = data.get<double>();
    } else if (data.is_number_integer()) {
        retval = data.get<int>();
    } else if (data.is_boolean()) {
        retval = data.get<bool>();
    } else if (data.is_array()) {
        retval = data.get<Array>();
    } else if (data.is_object()) {
        retval = data.get<Object>();
    } else if (data.is_null()) {
        retval = nullptr;
    } else {
        EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG JSON TYPE: ", data.type_name())); // FIXME
    }
    return retval;
}

template <> json convertTo<json>(Parameters params) {
    BOOST_LOG_FUNCTION();
    json j = json({});
    for (auto const& param : params) {
        if (param.second.type() == typeid(std::string)) {
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
            EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG C++ TYPE: ", param.second.type().name())); // FIXME
        }
    }
    return j;
}

template <> Parameters convertTo<Parameters>(json data) {
    BOOST_LOG_FUNCTION();
    Parameters params;
    for (auto const& arg : data.items()) {
        json value = arg.value();
        if (value.is_string()) {
            params[arg.key()] = value.get<std::string>();
        } else if (value.is_number_float()) {
            params[arg.key()] = value.get<double>();
        } else if (value.is_number_integer()) {
            params[arg.key()] = value.get<int>();
        } else if (value.is_boolean()) {
            params[arg.key()] = value.get<bool>();
        } else if (value.is_array()) {
            params[arg.key()] = value.get<Array>();
        } else if (value.is_object()) {
            params[arg.key()] = value.get<Object>();
        } else if (value.is_null()) {
            params[arg.key()] = nullptr;
        } else {
            EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG JSON TYPE: ", value.type_name(),
                                        " FOR ENTRY: ", arg.key())); // FIXME
        }
    }
    return params;
}

template <> json convertTo<json>(Value value) {
    BOOST_LOG_FUNCTION();
    if (value.type() == typeid(std::string)) {
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
        EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG C++ TYPE: ", value.type().name())); // FIXME
    }
}

template <> Value convertTo<Value>(json data) {
    BOOST_LOG_FUNCTION();
    if (data.is_string()) {
        return data.get<std::string>();
    } else if (data.is_number_float()) {
        return data.get<double>();
    } else if (data.is_number_integer()) {
        return data.get<int>();
    } else if (data.is_boolean()) {
        return data.get<bool>();
    } else if (data.is_array()) {
        return data.get<Array>();
    } else if (data.is_object()) {
        return data.get<Object>();
    } else {
        EVLOG_AND_THROW(EVEXCEPTION(EverestApiError, "WRONG JSON TYPE: ", data.type_name())); // FIXME
    }
}

} // namespace Everest
