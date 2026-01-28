// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <ocpp/v16/charge_point_configuration_base.hpp>
#include <ocpp/v16/utils.hpp>

#include <exception>
#include <regex>
#include <string>

namespace ocpp::v16 {

const ChargePointConfigurationBase::FeaturesMap
    ChargePointConfigurationBase::supported_message_types_from_charge_point = {
        {SupportedFeatureProfiles::Core,
         {MessageType::Authorize, MessageType::BootNotification, MessageType::ChangeAvailabilityResponse,
          MessageType::ChangeConfigurationResponse, MessageType::ClearCacheResponse, MessageType::DataTransfer,
          MessageType::DataTransferResponse, MessageType::GetConfigurationResponse, MessageType::Heartbeat,
          MessageType::MeterValues, MessageType::RemoteStartTransactionResponse,
          MessageType::RemoteStopTransactionResponse, MessageType::ResetResponse, MessageType::StartTransaction,
          MessageType::StatusNotification, MessageType::StopTransaction, MessageType::UnlockConnectorResponse}},
        {SupportedFeatureProfiles::FirmwareManagement,
         {MessageType::GetDiagnosticsResponse, MessageType::DiagnosticsStatusNotification,
          MessageType::FirmwareStatusNotification, MessageType::UpdateFirmwareResponse}},
        {SupportedFeatureProfiles::LocalAuthListManagement,
         {MessageType::GetLocalListVersionResponse, MessageType::SendLocalListResponse}},
        {SupportedFeatureProfiles::RemoteTrigger, {MessageType::TriggerMessageResponse}},
        {SupportedFeatureProfiles::Reservation,
         {MessageType::CancelReservationResponse, MessageType::ReserveNowResponse}},
        {SupportedFeatureProfiles::SmartCharging,
         {MessageType::ClearChargingProfileResponse, MessageType::GetCompositeScheduleResponse,
          MessageType::SetChargingProfileResponse}},
        {SupportedFeatureProfiles::Security,
         {MessageType::CertificateSignedResponse, MessageType::DeleteCertificateResponse,
          MessageType::ExtendedTriggerMessageResponse, MessageType::GetInstalledCertificateIdsResponse,
          MessageType::GetLogResponse, MessageType::InstallCertificateResponse, MessageType::LogStatusNotification,
          MessageType::SecurityEventNotification, MessageType::SignCertificate,
          MessageType::SignedFirmwareStatusNotification, MessageType::SignedUpdateFirmwareResponse}}};

const ChargePointConfigurationBase::FeaturesMap
    ChargePointConfigurationBase::supported_message_types_from_central_system = {
        {SupportedFeatureProfiles::Core,
         {MessageType::AuthorizeResponse, MessageType::BootNotificationResponse, MessageType::ChangeAvailability,
          MessageType::ChangeConfiguration, MessageType::ClearCache, MessageType::DataTransfer,
          MessageType::DataTransferResponse, MessageType::GetConfiguration, MessageType::HeartbeatResponse,
          MessageType::MeterValuesResponse, MessageType::RemoteStartTransaction, MessageType::RemoteStopTransaction,
          MessageType::Reset, MessageType::StartTransactionResponse, MessageType::StatusNotificationResponse,
          MessageType::StopTransactionResponse, MessageType::UnlockConnector}},
        {SupportedFeatureProfiles::FirmwareManagement,
         {MessageType::GetDiagnostics, MessageType::DiagnosticsStatusNotificationResponse,
          MessageType::FirmwareStatusNotificationResponse, MessageType::UpdateFirmware}},
        {SupportedFeatureProfiles::LocalAuthListManagement,
         {MessageType::GetLocalListVersion, MessageType::SendLocalList}},
        {SupportedFeatureProfiles::RemoteTrigger, {MessageType::TriggerMessage}},
        {SupportedFeatureProfiles::Reservation, {MessageType::CancelReservation, MessageType::ReserveNow}},
        {SupportedFeatureProfiles::SmartCharging,
         {MessageType::ClearChargingProfile, MessageType::GetCompositeSchedule, MessageType::SetChargingProfile}},
        {SupportedFeatureProfiles::Security,
         {MessageType::CertificateSigned, MessageType::DeleteCertificate, MessageType::ExtendedTriggerMessage,
          MessageType::GetInstalledCertificateIds, MessageType::GetLog, MessageType::InstallCertificate,
          MessageType::LogStatusNotificationResponse, MessageType::SecurityEventNotificationResponse,
          MessageType::SignCertificateResponse, MessageType::SignedFirmwareStatusNotificationResponse,
          MessageType::SignedUpdateFirmware}}};

void ChargePointConfigurationBase::initialise(const ProfilesSet& initial_set,
                                              const std::optional<std::string>& supported_profiles_csl,
                                              const std::optional<std::string>& supported_measurands_csl) {
    supported_feature_profiles = initial_set;

    if (supported_profiles_csl) {
        const auto supported_profiles = utils::from_csl(supported_profiles_csl.value());
        for (const auto& component : supported_profiles) {
            try {
                supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles(component));
            } catch (const StringToEnumException& e) {
                EVLOG_error << "Feature profile: \"" << component << "\" not recognized";
                throw std::runtime_error("Unknown component in SupportedFeatureProfiles config option.");
            }
        }
    }

    // add Security behind the scenes as supported feature profile
    supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("Security"));

    // add Internal behind the scenes as supported feature profile
    supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("Internal"));

    // check Core profile is included
    if (const auto it = supported_feature_profiles.find(SupportedFeatureProfiles::Core);
        it == supported_feature_profiles.end()) {
        throw std::runtime_error("Core profile not listed in SupportedFeatureProfiles. This is required.");
    }

    // add supported messages based on supported features
    for (const auto& feature_profile : supported_feature_profiles) {
        if (const auto it = supported_message_types_from_charge_point.find(feature_profile);
            it != supported_message_types_from_charge_point.end()) {
            supported_message_types_sending.insert(it->second.begin(), it->second.end());
        }

        if (const auto it = supported_message_types_from_central_system.find(feature_profile);
            it != supported_message_types_from_central_system.end()) {
            supported_message_types_receiving.insert(it->second.begin(), it->second.end());
        }
    }

    // those MessageTypes should still be accepted and implement their individual handling in case the feature profile
    // is not supported
    supported_message_types_receiving.insert(MessageType::GetLocalListVersion);
    supported_message_types_receiving.insert(MessageType::SendLocalList);
    supported_message_types_receiving.insert(MessageType::ReserveNow);

    // populate supported_measurands

    if (supported_measurands_csl) {
        const auto supported = utils::from_csl(supported_measurands_csl.value());
        for (const auto& measurand : supported) {
            try {
                const auto measurand_type = conversions::string_to_measurand(measurand);
                switch (measurand_type) {
                case Measurand::Energy_Active_Export_Register:
                case Measurand::Energy_Active_Import_Register:
                case Measurand::Energy_Reactive_Export_Register:
                case Measurand::Energy_Reactive_Import_Register:
                case Measurand::Energy_Active_Export_Interval:
                case Measurand::Energy_Active_Import_Interval:
                case Measurand::Energy_Reactive_Export_Interval:
                case Measurand::Energy_Reactive_Import_Interval:
                case Measurand::Power_Active_Export:
                case Measurand::Power_Active_Import:
                case Measurand::Voltage:
                case Measurand::Frequency:
                case Measurand::Power_Reactive_Export:
                case Measurand::Power_Reactive_Import:
                    supported_measurands[measurand_type] = {Phase::L1, Phase::L2, Phase::L3};
                    break;
                case Measurand::Current_Import:
                case Measurand::Current_Export:
                    supported_measurands[measurand_type] = {Phase::L1, Phase::L2, Phase::L3, Phase::N};
                    break;
                case Measurand::Power_Factor:
                case Measurand::Current_Offered:
                case Measurand::Power_Offered:
                case Measurand::Temperature:
                case Measurand::SoC:
                case Measurand::RPM:
                    supported_measurands[measurand_type] = {};
                    break;
                default:
                    EVLOG_AND_THROW(std::runtime_error("Given SupportedMeasurands are invalid"));
                }
            } catch (const StringToEnumException& o) {
                EVLOG_AND_THROW(std::runtime_error("Given SupportedMeasurands are invalid"));
            }
        }
    }
}

bool ChargePointConfigurationBase::is_valid_supported_measurands(const std::string& csl) {
    const auto elements = utils::from_csl(csl);
    bool result{true};

    for (const auto& element : elements) {
        try {
            auto measurand = conversions::string_to_measurand(element);
            if (const auto it = supported_measurands.find(measurand); it == supported_measurands.end()) {
                // this measurand is not supported
                result = false;
                break;
            }
        } catch (const StringToEnumException& o) {
            EVLOG_warning << "Measurand: " << element << " is not supported!";
            result = false;
            break;
        }
    }

    return result;
}

std::optional<ChargePointConfigurationBase::MeasurandWithPhaseList>
ChargePointConfigurationBase::csv_to_measurand_with_phase_vector(const std::string& csl) {
    const auto elements = utils::from_csl(csl);
    bool element_error{false};

    // collect results in a vector (ensures no duplicates)
    std::set<Measurand> result_set; // capture measurands already in the vector
    std::vector<MeasurandWithPhase> result_vector;
    std::optional<std::vector<MeasurandWithPhase>> result;

    for (const auto& element : elements) {
        try {
            MeasurandWithPhase measurand_with_phase;
            measurand_with_phase.measurand = conversions::string_to_measurand(element);

            if (const auto it = supported_measurands.find(measurand_with_phase.measurand);
                it != supported_measurands.end()) {
                // this measurand is supported

                if (const auto included = result_set.find(measurand_with_phase.measurand);
                    included == result_set.end()) {
                    // this measurand hasn't been added to the result vector
                    result_set.insert(measurand_with_phase.measurand);

                    // the measurand without a phase as a total value
                    result_vector.push_back(measurand_with_phase);

                    if (it->second.size() > 0) {
                        //  measurand can be provided on multiple phases
                        for (const auto& phase : it->second) {
                            measurand_with_phase.phase = phase;
                            result_vector.push_back(measurand_with_phase);
                        }
                    }
                }
            }
        } catch (const StringToEnumException& o) {
            EVLOG_warning << "Measurand: " << element << " is not supported!";
            element_error = true;
            break;
        }
    }

    if (!element_error) {
        for (auto m : result_vector) {
            if (!m.phase) {
                EVLOG_debug << "measurand without phase: " << m.measurand;
            } else {
                EVLOG_debug << "measurand: " << m.measurand
                            << " with phase: " << conversions::phase_to_string(m.phase.value());
            }
        }

        result = std::move(result_vector);
    }

    return result;
}

std::optional<std::uint32_t> ChargePointConfigurationBase::extract_connector_id(const std::string& str) {
    // expected string "MeterPublicKey[1]"

    std::optional<std::uint32_t> result;
    const std::regex id_regex(R"(^.+\[(\d+)\]$)");
    std::smatch id_match;

    if (std::regex_match(str, id_match, id_regex)) {
        if (id_match.size() == 2) {
            try {
                result = std::stoul(id_match[1]);
            } catch (const std::exception& ex) {
                EVLOG_error << "Unable to extract connector ID from '" << str << "': " << ex.what();
            }
        }
    } else {
        EVLOG_error << "Connector ID not found: '" << str << '\'';
    }

    return result;
}

std::string ChargePointConfigurationBase::MeterPublicKey_string(std::uint32_t connector_id) {
    return std::string{"MeterPublicKey["} + std::to_string(connector_id) + ']';
}

bool ChargePointConfigurationBase::to_bool(const std::string& value) {
    std::string out = value;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    if ((out == "true") || out == "1") {
        return true;
    } else if ((out == "false") || out == "0") {
        return false;
    }
    throw std::invalid_argument("Not a boolean value");
}

bool ChargePointConfigurationBase::isBool(const std::string& str) {
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out == "true" || out == "false";
}

bool ChargePointConfigurationBase::isPositiveInteger(const std::string& str) {
    bool result{false};
    try {
        result = std::get<bool>(is_positive_integer(str));
    } catch (const std::invalid_argument& e) {
    } catch (const std::out_of_range& e) {
    }
    return result;
}

bool ChargePointConfigurationBase::isConnectorPhaseRotationValid(std::int32_t num_connectors,
                                                                 const std::string& value) {
    std::string str{value};
    bool result{true};

    str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
    auto elements = v16::utils::from_csl(str);

    // Filter per element of type 0.NotApplicable, 1.NotApplicable, or 0.Unknown etc
    for (int connector_id = 0; connector_id <= num_connectors; connector_id++) {
        std::string myNotApplicable = std::to_string(connector_id) + ".NotApplicable";
        std::string myNotDefined = std::to_string(connector_id) + ".Unknown";
        elements.erase(std::remove(elements.begin(), elements.end(), myNotApplicable), elements.end());
        elements.erase(std::remove(elements.begin(), elements.end(), myNotDefined), elements.end());
    }
    // if all elements are hit, accept it, else check the remaining
    if (elements.size() > 0) {
        for (const std::string& e : elements) {
            if (e.size() != 5) {
                result = false;
            } else {
                try {
                    auto connector = std::stoi(e.substr(0, 1));
                    if (connector < 0 or connector > num_connectors) {
                        result = false;
                    } else {
                        std::string phase_rotation = e.substr(2, 5);
                        if (phase_rotation != "RST" and phase_rotation != "RTS" and phase_rotation != "SRT" and
                            phase_rotation != "STR" and phase_rotation != "TRS" and phase_rotation != "TSR") {
                            result = false;
                        }
                    }
                } catch (const std::invalid_argument&) {
                    result = false;
                }
            }

            if (!result) {
                break;
            }
        }
    }
    return result;
}

bool ChargePointConfigurationBase::isTimeOffset(const std::string& offset) {
    bool result{false};

    const auto times = v16::utils::split_string(':', offset);
    if (times.size() != 2) {
        EVLOG_error
            << R"(Could not set display time offset: format not correct (should be something like "-05:00", but is )"
            << offset << ')';
    } else {
        try {
            result = true;
            // Check if strings are numbers.
            const int32_t hours = std::stoi(times.at(0));
            const int32_t minutes = std::stoi(times.at(1));

            // And check if numbers are valid.
            if (hours < -24 or hours > 24) {
                EVLOG_error << "Could not set display time offset: hours should be between -24 and +24, but is "
                            << times.at(0);
                result = false;
            }

            if (minutes < 0 or minutes > 59) {
                EVLOG_error << "Could not set display time offset: minutes should be between 0 and 59, but is "
                            << times.at(1);
                result = false;
            }

        } catch (const std::exception& e) {
            EVLOG_error
                << R"(Could not set display time offset: format not correct (should be something like "-19:15", but is )"
                << offset << "): " << e.what();
            result = false;
        }
    }
    return result;
}

bool ChargePointConfigurationBase::areValidEvseIds(const std::string& value) {
    bool result{true};

    if (value.length() <= CONNECTOR_EVSE_IDS_MAX_LENGTH) {
        // this fullfills parts of HUB-24-003 of Requirements EVSE Check PnC with ISO15118-2 v4
        const auto evse_ids = v16::utils::from_csl(value);
        for (const auto& evse_id : evse_ids) {
            if (evse_id.size() < 7 or evse_id.size() > 37) {
                EVLOG_warning << "Attempting to set ConnectorEvseIds to invalid value: " << evse_id;
                result = false;
                break;
            }
        }
    } else {
        result = false;
    }
    return result;
}

std::string ChargePointConfigurationBase::hexToString(const std::string& s) {
    std::string str;
    for (size_t i = 0; i < s.length(); i += 2) {
        std::string byte = s.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        str.push_back(chr);
    }
    return str;
}

bool ChargePointConfigurationBase::isHexNotation(const std::string& s) {
    // TODO(james-ctc): this check is problematic
    // need to identify what is considered valid
    // e.g. what can the first two characters be?
    // why are they not ignored in hexToString()?
    // the size should be a multiple of 2 …
    //
    // consider combining isHexNotation and hexToString to avoid reparsing
    // the string

    bool result = s.size() > 2 and s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
    if (result) {
        // check if every char is printable
        for (size_t i = 0; i < s.length(); i += 2) {
            std::string byte = s.substr(i, 2);
            char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
            // ignoring check for \n (0x0a) because find_first_not_of() has not
            // found one
            if (!std::isprint(chr)) {
                result = false;
                break;
            }
        }
    }
    return result;
}

} // namespace ocpp::v16
