// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyEvent.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyEventRequest::get_type() const {
    return "NotifyEvent";
}

void to_json(json& j, const NotifyEventRequest& k) {
    // the required parts of the message
    j = json{
        {"generatedAt", k.generatedAt.to_rfc3339()},
        {"seqNo", k.seqNo},
        {"eventData", k.eventData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
}

void from_json(const json& j, NotifyEventRequest& k) {
    // the required parts of the message
    k.generatedAt = ocpp::DateTime(std::string(j.at("generatedAt")));
    k.seqNo = j.at("seqNo");
    for (auto val : j.at("eventData")) {
        k.eventData.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
}

/// \brief Writes the string representation of the given NotifyEventRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyEventRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyEventRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyEventResponse::get_type() const {
    return "NotifyEventResponse";
}

void to_json(json& j, const NotifyEventResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyEventResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyEventResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyEventResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyEventResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
