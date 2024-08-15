// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <fstream>
#include <future>
#include <mutex>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <ocpp/common/schemas.hpp>
#include <ocpp/common/utils.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

ChargePointConfiguration::ChargePointConfiguration(const std::string& config, const fs::path& ocpp_main_path,
                                                   const fs::path& user_config_path) {

    this->user_config_path = user_config_path;
    if (!fs::exists(this->user_config_path)) {
        EVLOG_critical << "User config file does not exist";
        throw std::runtime_error("User config file does not exist");
    }

    // validate config entries
    const auto schemas_path = ocpp_main_path / "profile_schemas";
    Schemas schemas = Schemas(schemas_path);

    try {
        this->config = json::parse(config);
        const auto custom_schema_path = schemas_path / "Custom.json";
        if (fs::exists(custom_schema_path)) {
            std::ifstream ifs(custom_schema_path.c_str());
            std::string custom_schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            this->custom_schema = json::parse(custom_schema_file);
        }
    } catch (const json::parse_error& e) {
        EVLOG_error << "Error while parsing config file.";
        EVLOG_AND_THROW(e);
    }

    try {
        auto patch = schemas.get_validator()->validate(this->config);
        if (patch.is_null()) {
            // no defaults substituted
            EVLOG_debug << "Using a charge point configuration without default values.";
        } else {
            // extend config with default values
            EVLOG_debug << "Adding the following default values to the charge point configuration: " << patch;
            auto patched_config = this->config.patch(patch);
            this->config = patched_config;
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while validating OCPP config against schemas: " << e.what();
        EVLOG_AND_THROW(e);
    }

    if (!this->config["Core"].contains("SupportedFeatureProfiles")) {
        throw std::runtime_error("SupportedFeatureProfiles key is missing from config");
    }

    {
        std::vector<std::string> components;
        auto profiles = this->getSupportedFeatureProfiles();
        if (!profiles.empty()) {
            boost::split(components, profiles, boost::is_any_of(","));
            for (auto component : components) {
                try {
                    this->supported_feature_profiles.insert(
                        conversions::string_to_supported_feature_profiles(component));
                } catch (const StringToEnumException& e) {
                    EVLOG_error << "Feature profile: \"" << component << "\" not recognized";
                    throw std::runtime_error("Unknown component in SupportedFeatureProfiles config option.");
                }
            }
            // add Security behind the scenes as supported feature profile
            this->supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("Security"));

            // add Internal behind the scenes as supported feature profile
            this->supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("Internal"));

            if (this->config.contains("PnC")) {
                // add PnC behind the scenes as supported feature profile
                this->supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("PnC"));
            }

            if (this->config.contains("CostAndPrice")) {
                // Add California Pricing Requirements behind the scenes as supported feature profile
                this->supported_feature_profiles.insert(
                    conversions::string_to_supported_feature_profiles("CostAndPrice"));
            }

            if (this->config.contains("Custom")) {
                // add Custom behind the scenes as supported feature profile
                this->supported_feature_profiles.insert(conversions::string_to_supported_feature_profiles("Custom"));
            }
        }
    }
    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::Core) == 0) {
        throw std::runtime_error("Core profile not listed in SupportedFeatureProfiles. This is required.");
    }

    this->init_supported_measurands();

    if (!this->validate_measurands(this->config)) {
        EVLOG_AND_THROW(std::runtime_error("Given Measurands of either MeterValuesAlignedData, MeterValuesSampledData, "
                                           "StopTxnAlignedData or StopTxnSampledData are invalid or do not match the "
                                           "Measurands configured in SupportedMeasurands"));
    }

    this->supported_message_types_from_charge_point = {
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

    this->supported_message_types_from_central_system = {
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

    for (auto feature_profile : this->supported_feature_profiles) {
        this->supported_message_types_sending.insert(
            this->supported_message_types_from_charge_point[feature_profile].begin(),
            this->supported_message_types_from_charge_point[feature_profile].end());

        this->supported_message_types_receiving.insert(
            this->supported_message_types_from_central_system[feature_profile].begin(),
            this->supported_message_types_from_central_system[feature_profile].end());
    }

    // those MessageTypes should still be accepted and implement their individual handling in case the feature profile
    // is not supported
    this->supported_message_types_receiving.insert(MessageType::GetLocalListVersion);
    this->supported_message_types_receiving.insert(MessageType::SendLocalList);
    this->supported_message_types_receiving.insert(MessageType::ReserveNow);
}

json ChargePointConfiguration::get_user_config() {
    if (fs::exists(this->user_config_path)) {
        // reading from and overriding to existing user config
        std::fstream ifs(user_config_path.c_str());
        std::string user_config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        ifs.close();
        return json::parse(user_config_file);
    }

    return json({}, true);
}

void ChargePointConfiguration::setInUserConfig(std::string profile, std::string key, const json value) {
    json user_config = this->get_user_config();
    user_config[profile][key] = value;
    std::ofstream ofs(this->user_config_path.c_str());
    ofs << user_config << std::endl;
    ofs.close();
}

std::string to_csl(const std::vector<std::string>& vec) {
    std::string csl;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (it != vec.begin()) {
            csl += ",";
        }
        csl += *it;
    }
    return csl;
}

void ChargePointConfiguration::init_supported_measurands() {
    const auto _supported_measurands = ocpp::get_vector_from_csv(this->config["Internal"]["SupportedMeasurands"]);
    for (const auto& measurand : _supported_measurands) {
        try {
            const auto _measurand = conversions::string_to_measurand(measurand);
            switch (_measurand) {
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
                this->supported_measurands[_measurand] = {Phase::L1, Phase::L2, Phase::L3};
                break;
            case Measurand::Current_Import:
            case Measurand::Current_Export:
                this->supported_measurands[_measurand] = {Phase::L1, Phase::L2, Phase::L3, Phase::N};
                break;
            case Measurand::Power_Factor:
            case Measurand::Current_Offered:
            case Measurand::Power_Offered:
            case Measurand::Temperature:
            case Measurand::SoC:
            case Measurand::RPM:
                this->supported_measurands[_measurand] = {};
                break;
            default:
                EVLOG_AND_THROW(std::runtime_error("Given SupportedMeasurands are invalid"));
            }
        } catch (const StringToEnumException& o) {
            EVLOG_AND_THROW(std::runtime_error("Given SupportedMeasurands are invalid"));
        }
    }
}

// Internal config options
std::string ChargePointConfiguration::getChargePointId() {
    return this->config["Internal"]["ChargePointId"];
}
std::string ChargePointConfiguration::getCentralSystemURI() {
    return this->config["Internal"]["CentralSystemURI"];
}
std::string ChargePointConfiguration::getChargeBoxSerialNumber() {
    return this->config["Internal"]["ChargeBoxSerialNumber"];
}

CiString<20> ChargePointConfiguration::getChargePointModel() {
    return CiString<20>(this->config["Internal"]["ChargePointModel"].get<std::string>());
}
std::optional<CiString<25>> ChargePointConfiguration::getChargePointSerialNumber() {
    std::optional<CiString<25>> charge_point_serial_number = std::nullopt;
    if (this->config["Internal"].contains("ChargePointSerialNumber")) {
        charge_point_serial_number.emplace(this->config["Internal"]["ChargePointSerialNumber"].get<std::string>());
    }
    return charge_point_serial_number;
}

CiString<20> ChargePointConfiguration::getChargePointVendor() {
    return CiString<20>(this->config["Internal"]["ChargePointVendor"].get<std::string>());
}
CiString<50> ChargePointConfiguration::getFirmwareVersion() {
    return CiString<50>(this->config["Internal"]["FirmwareVersion"].get<std::string>());
}
std::optional<CiString<20>> ChargePointConfiguration::getICCID() {
    std::optional<CiString<20>> iccid = std::nullopt;
    if (this->config["Internal"].contains("ICCID")) {
        iccid.emplace(this->config["Internal"]["ICCID"].get<std::string>());
    }
    return iccid;
}
std::optional<CiString<20>> ChargePointConfiguration::getIMSI() {
    std::optional<CiString<20>> imsi = std::nullopt;
    if (this->config["Internal"].contains("IMSI")) {
        imsi.emplace(this->config["Internal"]["IMSI"].get<std::string>());
    }
    return imsi;
}
std::optional<CiString<25>> ChargePointConfiguration::getMeterSerialNumber() {
    std::optional<CiString<25>> meter_serial_number = std::nullopt;
    if (this->config["Internal"].contains("MeterSerialNumber")) {
        meter_serial_number.emplace(this->config["Internal"]["MeterSerialNumber"].get<std::string>());
    }
    return meter_serial_number;
}
std::optional<CiString<25>> ChargePointConfiguration::getMeterType() {
    std::optional<CiString<25>> meter_type = std::nullopt;
    if (this->config["Internal"].contains("MeterType")) {
        meter_type.emplace(this->config["Internal"]["MeterType"].get<std::string>());
    }
    return meter_type;
}
bool ChargePointConfiguration::getAuthorizeConnectorZeroOnConnectorOne() {
    if (this->getNumberOfConnectors() == 1) {
        return this->config["Internal"]["AuthorizeConnectorZeroOnConnectorOne"];
    }
    return false;
}
bool ChargePointConfiguration::getLogMessages() {
    return this->config["Internal"]["LogMessages"];
}
std::vector<std::string> ChargePointConfiguration::getLogMessagesFormat() {
    return this->config["Internal"]["LogMessagesFormat"];
}

bool ChargePointConfiguration::getLogRotation() {
    return this->config["Internal"]["LogRotation"];
}

bool ChargePointConfiguration::getLogRotationDateSuffix() {
    return this->config["Internal"]["LogRotationDateSuffix"];
}

uint64_t ChargePointConfiguration::getLogRotationMaximumFileSize() {
    return this->config["Internal"]["LogRotationMaximumFileSize"];
}

uint64_t ChargePointConfiguration::getLogRotationMaximumFileCount() {
    return this->config["Internal"]["LogRotationMaximumFileCount"];
}

std::vector<ChargingProfilePurposeType> ChargePointConfiguration::getSupportedChargingProfilePurposeTypes() {
    std::vector<ChargingProfilePurposeType> supported_purpose_types;
    const auto str_list = this->config["Internal"]["SupportedChargingProfilePurposeTypes"];
    for (const auto& str : str_list) {
        supported_purpose_types.push_back(conversions::string_to_charging_profile_purpose_type(str));
    }
    return supported_purpose_types;
}

int32_t ChargePointConfiguration::getMaxCompositeScheduleDuration() {
    return this->config["Internal"]["MaxCompositeScheduleDuration"];
}

std::string ChargePointConfiguration::getSupportedCiphers12() {

    std::vector<std::string> supported_ciphers = this->config["Internal"]["SupportedCiphers12"];
    return boost::algorithm::join(supported_ciphers, ":");
}

std::string ChargePointConfiguration::getSupportedCiphers13() {

    std::vector<std::string> supported_ciphers = this->config["Internal"]["SupportedCiphers13"];
    return boost::algorithm::join(supported_ciphers, ":");
}

bool ChargePointConfiguration::getUseSslDefaultVerifyPaths() {
    return this->config["Internal"]["UseSslDefaultVerifyPaths"];
}

bool ChargePointConfiguration::getVerifyCsmsCommonName() {
    return this->config["Internal"]["VerifyCsmsCommonName"];
}

bool ChargePointConfiguration::getUseTPM() {
    return this->config["Internal"]["UseTPM"];
}

bool ChargePointConfiguration::getVerifyCsmsAllowWildcards() {
    return this->config["Internal"]["VerifyCsmsAllowWildcards"];
}

void ChargePointConfiguration::setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards) {
    this->config["Internal"]["VerifyCsmsAllowWildcards"] = verify_csms_allow_wildcards;
    this->setInUserConfig("Internal", "VerifyCsmsAllowWildcards", verify_csms_allow_wildcards);
}

std::string ChargePointConfiguration::getSupportedMeasurands() {
    return this->config["Internal"]["SupportedMeasurands"];
}

KeyValue ChargePointConfiguration::getChargePointIdKeyValue() {
    KeyValue kv;
    kv.key = "ChargePointId";
    kv.readonly = true;
    kv.value.emplace(this->getChargePointId());
    return kv;
}

KeyValue ChargePointConfiguration::getCentralSystemURIKeyValue() {
    KeyValue kv;
    kv.key = "CentralSystemURI";
    kv.readonly = true;
    kv.value.emplace(this->getCentralSystemURI());
    return kv;
}

KeyValue ChargePointConfiguration::getChargeBoxSerialNumberKeyValue() {
    KeyValue kv;
    kv.key = "ChargeBoxSerialNumber";
    kv.readonly = true;
    kv.value.emplace(this->getChargeBoxSerialNumber());
    return kv;
}

KeyValue ChargePointConfiguration::getChargePointModelKeyValue() {
    KeyValue kv;
    kv.key = "ChargePointModel";
    kv.readonly = true;
    kv.value.emplace(this->getChargePointModel());
    return kv;
}

std::optional<KeyValue> ChargePointConfiguration::getChargePointSerialNumberKeyValue() {
    std::optional<KeyValue> charge_point_serial_number_kv = std::nullopt;
    auto charge_point_serial_number = this->getChargePointSerialNumber();
    if (charge_point_serial_number.has_value()) {
        KeyValue kv;
        kv.key = "ChargePointSerialNumber";
        kv.readonly = true;
        kv.value.emplace(charge_point_serial_number.value());
        charge_point_serial_number_kv.emplace(kv);
    }
    return charge_point_serial_number_kv;
}

KeyValue ChargePointConfiguration::getChargePointVendorKeyValue() {
    KeyValue kv;
    kv.key = "ChargePointVendor";
    kv.readonly = true;
    kv.value.emplace(this->getChargePointVendor());
    return kv;
}

KeyValue ChargePointConfiguration::getFirmwareVersionKeyValue() {
    KeyValue kv;
    kv.key = "FirmwareVersion";
    kv.readonly = true;
    kv.value.emplace(this->getFirmwareVersion());
    return kv;
}

std::optional<KeyValue> ChargePointConfiguration::getICCIDKeyValue() {
    std::optional<KeyValue> kv_opt;
    auto value = this->getICCID();
    if (value.has_value()) {
        KeyValue kv;
        kv.key = "ICCID";
        kv.readonly = true;
        kv.value.emplace(value.value());
        kv_opt.emplace(kv);
    }
    return kv_opt;
}

std::optional<KeyValue> ChargePointConfiguration::getIMSIKeyValue() {
    std::optional<KeyValue> kv_opt;
    auto value = this->getIMSI();
    if (value.has_value()) {
        KeyValue kv;
        kv.key = "IMSI";
        kv.readonly = true;
        kv.value.emplace(value.value());
        kv_opt.emplace(kv);
    }
    return kv_opt;
}

std::optional<KeyValue> ChargePointConfiguration::getMeterSerialNumberKeyValue() {
    std::optional<KeyValue> kv_opt;
    auto value = this->getMeterSerialNumber();
    if (value.has_value()) {
        KeyValue kv;
        kv.key = "MeterSerialNumber";
        kv.readonly = true;
        kv.value.emplace(value.value());
        kv_opt.emplace(kv);
    }
    return kv_opt;
}

std::optional<KeyValue> ChargePointConfiguration::getMeterTypeKeyValue() {
    std::optional<KeyValue> kv_opt;
    auto value = this->getMeterType();
    if (value.has_value()) {
        KeyValue kv;
        kv.key = "MeterType";
        kv.readonly = true;
        kv.value.emplace(value.value());
        kv_opt.emplace(kv);
    }
    return kv_opt;
}

KeyValue ChargePointConfiguration::getAuthorizeConnectorZeroOnConnectorOneKeyValue() {
    KeyValue kv;
    kv.key = "AuthorizeConnectorZeroOnConnectorOne";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getAuthorizeConnectorZeroOnConnectorOne()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogMessagesKeyValue() {
    KeyValue kv;
    kv.key = "LogMessages";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLogMessages()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogMessagesFormatKeyValue() {
    KeyValue kv;
    kv.key = "LogMessagesFormat";
    kv.readonly = true;
    kv.value.emplace(to_csl(this->getLogMessagesFormat()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogRotationKeyValue() {
    KeyValue kv;
    kv.key = "LogRotation";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLogRotation()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogRotationDateSuffixKeyValue() {
    KeyValue kv;
    kv.key = "LogRotationDateSuffix";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLogRotationDateSuffix()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogRotationMaximumFileSizeKeyValue() {
    KeyValue kv;
    kv.key = "LogRotationMaximumFileSize";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getLogRotationMaximumFileSize()));
    return kv;
}

KeyValue ChargePointConfiguration::getLogRotationMaximumFileCountKeyValue() {
    KeyValue kv;
    kv.key = "LogRotationMaximumFileCount";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getLogRotationMaximumFileCount()));
    return kv;
}

KeyValue ChargePointConfiguration::getSupportedChargingProfilePurposeTypesKeyValue() {
    KeyValue kv;
    kv.key = "SupportedChargingProfilePurposeTypes";
    kv.readonly = true;
    std::vector<std::string> purpose_types;
    for (const auto& entry : this->getSupportedChargingProfilePurposeTypes()) {
        purpose_types.push_back(conversions::charging_profile_purpose_type_to_string(entry));
    }
    kv.value.emplace(to_csl(purpose_types));
    return kv;
}

KeyValue ChargePointConfiguration::getMaxCompositeScheduleDurationKeyValue() {
    KeyValue kv;
    kv.key = "MaxCompositeScheduleDuration";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getMaxCompositeScheduleDuration()));
    return kv;
}

KeyValue ChargePointConfiguration::getSupportedCiphers12KeyValue() {
    KeyValue kv;
    kv.key = "SupportedCiphers12";
    kv.readonly = true;
    kv.value.emplace(this->getSupportedCiphers12());
    return kv;
}

KeyValue ChargePointConfiguration::getSupportedCiphers13KeyValue() {
    KeyValue kv;
    kv.key = "SupportedCiphers13";
    kv.readonly = true;
    kv.value.emplace(this->getSupportedCiphers13());
    return kv;
}

KeyValue ChargePointConfiguration::getUseSslDefaultVerifyPathsKeyValue() {
    KeyValue kv;
    kv.key = "UseSslDefaultVerifyPaths";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getUseSslDefaultVerifyPaths()));
    return kv;
}

KeyValue ChargePointConfiguration::getVerifyCsmsCommonNameKeyValue() {
    KeyValue kv;
    kv.key = "VerifyCsmsCommonName";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getVerifyCsmsCommonName()));
    return kv;
}

KeyValue ChargePointConfiguration::getVerifyCsmsAllowWildcardsKeyValue() {
    KeyValue kv;
    kv.key = "VerifyCsmsAllowWildcards";
    kv.readonly = true;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getVerifyCsmsAllowWildcards()));
    return kv;
}

KeyValue ChargePointConfiguration::getSupportedMeasurandsKeyValue() {
    KeyValue kv;
    kv.key = "SupportedMeasurands";
    kv.readonly = true;
    kv.value.emplace(this->getSupportedMeasurands());
    return kv;
}

int ChargePointConfiguration::getMaxMessageSize() {
    return this->config["Internal"]["MaxMessageSize"];
}

KeyValue ChargePointConfiguration::getMaxMessageSizeKeyValue() {
    KeyValue kv;
    kv.key = "MaxMessageSize";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getMaxMessageSize()));
    return kv;
}

KeyValue ChargePointConfiguration::getWebsocketPingPayloadKeyValue() {
    KeyValue kv;
    kv.key = "WebsocketPingPayload";
    kv.readonly = true;
    kv.value.emplace(this->getWebsocketPingPayload());
    return kv;
}

KeyValue ChargePointConfiguration::getWebsocketPongTimeoutKeyValue() {
    KeyValue kv;
    kv.key = "WebsocketPongTimeout";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getWebsocketPongTimeout()));
    return kv;
}

int32_t ChargePointConfiguration::getRetryBackoffRandomRange() {
    return this->config["Internal"]["RetryBackoffRandomRange"];
}

void ChargePointConfiguration::setRetryBackoffRandomRange(int32_t retry_backoff_random_range) {
    this->config["Internal"]["RetryBackoffRandomRange"] = retry_backoff_random_range;
    this->setInUserConfig("Internal", "RetryBackoffRandomRange", retry_backoff_random_range);
}

KeyValue ChargePointConfiguration::getRetryBackoffRandomRangeKeyValue() {
    KeyValue kv;
    kv.key = "RetryBackoffRandomRange";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getRetryBackoffRandomRange()));
    return kv;
}

int32_t ChargePointConfiguration::getRetryBackoffRepeatTimes() {
    return this->config["Internal"]["RetryBackoffRepeatTimes"];
}

void ChargePointConfiguration::setRetryBackoffRepeatTimes(int32_t retry_backoff_repeat_times) {
    this->config["Internal"]["RetryBackoffRepeatTimes"] = retry_backoff_repeat_times;
    this->setInUserConfig("Internal", "RetryBackoffRepeatTimes", retry_backoff_repeat_times);
}

KeyValue ChargePointConfiguration::getRetryBackoffRepeatTimesKeyValue() {
    KeyValue kv;
    kv.key = "RetryBackoffRepeatTimes";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getRetryBackoffRepeatTimes()));
    return kv;
}

int32_t ChargePointConfiguration::getRetryBackoffWaitMinimum() {
    return this->config["Internal"]["RetryBackoffWaitMinimum"];
}

void ChargePointConfiguration::setRetryBackoffWaitMinimum(int32_t retry_backoff_wait_minimum) {
    this->config["Internal"]["RetryBackoffWaitMinimum"] = retry_backoff_wait_minimum;
    this->setInUserConfig("Internal", "RetryBackoffWaitMinimum", retry_backoff_wait_minimum);
}

KeyValue ChargePointConfiguration::getRetryBackoffWaitMinimumKeyValue() {
    KeyValue kv;
    kv.key = "RetryBackoffWaitMinimum";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getRetryBackoffWaitMinimum()));
    return kv;
}

std::vector<MeasurandWithPhase> ChargePointConfiguration::csv_to_measurand_with_phase_vector(std::string csv) {
    std::vector<std::string> components;

    boost::split(components, csv, boost::is_any_of(","));
    std::vector<MeasurandWithPhase> measurand_with_phase_vector;
    if (csv.empty()) {
        return measurand_with_phase_vector;
    }
    for (const auto& component : components) {
        MeasurandWithPhase measurand_with_phase;
        Measurand measurand = conversions::string_to_measurand(component);
        // check if this measurand can be provided on multiple phases
        if (this->supported_measurands.count(measurand) and this->supported_measurands.at(measurand).size() > 0) {
            // multiple phases are available
            // also add the measurand without a phase as a total value
            measurand_with_phase.measurand = measurand;
            measurand_with_phase_vector.push_back(measurand_with_phase);

            for (auto phase : this->supported_measurands[measurand]) {
                measurand_with_phase.phase.emplace(phase);
                if (std::find(measurand_with_phase_vector.begin(), measurand_with_phase_vector.end(),
                              measurand_with_phase) == measurand_with_phase_vector.end()) {
                    measurand_with_phase_vector.push_back(measurand_with_phase);
                }
            }
        } else {
            // this is a measurand without any phase support
            measurand_with_phase.measurand = measurand;
            if (std::find(measurand_with_phase_vector.begin(), measurand_with_phase_vector.end(),
                          measurand_with_phase) == measurand_with_phase_vector.end()) {
                measurand_with_phase_vector.push_back(measurand_with_phase);
            }
        }
    }
    for (auto m : measurand_with_phase_vector) {
        if (!m.phase) {
            EVLOG_debug << "measurand without phase: " << m.measurand;
        } else {
            EVLOG_debug << "measurand: " << m.measurand
                        << " with phase: " << conversions::phase_to_string(m.phase.value());
        }
    }
    return measurand_with_phase_vector;
}

bool ChargePointConfiguration::validate_measurands(const json& config) {
    std::vector<std::string> measurands_vector;

    // validate measurands of all these configuration keys
    measurands_vector.push_back(config["Core"]["MeterValuesAlignedData"]);
    measurands_vector.push_back(config["Core"]["MeterValuesSampledData"]);
    measurands_vector.push_back(config["Core"]["StopTxnAlignedData"]);
    measurands_vector.push_back(config["Core"]["StopTxnSampledData"]);

    for (const auto measurands : measurands_vector) {
        if (!this->measurands_supported(measurands)) {
            return false;
        }
    }
    return true;
}

bool ChargePointConfiguration::measurands_supported(std::string csv) {

    if (csv.empty()) {
        return true;
    }

    std::vector<std::string> components;

    boost::split(components, csv, boost::is_any_of(","));
    for (auto component : components) {
        try {
            conversions::string_to_measurand(component);
        } catch (const StringToEnumException& o) {
            EVLOG_warning << "Measurand: " << component << " is not supported!";
            return false;
        }
    }

    auto requested_measurands = this->csv_to_measurand_with_phase_vector(csv);
    // check if the requested measurands are supported, otherwise return false
    for (auto req : requested_measurands) {
        if (this->supported_measurands.count(req.measurand) == 0) {
            return false;
        }

        if (req.phase) {
            auto phase = req.phase.value();
            auto measurand = this->supported_measurands[req.measurand];

            if (std::find(measurand.begin(), measurand.end(), phase) == measurand.end()) {
                // phase not found, this is an error
                return false;
            }
        }
    }
    return true;
}

std::set<MessageType> ChargePointConfiguration::getSupportedMessageTypesSending() {
    return this->supported_message_types_sending;
}

std::set<MessageType> ChargePointConfiguration::getSupportedMessageTypesReceiving() {
    return this->supported_message_types_receiving;
}

std::string ChargePointConfiguration::getWebsocketPingPayload() {
    return this->config["Internal"]["WebsocketPingPayload"];
}

int32_t ChargePointConfiguration::getWebsocketPongTimeout() {
    return this->config["Internal"]["WebsocketPongTimeout"];
}

std::optional<std::string> ChargePointConfiguration::getHostName() {
    std::optional<std::string> hostName_key = std::nullopt;
    if (this->config["Internal"].contains("HostName")) {
        hostName_key.emplace(this->config["Internal"]["HostName"]);
    }
    return hostName_key;
}

std::optional<std::string> ChargePointConfiguration::getIFace() {
    std::optional<std::string> iFace_key = std::nullopt;
    if (this->config["Internal"].contains("IFace")) {
        iFace_key.emplace(this->config["Internal"]["IFace"]);
    }
    return iFace_key;
}

std::optional<bool> ChargePointConfiguration::getQueueAllMessages() {
    std::optional<bool> queue_all_messages = std::nullopt;
    if (this->config["Internal"].contains("QueueAllMessages")) {
        queue_all_messages.emplace(this->config["Internal"]["QueueAllMessages"]);
    }
    return queue_all_messages;
}

std::optional<KeyValue> ChargePointConfiguration::getQueueAllMessagesKeyValue() {
    std::optional<KeyValue> queue_all_messages_kv = std::nullopt;
    auto queue_all_messages = this->getQueueAllMessages();
    if (queue_all_messages.has_value()) {
        KeyValue kv;
        kv.key = "QueueAllMessages";
        kv.readonly = true;
        kv.value.emplace(ocpp::conversions::bool_to_string(queue_all_messages.value()));
        queue_all_messages_kv.emplace(kv);
    }
    return queue_all_messages_kv;
}

std::optional<int> ChargePointConfiguration::getMessageQueueSizeThreshold() {
    std::optional<int> message_queue_size_threshold = std::nullopt;
    if (this->config["Internal"].contains("MessageQueueSizeThreshold")) {
        message_queue_size_threshold.emplace(this->config["Internal"]["MessageQueueSizeThreshold"]);
    }
    return message_queue_size_threshold;
}

std::optional<KeyValue> ChargePointConfiguration::getMessageQueueSizeThresholdKeyValue() {
    std::optional<KeyValue> message_queue_size_threshold_kv = std::nullopt;
    auto message_queue_size_threshold = this->getMessageQueueSizeThreshold();
    if (message_queue_size_threshold.has_value()) {
        KeyValue kv;
        kv.key = "MessageQueueSizeThreshold";
        kv.readonly = true;
        kv.value.emplace(std::to_string(message_queue_size_threshold.value()));
        message_queue_size_threshold_kv.emplace(kv);
    }
    return message_queue_size_threshold_kv;
}

// Core Profile - optional
std::optional<bool> ChargePointConfiguration::getAllowOfflineTxForUnknownId() {
    std::optional<bool> unknown_offline_auth = std::nullopt;
    if (this->config["Core"].contains("AllowOfflineTxForUnknownId")) {
        unknown_offline_auth.emplace(this->config["Core"]["AllowOfflineTxForUnknownId"]);
    }
    return unknown_offline_auth;
}
void ChargePointConfiguration::setAllowOfflineTxForUnknownId(bool enabled) {
    if (this->getAllowOfflineTxForUnknownId() != std::nullopt) {
        this->config["Core"]["AllowOfflineTxForUnknownId"] = enabled;
        this->setInUserConfig("Core", "AllowOfflineTxForUnknownId", enabled);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getAllowOfflineTxForUnknownIdKeyValue() {
    std::optional<KeyValue> unknown_offline_auth_kv = std::nullopt;
    auto unknown_offline_auth = this->getAllowOfflineTxForUnknownId();
    if (unknown_offline_auth != std::nullopt) {
        KeyValue kv;
        kv.key = "AllowOfflineTxForUnknownId";
        kv.readonly = false;
        kv.value.emplace(ocpp::conversions::bool_to_string(unknown_offline_auth.value()));
        unknown_offline_auth_kv.emplace(kv);
    }
    return unknown_offline_auth_kv;
}

// Core Profile - optional
std::optional<bool> ChargePointConfiguration::getAuthorizationCacheEnabled() {
    std::optional<bool> enabled = std::nullopt;
    if (this->config["Core"].contains("AuthorizationCacheEnabled")) {
        enabled.emplace(this->config["Core"]["AuthorizationCacheEnabled"]);
    }
    return enabled;
}
void ChargePointConfiguration::setAuthorizationCacheEnabled(bool enabled) {
    if (this->getAuthorizationCacheEnabled() != std::nullopt) {
        this->config["Core"]["AuthorizationCacheEnabled"] = enabled;
        this->setInUserConfig("Core", "AuthorizationCacheEnabled", enabled);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getAuthorizationCacheEnabledKeyValue() {
    std::optional<KeyValue> enabled_kv = std::nullopt;
    auto enabled = this->getAuthorizationCacheEnabled();
    if (enabled != std::nullopt) {
        KeyValue kv;
        kv.key = "AuthorizationCacheEnabled";
        kv.readonly = false;
        kv.value.emplace(ocpp::conversions::bool_to_string(enabled.value()));
        enabled_kv.emplace(kv);
    }
    return enabled_kv;
}

// Core Profile
bool ChargePointConfiguration::getAuthorizeRemoteTxRequests() {
    return this->config["Core"]["AuthorizeRemoteTxRequests"];
}
void ChargePointConfiguration::setAuthorizeRemoteTxRequests(bool enabled) {
    this->config["Core"]["AuthorizeRemoteTxRequests"] = enabled;
    this->setInUserConfig("Core", "AuthorizeRemoteTxRequests", enabled);
}
KeyValue ChargePointConfiguration::getAuthorizeRemoteTxRequestsKeyValue() {
    KeyValue kv;
    kv.key = "AuthorizeRemoteTxRequests";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getAuthorizeRemoteTxRequests()));
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getBlinkRepeat() {
    std::optional<int32_t> blink_repeat = std::nullopt;
    if (this->config["Core"].contains("BlinkRepeat")) {
        blink_repeat.emplace(this->config["Core"]["BlinkRepeat"]);
    }
    return blink_repeat;
}
void ChargePointConfiguration::setBlinkRepeat(int32_t blink_repeat) {
    if (this->getBlinkRepeat() != std::nullopt) {
        this->config["Core"]["BlinkRepeat"] = blink_repeat;
        this->setInUserConfig("Core", "BlinkRepeat", blink_repeat);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getBlinkRepeatKeyValue() {
    std::optional<KeyValue> blink_repeat_kv = std::nullopt;
    auto blink_repeat = this->getBlinkRepeat();
    if (blink_repeat != std::nullopt) {
        KeyValue kv;
        kv.key = "BlinkRepeat";
        kv.readonly = false;
        kv.value.emplace(std::to_string(blink_repeat.value()));
        blink_repeat_kv.emplace(kv);
    }
    return blink_repeat_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getClockAlignedDataInterval() {
    return this->config["Core"]["ClockAlignedDataInterval"];
}
void ChargePointConfiguration::setClockAlignedDataInterval(int32_t interval) {
    this->config["Core"]["ClockAlignedDataInterval"] = interval;
    this->setInUserConfig("Core", "ClockAlignedDataInterval", interval);
}
KeyValue ChargePointConfiguration::getClockAlignedDataIntervalKeyValue() {
    KeyValue kv;
    kv.key = "ClockAlignedDataInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getClockAlignedDataInterval()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getConnectionTimeOut() {
    return this->config["Core"]["ConnectionTimeOut"];
}
void ChargePointConfiguration::setConnectionTimeOut(int32_t timeout) {
    this->config["Core"]["ConnectionTimeOut"] = timeout;
    this->setInUserConfig("Core", "ConnectionTimeOut", timeout);
}
KeyValue ChargePointConfiguration::getConnectionTimeOutKeyValue() {
    KeyValue kv;
    kv.key = "ConnectionTimeOut";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getConnectionTimeOut()));
    return kv;
}

// Core Profile
std::string ChargePointConfiguration::getConnectorPhaseRotation() {
    return this->config["Core"]["ConnectorPhaseRotation"];
}
void ChargePointConfiguration::setConnectorPhaseRotation(std::string connector_phase_rotation) {
    this->config["Core"]["ConnectorPhaseRotation"] = connector_phase_rotation;
    this->setInUserConfig("Core", "ConnectorPhaseRotation", connector_phase_rotation);
}
KeyValue ChargePointConfiguration::getConnectorPhaseRotationKeyValue() {
    KeyValue kv;
    kv.key = "ConnectorPhaseRotation";
    kv.readonly = false;
    kv.value.emplace(this->getConnectorPhaseRotation());
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getConnectorPhaseRotationMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("ConnectorPhaseRotationMaxLength")) {
        max_length.emplace(this->config["Core"]["ConnectorPhaseRotationMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getConnectorPhaseRotationMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getConnectorPhaseRotationMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "ConnectorPhaseRotationMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getGetConfigurationMaxKeys() {
    return this->config["Core"]["GetConfigurationMaxKeys"];
}
KeyValue ChargePointConfiguration::getGetConfigurationMaxKeysKeyValue() {
    KeyValue kv;
    kv.key = "GetConfigurationMaxKeys";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getGetConfigurationMaxKeys()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getHeartbeatInterval() {
    return this->config["Core"]["HeartbeatInterval"];
}
void ChargePointConfiguration::setHeartbeatInterval(int32_t interval) {
    this->config["Core"]["HeartbeatInterval"] = interval;
    this->setInUserConfig("Core", "HeartbeatInterval", interval);
}
KeyValue ChargePointConfiguration::getHeartbeatIntervalKeyValue() {
    KeyValue kv;
    kv.key = "HeartbeatInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getHeartbeatInterval()));
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getLightIntensity() {
    std::optional<int32_t> light_intensity = std::nullopt;
    if (this->config["Core"].contains("LightIntensity")) {
        light_intensity.emplace(this->config["Core"]["LightIntensity"]);
    }
    return light_intensity;
}
void ChargePointConfiguration::setLightIntensity(int32_t light_intensity) {
    if (this->getLightIntensity() != std::nullopt) {
        this->config["Core"]["LightIntensity"] = light_intensity;
        this->setInUserConfig("Core", "LightIntensity", light_intensity);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getLightIntensityKeyValue() {
    std::optional<KeyValue> light_intensity_kv = std::nullopt;
    auto light_intensity = this->getLightIntensity();
    if (light_intensity != std::nullopt) {
        KeyValue kv;
        kv.key = "LightIntensity";
        kv.readonly = false;
        kv.value.emplace(std::to_string(light_intensity.value()));
        light_intensity_kv.emplace(kv);
    }
    return light_intensity_kv;
}

// Core Profile
bool ChargePointConfiguration::getLocalAuthorizeOffline() {
    return this->config["Core"]["LocalAuthorizeOffline"];
}
void ChargePointConfiguration::setLocalAuthorizeOffline(bool local_authorize_offline) {
    this->config["Core"]["LocalAuthorizeOffline"] = local_authorize_offline;
    this->setInUserConfig("Core", "LocalAuthorizeOffline", local_authorize_offline);
}
KeyValue ChargePointConfiguration::getLocalAuthorizeOfflineKeyValue() {
    KeyValue kv;
    kv.key = "LocalAuthorizeOffline";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLocalAuthorizeOffline()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getLocalPreAuthorize() {
    return this->config["Core"]["LocalPreAuthorize"];
}
void ChargePointConfiguration::setLocalPreAuthorize(bool local_pre_authorize) {
    this->config["Core"]["LocalPreAuthorize"] = local_pre_authorize;
    this->setInUserConfig("Core", "LocalPreAuthorize", local_pre_authorize);
}
KeyValue ChargePointConfiguration::getLocalPreAuthorizeKeyValue() {
    KeyValue kv;
    kv.key = "LocalPreAuthorize";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLocalPreAuthorize()));
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getMaxEnergyOnInvalidId() {
    std::optional<int32_t> max_energy = std::nullopt;
    if (this->config["Core"].contains("MaxEnergyOnInvalidId")) {
        max_energy.emplace(this->config["Core"]["MaxEnergyOnInvalidId"]);
    }
    return max_energy;
}
void ChargePointConfiguration::setMaxEnergyOnInvalidId(int32_t max_energy) {
    if (this->getMaxEnergyOnInvalidId() != std::nullopt) {
        this->config["Core"]["MaxEnergyOnInvalidId"] = max_energy;
        this->setInUserConfig("Core", "MaxEnergyOnInvalidId", max_energy);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getMaxEnergyOnInvalidIdKeyValue() {
    std::optional<KeyValue> max_energy_kv = std::nullopt;
    auto max_energy = this->getMaxEnergyOnInvalidId();
    if (max_energy != std::nullopt) {
        KeyValue kv;
        kv.key = "MaxEnergyOnInvalidId";
        kv.readonly = false;
        kv.value.emplace(std::to_string(max_energy.value()));
        max_energy_kv.emplace(kv);
    }
    return max_energy_kv;
}

// Core Profile
std::string ChargePointConfiguration::getMeterValuesAlignedData() {
    return this->config["Core"]["MeterValuesAlignedData"];
}
bool ChargePointConfiguration::setMeterValuesAlignedData(std::string meter_values_aligned_data) {
    if (!this->measurands_supported(meter_values_aligned_data)) {
        return false;
    }
    this->config["Core"]["MeterValuesAlignedData"] = meter_values_aligned_data;
    this->setInUserConfig("Core", "MeterValuesAlignedData", meter_values_aligned_data);
    return true;
}
KeyValue ChargePointConfiguration::getMeterValuesAlignedDataKeyValue() {
    KeyValue kv;
    kv.key = "MeterValuesAlignedData";
    kv.readonly = false;
    kv.value.emplace(this->getMeterValuesAlignedData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getMeterValuesAlignedDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getMeterValuesAlignedData());
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getMeterValuesAlignedDataMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("MeterValuesAlignedDataMaxLength")) {
        max_length.emplace(this->config["Core"]["MeterValuesAlignedDataMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getMeterValuesAlignedDataMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getMeterValuesAlignedDataMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "MeterValuesAlignedDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getMeterValuesSampledData() {
    return this->config["Core"]["MeterValuesSampledData"];
}
bool ChargePointConfiguration::setMeterValuesSampledData(std::string meter_values_sampled_data) {
    if (!this->measurands_supported(meter_values_sampled_data)) {
        return false;
    }
    this->config["Core"]["MeterValuesSampledData"] = meter_values_sampled_data;
    this->setInUserConfig("Core", "MeterValuesSampledData", meter_values_sampled_data);
    return true;
}
KeyValue ChargePointConfiguration::getMeterValuesSampledDataKeyValue() {
    KeyValue kv;
    kv.key = "MeterValuesSampledData";
    kv.readonly = false;
    kv.value.emplace(this->getMeterValuesSampledData());
    return kv;
}
std::vector<MeasurandWithPhase> ChargePointConfiguration::getMeterValuesSampledDataVector() {
    return this->csv_to_measurand_with_phase_vector(this->getMeterValuesSampledData());
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getMeterValuesSampledDataMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("MeterValuesSampledDataMaxLength")) {
        max_length.emplace(this->config["Core"]["MeterValuesSampledDataMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getMeterValuesSampledDataMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getMeterValuesSampledDataMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "MeterValuesSampledDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getMeterValueSampleInterval() {
    return this->config["Core"]["MeterValueSampleInterval"];
}
void ChargePointConfiguration::setMeterValueSampleInterval(int32_t interval) {
    this->config["Core"]["MeterValueSampleInterval"] = interval;
    this->setInUserConfig("Core", "MeterValueSampleInterval", interval);
}
KeyValue ChargePointConfiguration::getMeterValueSampleIntervalKeyValue() {
    KeyValue kv;
    kv.key = "MeterValueSampleInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getMeterValueSampleInterval()));
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getMinimumStatusDuration() {
    std::optional<int32_t> minimum_status_duration = std::nullopt;
    if (this->config["Core"].contains("MinimumStatusDuration")) {
        minimum_status_duration.emplace(this->config["Core"]["MinimumStatusDuration"]);
    }
    return minimum_status_duration;
}
void ChargePointConfiguration::setMinimumStatusDuration(int32_t minimum_status_duration) {
    if (this->getMinimumStatusDuration() != std::nullopt) {
        this->config["Core"]["MinimumStatusDuration"] = minimum_status_duration;
        this->setInUserConfig("Core", "MinimumStatusDuration", minimum_status_duration);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getMinimumStatusDurationKeyValue() {
    std::optional<KeyValue> minimum_status_duration_kv = std::nullopt;
    auto minimum_status_duration = this->getMinimumStatusDuration();
    if (minimum_status_duration != std::nullopt) {
        KeyValue kv;
        kv.key = "MinimumStatusDuration";
        kv.readonly = false;
        kv.value.emplace(std::to_string(minimum_status_duration.value()));
        minimum_status_duration_kv.emplace(kv);
    }
    return minimum_status_duration_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getNumberOfConnectors() {
    return this->config["Core"]["NumberOfConnectors"];
}
KeyValue ChargePointConfiguration::getNumberOfConnectorsKeyValue() {
    KeyValue kv;
    kv.key = "NumberOfConnectors";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getNumberOfConnectors()));
    return kv;
}

// Reservation Profile
std::optional<bool> ChargePointConfiguration::getReserveConnectorZeroSupported() {
    std::optional<bool> reserve_connector_zero_supported = std::nullopt;
    if (this->config.contains("Reservation") && this->config["Reservation"].contains("ReserveConnectorZeroSupported")) {
        reserve_connector_zero_supported.emplace(this->config["Reservation"]["ReserveConnectorZeroSupported"]);
    }
    return reserve_connector_zero_supported;
}

std::optional<KeyValue> ChargePointConfiguration::getReserveConnectorZeroSupportedKeyValue() {
    std::optional<KeyValue> reserve_connector_zero_supported_kv = std::nullopt;
    auto reserve_connector_zero_supported = this->getReserveConnectorZeroSupported();
    if (reserve_connector_zero_supported != std::nullopt) {
        KeyValue kv;
        kv.key = "ReserveConnectorZeroSupported";
        kv.readonly = true;
        kv.value.emplace(std::to_string(reserve_connector_zero_supported.value()));
        reserve_connector_zero_supported_kv.emplace(kv);
    }
    return reserve_connector_zero_supported_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getResetRetries() {
    return this->config["Core"]["ResetRetries"];
}
void ChargePointConfiguration::setResetRetries(int32_t retries) {
    this->config["Core"]["ResetRetries"] = retries;
    this->setInUserConfig("Core", "ResetRetries", retries);
}
KeyValue ChargePointConfiguration::getResetRetriesKeyValue() {
    KeyValue kv;
    kv.key = "ResetRetries";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getResetRetries()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getStopTransactionOnEVSideDisconnect() {
    return this->config["Core"]["StopTransactionOnEVSideDisconnect"];
}
void ChargePointConfiguration::setStopTransactionOnEVSideDisconnect(bool stop_transaction_on_ev_side_disconnect) {
    this->config["Core"]["StopTransactionOnEVSideDisconnect"] = stop_transaction_on_ev_side_disconnect;
    this->setInUserConfig("Core", "StopTransactionOnEVSideDisconnect", stop_transaction_on_ev_side_disconnect);
}
KeyValue ChargePointConfiguration::getStopTransactionOnEVSideDisconnectKeyValue() {
    KeyValue kv;
    kv.key = "StopTransactionOnEVSideDisconnect";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getStopTransactionOnEVSideDisconnect()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getStopTransactionOnInvalidId() {
    return this->config["Core"]["StopTransactionOnInvalidId"];
}
void ChargePointConfiguration::setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) {
    this->config["Core"]["StopTransactionOnInvalidId"] = stop_transaction_on_invalid_id;
    this->setInUserConfig("Core", "StopTransactionOnInvalidId", stop_transaction_on_invalid_id);
}
KeyValue ChargePointConfiguration::getStopTransactionOnInvalidIdKeyValue() {
    KeyValue kv;
    kv.key = "StopTransactionOnInvalidId";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getStopTransactionOnInvalidId()));
    return kv;
}

// Core Profile
std::string ChargePointConfiguration::getStopTxnAlignedData() {
    return this->config["Core"]["StopTxnAlignedData"];
}
bool ChargePointConfiguration::setStopTxnAlignedData(std::string stop_txn_aligned_data) {
    if (!this->measurands_supported(stop_txn_aligned_data)) {
        return false;
    }
    this->config["Core"]["StopTxnAlignedData"] = stop_txn_aligned_data;
    this->setInUserConfig("Core", "StopTxnAlignedData", stop_txn_aligned_data);
    return true;
}
KeyValue ChargePointConfiguration::getStopTxnAlignedDataKeyValue() {
    KeyValue kv;
    kv.key = "StopTxnAlignedData";
    kv.readonly = false;
    kv.value.emplace(this->getStopTxnAlignedData());
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getStopTxnAlignedDataMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("StopTxnAlignedDataMaxLength")) {
        max_length.emplace(this->config["Core"]["StopTxnAlignedDataMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getStopTxnAlignedDataMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getStopTxnAlignedDataMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "StopTxnAlignedDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getStopTxnSampledData() {
    return this->config["Core"]["StopTxnSampledData"];
}
bool ChargePointConfiguration::setStopTxnSampledData(std::string stop_txn_sampled_data) {
    if (!this->measurands_supported(stop_txn_sampled_data)) {
        return false;
    }
    this->config["Core"]["StopTxnSampledData"] = stop_txn_sampled_data;
    this->setInUserConfig("Core", "StopTxnSampledData", stop_txn_sampled_data);

    return true;
}
KeyValue ChargePointConfiguration::getStopTxnSampledDataKeyValue() {
    KeyValue kv;
    kv.key = "StopTxnSampledData";
    kv.readonly = false;
    kv.value.emplace(this->getStopTxnSampledData());
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getStopTxnSampledDataMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("StopTxnSampledDataMaxLength")) {
        max_length.emplace(this->config["Core"]["StopTxnSampledDataMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getStopTxnSampledDataMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getStopTxnSampledDataMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "StopTxnSampledDataMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
std::string ChargePointConfiguration::getSupportedFeatureProfiles() {
    return this->config["Core"]["SupportedFeatureProfiles"];
}
KeyValue ChargePointConfiguration::getSupportedFeatureProfilesKeyValue() {
    KeyValue kv;
    kv.key = "SupportedFeatureProfiles";
    kv.readonly = true;
    kv.value.emplace(this->getSupportedFeatureProfiles());
    return kv;
}
std::set<SupportedFeatureProfiles> ChargePointConfiguration::getSupportedFeatureProfilesSet() {
    return this->supported_feature_profiles;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getSupportedFeatureProfilesMaxLength() {
    std::optional<int32_t> max_length = std::nullopt;
    if (this->config["Core"].contains("SupportedFeatureProfilesMaxLength")) {
        max_length.emplace(this->config["Core"]["SupportedFeatureProfilesMaxLength"]);
    }
    return max_length;
}
std::optional<KeyValue> ChargePointConfiguration::getSupportedFeatureProfilesMaxLengthKeyValue() {
    std::optional<KeyValue> max_length_kv = std::nullopt;
    auto max_length = this->getSupportedFeatureProfilesMaxLength();
    if (max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "SupportedFeatureProfilesMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(max_length.value()));
        max_length_kv.emplace(kv);
    }
    return max_length_kv;
}

// Core Profile
int32_t ChargePointConfiguration::getTransactionMessageAttempts() {
    return this->config["Core"]["TransactionMessageAttempts"];
}
void ChargePointConfiguration::setTransactionMessageAttempts(int32_t attempts) {
    this->config["Core"]["TransactionMessageAttempts"] = attempts;
    this->setInUserConfig("Core", "TransactionMessageAttempts", attempts);
}
KeyValue ChargePointConfiguration::getTransactionMessageAttemptsKeyValue() {
    KeyValue kv;
    kv.key = "TransactionMessageAttempts";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getTransactionMessageAttempts()));
    return kv;
}

// Core Profile
int32_t ChargePointConfiguration::getTransactionMessageRetryInterval() {
    return this->config["Core"]["TransactionMessageRetryInterval"];
}
void ChargePointConfiguration::setTransactionMessageRetryInterval(int32_t retry_interval) {
    this->config["Core"]["TransactionMessageRetryInterval"] = retry_interval;
    this->setInUserConfig("Core", "TransactionMessageRetryInterval", retry_interval);
}
KeyValue ChargePointConfiguration::getTransactionMessageRetryIntervalKeyValue() {
    KeyValue kv;
    kv.key = "TransactionMessageRetryInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getTransactionMessageRetryInterval()));
    return kv;
}

// Core Profile
bool ChargePointConfiguration::getUnlockConnectorOnEVSideDisconnect() {
    return this->config["Core"]["UnlockConnectorOnEVSideDisconnect"];
}
void ChargePointConfiguration::setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect) {
    this->config["Core"]["UnlockConnectorOnEVSideDisconnect"] = unlock_connector_on_ev_side_disconnect;
    this->setInUserConfig("Core", "UnlockConnectorOnEVSideDisconnect", unlock_connector_on_ev_side_disconnect);
}
KeyValue ChargePointConfiguration::getUnlockConnectorOnEVSideDisconnectKeyValue() {
    KeyValue kv;
    kv.key = "UnlockConnectorOnEVSideDisconnect";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getUnlockConnectorOnEVSideDisconnect()));
    return kv;
}

// Core Profile - optional
std::optional<int32_t> ChargePointConfiguration::getWebsocketPingInterval() {
    std::optional<int32_t> websocket_ping_interval = std::nullopt;
    if (this->config["Core"].contains("WebsocketPingInterval")) {
        websocket_ping_interval.emplace(this->config["Core"]["WebsocketPingInterval"]);
    }
    return websocket_ping_interval;
}
void ChargePointConfiguration::setWebsocketPingInterval(int32_t websocket_ping_interval) {
    if (this->getWebsocketPingInterval() != std::nullopt) {
        this->config["Core"]["WebsocketPingInterval"] = websocket_ping_interval;
        this->setInUserConfig("Core", "WebsocketPingInterval", websocket_ping_interval);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getWebsocketPingIntervalKeyValue() {
    std::optional<KeyValue> websocket_ping_interval_kv = std::nullopt;
    auto websocket_ping_interval = this->getWebsocketPingInterval();
    if (websocket_ping_interval != std::nullopt) {
        KeyValue kv;
        kv.key = "WebsocketPingInterval";
        kv.readonly = false;
        kv.value.emplace(std::to_string(websocket_ping_interval.value()));
        websocket_ping_interval_kv.emplace(kv);
    }
    return websocket_ping_interval_kv;
}

std::optional<KeyValue> ChargePointConfiguration::getHostNameKeyValue() {
    std::optional<KeyValue> host_name_kv = std::nullopt;
    auto host_name = this->getHostName();
    if (host_name != std::nullopt) {
        KeyValue kv;
        kv.key = "HostName";
        kv.readonly = true;
        kv.value.emplace(host_name.value());
        host_name_kv.emplace(kv);
    }
    return host_name_kv;
}

std::optional<KeyValue> ChargePointConfiguration::getIFaceKeyValue() {
    std::optional<KeyValue> iface_name_kv = std::nullopt;
    auto iface = this->getIFace();
    if (iface != std::nullopt) {
        KeyValue kv;
        kv.key = "IFace";
        kv.readonly = true;
        kv.value.emplace(iface.value());
        iface_name_kv.emplace(kv);
    }
    return iface_name_kv;
}

// Core Profile end

int32_t ChargePointConfiguration::getChargeProfileMaxStackLevel() {
    return this->config["SmartCharging"]["ChargeProfileMaxStackLevel"];
}
KeyValue ChargePointConfiguration::getChargeProfileMaxStackLevelKeyValue() {
    KeyValue kv;
    kv.key = "ChargeProfileMaxStackLevel";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getChargeProfileMaxStackLevel()));
    return kv;
}

std::string ChargePointConfiguration::getChargingScheduleAllowedChargingRateUnit() {
    return this->config["SmartCharging"]["ChargingScheduleAllowedChargingRateUnit"];
}
KeyValue ChargePointConfiguration::getChargingScheduleAllowedChargingRateUnitKeyValue() {
    KeyValue kv;
    kv.key = "ChargingScheduleAllowedChargingRateUnit";
    kv.readonly = true;
    kv.value.emplace(this->getChargingScheduleAllowedChargingRateUnit());
    return kv;
}
std::vector<ChargingRateUnit> ChargePointConfiguration::getChargingScheduleAllowedChargingRateUnitVector() {
    std::vector<std::string> components;
    auto csv = this->getChargingScheduleAllowedChargingRateUnit();
    boost::split(components, csv, boost::is_any_of(","));
    std::vector<ChargingRateUnit> charging_rate_unit_vector;
    for (const auto& component : components) {
        if (component == "Current") {
            charging_rate_unit_vector.push_back(ChargingRateUnit::A);
        } else if (component == "Power") {
            charging_rate_unit_vector.push_back(ChargingRateUnit::W);
        }
    }
    return charging_rate_unit_vector;
}

int32_t ChargePointConfiguration::getChargingScheduleMaxPeriods() {
    return this->config["SmartCharging"]["ChargingScheduleMaxPeriods"];
}
KeyValue ChargePointConfiguration::getChargingScheduleMaxPeriodsKeyValue() {
    KeyValue kv;
    kv.key = "ChargingScheduleMaxPeriods";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getChargingScheduleMaxPeriods()));
    return kv;
}

std::optional<bool> ChargePointConfiguration::getConnectorSwitch3to1PhaseSupported() {
    std::optional<bool> connector_switch_3_to_1_phase_supported = std::nullopt;
    if (this->config["SmartCharging"].contains("ConnectorSwitch3to1PhaseSupported")) {
        connector_switch_3_to_1_phase_supported.emplace(
            this->config["SmartCharging"]["ConnectorSwitch3to1PhaseSupported"]);
    }
    return connector_switch_3_to_1_phase_supported;
}
std::optional<KeyValue> ChargePointConfiguration::getConnectorSwitch3to1PhaseSupportedKeyValue() {
    std::optional<KeyValue> connector_switch_3_to_1_phase_supported_kv = std::nullopt;

    auto connector_switch_3_to_1_phase_supported = this->getConnectorSwitch3to1PhaseSupported();
    if (connector_switch_3_to_1_phase_supported != std::nullopt) {
        KeyValue kv;
        kv.key = "ConnectorSwitch3to1PhaseSupported";
        kv.readonly = true;
        kv.value.emplace(ocpp::conversions::bool_to_string(connector_switch_3_to_1_phase_supported.value()));
        connector_switch_3_to_1_phase_supported_kv.emplace(kv);
    }
    return connector_switch_3_to_1_phase_supported_kv;
}

int32_t ChargePointConfiguration::getMaxChargingProfilesInstalled() {
    return this->config["SmartCharging"]["MaxChargingProfilesInstalled"];
}
KeyValue ChargePointConfiguration::getMaxChargingProfilesInstalledKeyValue() {
    KeyValue kv;
    kv.key = "MaxChargingProfilesInstalled";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getMaxChargingProfilesInstalled()));
    return kv;
}

// Security profile - optional
std::optional<bool> ChargePointConfiguration::getAdditionalRootCertificateCheck() {
    std::optional<bool> additional_root_certificate_check = std::nullopt;
    if (this->config["Security"].contains("AdditionalRootCertificateCheck")) {
        additional_root_certificate_check.emplace(this->config["Security"]["AdditionalRootCertificateCheck"]);
    }
    return additional_root_certificate_check;
}

std::optional<KeyValue> ChargePointConfiguration::getAdditionalRootCertificateCheckKeyValue() {
    std::optional<KeyValue> additional_root_certificate_check_kv = std::nullopt;

    auto additional_root_certificate_check = this->getAdditionalRootCertificateCheck();
    if (additional_root_certificate_check.has_value()) {
        KeyValue kv;
        kv.key = "AdditionalRootCertificateCheck";
        kv.readonly = true;
        kv.value.emplace(ocpp::conversions::bool_to_string(additional_root_certificate_check.value()));
        additional_root_certificate_check_kv.emplace(kv);
    }
    return additional_root_certificate_check_kv;
}

// Security Profile - optional
std::optional<std::string> ChargePointConfiguration::getAuthorizationKey() {
    std::optional<std::string> authorization_key = std::nullopt;
    if (this->config["Security"].contains("AuthorizationKey")) {
        authorization_key.emplace(this->config["Security"]["AuthorizationKey"]);
    }
    return authorization_key;
}

std::string hexToString(std::string const& s) {
    std::string str;
    for (size_t i = 0; i < s.length(); i += 2) {
        std::string byte = s.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        str.push_back(chr);
    }
    return str;
}

bool isHexNotation(std::string const& s) {
    bool is_hex = s.size() > 2 && s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;

    if (is_hex) {
        // check if every char is printable
        for (size_t i = 0; i < s.length(); i += 2) {
            std::string byte = s.substr(i, 2);
            char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
            if ((chr < 0x20 || chr > 0x7e) && chr != 0xa) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

void ChargePointConfiguration::setAuthorizationKey(std::string authorization_key) {

    // TODO(piet): SecurityLog entry

    std::string str;
    if (isHexNotation(authorization_key)) {
        str = hexToString(authorization_key);
    } else {
        str = authorization_key;
    }

    this->config["Security"]["AuthorizationKey"] = str;
    this->setInUserConfig("Security", "AuthorizationKey", str);
}

bool ChargePointConfiguration::isConnectorPhaseRotationValid(std::string str) {
    std::stringstream ss(str);
    std::vector<std::string> elements;

    str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
    boost::split(elements, str, boost::is_any_of(","));

    // Filter per element of type 0.NotApplicable, 1.NotApplicable, or 0.Unknown etc
    for (int connector_id = 0; connector_id <= this->getNumberOfConnectors(); connector_id++) {
        std::string myNotApplicable = std::to_string(connector_id) + ".NotApplicable";
        std::string myNotDefined = std::to_string(connector_id) + ".Unknown";
        elements.erase(std::remove(elements.begin(), elements.end(), myNotApplicable), elements.end());
        elements.erase(std::remove(elements.begin(), elements.end(), myNotDefined), elements.end());
    }
    // if all elemens are hit, accept it, else check the remaining
    if (elements.size() == 0) {
        return true;
    }

    for (const std::string& e : elements) {
        if (e.size() != 5) {
            return false;
        }
        try {
            auto connector = std::stoi(e.substr(0, 1));
            if (connector < 0 || connector > this->getNumberOfConnectors()) {
                return false;
            }
        } catch (const std::invalid_argument&) {
            return false;
        }
        std::string phase_rotation = e.substr(2, 5);
        if (phase_rotation != "RST" && phase_rotation != "RTS" && phase_rotation != "SRT" && phase_rotation != "STR" &&
            phase_rotation != "TRS" && phase_rotation != "TSR") {
            return false;
        }
    }
    return true;
}

bool ChargePointConfiguration::checkTimeOffset(const std::string& offset) {
    const std::vector<std::string> times = split_string(offset, ':');
    if (times.size() != 2) {
        EVLOG_error << "Could not set display time offset: format not correct (should be something like "
                       "\"-05:00\", but is "
                    << offset << ")";
        return false;
    } else {
        try {
            // Check if strings are numbers.
            const int32_t hours = std::stoi(times.at(0));
            const int32_t minutes = std::stoi(times.at(1));

            // And check if numbers are valid.
            if (hours < -24 || hours > 24) {
                EVLOG_error << "Could not set display time offset: hours should be between -24 and +24, but is "
                            << times.at(0);
                return false;
            }

            if (minutes < 0 || minutes > 59) {
                EVLOG_error << "Could not set display time offset: minutes should be between 0 and 59, but is "
                            << times.at(1);
                return false;
            }

        } catch (const std::exception& e) {
            EVLOG_error << "Could not set display time offset: format not correct (should be something "
                           "like \"-19:15\", but is "
                        << offset << "): " << e.what();
            return false;
        }
    }
    return true;
}

bool isBool(const std::string& str) {
    return str == "true" || str == "false";
}

std::optional<KeyValue> ChargePointConfiguration::getAuthorizationKeyKeyValue() {
    std::optional<KeyValue> enabled_kv = std::nullopt;
    std::optional<std::string> enabled = std::nullopt;

    KeyValue kv;
    kv.key = "AuthorizationKey";
    kv.readonly = false;

    if (this->config["Security"].contains("AuthorizationKey")) {
        // AuthorizationKey is writeOnly so we return a dummy
        enabled.emplace("DummyAuthorizationKey");
        kv.value.emplace(enabled.value());
    }
    enabled_kv.emplace(kv);
    return enabled_kv;
}

// Security profile - optional
std::optional<int32_t> ChargePointConfiguration::getCertificateSignedMaxChainSize() {
    std::optional<int32_t> certificate_max_chain_size = std::nullopt;
    if (this->config["Core"].contains("CertificateMaxChainSize")) {
        certificate_max_chain_size.emplace(this->config["Security"]["CertificateMaxChainSize"]);
    }
    return certificate_max_chain_size;
}

std::optional<KeyValue> ChargePointConfiguration::getCertificateSignedMaxChainSizeKeyValue() {
    std::optional<KeyValue> certificate_max_chain_size_kv = std::nullopt;
    auto certificate_max_chain_size = this->getCertificateSignedMaxChainSize();
    if (certificate_max_chain_size != std::nullopt) {
        KeyValue kv;
        kv.key = "CertificateMaxChainSize";
        kv.readonly = true;
        kv.value.emplace(std::to_string(certificate_max_chain_size.value()));
        certificate_max_chain_size_kv.emplace(kv);
    }
    return certificate_max_chain_size_kv;
}

// Security profile - optional
std::optional<int32_t> ChargePointConfiguration::getCertificateStoreMaxLength() {
    std::optional<int32_t> certificate_store_max_length = std::nullopt;
    if (this->config["Core"].contains("CertificateStoreMaxLength")) {
        certificate_store_max_length.emplace(this->config["Security"]["CertificateStoreMaxLength"]);
    }
    return certificate_store_max_length;
}

std::optional<KeyValue> ChargePointConfiguration::getCertificateStoreMaxLengthKeyValue() {
    std::optional<KeyValue> certificate_store_max_length_kv = std::nullopt;
    auto certificate_store_max_length = this->getCertificateStoreMaxLength();
    if (certificate_store_max_length != std::nullopt) {
        KeyValue kv;
        kv.key = "CertificateStoreMaxLength";
        kv.readonly = true;
        kv.value.emplace(std::to_string(certificate_store_max_length.value()));
        certificate_store_max_length_kv.emplace(kv);
    }
    return certificate_store_max_length_kv;
}

// Security Profile - optional
std::optional<std::string> ChargePointConfiguration::getCpoName() {
    std::optional<std::string> cpo_name = std::nullopt;
    if (this->config["Security"].contains("CpoName")) {
        cpo_name.emplace(this->config["Security"]["CpoName"]);
    }
    return cpo_name;
}

void ChargePointConfiguration::setCpoName(std::string cpoName) {
    this->config["Security"]["CpoName"] = cpoName;
    this->setInUserConfig("Security", "CpoName", cpoName);
}

std::optional<KeyValue> ChargePointConfiguration::getCpoNameKeyValue() {
    std::optional<KeyValue> cpo_name_kv = std::nullopt;
    auto cpo_name = this->getCpoName();
    KeyValue kv;
    kv.key = "CpoName";
    kv.readonly = false;
    if (cpo_name != std::nullopt) {
        kv.value.emplace(cpo_name.value());
    }
    cpo_name_kv.emplace(kv);
    return cpo_name_kv;
}

// Security profile - optional in ocpp but mandatory websocket connection
int32_t ChargePointConfiguration::getSecurityProfile() {
    return this->config["Security"]["SecurityProfile"];
}

void ChargePointConfiguration::setSecurityProfile(int32_t security_profile) {
    // TODO(piet): add boundaries for value of security profile
    this->config["Security"]["SecurityProfile"] = security_profile;
    // set security profile in user config
    this->setInUserConfig("Security", "SecurityProfile", security_profile);
}

KeyValue ChargePointConfiguration::getSecurityProfileKeyValue() {
    KeyValue kv;
    auto security_profile = this->getSecurityProfile();
    kv.key = "SecurityProfile";
    kv.readonly = false;
    kv.value.emplace(std::to_string(security_profile));
    return kv;
}

bool ChargePointConfiguration::getDisableSecurityEventNotifications() {
    return this->config["Security"]["DisableSecurityEventNotifications"];
}

void ChargePointConfiguration::setDisableSecurityEventNotifications(bool disable_security_event_notifications) {
    this->config["Security"]["DisableSecurityEventNotifications"] = disable_security_event_notifications;
    this->setInUserConfig("Security", "DisableSecurityEventNotifications", disable_security_event_notifications);
}

KeyValue ChargePointConfiguration::getDisableSecurityEventNotificationsKeyValue() {
    KeyValue kv;
    kv.key = "DisableSecurityEventNotifications";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getDisableSecurityEventNotifications()));
    return kv;
}

// Local Auth List Management Profile
bool ChargePointConfiguration::getLocalAuthListEnabled() {
    if (this->config.contains("LocalAuthListManagement")) {
        return this->config["LocalAuthListManagement"]["LocalAuthListEnabled"];
    } else {
        return false;
    }
}

void ChargePointConfiguration::setLocalAuthListEnabled(bool local_auth_list_enabled) {
    this->config["LocalAuthListManagement"]["LocalAuthListEnabled"] = local_auth_list_enabled;
    this->setInUserConfig("LocalAuthListManagement", "LocalAuthListEnabled", local_auth_list_enabled);
}
KeyValue ChargePointConfiguration::getLocalAuthListEnabledKeyValue() {
    KeyValue kv;
    kv.key = "LocalAuthListEnabled";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getLocalAuthListEnabled()));
    return kv;
}

// Local Auth List Management Profile
int32_t ChargePointConfiguration::getLocalAuthListMaxLength() {
    return this->config["LocalAuthListManagement"]["LocalAuthListMaxLength"];
}
KeyValue ChargePointConfiguration::getLocalAuthListMaxLengthKeyValue() {
    KeyValue kv;
    kv.key = "LocalAuthListMaxLength";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getLocalAuthListMaxLength()));
    return kv;
}

// Local Auth List Management Profile
int32_t ChargePointConfiguration::getSendLocalListMaxLength() {
    return this->config["LocalAuthListManagement"]["SendLocalListMaxLength"];
}
KeyValue ChargePointConfiguration::getSendLocalListMaxLengthKeyValue() {
    KeyValue kv;
    kv.key = "SendLocalListMaxLength";
    kv.readonly = true;
    kv.value.emplace(std::to_string(this->getSendLocalListMaxLength()));
    return kv;
}

bool ChargePointConfiguration::getISO15118PnCEnabled() {
    return this->config["PnC"]["ISO15118PnCEnabled"];
}

void ChargePointConfiguration::setISO15118PnCEnabled(const bool iso15118_pnc_enabled) {
    this->config["PnC"]["ISO15118PnCEnabled"] = iso15118_pnc_enabled;
    this->setInUserConfig("PnC", "ISO15118PnCEnabled", iso15118_pnc_enabled);
}

KeyValue ChargePointConfiguration::getISO15118PnCEnabledKeyValue() {
    KeyValue kv;
    kv.key = "ISO15118PnCEnabled";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getISO15118PnCEnabled()));
    return kv;
}

std::optional<bool> ChargePointConfiguration::getCentralContractValidationAllowed() {
    std::optional<bool> central_contract_validation_allowed = std::nullopt;
    if (this->config["PnC"].contains("CentralContractValidationAllowed")) {
        central_contract_validation_allowed.emplace(this->config["PnC"]["CentralContractValidationAllowed"]);
    }
    return central_contract_validation_allowed;
}

void ChargePointConfiguration::setCentralContractValidationAllowed(const bool central_contract_validation_allowed) {
    if (this->getCentralContractValidationAllowed() != std::nullopt) {
        this->config["PnC"]["CentralContractValidationAllowed"] = central_contract_validation_allowed;
        this->setInUserConfig("PnC", "CentralContractValidationAllowed", central_contract_validation_allowed);
    }
}
std::optional<KeyValue> ChargePointConfiguration::getCentralContractValidationAllowedKeyValue() {
    std::optional<KeyValue> central_contract_validation_allowed_kv = std::nullopt;
    auto central_contract_validation_allowed = this->getCentralContractValidationAllowed();
    if (central_contract_validation_allowed != std::nullopt) {
        KeyValue kv;
        kv.key = "CentralContractValidationAllowed";
        kv.readonly = false;
        kv.value.emplace(ocpp::conversions::bool_to_string(central_contract_validation_allowed.value()));
        central_contract_validation_allowed_kv.emplace(kv);
    }
    return central_contract_validation_allowed_kv;
}

std::optional<int32_t> ChargePointConfiguration::getCertSigningWaitMinimum() {
    std::optional<int32_t> cert_signing_wait_minimum = std::nullopt;
    if (this->config["PnC"].contains("CertSigningWaitMinimum")) {
        cert_signing_wait_minimum.emplace(this->config["PnC"]["CertSigningWaitMinimum"]);
    }
    return cert_signing_wait_minimum;
}

void ChargePointConfiguration::setCertSigningWaitMinimum(const int32_t cert_signing_wait_minimum) {
    if (this->getCertSigningWaitMinimum() != std::nullopt) {
        this->config["PnC"]["CertSigningWaitMinimum"] = cert_signing_wait_minimum;
        this->setInUserConfig("PnC", "CertSigningWaitMinimum", cert_signing_wait_minimum);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getCertSigningWaitMinimumKeyValue() {
    std::optional<KeyValue> cert_signing_wait_minimum_kv = std::nullopt;
    auto cert_signing_wait_minimum = this->getCertSigningWaitMinimum();
    if (cert_signing_wait_minimum != std::nullopt) {
        KeyValue kv;
        kv.key = "CertSigningWaitMinimum";
        kv.readonly = false;
        kv.value.emplace(std::to_string(cert_signing_wait_minimum.value()));
        cert_signing_wait_minimum_kv.emplace(kv);
    }
    return cert_signing_wait_minimum_kv;
}

std::optional<int32_t> ChargePointConfiguration::getCertSigningRepeatTimes() {
    std::optional<int32_t> get_cert_signing_repeat_times = std::nullopt;
    if (this->config["PnC"].contains("CertSigningRepeatTimes")) {
        get_cert_signing_repeat_times.emplace(this->config["PnC"]["CertSigningRepeatTimes"]);
    }
    return get_cert_signing_repeat_times;
}
void ChargePointConfiguration::setCertSigningRepeatTimes(const int32_t cert_signing_repeat_times) {
    if (this->getCertSigningRepeatTimes() != std::nullopt) {
        this->config["PnC"]["CertSigningRepeatTimes"] = cert_signing_repeat_times;
        this->setInUserConfig("PnC", "CertSigningRepeatTimes", cert_signing_repeat_times);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getCertSigningRepeatTimesKeyValue() {
    std::optional<KeyValue> cert_signing_repeat_times_kv = std::nullopt;
    auto cert_signing_repeat_times = this->getCertSigningRepeatTimes();
    if (cert_signing_repeat_times != std::nullopt) {
        KeyValue kv;
        kv.key = "CertSigningRepeatTimes";
        kv.readonly = false;
        kv.value.emplace(std::to_string(cert_signing_repeat_times.value()));
        cert_signing_repeat_times_kv.emplace(kv);
    }
    return cert_signing_repeat_times_kv;
}

bool ChargePointConfiguration::getContractValidationOffline() {
    return this->config["PnC"]["ContractValidationOffline"];
}

void ChargePointConfiguration::setContractValidationOffline(const bool contract_validation_offline) {
    this->config["PnC"]["ContractValidationOffline"] = contract_validation_offline;
    this->setInUserConfig("PnC", "ContractValidationOffline", contract_validation_offline);
}

KeyValue ChargePointConfiguration::getContractValidationOfflineKeyValue() {
    KeyValue kv;
    kv.key = "ContractValidationOffline";
    kv.readonly = false;
    kv.value.emplace(ocpp::conversions::bool_to_string(this->getContractValidationOffline()));
    return kv;
}

int32_t ChargePointConfiguration::getOcspRequestInterval() {
    return this->config["Internal"]["OcspRequestInterval"];
}

void ChargePointConfiguration::setOcspRequestInterval(const int32_t ocsp_request_interval) {
    this->config["Internal"]["OcspRequestInterval"] = ocsp_request_interval;
    this->setInUserConfig("Internal", "OcspRequestInterval", ocsp_request_interval);
}

KeyValue ChargePointConfiguration::getOcspRequestIntervalKeyValue() {
    KeyValue kv;
    kv.key = "OcspRequestInterval";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getOcspRequestInterval()));
    return kv;
}

std::optional<std::string> ChargePointConfiguration::getSeccLeafSubjectCommonName() {
    std::optional<std::string> secc_leaf_subject_common_name = std::nullopt;
    if (this->config["Internal"].contains("SeccLeafSubjectCommonName")) {
        secc_leaf_subject_common_name.emplace(this->config["Internal"]["SeccLeafSubjectCommonName"]);
    }
    return secc_leaf_subject_common_name;
}

void ChargePointConfiguration::setSeccLeafSubjectCommonName(const std::string& secc_leaf_subject_common_name) {
    if (this->getSeccLeafSubjectCommonName() != std::nullopt) {
        this->config["Internal"]["SeccLeafSubjectCommonName"] = secc_leaf_subject_common_name;
        this->setInUserConfig("Internal", "SeccLeafSubjectCommonName", secc_leaf_subject_common_name);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getSeccLeafSubjectCommonNameKeyValue() {
    std::optional<KeyValue> secc_leaf_subject_common_name_kv = std::nullopt;
    auto secc_leaf_subject_common_name = this->getSeccLeafSubjectCommonName();
    if (secc_leaf_subject_common_name != std::nullopt) {
        KeyValue kv;
        kv.key = "SeccLeafSubjectCommonName";
        kv.readonly = false;
        kv.value.emplace(secc_leaf_subject_common_name.value());
        secc_leaf_subject_common_name_kv.emplace(kv);
    }
    return secc_leaf_subject_common_name_kv;
}

std::optional<std::string> ChargePointConfiguration::getSeccLeafSubjectCountry() {
    std::optional<std::string> secc_leaf_subject_country = std::nullopt;
    if (this->config["Internal"].contains("SeccLeafSubjectCountry")) {
        secc_leaf_subject_country.emplace(this->config["Internal"]["SeccLeafSubjectCountry"]);
    }
    return secc_leaf_subject_country;
}

void ChargePointConfiguration::setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country) {
    if (this->getSeccLeafSubjectCountry() != std::nullopt) {
        this->config["Internal"]["SeccLeafSubjectCountry"] = secc_leaf_subject_country;
        this->setInUserConfig("Internal", "SeccLeafSubjectCountry", secc_leaf_subject_country);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getSeccLeafSubjectCountryKeyValue() {
    std::optional<KeyValue> secc_leaf_subject_country_kv = std::nullopt;
    auto secc_leaf_subject_country = this->getSeccLeafSubjectCountry();
    if (secc_leaf_subject_country != std::nullopt) {
        KeyValue kv;
        kv.key = "SeccLeafSubjectCountry";
        kv.readonly = false;
        kv.value.emplace(secc_leaf_subject_country.value());
        secc_leaf_subject_country_kv.emplace(kv);
    }
    return secc_leaf_subject_country_kv;
}

std::optional<std::string> ChargePointConfiguration::getSeccLeafSubjectOrganization() {
    std::optional<std::string> secc_leaf_subject_organization = std::nullopt;
    if (this->config["Internal"].contains("SeccLeafSubjectOrganization")) {
        secc_leaf_subject_organization.emplace(this->config["Internal"]["SeccLeafSubjectOrganization"]);
    }
    return secc_leaf_subject_organization;
}

void ChargePointConfiguration::setSeccLeafSubjectOrganization(const std::string& secc_leaf_subject_organization) {
    if (this->getSeccLeafSubjectOrganization() != std::nullopt) {
        this->config["Internal"]["SeccLeafSubjectOrganization"] = secc_leaf_subject_organization;
        this->setInUserConfig("Internal", "SeccLeafSubjectOrganization", secc_leaf_subject_organization);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getSeccLeafSubjectOrganizationKeyValue() {
    std::optional<KeyValue> secc_leaf_subject_organization_kv = std::nullopt;
    auto secc_leaf_subject_organization = this->getSeccLeafSubjectOrganization();
    if (secc_leaf_subject_organization != std::nullopt) {
        KeyValue kv;
        kv.key = "SeccLeafSubjectOrganization";
        kv.readonly = false;
        kv.value.emplace(secc_leaf_subject_organization.value());
        secc_leaf_subject_organization_kv.emplace(kv);
    }
    return secc_leaf_subject_organization_kv;
}

std::optional<std::string> ChargePointConfiguration::getConnectorEvseIds() {
    std::optional<std::string> connector_evse_ids = std::nullopt;
    if (this->config["Internal"].contains("ConnectorEvseIds")) {
        connector_evse_ids.emplace(this->config["Internal"]["ConnectorEvseIds"]);
    }
    return connector_evse_ids;
}

void ChargePointConfiguration::setConnectorEvseIds(const std::string& connector_evse_ids) {
    if (this->getConnectorEvseIds() != std::nullopt) {
        this->config["Internal"]["ConnectorEvseIds"] = connector_evse_ids;
        this->setInUserConfig("Internal", "ConnectorEvseIds", connector_evse_ids);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getConnectorEvseIdsKeyValue() {
    std::optional<KeyValue> connector_evse_ids_kv = std::nullopt;
    auto connector_evse_ids = this->getConnectorEvseIds();
    if (connector_evse_ids != std::nullopt) {
        KeyValue kv;
        kv.key = "ConnectorEvseIds";
        kv.readonly = false;
        kv.value.emplace(connector_evse_ids.value());
        connector_evse_ids_kv.emplace(kv);
    }
    return connector_evse_ids_kv;
}

std::optional<bool> ChargePointConfiguration::getAllowChargingProfileWithoutStartSchedule() {
    std::optional<bool> allow = std::nullopt;
    if (this->config["Internal"].contains("AllowChargingProfileWithoutStartSchedule")) {
        allow.emplace(this->config["Internal"]["AllowChargingProfileWithoutStartSchedule"]);
    }
    return allow;
}

void ChargePointConfiguration::setAllowChargingProfileWithoutStartSchedule(const bool allow) {
    if (this->getAllowChargingProfileWithoutStartSchedule() != std::nullopt) {
        this->config["Internal"]["AllowChargingProfileWithoutStartSchedule"] = allow;
        this->setInUserConfig("Internal", "AllowChargingProfileWithoutStartSchedule", allow);
    }
}

std::optional<KeyValue> ChargePointConfiguration::getAllowChargingProfileWithoutStartScheduleKeyValue() {
    std::optional<KeyValue> allow_opt = std::nullopt;
    auto allow = this->getAllowChargingProfileWithoutStartSchedule();
    if (allow != std::nullopt) {
        KeyValue kv;
        kv.key = "AllowChargingProfileWithoutStartSchedule";
        kv.readonly = false;
        kv.value.emplace(std::to_string(allow.value()));
        allow_opt.emplace(kv);
    }
    return allow_opt;
}

int32_t ChargePointConfiguration::getWaitForStopTransactionsOnResetTimeout() {
    return this->config["Internal"]["WaitForStopTransactionsOnResetTimeout"];
}

void ChargePointConfiguration::setWaitForStopTransactionsOnResetTimeout(
    const int32_t wait_for_stop_transactions_on_reset_timeout) {
    this->config["Internal"]["WaitForStopTransactionsOnResetTimeout"] = wait_for_stop_transactions_on_reset_timeout;
    this->setInUserConfig("Internal", "WaitForStopTransactionsOnResetTimeout",
                          wait_for_stop_transactions_on_reset_timeout);
}

KeyValue ChargePointConfiguration::getWaitForStopTransactionsOnResetTimeoutKeyValue() {
    KeyValue kv;
    kv.key = "WaitForStopTransactionsOnResetTimeout";
    kv.readonly = false;
    kv.value.emplace(std::to_string(this->getWaitForStopTransactionsOnResetTimeout()));
    return kv;
}

// California Pricing Requirements
bool ChargePointConfiguration::getCustomDisplayCostAndPriceEnabled() {
    if (this->config.contains("CostAndPrice") &&
        this->config.at("CostAndPrice").contains("CustomDisplayCostAndPrice")) {
        return this->config["CostAndPrice"]["CustomDisplayCostAndPrice"];
    }

    return false;
}

KeyValue ChargePointConfiguration::getCustomDisplayCostAndPriceEnabledKeyValue() {
    const bool enabled = getCustomDisplayCostAndPriceEnabled();
    KeyValue kv;
    kv.key = "CustomDisplayCostAndPrice";
    kv.value = std::to_string(enabled);
    kv.readonly = true;
    return kv;
}

std::optional<uint32_t> ChargePointConfiguration::getPriceNumberOfDecimalsForCostValues() {
    if (this->config.contains("CostAndPrice") &&
        this->config.at("CostAndPrice").contains("NumberOfDecimalsForCostValues")) {
        return this->config["CostAndPrice"]["NumberOfDecimalsForCostValues"];
    }

    return std::nullopt;
}

std::optional<KeyValue> ChargePointConfiguration::getPriceNumberOfDecimalsForCostValuesKeyValue() {
    std::optional<KeyValue> kv_opt = std::nullopt;
    const std::optional<uint32_t> number_of_decimals = getPriceNumberOfDecimalsForCostValues();
    if (number_of_decimals.has_value()) {
        kv_opt = KeyValue();
        kv_opt->key = "NumberOfDecimalsForCostValues";
        kv_opt->value = std::to_string(number_of_decimals.value());
        kv_opt->readonly = true;
    }

    return kv_opt;
}

std::optional<std::string> ChargePointConfiguration::getDefaultPriceText(const std::string& language) {
    if (this->config.contains("CostAndPrice") && this->config.at("CostAndPrice").contains("DefaultPriceText")) {
        bool found = false;
        json result = json::object();
        json& default_price = this->config["CostAndPrice"]["DefaultPriceText"];

        if (!default_price.contains("priceTexts")) {
            return std::nullopt;
        }

        for (auto& price_text : default_price.at("priceTexts").items()) {
            if (language == price_text.value().at("language")) {
                // Language found.
                result["priceText"] = price_text.value().at("priceText");
                if (price_text.value().contains("priceTextOffline")) {
                    result["priceTextOffline"] = price_text.value().at("priceTextOffline");
                }
                found = true;
                break;
            }
        }

        if (found) {
            return result.dump(2);
        }
    }

    return std::nullopt;
}

ConfigurationStatus ChargePointConfiguration::setDefaultPriceText(const CiString<50>& key, const CiString<500>& value) {
    std::string language;
    const std::vector<std::string> default_prices = split_string(key.get(), ',');
    if (default_prices.size() > 1) {
        // Second value is language.
        language = default_prices.at(1);
        // Check if language is allowed. It should be in the list of config item multi language supported languages
        const auto supported_languages = getMultiLanguageSupportedLanguages();
        if (!supported_languages.has_value()) {
            EVLOG_error << "Can not set a default price text for language '" << language
                        << "', because the config item for multi language support is not set in the config.";
            return ConfigurationStatus::Rejected;
        }

        const std::vector<std::string> languages = split_string(supported_languages.value(), ',');
        const auto& found_it =
            std::find_if(languages.begin(), languages.end(), [&language](const std::string& supported_language) {
                return trim_string(supported_language) == trim_string(language);
            });
        if (found_it == languages.end()) {
            EVLOG_error
                << "Can not set default price text for language '" << language
                << "', because the language is currently not supported in this charging station. Supported languages: "
                << supported_languages.value();
            return ConfigurationStatus::Rejected;
        }
    } else {
        EVLOG_error << "Configuration DefaultPriceText is set, but does not contain a language (Configuration should "
                       "be something like 'DefaultPriceText,en', but is "
                    << value << ").";
        return ConfigurationStatus::Rejected;
    }

    json default_price = json::object();
    if (this->config.contains("CostAndPrice") && this->config.at("CostAndPrice").contains("DefaultPriceText")) {
        json result = json::object();
        default_price = this->config["CostAndPrice"]["DefaultPriceText"];
    }

    // priceText is mandatory
    json j = json::parse(value.get());
    if (!j.contains("priceText")) {
        EVLOG_error << "Configuration DefaultPriceText is set, but does not contain 'priceText'";
        return ConfigurationStatus::Rejected;
    }

    j["language"] = language;

    if (!default_price.contains("priceTexts")) {
        default_price["priceTexts"] = json::array();
    }

    default_price["priceTexts"].push_back(j);

    this->config["CostAndPrice"]["DefaultPriceText"] = default_price;
    this->setInUserConfig("CostAndPrice", "DefaultPriceText", default_price);

    return ConfigurationStatus::Accepted;
}

KeyValue ChargePointConfiguration::getDefaultPriceTextKeyValue(const std::string& language) {
    KeyValue result;
    result.key = "DefaultPriceText," + language;
    result.readonly = false;
    std::optional<std::string> default_price = getDefaultPriceText(language);
    if (default_price.has_value()) {
        result.value = default_price.value();
    } else {
        // It's a bit odd to return an empty string here, but it must be possible to set a default price text for a
        // new language. But since the 'set' function for configurations first performs a 'get' and does not continue
        // if it receives a nullopt, we can better just return an empty string so the 'set' function can continue.
        // Resolving it differently required more complexer code, this was the easiest way to do it.
        result.value = "";
    }

    return result;
}

std::optional<std::vector<KeyValue>> ChargePointConfiguration::getAllDefaultPriceTextKeyValues() {
    if (this->config.contains("CostAndPrice") && this->config.at("CostAndPrice").contains("DefaultPriceText")) {
        std::vector<KeyValue> key_values;
        const json& default_price = this->config["CostAndPrice"]["DefaultPriceText"];
        if (!default_price.contains("priceTexts")) {
            return std::nullopt;
        }

        for (auto& price_text : default_price.at("priceTexts").items()) {
            json result = json::object();
            const std::string language = price_text.value().at("language");
            result["priceText"] = price_text.value().at("priceText");
            if (price_text.value().contains("priceTextOffline")) {
                result["priceTextOffline"] = price_text.value().at("priceTextOffline");
            }

            KeyValue kv;
            kv.value = result.dump(2);
            kv.readonly = false;
            kv.key = "DefaultPriceText," + language;
            key_values.push_back(kv);
        }

        if (key_values.empty()) {
            return std::nullopt;
        }

        return key_values;
    }

    return std::nullopt;
}

std::optional<std::string> ChargePointConfiguration::getDefaultPrice() {
    if (this->config.contains("CostAndPrice") && this->config.at("CostAndPrice").contains("DefaultPrice")) {
        return this->config["CostAndPrice"]["DefaultPrice"].dump(2);
    }

    return std::nullopt;
}

ConfigurationStatus ChargePointConfiguration::setDefaultPrice(const std::string& value) {

    json default_price = json::object();
    try {
        default_price = json::parse(value);
    } catch (const std::exception& e) {
        EVLOG_error << "Default price json not correct, can not store default price : " << e.what();
        return ConfigurationStatus::Rejected;
    }

    this->config["CostAndPrice"]["DefaultPrice"] = default_price;
    this->setInUserConfig("CostAndPrice", "DefaultPrice", default_price);

    return ConfigurationStatus::Accepted;
}

std::optional<KeyValue> ChargePointConfiguration::getDefaultPriceKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> default_price = getDefaultPrice();
    if (default_price.has_value()) {
        result = KeyValue();
        result->key = "DefaultPrice";
        result->value = default_price.value();
        result->readonly = false;
    }

    return result;
}

std::optional<std::string> ChargePointConfiguration::getDisplayTimeOffset() {
    if (this->config.contains("CostAndPrice") && this->config["CostAndPrice"].contains("TimeOffset")) {
        return this->config["CostAndPrice"]["TimeOffset"];
    }

    return std::nullopt;
}

ConfigurationStatus ChargePointConfiguration::setDisplayTimeOffset(const std::string& offset) {
    if (!checkTimeOffset(offset)) {
        return ConfigurationStatus::Rejected;
    }
    this->config["CostAndPrice"]["TimeOffset"] = offset;
    this->setInUserConfig("CostAndPrice", "TimeOffset", offset);
    return ConfigurationStatus::Accepted;
}

std::optional<KeyValue> ChargePointConfiguration::getDisplayTimeOffsetKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> time_offset = getDisplayTimeOffset();
    if (time_offset.has_value()) {
        result = KeyValue();
        result->key = "TimeOffset";
        result->value = time_offset.value();
        result->readonly = false;
    }

    return result;
}

std::optional<std::string> ChargePointConfiguration::getNextTimeOffsetTransitionDateTime() {
    if (this->config.contains("CostAndPrice") &&
        this->config["CostAndPrice"].contains("NextTimeOffsetTransitionDateTime")) {
        return this->config["CostAndPrice"]["NextTimeOffsetTransitionDateTime"];
    }

    return std::nullopt;
}

ConfigurationStatus ChargePointConfiguration::setNextTimeOffsetTransitionDateTime(const std::string& date_time) {
    DateTime d(date_time);
    if (d.to_time_point() > date::utc_clock::now()) {
        this->config["CostAndPrice"]["NextTimeOffsetTransitionDateTime"] = date_time;
        this->setInUserConfig("CostAndPrice", "NextTimeOffsetTransitionDateTime", date_time);
        return ConfigurationStatus::Accepted;
    }

    EVLOG_error << "Set next time offset transition date time: date time format not correct: " << date_time;
    return ConfigurationStatus::Rejected;
}

std::optional<KeyValue> ChargePointConfiguration::getNextTimeOffsetTransitionDateTimeKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> offset = getNextTimeOffsetTransitionDateTime();
    if (offset.has_value()) {
        result = KeyValue();
        result->key = "NextTimeOffsetTransitionDateTime";
        result->value = offset.value();
        result->readonly = false;
    }

    return result;
}

std::optional<std::string> ChargePointConfiguration::getTimeOffsetNextTransition() {
    if (this->config.contains("CostAndPrice") && this->config["CostAndPrice"].contains("TimeOffsetNextTransition")) {
        return this->config["CostAndPrice"]["TimeOffsetNextTransition"];
    }

    return std::nullopt;
}

ConfigurationStatus ChargePointConfiguration::setTimeOffsetNextTransition(const std::string& offset) {
    if (!checkTimeOffset(offset)) {
        return ConfigurationStatus::Rejected;
    }
    this->config["CostAndPrice"]["TimeOffsetNextTransition"] = offset;
    this->setInUserConfig("CostAndPrice", "TimeOffsetNextTransition", offset);
    return ConfigurationStatus::Accepted;
}

std::optional<KeyValue> ChargePointConfiguration::getTimeOffsetNextTransitionKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> offset = getTimeOffsetNextTransition();
    if (offset.has_value()) {
        result = KeyValue();
        result->key = "TimeOffsetNextTransition";
        result->value = offset.value();
        result->readonly = false;
    }

    return result;
}

std::optional<bool> ChargePointConfiguration::getCustomIdleFeeAfterStop() {
    if (this->config.contains("CostAndPrice") && this->config["CostAndPrice"].contains("CustomIdleFeeAfterStop")) {
        return this->config["CostAndPrice"]["CustomIdleFeeAfterStop"];
    }

    return std::nullopt;
}

void ChargePointConfiguration::setCustomIdleFeeAfterStop(const bool& value) {
    this->config["CostAndPrice"]["CustomIdleFeeAfterStop"] = value;
    this->setInUserConfig("CostAndPrice", "CustomIdleFeeAfterStop", value);
}

std::optional<KeyValue> ChargePointConfiguration::getCustomIdleFeeAfterStopKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<bool> idle_fee = getCustomIdleFeeAfterStop();
    if (idle_fee.has_value()) {
        result = KeyValue();
        result->key = "CustomIdleFeeAfterStop";
        result->value = std::to_string(idle_fee.value());
        result->readonly = false;
    }

    return result;
}

std::optional<bool> ChargePointConfiguration::getCustomMultiLanguageMessagesEnabled() {
    if (this->config.contains("CostAndPrice") && this->config["CostAndPrice"].contains("CustomMultiLanguageMessages")) {
        return this->config["CostAndPrice"]["CustomMultiLanguageMessages"];
    }

    return std::nullopt;
}

std::optional<KeyValue> ChargePointConfiguration::getCustomMultiLanguageMessagesEnabledKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<bool> multi_language = getCustomMultiLanguageMessagesEnabled();
    if (multi_language.has_value()) {
        result = KeyValue();
        result->key = "CustomMultiLanguageMessages";
        result->value = std::to_string(multi_language.value());
        result->readonly = true;
    }

    return result;
}

std::optional<std::string> ChargePointConfiguration::getMultiLanguageSupportedLanguages() {
    if (this->config.contains("CostAndPrice") &&
        this->config["CostAndPrice"].contains("MultiLanguageSupportedLanguages")) {
        return this->config["CostAndPrice"]["MultiLanguageSupportedLanguages"];
    }

    return std::nullopt;
}

std::optional<KeyValue> ChargePointConfiguration::getMultiLanguageSupportedLanguagesKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> languages = getMultiLanguageSupportedLanguages();
    if (languages.has_value()) {
        result = KeyValue();
        result->key = "MultiLanguageSupportedLanguages";
        result->value = languages.value();
        result->readonly = true;
    }

    return result;
}

std::optional<std::string> ChargePointConfiguration::getLanguage() {
    if (this->config.contains("CostAndPrice") && this->config["CostAndPrice"].contains("Language")) {
        return this->config["CostAndPrice"]["Language"];
    }

    return std::nullopt;
}

void ChargePointConfiguration::setLanguage(const std::string& language) {
    this->config["CostAndPrice"]["Language"] = language;
    this->setInUserConfig("CostAndPrice", "Language", language);
}

std::optional<KeyValue> ChargePointConfiguration::getLanguageKeyValue() {
    std::optional<KeyValue> result = std::nullopt;
    std::optional<std::string> language = getLanguage();
    if (language.has_value()) {
        result = KeyValue();
        result->key = "Language";
        result->value = language.value();
        result->readonly = true;
    }

    return result;
}

// Custom
std::optional<KeyValue> ChargePointConfiguration::getCustomKeyValue(CiString<50> key) {
    std::lock_guard<std::recursive_mutex> lock(this->configuration_mutex);
    if (!this->config["Custom"].contains(key.get())) {
        return std::nullopt;
    }

    try {
        KeyValue kv;
        kv.readonly = this->custom_schema["properties"][key]["readOnly"];
        kv.key = key;
        if (this->config["Custom"][key].is_string()) {
            kv.value = std::string(this->config["Custom"][key]);
        } else {
            kv.value = this->config["Custom"][key].dump();
        }
        return kv;
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

ConfigurationStatus ChargePointConfiguration::setCustomKey(CiString<50> key, CiString<500> value, bool force) {
    const auto kv = this->getCustomKeyValue(key);
    if (!kv.has_value() or (kv.value().readonly and !force)) {
        return ConfigurationStatus::Rejected;
    }
    std::lock_guard<std::recursive_mutex> lock(this->configuration_mutex);
    try {
        const auto type = this->custom_schema["properties"][key]["type"];
        if (type == "integer") {
            this->config["Custom"][key] = std::stoi(value.get());
        } else if (type == "number") {
            this->config["Custom"][key] = std::stof(value.get());
        } else if (type == "string" or type == "array") {
            this->config["Custom"][key] = value.get();
        } else if (type == "boolean") {
            this->config["Custom"][key] = ocpp::conversions::string_to_bool(value.get());
        } else {
            return ConfigurationStatus::Rejected;
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Could not set custom configuration key";
        return ConfigurationStatus::Rejected;
    }

    this->setInUserConfig("Custom", key, this->config["Custom"][key]);
    return ConfigurationStatus::Accepted;
}

std::optional<KeyValue> ChargePointConfiguration::get(CiString<50> key) {
    std::lock_guard<std::recursive_mutex> lock(this->configuration_mutex);
    // Internal Profile
    if (key == "ChargePointId") {
        return this->getChargePointIdKeyValue();
    }
    if (key == "CentralSystemURI") {
        return this->getCentralSystemURIKeyValue();
    }
    if (key == "ChargeBoxSerialNumber") {
        return this->getChargeBoxSerialNumberKeyValue();
    }
    if (key == "ChargePointModel") {
        return this->getChargePointModelKeyValue();
    }
    if (key == "ChargePointSerialNumber") {
        return this->getChargePointSerialNumberKeyValue();
    }
    if (key == "ChargePointVendor") {
        return this->getChargePointVendorKeyValue();
    }
    if (key == "FirmwareVersion") {
        return this->getFirmwareVersionKeyValue();
    }
    if (key == "ICCID") {
        return this->getICCIDKeyValue();
    }
    if (key == "IMSI") {
        return this->getIMSIKeyValue();
    }
    if (key == "MeterSerialNumber") {
        return this->getMeterSerialNumberKeyValue();
    }
    if (key == "MeterType") {
        return this->getMeterTypeKeyValue();
    }
    if (key == "SupportedCiphers12") {
        return this->getSupportedCiphers12KeyValue();
    }
    if (key == "SupportedCiphers13") {
        return this->getSupportedCiphers13KeyValue();
    }
    if (key == "RetryBackoffRandomRange") {
        return this->getRetryBackoffRandomRangeKeyValue();
    }
    if (key == "RetryBackoffRepeatTimes") {
        return this->getRetryBackoffRepeatTimesKeyValue();
    }
    if (key == "RetryBackoffWaitMinimum") {
        return this->getRetryBackoffWaitMinimumKeyValue();
    }
    if (key == "AuthorizeConnectorZeroOnConnectorOne") {
        return this->getAuthorizeConnectorZeroOnConnectorOneKeyValue();
    }
    if (key == "LogMessages") {
        return this->getLogMessagesKeyValue();
    }
    if (key == "LogMessagesFormat") {
        return this->getLogMessagesFormatKeyValue();
    }
    if (key == "SupportedChargingProfilePurposeTypes") {
        return this->getSupportedChargingProfilePurposeTypesKeyValue();
    }
    if (key == "MaxCompositeScheduleDuration") {
        return this->getMaxCompositeScheduleDurationKeyValue();
    }
    if (key == "WebsocketPingPayload") {
        return this->getWebsocketPingPayloadKeyValue();
    }
    if (key == "WebsocketPongTimeout") {
        return this->getWebsocketPongTimeoutKeyValue();
    }
    if (key == "UseSslDefaultVerifyPaths") {
        return this->getUseSslDefaultVerifyPathsKeyValue();
    }
    if (key == "VerifyCsmsCommonName") {
        return this->getVerifyCsmsCommonNameKeyValue();
    }
    if (key == "VerifyCsmsAllowWildcards") {
        return this->getVerifyCsmsAllowWildcardsKeyValue();
    }
    if (key == "OcspRequestInterval") {
        return this->getOcspRequestIntervalKeyValue();
    }
    if (key == "SeccLeafSubjectCommonName") {
        return this->getSeccLeafSubjectCommonNameKeyValue();
    }
    if (key == "SeccLeafSubjectCountry") {
        return this->getSeccLeafSubjectCountryKeyValue();
    }
    if (key == "SeccLeafSubjectOrganization") {
        return this->getSeccLeafSubjectOrganizationKeyValue();
    }
    if (key == "ConnectorEvseIds") {
        return this->getConnectorEvseIdsKeyValue();
    }
    if (key == "AllowChargingProfileWithoutStartSchedule") {
        return this->getAllowChargingProfileWithoutStartScheduleKeyValue();
    }
    if (key == "WaitForStopTransactionsOnResetTimeout") {
        return this->getWaitForStopTransactionsOnResetTimeoutKeyValue();
    }
    if (key == "HostName") {
        return this->getHostNameKeyValue();
    }
    if (key == "SupportedMeasurands") {
        return this->getSupportedMeasurandsKeyValue();
    }
    if (key == "MaxMessageSize") {
        return this->getMaxMessageSizeKeyValue();
    }
    if (key == "QueueAllMessages") {
        return this->getQueueAllMessagesKeyValue();
    }
    if (key == "MessageQueueSizeThreshold") {
        return this->getMessageQueueSizeThresholdKeyValue();
    }

    // Core Profile
    if (key == "AllowOfflineTxForUnknownId") {
        return this->getAllowOfflineTxForUnknownIdKeyValue();
    }
    if (key == "AuthorizationCacheEnabled") {
        return this->getAuthorizationCacheEnabledKeyValue();
    }
    // we should not return an AuthorizationKey because it's readonly
    // if (key == "AuthorizationKey") {
    //     return this->getAuthorizationKeyKeyValue();
    // }
    if (key == "AuthorizeRemoteTxRequests") {
        return this->getAuthorizeRemoteTxRequestsKeyValue();
    }
    if (key == "BlinkRepeat") {
        return this->getBlinkRepeatKeyValue();
    }
    if (key == "ClockAlignedDataInterval") {
        return this->getClockAlignedDataIntervalKeyValue();
    }
    if (key == "ConnectionTimeOut") {
        return this->getConnectionTimeOutKeyValue();
    }
    if (key == "ConnectorPhaseRotation") {
        return this->getConnectorPhaseRotationKeyValue();
    }
    if (key == "ConnectorPhaseRotationMaxLength") {
        return this->getConnectorPhaseRotationMaxLengthKeyValue();
    }
    if (key == "CpoName") {
        return this->getCpoNameKeyValue();
    }
    if (key == "GetConfigurationMaxKeys") {
        return this->getGetConfigurationMaxKeysKeyValue();
    }
    if (key == "HeartbeatInterval") {
        return this->getHeartbeatIntervalKeyValue();
    }
    if (key == "LightIntensity") {
        return this->getLightIntensityKeyValue();
    }
    if (key == "LocalAuthorizeOffline") {
        return this->getLocalAuthorizeOfflineKeyValue();
    }
    if (key == "LocalPreAuthorize") {
        return this->getLocalPreAuthorizeKeyValue();
    }
    if (key == "MaxEnergyOnInvalidId") {
        return this->getMaxEnergyOnInvalidIdKeyValue();
    }
    if (key == "MeterValuesAlignedData") {
        return this->getMeterValuesAlignedDataKeyValue();
    }
    if (key == "MeterValuesAlignedDataMaxLength") {
        return this->getMeterValuesAlignedDataMaxLengthKeyValue();
    }
    if (key == "MeterValuesSampledData") {
        return this->getMeterValuesSampledDataKeyValue();
    }
    if (key == "MeterValuesSampledDataMaxLength") {
        return this->getMeterValuesSampledDataMaxLengthKeyValue();
    }
    if (key == "MeterValueSampleInterval") {
        return this->getMeterValueSampleIntervalKeyValue();
    }
    if (key == "MinimumStatusDuration") {
        return this->getMinimumStatusDurationKeyValue();
    }
    if (key == "NumberOfConnectors") {
        return this->getNumberOfConnectorsKeyValue();
    }
    if (key == "ReserveConnectorZeroSupported") {
        return this->getReserveConnectorZeroSupportedKeyValue();
    }
    if (key == "ResetRetries") {
        return this->getResetRetriesKeyValue();
    }
    if (key == "SecurityProfile") {
        return this->getSecurityProfileKeyValue();
    }
    if (key == "DisableSecurityEventNotifications") {
        return this->getDisableSecurityEventNotificationsKeyValue();
    }
    if (key == "StopTransactionOnEVSideDisconnect") {
        return this->getStopTransactionOnEVSideDisconnectKeyValue();
    }
    if (key == "StopTransactionOnInvalidId") {
        return this->getStopTransactionOnInvalidIdKeyValue();
    }
    if (key == "StopTxnAlignedData") {
        return this->getStopTxnAlignedDataKeyValue();
    }
    if (key == "StopTxnAlignedDataMaxLength") {
        return this->getStopTxnAlignedDataMaxLengthKeyValue();
    }
    if (key == "StopTxnSampledData") {
        return this->getStopTxnSampledDataKeyValue();
    }
    if (key == "StopTxnSampledDataMaxLength") {
        return this->getStopTxnSampledDataMaxLengthKeyValue();
    }
    if (key == "SupportedFeatureProfiles") {
        return this->getSupportedFeatureProfilesKeyValue();
    }
    if (key == "SupportedFeatureProfilesMaxLength") {
        return this->getSupportedFeatureProfilesMaxLengthKeyValue();
    }
    if (key == "TransactionMessageAttempts") {
        return this->getTransactionMessageAttemptsKeyValue();
    }
    if (key == "TransactionMessageRetryInterval") {
        return this->getTransactionMessageRetryIntervalKeyValue();
    }
    if (key == "UnlockConnectorOnEVSideDisconnect") {
        return this->getUnlockConnectorOnEVSideDisconnectKeyValue();
    }
    if (key == "WebsocketPingInterval") {
        return this->getWebsocketPingIntervalKeyValue();
    }

    // PnC
    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::PnC)) {
        if (key == "ISO15118PnCEnabled") {
            return this->getISO15118PnCEnabledKeyValue();
        }
        if (key == "CentralContractValidationAllowed") {
            return this->getCentralContractValidationAllowedKeyValue();
        }
        if (key == "CertSigningWaitMinimum") {
            return this->getCertSigningWaitMinimumKeyValue();
        }
        if (key == "CertSigningRepeatTimes") {
            return this->getCertSigningRepeatTimesKeyValue();
        }
        if (key == "ContractValidationOffline") {
            return this->getContractValidationOfflineKeyValue();
        }
    }

    // Smart Charging
    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::SmartCharging)) {
        if (key == "ChargeProfileMaxStackLevel") {
            return this->getChargeProfileMaxStackLevelKeyValue();
        }
        if (key == "ChargingScheduleAllowedChargingRateUnit") {
            return this->getChargingScheduleAllowedChargingRateUnitKeyValue();
        }
        if (key == "ChargingScheduleMaxPeriods") {
            return this->getChargingScheduleMaxPeriodsKeyValue();
        }
        if (key == "ConnectorSwitch3to1PhaseSupported") {
            return this->getConnectorSwitch3to1PhaseSupportedKeyValue();
        }
        if (key == "MaxChargingProfilesInstalled") {
            return this->getMaxChargingProfilesInstalledKeyValue();
        }
    }

    // Local Auth List Managementg
    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::LocalAuthListManagement)) {
        if (key == "LocalAuthListEnabled") {
            return this->getLocalAuthListEnabledKeyValue();
        }
        if (key == "LocalAuthListMaxLength") {
            return this->getLocalAuthListMaxLengthKeyValue();
        }
        if (key == "SendLocalListMaxLength") {
            return this->getSendLocalListMaxLengthKeyValue();
        }
    }

    // California Pricing
    if (key == "CustomDisplayCostAndPrice") {
        return this->getCustomDisplayCostAndPriceEnabledKeyValue();
    }

    if (getCustomDisplayCostAndPriceEnabled()) {
        if (key == "NumberOfDecimalsForCostValues") {
            return this->getPriceNumberOfDecimalsForCostValuesKeyValue();
        }
        if (key == "DefaultPrice") {
            return this->getDefaultPriceKeyValue();
        }
        if (key.get().find("DefaultPriceText") == 0 && this->getCustomMultiLanguageMessagesEnabled().has_value() &&
            this->getCustomMultiLanguageMessagesEnabled().value()) {
            const std::vector<std::string> message_language = split_string(key, ',');
            if (message_language.size() > 1) {
                return this->getDefaultPriceTextKeyValue(message_language.at(1));
            }
        }
        if (key == "TimeOffset") {
            return this->getDisplayTimeOffsetKeyValue();
        }
        if (key == "NextTimeOffsetTransitionDateTime") {
            return this->getNextTimeOffsetTransitionDateTimeKeyValue();
        }
        if (key == "TimeOffsetNextTransition") {
            return this->getTimeOffsetNextTransitionKeyValue();
        }
        if (key == "CustomIdleFeeAfterStop") {
            return this->getCustomIdleFeeAfterStopKeyValue();
        }
        if (key == "MultiLanguageSupportedLanguages") {
            return this->getMultiLanguageSupportedLanguagesKeyValue();
        }
        if (key == "CustomMultiLanguageMessages") {
            return this->getCustomMultiLanguageMessagesEnabledKeyValue();
        }
        if (key == "Language") {
            return this->getLanguageKeyValue();
        }
    }

    if (this->supported_feature_profiles.count(SupportedFeatureProfiles::Custom)) {
        return this->getCustomKeyValue(key);
    }

    return std::nullopt;
}

std::vector<KeyValue> ChargePointConfiguration::get_all_key_value() {
    std::vector<KeyValue> all;
    for (auto feature_profile : this->getSupportedFeatureProfilesSet()) {
        auto feature_profile_string = conversions::supported_feature_profiles_to_string(feature_profile);
        if (this->config.contains(feature_profile_string)) {
            auto& feature_config = this->config[feature_profile_string];
            for (auto& feature_config_entry : feature_config.items()) {
                const auto config_key = CiString<50>(feature_config_entry.key());
                // DefaultPriceText is a special here, as it has multiple possible languages which are all separate
                // key value pairs.
                if (config_key.get().find("DefaultPriceText") == 0) {
                    const auto price_text_key_values = getAllDefaultPriceTextKeyValues();
                    if (price_text_key_values.has_value()) {
                        for (const auto& kv : price_text_key_values.value()) {
                            all.push_back(kv);
                        }
                    }
                } else {
                    auto config_value = this->get(config_key);
                    if (config_value != std::nullopt) {
                        all.push_back(config_value.value());
                    }
                }
            }
        }
    }
    return all;
}

ConfigurationStatus ChargePointConfiguration::set(CiString<50> key, CiString<500> value) {
    std::lock_guard<std::recursive_mutex> lock(this->configuration_mutex);
    if (key == "AllowOfflineTxForUnknownId") {
        if (this->getAllowOfflineTxForUnknownId() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        if (isBool(value.get())) {
            this->setAllowOfflineTxForUnknownId(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "AuthorizationCacheEnabled") {
        if (this->getAuthorizationCacheEnabled() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        if (isBool(value.get())) {
            this->setAuthorizationCacheEnabled(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "AuthorizationKey") {
        std::string authorization_key = value.get();
        if (authorization_key.length() >= 8) {
            this->setAuthorizationKey(value.get());
            return ConfigurationStatus::Accepted;
        } else {
            EVLOG_warning << "Attempt to change AuthorizationKey to value with < 8 characters";
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "AuthorizeRemoteTxRequests") {
        this->setAuthorizeRemoteTxRequests(ocpp::conversions::string_to_bool(value.get()));
    }
    if (key == "BlinkRepeat") {
        if (this->getBlinkRepeat() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        try {
            auto [valid, blink_repeat] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setBlinkRepeat(blink_repeat);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "ClockAlignedDataInterval") {
        try {
            auto [valid, interval] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setClockAlignedDataInterval(interval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "ConnectionTimeOut") {
        try {
            auto [valid, timeout] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setConnectionTimeOut(timeout);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "ConnectorPhaseRotation") {
        if (this->isConnectorPhaseRotationValid(value.get())) {
            this->setConnectorPhaseRotation(value.get());
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "CentralContractValidationAllowed") {
        if (this->getCentralContractValidationAllowed() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        } else {
            this->setContractValidationOffline(ocpp::conversions::string_to_bool(value.get()));
        }
    }
    if (key == "CertSigningWaitMinimum") {
        if (this->getCertSigningWaitMinimum() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        } else {
            try {
                auto [valid, cert_signing_wait_minimum] = is_positive_integer(value.get());
                if (!valid) {
                    return ConfigurationStatus::Rejected;
                }
                this->setCertSigningWaitMinimum(cert_signing_wait_minimum);
            } catch (const std::invalid_argument& e) {
                return ConfigurationStatus::Rejected;
            } catch (const std::out_of_range& e) {
                return ConfigurationStatus::Rejected;
            }
        }
    }
    if (key == "CertSigningRepeatTimes") {
        if (this->getCertSigningRepeatTimes() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        } else {
            try {
                auto [valid, cert_signing_repeat_times] = is_positive_integer(value.get());
                if (!valid) {
                    return ConfigurationStatus::Rejected;
                }
                this->setCertSigningRepeatTimes(cert_signing_repeat_times);
            } catch (const std::invalid_argument& e) {
                return ConfigurationStatus::Rejected;
            } catch (const std::out_of_range& e) {
                return ConfigurationStatus::Rejected;
            }
        }
    }
    if (key == "ContractValidationOffline") {
        this->setContractValidationOffline(ocpp::conversions::string_to_bool(value.get()));
    }
    if (key == "CpoName") {
        this->setCpoName(value.get());
    }
    if (key == "DisableSecurityEventNotifications") {
        this->setDisableSecurityEventNotifications(ocpp::conversions::string_to_bool(value.get()));
    }
    if (key == "HeartbeatInterval") {
        try {
            auto [valid, interval] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setHeartbeatInterval(interval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "ISO15118PnCEnabled") {
        this->setISO15118PnCEnabled(ocpp::conversions::string_to_bool(value.get()));
    }
    if (key == "LightIntensity") {
        if (this->getLightIntensity() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        try {
            auto [valid, light_intensity] = is_positive_integer(value.get());
            if (!valid or light_intensity > 100) {
                return ConfigurationStatus::Rejected;
            }
            this->setLightIntensity(light_intensity);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "LocalAuthorizeOffline") {
        if (isBool(value.get())) {
            this->setLocalAuthorizeOffline(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "LocalPreAuthorize") {
        if (isBool(value.get())) {
            this->setLocalPreAuthorize(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MaxEnergyOnInvalidId") {
        if (this->getMaxEnergyOnInvalidId() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        try {
            auto [valid, max_energy] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setMaxEnergyOnInvalidId(max_energy);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MeterValuesAlignedData") {
        if (!this->setMeterValuesAlignedData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MeterValuesSampledData") {
        if (!this->setMeterValuesSampledData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MeterValueSampleInterval") {
        try {
            auto [valid, meter_value_sample_interval] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setMeterValueSampleInterval(meter_value_sample_interval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "MinimumStatusDuration") {
        if (this->getMinimumStatusDuration() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        try {
            auto [valid, duration] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setMinimumStatusDuration(duration);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "OcspRequestInterval") {
        try {
            auto [valid, ocsp_request_interval] = is_positive_integer(value.get());
            if (!valid or ocsp_request_interval < 86400) {
                return ConfigurationStatus::Rejected;
            }
            this->setOcspRequestInterval(ocsp_request_interval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "WaitForStopTransactionsOnResetTimeout") {
        try {
            auto [valid, wait_for_stop_transactions_on_reset_timeout] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setWaitForStopTransactionsOnResetTimeout(wait_for_stop_transactions_on_reset_timeout);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "ResetRetries") {
        try {
            auto [valid, reset_retries] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setResetRetries(reset_retries);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "StopTransactionOnEVSideDisconnect") {
        if (isBool(value.get())) {
            this->setStopTransactionOnEVSideDisconnect(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "StopTransactionOnInvalidId") {
        if (isBool(value.get())) {
            this->setStopTransactionOnInvalidId(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "StopTxnAlignedData") {
        if (!this->setStopTxnAlignedData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "StopTxnSampledData") {
        if (!this->setStopTxnSampledData(value.get())) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "TransactionMessageAttempts") {
        try {
            auto [valid, message_attempts] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setTransactionMessageAttempts(message_attempts);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "TransactionMessageRetryInterval") {
        try {
            auto [valid, retry_inverval] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setTransactionMessageRetryInterval(retry_inverval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "UnlockConnectorOnEVSideDisconnect") {
        if (isBool(value.get())) {
            this->setUnlockConnectorOnEVSideDisconnect(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }
    if (key == "WebsocketPingInterval") {
        if (this->getWebsocketPingInterval() == std::nullopt) {
            return ConfigurationStatus::NotSupported;
        }
        try {
            auto [valid, interval] = is_positive_integer(value.get());
            if (!valid) {
                return ConfigurationStatus::Rejected;
            }
            this->setWebsocketPingInterval(interval);
        } catch (const std::invalid_argument& e) {
            return ConfigurationStatus::Rejected;
        } catch (const std::out_of_range& e) {
            return ConfigurationStatus::Rejected;
        }
    }

    // Local Auth List Management
    if (key == "LocalAuthListEnabled") {
        if (this->supported_feature_profiles.count(SupportedFeatureProfiles::LocalAuthListManagement)) {
            if (isBool(value.get())) {
                this->setLocalAuthListEnabled(ocpp::conversions::string_to_bool(value.get()));
            } else {
                return ConfigurationStatus::Rejected;
            }
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }

    if (key == "VerifyCsmsAllowWildcards") {
        if (isBool(value.get())) {
            this->setVerifyCsmsAllowWildcards(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::Rejected;
        }
    }

    // Hubject PnC Extension keys
    if (key == "SeccLeafSubjectCommonName") {
        if (this->getSeccLeafSubjectCommonName().has_value()) {
            this->setSeccLeafSubjectCommonName(value.get());
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }
    if (key == "SeccLeafSubjectCountry") {
        if (this->getSeccLeafSubjectCountry().has_value()) {
            this->setSeccLeafSubjectCountry(value.get());
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }
    if (key == "SeccLeafSubjectOrganization") {
        if (this->getSeccLeafSubjectOrganization().has_value()) {
            this->setSeccLeafSubjectOrganization(value.get());
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }
    if (key == "ConnectorEvseIds") {
        if (this->getConnectorEvseIds().has_value()) {
            this->setConnectorEvseIds(value.get());
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }
    if (key == "AllowChargingProfileWithoutStartSchedule") {
        if (this->getAllowChargingProfileWithoutStartSchedule().has_value()) {
            this->setAllowChargingProfileWithoutStartSchedule(ocpp::conversions::string_to_bool(value.get()));
        } else {
            return ConfigurationStatus::NotSupported;
        }
    }

    if (key.get().find("DefaultPriceText") == 0) {
        const ConfigurationStatus result = this->setDefaultPriceText(key, value);
        if (result != ConfigurationStatus::Accepted) {
            return result;
        }
    }

    if (key == "DefaultPrice") {
        const ConfigurationStatus result = this->setDefaultPrice(value);
        if (result != ConfigurationStatus::Accepted) {
            return result;
        }
    }

    if (key == "TimeOffset") {
        const ConfigurationStatus result = this->setDisplayTimeOffset(value);
        if (result != ConfigurationStatus::Accepted) {
            return result;
        }
    }

    if (key == "NextTimeOffsetTransitionDateTime") {
        const ConfigurationStatus result = this->setNextTimeOffsetTransitionDateTime(value);
        if (result != ConfigurationStatus::Accepted) {
            return result;
        }
    }

    if (key == "TimeOffsetNextTransition") {
        const ConfigurationStatus result = this->setTimeOffsetNextTransition(value);
        if (result != ConfigurationStatus::Accepted) {
            return result;
        }
    }

    if (key == "CustomIdleFeeAfterStop") {
        this->setCustomIdleFeeAfterStop(ocpp::conversions::string_to_bool(value));
    }

    if (key == "Language") {
        this->setLanguage(value);
    }

    if (this->config.contains("Custom") and this->config["Custom"].contains(key.get())) {
        return this->setCustomKey(key, value, false);
    }

    return ConfigurationStatus::Accepted;
}

} // namespace v16
} // namespace ocpp
