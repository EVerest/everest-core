// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_CALL_TYPES_HPP
#define OCPP_COMMON_CALL_TYPES_HPP

#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <sstream>
#include <stdexcept>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

#include <ocpp/common/cistring.hpp>

using json = nlohmann::json;

namespace ocpp {

// The locations inside the message array
const auto MESSAGE_TYPE_ID = 0;
const auto MESSAGE_ID = 1;
const auto CALL_ACTION = 2;
const auto CALL_PAYLOAD = 3;
const auto CALLRESULT_PAYLOAD = 2;
const auto CALLERROR_ERROR_CODE = 2;
const auto CALLERROR_ERROR_DESCRIPTION = 3;
const auto CALLERROR_ERROR_DETAILS = 4;

/// \brief Contains a MessageId implementation based on a case insensitive string with a maximum length of 36 printable
/// ASCII characters
class MessageId : public CiString<36> {
    using CiString::CiString;
};

/// \brief Comparison operator< between two MessageId \p lhs and \p rhs
bool operator<(const MessageId& lhs, const MessageId& rhs);

/// \brief Conversion from a given MessageId \p k to a given json object \p j
void to_json(json& j, const MessageId& k);

/// \brief Conversion from a given json object \p j to a given MessageId \p k
void from_json(const json& j, MessageId& k);

/// \brief Contains the different message type ids
enum class MessageTypeId {
    CALL = 2,
    CALLRESULT = 3,
    CALLERROR = 4,
    UNKNOWN,
};

/// \brief Contains a OCPP Call message
template <class T> struct Call {
    T msg;
    MessageId uniqueId;

    /// \brief Creates a new Call message object
    Call() {
    }

    /// \brief Creates a new Call message object with the given OCPP message \p msg and \p uniqueID
    Call(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    /// \brief Conversion from a given Call message \p c to a given json object \p j
    friend void to_json(json& j, const Call& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALL);
        j.push_back(c.uniqueId.get());
        j.push_back(c.msg.get_type());
        j.push_back(json(c.msg));
    }

    /// \brief Conversion from a given json object \p j to a given Call message \p c
    friend void from_json(const json& j, Call& c) {
        // the required parts of the message
        c.msg = j.at(CALL_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    /// \brief Writes the given case Call \p c to the given output stream \p os
    /// \returns an output stream with the Call written to
    friend std::ostream& operator<<(std::ostream& os, const Call& c) {
        os << json(c).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP CallResult message
template <class T> struct CallResult {
    T msg;
    MessageId uniqueId;

    /// \brief Creates a new CallResult message object
    CallResult() {
    }

    /// \brief Creates a new CallResult message object with the given OCPP message \p msg and \p uniqueID
    CallResult(T msg, MessageId uniqueId) {
        this->msg = msg;
        this->uniqueId = uniqueId;
    }

    /// \brief Conversion from a given CallResult message \p c to a given json object \p j
    friend void to_json(json& j, const CallResult& c) {
        j = json::array();
        j.push_back(MessageTypeId::CALLRESULT);
        j.push_back(c.uniqueId.get());
        j.push_back(json(c.msg));
    }

    /// \brief Conversion from a given json object \p j to a given CallResult message \p c
    friend void from_json(const json& j, CallResult& c) {
        // the required parts of the message
        c.msg = j.at(CALLRESULT_PAYLOAD);
        c.uniqueId.set(j.at(MESSAGE_ID));
    }

    /// \brief Writes the given case CallResult \p c to the given output stream \p os
    /// \returns an output stream with the CallResult written to
    friend std::ostream& operator<<(std::ostream& os, const CallResult& c) {
        os << json(c).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP CallError message
struct CallError {
    MessageId uniqueId;
    std::string errorCode;
    std::string errorDescription;
    json errorDetails;

    /// \brief Creates a new CallError message object
    CallError();

    /// \brief Creates a new CallResult message object with the given \p uniqueID \p errorCode \p errorDescription and
    /// \p errorDetails
    CallError(const MessageId& uniqueId, const std::string& errorCode, const std::string& errorDescription,
              const json& errorDetails);
};

/// \brief Conversion from a given CallError message \p c to a given json object \p j
void to_json(json& j, const CallError& c);

/// \brief Conversion from a given json object \p j to a given CallError message \p c
void from_json(const json& j, CallError& c);

/// \brief Writes the given case CallError \p c to the given output stream \p os
/// \returns an output stream with the CallError written to
std::ostream& operator<<(std::ostream& os, const CallError& c);

} // namespace ocpp

#endif
