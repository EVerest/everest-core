// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyPeriodicEventStream.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyPeriodicEventStream::get_type() const {
    return "NotifyPeriodicEventStream";
}

void to_json(json& j, const NotifyPeriodicEventStream& k) {
    // the required parts of the message
    j = json{
        {"data", k.data},
        {"id", k.id},
        {"pending", k.pending},
        {"basetime", k.basetime.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyPeriodicEventStream& k) {
    // the required parts of the message
    for (auto val : j.at("data")) {
        k.data.push_back(val);
    }
    k.id = j.at("id");
    k.pending = j.at("pending");
    k.basetime = ocpp::DateTime(std::string(j.at("basetime")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyPeriodicEventStream \p k to the given output stream \p os
/// \returns an output stream with the NotifyPeriodicEventStream written to
std::ostream& operator<<(std::ostream& os, const NotifyPeriodicEventStream& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
