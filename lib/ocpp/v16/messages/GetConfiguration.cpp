// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v16/messages/GetConfiguration.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string GetConfigurationRequest::get_type() const {
    return "GetConfiguration";
}

void to_json(json& j, const GetConfigurationRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.key) {
        if (j.size() == 0) {
            j = json{{"key", json::array()}};
        } else {
            j["key"] = json::array();
        }
        for (auto val : k.key.value()) {
            j["key"].push_back(val);
        }
    }
}

void from_json(const json& j, GetConfigurationRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("key")) {
        json arr = j.at("key");
        std::vector<CiString<50>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.key.emplace(vec);
    }
}

/// \brief Writes the string representation of the given GetConfigurationRequest \p k to the given output stream \p os
/// \returns an output stream with the GetConfigurationRequest written to
std::ostream& operator<<(std::ostream& os, const GetConfigurationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetConfigurationResponse::get_type() const {
    return "GetConfigurationResponse";
}

void to_json(json& j, const GetConfigurationResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.configurationKey) {
        if (j.size() == 0) {
            j = json{{"configurationKey", json::array()}};
        } else {
            j["configurationKey"] = json::array();
        }
        for (auto val : k.configurationKey.value()) {
            j["configurationKey"].push_back(val);
        }
    }
    if (k.unknownKey) {
        if (j.size() == 0) {
            j = json{{"unknownKey", json::array()}};
        } else {
            j["unknownKey"] = json::array();
        }
        for (auto val : k.unknownKey.value()) {
            j["unknownKey"].push_back(val);
        }
    }
}

void from_json(const json& j, GetConfigurationResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("configurationKey")) {
        json arr = j.at("configurationKey");
        std::vector<KeyValue> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.configurationKey.emplace(vec);
    }
    if (j.contains("unknownKey")) {
        json arr = j.at("unknownKey");
        std::vector<CiString<50>> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.unknownKey.emplace(vec);
    }
}

/// \brief Writes the string representation of the given GetConfigurationResponse \p k to the given output stream \p os
/// \returns an output stream with the GetConfigurationResponse written to
std::ostream& operator<<(std::ostream& os, const GetConfigurationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
