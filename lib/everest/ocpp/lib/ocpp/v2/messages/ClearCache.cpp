// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/ClearCache.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string ClearCacheRequest::get_type() const {
    return "ClearCache";
}

void to_json(json& j, const ClearCacheRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearCacheRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearCacheRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheRequest written to
std::ostream& operator<<(std::ostream& os, const ClearCacheRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearCacheResponse::get_type() const {
    return "ClearCacheResponse";
}

void to_json(json& j, const ClearCacheResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::clear_cache_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClearCacheResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_cache_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClearCacheResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearCacheResponse written to
std::ostream& operator<<(std::ostream& os, const ClearCacheResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
