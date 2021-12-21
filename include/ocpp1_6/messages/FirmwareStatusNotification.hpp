// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP
#define OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct FirmwareStatusNotificationRequest : public Message {
    FirmwareStatus status;

    std::string get_type() const {
        return "FirmwareStatusNotification";
    }

    friend void to_json(json& j, const FirmwareStatusNotificationRequest& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::firmware_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, FirmwareStatusNotificationRequest& k) {
        // the required parts of the message
        k.status = conversions::string_to_firmware_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct FirmwareStatusNotificationResponse : public Message {

    std::string get_type() const {
        return "FirmwareStatusNotificationResponse";
    }

    friend void to_json(json& j, const FirmwareStatusNotificationResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    friend void from_json(const json& j, FirmwareStatusNotificationResponse& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const FirmwareStatusNotificationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_FIRMWARESTATUSNOTIFICATION_HPP
