// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_NOTIFYDISPLAYMESSAGES_HPP
#define OCPP_V201_NOTIFYDISPLAYMESSAGES_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP NotifyDisplayMessages message
struct NotifyDisplayMessagesRequest : public ocpp::Message {
    int32_t requestId;
    boost::optional<CustomData> customData;
    boost::optional<std::vector<MessageInfo>> messageInfo;
    boost::optional<bool> tbc;

    /// \brief Provides the type of this NotifyDisplayMessages message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyDisplayMessagesRequest \p k to a given json object \p j
void to_json(json& j, const NotifyDisplayMessagesRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyDisplayMessagesRequest \p k
void from_json(const json& j, NotifyDisplayMessagesRequest& k);

/// \brief Writes the string representation of the given NotifyDisplayMessagesRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyDisplayMessagesRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyDisplayMessagesRequest& k);

/// \brief Contains a OCPP NotifyDisplayMessagesResponse message
struct NotifyDisplayMessagesResponse : public ocpp::Message {
    boost::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyDisplayMessagesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyDisplayMessagesResponse \p k to a given json object \p j
void to_json(json& j, const NotifyDisplayMessagesResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyDisplayMessagesResponse \p k
void from_json(const json& j, NotifyDisplayMessagesResponse& k);

/// \brief Writes the string representation of the given NotifyDisplayMessagesResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyDisplayMessagesResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyDisplayMessagesResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_NOTIFYDISPLAYMESSAGES_HPP
