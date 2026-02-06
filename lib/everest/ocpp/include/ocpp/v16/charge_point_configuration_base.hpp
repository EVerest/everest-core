// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_BASE_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_BASE_HPP

#include <ocpp/v16/types.hpp>

#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string_view>

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
    std::filesystem::path ocpp_main_path;

    void initialise(const ProfilesSet& initial_set, const std::optional<std::string>& supported_profiles_csl,
                    const std::optional<std::string>& supported_measurands_csl);

public:
    ChargePointConfigurationBase(const std::string_view& ocpp_main_path) : ocpp_main_path(ocpp_main_path) {
    }
    ChargePointConfigurationBase(const std::filesystem::path& ocpp_main_path) : ocpp_main_path(ocpp_main_path) {
    }
    ChargePointConfigurationBase() = delete;
    ChargePointConfigurationBase(const ChargePointConfigurationBase&) = delete;
    ChargePointConfigurationBase(ChargePointConfigurationBase&&) = delete;
    ChargePointConfigurationBase& operator=(const ChargePointConfigurationBase&) = delete;
    ChargePointConfigurationBase& operator=(ChargePointConfigurationBase&&) = delete;

    virtual ~ChargePointConfigurationBase() = default;

    bool validate(const std::string_view& schema_file, const json& object) const;

    bool isValidSupportedMeasurands(const std::string& csl) const;
    std::optional<MeasurandWithPhaseList> csvToMeasurandWithPhaseVector(const std::string& csl) const;

    static std::optional<std::uint32_t> extractConnectorId(const std::string& str);
    static std::string meterPublicKeyString(std::uint32_t connector_id);

    static bool toBool(const std::string& value);
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
