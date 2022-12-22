// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v16/messages/UpdateFirmware.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string UpdateFirmwareRequest::get_type() const {
    return "UpdateFirmware";
}

void to_json(json& j, const UpdateFirmwareRequest& k) {
    // the required parts of the message
    j = json{
        {"location", k.location},
        {"retrieveDate", k.retrieveDate.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.retries) {
        j["retries"] = k.retries.value();
    }
    if (k.retryInterval) {
        j["retryInterval"] = k.retryInterval.value();
    }
}

void from_json(const json& j, UpdateFirmwareRequest& k) {
    // the required parts of the message
    k.location = j.at("location");
    k.retrieveDate = ocpp::DateTime(std::string(j.at("retrieveDate")));

    // the optional parts of the message
    if (j.contains("retries")) {
        k.retries.emplace(j.at("retries"));
    }
    if (j.contains("retryInterval")) {
        k.retryInterval.emplace(j.at("retryInterval"));
    }
}

/// \brief Writes the string representation of the given UpdateFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string UpdateFirmwareResponse::get_type() const {
    return "UpdateFirmwareResponse";
}

void to_json(json& j, const UpdateFirmwareResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, UpdateFirmwareResponse& k) {
    // the required parts of the message

    // the optional parts of the message
}

/// \brief Writes the string representation of the given UpdateFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
