// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_RESET_HPP
#define OCPP_V201_RESET_HPP

#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP Reset message
struct ResetRequest : public ocpp::Message {
    ResetEnum type;
    std::optional<CustomData> customData;
    std::optional<int32_t> evseId;

    /// \brief Provides the type of this Reset message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ResetRequest \p k to a given json object \p j
void to_json(json& j, const ResetRequest& k);

/// \brief Conversion from a given json object \p j to a given ResetRequest \p k
void from_json(const json& j, ResetRequest& k);

/// \brief Writes the string representation of the given ResetRequest \p k to the given output stream \p os
/// \returns an output stream with the ResetRequest written to
std::ostream& operator<<(std::ostream& os, const ResetRequest& k);

/// \brief Contains a OCPP ResetResponse message
struct ResetResponse : public ocpp::Message {
    ResetStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this ResetResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ResetResponse \p k to a given json object \p j
void to_json(json& j, const ResetResponse& k);

/// \brief Conversion from a given json object \p j to a given ResetResponse \p k
void from_json(const json& j, ResetResponse& k);

/// \brief Writes the string representation of the given ResetResponse \p k to the given output stream \p os
/// \returns an output stream with the ResetResponse written to
std::ostream& operator<<(std::ostream& os, const ResetResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_RESET_HPP
