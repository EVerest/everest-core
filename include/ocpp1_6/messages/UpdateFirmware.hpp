// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_UPDATEFIRMWARE_HPP
#define OCPP1_6_UPDATEFIRMWARE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct UpdateFirmwareRequest : public Message {
    TODO : std::string with format : uri location;
    DateTime retrieveDate;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;

    std::string get_type() const {
        return "UpdateFirmware";
    }

    friend void to_json(json& j, const UpdateFirmwareRequest& k) {
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

    friend void from_json(const json& j, UpdateFirmwareRequest& k) {
        // the required parts of the message
        k.location = j.at("location");
        k.retrieveDate = DateTime(std::string(j.at("retrieveDate")));
        ;

        // the optional parts of the message
        if (j.contains("retries")) {
            k.retries.emplace(j.at("retries"));
        }
        if (j.contains("retryInterval")) {
            k.retryInterval.emplace(j.at("retryInterval"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const UpdateFirmwareRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct UpdateFirmwareResponse : public Message {

    std::string get_type() const {
        return "UpdateFirmwareResponse";
    }

    friend void to_json(json& j, const UpdateFirmwareResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    friend void from_json(const json& j, UpdateFirmwareResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const UpdateFirmwareResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_UPDATEFIRMWARE_HPP
