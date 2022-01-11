// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_AUTHORIZE_HPP
#define OCPP1_6_AUTHORIZE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 Authorize message
struct AuthorizeRequest : public Message {
    CiString20Type idTag;

    /// \brief Provides the type of this Authorize message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "Authorize";
    }

    /// \brief Conversion from a given AuthorizeRequest \p k to a given json object \p j
    friend void to_json(json& j, const AuthorizeRequest& k) {
        // the required parts of the message
        j = json{
            {"idTag", k.idTag},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given AuthorizeRequest \p k
    friend void from_json(const json& j, AuthorizeRequest& k) {
        // the required parts of the message
        k.idTag = j.at("idTag");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given AuthorizeRequest \p k to the given output stream \p os
    /// \returns an output stream with the AuthorizeRequest written to
    friend std::ostream& operator<<(std::ostream& os, const AuthorizeRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 AuthorizeResponse message
struct AuthorizeResponse : public Message {
    IdTagInfo idTagInfo;

    /// \brief Provides the type of this AuthorizeResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "AuthorizeResponse";
    }

    /// \brief Conversion from a given AuthorizeResponse \p k to a given json object \p j
    friend void to_json(json& j, const AuthorizeResponse& k) {
        // the required parts of the message
        j = json{
            {"idTagInfo", k.idTagInfo},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given AuthorizeResponse \p k
    friend void from_json(const json& j, AuthorizeResponse& k) {
        // the required parts of the message
        k.idTagInfo = j.at("idTagInfo");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given AuthorizeResponse \p k to the given output stream \p os
    /// \returns an output stream with the AuthorizeResponse written to
    friend std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_AUTHORIZE_HPP
