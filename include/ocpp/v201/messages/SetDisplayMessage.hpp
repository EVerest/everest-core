// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_SETDISPLAYMESSAGE_HPP
#define OCPP_V201_SETDISPLAYMESSAGE_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP SetDisplayMessage message
struct SetDisplayMessageRequest : public ocpp::Message {
    MessageInfo message;
    boost::optional<CustomData> customData;

    /// \brief Provides the type of this SetDisplayMessage message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetDisplayMessageRequest \p k to a given json object \p j
void to_json(json& j, const SetDisplayMessageRequest& k);

/// \brief Conversion from a given json object \p j to a given SetDisplayMessageRequest \p k
void from_json(const json& j, SetDisplayMessageRequest& k);

/// \brief Writes the string representation of the given SetDisplayMessageRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDisplayMessageRequest written to
std::ostream& operator<<(std::ostream& os, const SetDisplayMessageRequest& k);

/// \brief Contains a OCPP SetDisplayMessageResponse message
struct SetDisplayMessageResponse : public ocpp::Message {
    DisplayMessageStatusEnum status;
    boost::optional<CustomData> customData;
    boost::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this SetDisplayMessageResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetDisplayMessageResponse \p k to a given json object \p j
void to_json(json& j, const SetDisplayMessageResponse& k);

/// \brief Conversion from a given json object \p j to a given SetDisplayMessageResponse \p k
void from_json(const json& j, SetDisplayMessageResponse& k);

/// \brief Writes the string representation of the given SetDisplayMessageResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDisplayMessageResponse written to
std::ostream& operator<<(std::ostream& os, const SetDisplayMessageResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_SETDISPLAYMESSAGE_HPP
