// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/GetPeriodicEventStream.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string GetPeriodicEventStreamRequest::get_type() const {
    return "GetPeriodicEventStream";
}

void to_json(json& j, const GetPeriodicEventStreamRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetPeriodicEventStreamRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetPeriodicEventStreamRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the GetPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const GetPeriodicEventStreamRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetPeriodicEventStreamResponse::get_type() const {
    return "GetPeriodicEventStreamResponse";
}

void to_json(json& j, const GetPeriodicEventStreamResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.constantStreamData) {
        if (j.empty()) {
            j = json{{"constantStreamData", json::array()}};
        } else {
            j["constantStreamData"] = json::array();
        }
        for (const auto& val : k.constantStreamData.value()) {
            j["constantStreamData"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetPeriodicEventStreamResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("constantStreamData")) {
        const json& arr = j.at("constantStreamData");
        std::vector<ConstantStreamData> vec;
        for (const auto& val : arr) {
            vec.push_back(val);
        }
        k.constantStreamData.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetPeriodicEventStreamResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the GetPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const GetPeriodicEventStreamResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
