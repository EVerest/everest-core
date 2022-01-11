// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_HEARTBEAT_HPP
#define OCPP1_6_HEARTBEAT_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 Heartbeat message
struct HeartbeatRequest : public Message {

    /// \brief Provides the type of this Heartbeat message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "Heartbeat";
    }

    /// \brief Conversion from a given HeartbeatRequest \p k to a given json object \p j
    friend void to_json(json& j, const HeartbeatRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given HeartbeatRequest \p k
    friend void from_json(const json& j, HeartbeatRequest& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given HeartbeatRequest \p k to the given output stream \p os
    /// \returns an output stream with the HeartbeatRequest written to
    friend std::ostream& operator<<(std::ostream& os, const HeartbeatRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 HeartbeatResponse message
struct HeartbeatResponse : public Message {
    DateTime currentTime;

    /// \brief Provides the type of this HeartbeatResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "HeartbeatResponse";
    }

    /// \brief Conversion from a given HeartbeatResponse \p k to a given json object \p j
    friend void to_json(json& j, const HeartbeatResponse& k) {
        // the required parts of the message
        j = json{
            {"currentTime", k.currentTime.to_rfc3339()},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given HeartbeatResponse \p k
    friend void from_json(const json& j, HeartbeatResponse& k) {
        // the required parts of the message
        k.currentTime = DateTime(std::string(j.at("currentTime")));
        ;

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given HeartbeatResponse \p k to the given output stream \p os
    /// \returns an output stream with the HeartbeatResponse written to
    friend std::ostream& operator<<(std::ostream& os, const HeartbeatResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_HEARTBEAT_HPP
