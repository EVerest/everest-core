// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <ocpp/v16/messages/GetLocalListVersion.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string GetLocalListVersionRequest::get_type() const {
    return "GetLocalListVersion";
}

void to_json(json& j, const GetLocalListVersionRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    (void)k; // no elements to unpack, silence unused parameter warning
}

void from_json(const json& j, GetLocalListVersionRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    // no elements to unpack, silence unused parameter warning
    (void)j;
    (void)k;
}

/// \brief Writes the string representation of the given GetLocalListVersionRequest \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionRequest written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetLocalListVersionResponse::get_type() const {
    return "GetLocalListVersionResponse";
}

void to_json(json& j, const GetLocalListVersionResponse& k) {
    // the required parts of the message
    j = json{
        {"listVersion", k.listVersion},
    };
    // the optional parts of the message
}

void from_json(const json& j, GetLocalListVersionResponse& k) {
    // the required parts of the message
    k.listVersion = j.at("listVersion");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given GetLocalListVersionResponse \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionResponse written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
