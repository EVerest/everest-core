// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYCRL_HPP
#define OCPP_V21_NOTIFYCRL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyCRL message
struct NotifyCRLRequest : public ocpp::Message {
    int32_t requestId;
    NotifyCRLStatusEnum status;
    std::optional<CiString<2000>> location;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyCRL message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyCRLRequest \p k to a given json object \p j
void to_json(json& j, const NotifyCRLRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyCRLRequest \p k
void from_json(const json& j, NotifyCRLRequest& k);

/// \brief Writes the string representation of the given NotifyCRLRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyCRLRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyCRLRequest& k);

/// \brief Contains a OCPP NotifyCRLResponse message
struct NotifyCRLResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyCRLResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyCRLResponse \p k to a given json object \p j
void to_json(json& j, const NotifyCRLResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyCRLResponse \p k
void from_json(const json& j, NotifyCRLResponse& k);

/// \brief Writes the string representation of the given NotifyCRLResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyCRLResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyCRLResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYCRL_HPP
