// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_BASE_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_BASE_HPP

#include <ocpp/v16/types.hpp>

#include <cstddef>
#include <map>
#include <optional>
#include <set>

namespace ocpp::v16 {

constexpr std::size_t AUTHORIZATION_KEY_MIN_LENGTH = 8;
constexpr std::size_t OCSP_REQUEST_INTERVAL_MIN = 86400;
constexpr std::size_t SECC_LEAF_SUBJECT_COMMON_NAME_MIN_LENGTH = 7;
constexpr std::size_t SECC_LEAF_SUBJECT_COMMON_NAME_MAX_LENGTH = 64;
constexpr std::size_t SECC_LEAF_SUBJECT_COUNTRY_LENGTH = 2;
constexpr std::size_t SECC_LEAF_SUBJECT_ORGANIZATION_MAX_LENGTH = 64;
constexpr std::size_t CONNECTOR_EVSE_IDS_MAX_LENGTH = 1000;
constexpr std::int32_t MAX_WAIT_FOR_SET_USER_PRICE_TIMEOUT_MS = 30000;

/// \brief contains the configuration of the charge point
class ChargePointConfigurationBase {
public:
    using MessagesSet = std::set<MessageType>;
    using ProfilesSet = std::set<SupportedFeatureProfiles>;
    using FeaturesMap = std::map<SupportedFeatureProfiles, MessagesSet>;
    using MeasurandsMap = std::map<Measurand, std::vector<Phase>>;
    using MeasurandWithPhaseList = std::vector<MeasurandWithPhase>;

protected:
    static const FeaturesMap supported_message_types_from_charge_point;
    static const FeaturesMap supported_message_types_from_central_system;

    MessagesSet supported_message_types_receiving;
    MessagesSet supported_message_types_sending;
    ProfilesSet supported_feature_profiles;
    MeasurandsMap supported_measurands;

    void initialise(const ProfilesSet& initial_set, const std::optional<std::string>& supported_profiles_csl,
                    const std::optional<std::string>& supported_measurands_csl);

public:
    bool is_valid_supported_measurands(const std::string& csl);
    std::optional<MeasurandWithPhaseList> csv_to_measurand_with_phase_vector(const std::string& csl);

    static std::optional<std::uint32_t> extract_connector_id(const std::string& str);
    static std::string MeterPublicKey_string(std::uint32_t connector_id);

    static bool to_bool(const std::string& value);
    static bool isBool(const std::string& str);
    static bool isPositiveInteger(const std::string& str);
    static bool isConnectorPhaseRotationValid(std::int32_t num_connectors, const std::string& value);
    static bool isTimeOffset(const std::string& offset);
    static bool areValidEvseIds(const std::string& value);
    static std::string hexToString(const std::string& s);
    static bool isHexNotation(const std::string& s);
};

} // namespace ocpp::v16

#endif // OCPP_V16_CHARGE_POINT_CONFIGURATION_BASE_HPP
