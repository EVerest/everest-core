// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "ocpp/v2/ocpp_enums.hpp"
#include <ocpp/common/utils.hpp>
#include <ocpp/v16/charge_point_configuration_devicemodel.hpp>
#include <ocpp/v16/known_keys.hpp>
#include <ocpp/v16/types.hpp>
#include <ocpp/v16/utils.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/ocpp_types.hpp>

#include <algorithm>
#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace {
using namespace ocpp;
using SetResult = v16::ChargePointConfigurationDeviceModel::SetResult;
using DeviceModelInterface = v2::DeviceModelInterface;

constexpr const char* custom_component = "Custom";

constexpr v16::ConfigurationStatus convert(SetResult res) {
    switch (res) {
    case SetResult::Accepted:
        return v16::ConfigurationStatus::Accepted;
    case SetResult::Rejected:
        return v16::ConfigurationStatus::Rejected;
    case SetResult::RebootRequired:
        return v16::ConfigurationStatus::RebootRequired;
    case SetResult::NotSupportedAttributeType:
    case SetResult::UnknownComponent:
    case SetResult::UnknownVariable:
    default:
        return v16::ConfigurationStatus::NotSupported;
    }
}

// std::to_string uses "1" and "0"
constexpr const char* to_string(bool value) {
    return (value) ? "true" : "false";
}

/// \brief check if a key is valid against the supported features
inline bool ignore_key(v16::keys::valid_keys key, const std::set<v16::SupportedFeatureProfiles>& profiles,
                       v16::SupportedFeatureProfiles profile) {
    // if the profile is not valid and the key is then return true
    bool supported = profiles.find(profile) != profiles.end();
    v16::keys::sections section{v16::keys::sections::Custom};

    switch (profile) {
    case v16::SupportedFeatureProfiles::Core:
        section = v16::keys::sections::Core;
        break;
    case v16::SupportedFeatureProfiles::CostAndPrice:
        section = v16::keys::sections::CostAndPrice;
        break;
    case v16::SupportedFeatureProfiles::Custom:
        section = v16::keys::sections::Custom;
        break;
    case v16::SupportedFeatureProfiles::FirmwareManagement:
        section = v16::keys::sections::FirmwareManagement;
        break;
    case v16::SupportedFeatureProfiles::Internal:
        section = v16::keys::sections::Internal;
        break;
    case v16::SupportedFeatureProfiles::LocalAuthListManagement:
        section = v16::keys::sections::LocalAuthListManagement;
        break;
    case v16::SupportedFeatureProfiles::PnC:
        section = v16::keys::sections::PnC;
        break;
    case v16::SupportedFeatureProfiles::Reservation:
        section = v16::keys::sections::Reservation;
        break;
    case v16::SupportedFeatureProfiles::Security:
        section = v16::keys::sections::Security;
        break;
    case v16::SupportedFeatureProfiles::SmartCharging:
        section = v16::keys::sections::SmartCharging;
        break;
    case v16::SupportedFeatureProfiles::RemoteTrigger:
    default:
        break;
    }

    bool valid = v16::keys::to_section(key) == section;
    return !supported && valid;
}

/// \brief raise exception when accessing a key that doesn't exist
void raise_not_found(const std::string_view& section, const std::string_view& name) {
    EVLOG_critical << "Directly requested value for ComponentVariable that doesn't exist in the device model: "
                   << section << '.' << name;
    EVLOG_AND_THROW(
        std::runtime_error("Directly requested value for ComponentVariable that doesn't exist in the device model."));
}

// Component/Variable support ...

std::optional<bool> isReadOnly(DeviceModelInterface& storage, const std::string_view& component,
                               const std::string_view& variable) {
    // known keys are checked via is_readonly() in known_keys.hpp
    // for other keys get_mutability() is used
    const v2::Component component_v{std::string{component}};
    const v2::Variable variable_v{std::string{variable}};
    std::optional<bool> result;
    const auto res = storage.get_mutability(component_v, variable_v, v2::AttributeEnum::Actual);
    if (res) {
        result = res.value() == v2::MutabilityEnum::ReadOnly;
    }
    return result;
}

template <typename T>
std::optional<T> get_optional(DeviceModelInterface& storage, const std::string_view& component,
                              const std::string_view& variable) {
    const v2::Component component_v{std::string{component}};
    const v2::Variable variable_v{std::string{variable}};
    std::string value;
    const auto get_result = storage.get_variable(component_v, variable_v, v2::AttributeEnum::Actual, value);
    if (get_result == v2::GetVariableStatusEnum::Accepted) {
        try {
            if constexpr (std::is_same_v<bool, T>) {
                return v16::ChargePointConfigurationBase::toBool(value);
            } else if constexpr (std::is_same_v<std::string, T>) {
                return value;
            } else {
                return v2::to_specific_type<T>(value);
            }
        } catch (const std::exception& ex) {
            EVLOG_warning << component_v.name << '[' << variable_v.name << "] '" << value
                          << "' to_specific_type exception: " << ex.what();
        }
    }
    return std::nullopt;
}

template <typename T> std::optional<T> get_optional(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    const auto component = v16::keys::to_section_string_view(key);
    const auto variable = v16::keys::convert(key);
    return get_optional<T>(storage, component, variable);
}

template <typename T> T inline get_value(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    const auto result = get_optional<T>(storage, key);
    if (!result) {
        raise_not_found(v16::keys::to_section_string_view(key), v16::keys::convert(key));
    }
    return result.value();
}

template <typename T>
inline void get_value(std::optional<T>& value, DeviceModelInterface& storage, v16::keys::valid_keys key) {
    value = get_optional<T>(storage, key);
}

template <typename T> inline void get_value(T& value, DeviceModelInterface& storage, v16::keys::valid_keys key) {
    value = get_value<T>(storage, key);
}

bool key_exists(DeviceModelInterface& storage, const std::string_view& component, const std::string_view& variable) {
    const v2::Component component_v{std::string{component}};
    const v2::Variable variable_v{std::string{variable}};
    const auto result = storage.get_variable_meta_data(component_v, variable_v);
    return result.has_value();
}

bool key_exists(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    const auto component = v16::keys::to_section_string_view(key);
    const auto variable = v16::keys::convert(key);
    return key_exists(storage, component, variable);
}

std::optional<v16::KeyValue> get_key_value_optional(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    auto get_result = get_optional<std::string>(storage, key);
    std::optional<v16::KeyValue> result;
    if (get_result) {
        v16::KeyValue kv;
        kv.key = std::move(std::string{v16::keys::convert(key)});
        kv.readonly = v16::keys::is_readonly(key);
        kv.value = std::move(get_result.value());
        result = kv;
    }
    return result;
}

template <typename T> v16::KeyValue get_key_value(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    const auto result = get_key_value_optional(storage, key);
    if (!result) {
        raise_not_found(v16::keys::to_section_string_view(key), v16::keys::convert(key));
    }
    return result.value();
}

inline v16::KeyValue get_key_value(DeviceModelInterface& storage, v16::keys::valid_keys key) {
    return get_key_value<std::string>(storage, key);
}

/// set known key to specified value
SetResult set_value(DeviceModelInterface& storage, const std::string_view& component, const std::string_view& variable,
                    const std::string& value) {
    const v2::Component component_v{std::string{component}};
    const v2::Variable variable_v{std::string{variable}};
    return storage.set_value(component_v, variable_v, v2::AttributeEnum::Actual, value, "OCPP 1.6");
}

SetResult set_value(DeviceModelInterface& storage, v16::keys::valid_keys key, const std::string& value) {
    const v2::Component component{std::string{v16::keys::to_section_string_view(key)}};
    const v2::Variable variable{std::string{v16::keys::convert(key)}};
    return storage.set_value(component, variable, v2::AttributeEnum::Actual, value, "OCPP 1.6");
}

/// set known key to optional specified value
constexpr SetResult set_value(DeviceModelInterface& storage, v16::keys::valid_keys key,
                              const std::optional<std::string>& value) {
    // using accepted since there isn't a value to set - so not an error
    SetResult result{SetResult::Accepted};
    if (value) {
        result = set_value(storage, key, value.value());
    }
    return result;
}

using check_fn = std::function<bool(const std::string&)>;

/// set known key to specified value checking the value is valid
inline SetResult set_value(check_fn fn, DeviceModelInterface& storage, v16::keys::valid_keys key,
                           const std::string& value) {
    SetResult result{SetResult::Rejected};
    if (fn(value)) {
        result = set_value(storage, key, value);
    } else {
        EVLOG_warning << "set " << v16::keys::to_section_string_view(key) << '[' << v16::keys::convert(key)
                      << "]=" << value << " failed, invalid value";
    }
    return result;
}

/// set known key to specified value checking the key already exists
inline SetResult set_value_check(DeviceModelInterface& storage, v16::keys::valid_keys key, const std::string& value) {
    SetResult result{SetResult::UnknownVariable};
    if (key_exists(storage, key)) {
        result = set_value(storage, key, value);
    } else {
        EVLOG_warning << "set " << v16::keys::to_section_string_view(key) << '[' << v16::keys::convert(key)
                      << "]=" << value << " failed, key doesn't exist";
    }
    return result;
}

/// set known key to specified value checking the key already exists and the value is valid
inline SetResult set_value_check(check_fn fn, DeviceModelInterface& storage, v16::keys::valid_keys key,
                                 const std::string& value) {
    SetResult result{SetResult::UnknownVariable};
    if (key_exists(storage, key)) {
        if (fn(value)) {
            result = set_value(storage, key, value);
        } else {
            EVLOG_warning << "set " << v16::keys::to_section_string_view(key) << '[' << v16::keys::convert(key)
                          << "]=" << value << " failed, value not valid";
        }
    } else {
        EVLOG_warning << "set " << v16::keys::to_section_string_view(key) << '[' << v16::keys::convert(key)
                      << "]=" << value << " failed, key doesn't exist";
        result = SetResult::Rejected;
    }
    return result;
}

// Custom key support ...

template <typename T> std::optional<T> get_optional(DeviceModelInterface& storage, const std::string_view& name) {
    return get_optional<T>(storage, custom_component, name);
}

template <typename T> inline T get_value(DeviceModelInterface& storage, const std::string_view& name) {
    const auto result = get_optional<T>(storage, name);
    if (!result) {
        raise_not_found(custom_component, name);
    }
    return result.value();
}

template <typename T>
inline void get_value(std::optional<T>& value, DeviceModelInterface& storage, const std::string_view& name) {
    value = get_optional<T>(storage, name);
}

template <typename T> inline void get_value(T& value, DeviceModelInterface& storage, const std::string_view& name) {
    value = get_value<T>(storage, name);
}

std::optional<v16::KeyValue> get_key_value_optional(DeviceModelInterface& storage, const std::string_view& name) {
    auto get_result = get_optional<std::string>(storage, name);
    std::optional<v16::KeyValue> result;
    if (get_result) {
        v16::KeyValue kv;
        kv.key = std::move(std::string{name});
        kv.readonly = isReadOnly(storage, custom_component, name).value_or(true);
        kv.value = get_result.value();
        result = kv;
    }
    return result;
}

v16::KeyValue get_key_value(DeviceModelInterface& storage, const std::string_view& name) {
    const auto result = get_key_value_optional(storage, name);
    if (!result) {
        raise_not_found(custom_component, name);
    }
    return result.value();
}

/// \brief check if a custom key exists
bool key_exists(DeviceModelInterface& storage, const std::string_view& name) {
    return key_exists(storage, custom_component, name);
}

/// \brief set custom value
SetResult set_value(DeviceModelInterface& storage, const std::string_view& name, const std::string& value) {
    const v2::Component component{custom_component};
    const v2::Variable variable{std::string{name}};
    return storage.set_value(component, variable, v2::AttributeEnum::Actual, value, "OCPP 1.6");
}

/// \brief set custom value
constexpr SetResult set_value(DeviceModelInterface& storage, const std::string_view& name,
                              const std::optional<std::string>& value) {
    // using accepted since there isn't a value to set - so not an error
    SetResult result{SetResult::Accepted};
    if (value) {
        result = set_value(storage, name, value.value());
    }
    return result;
}

inline SetResult set_value_check(DeviceModelInterface& storage, const std::string_view& name,
                                 const std::string& value) {
    SetResult result{SetResult::UnknownVariable};
    if (key_exists(storage, name)) {
        result = set_value(storage, name, value);
    } else {
        EVLOG_warning << "set " << name << '=' << value << " failed, key doesn't exist";
    }
    return result;
}

} // namespace

// ----------------------------------------------------------------------------
// ChargePointConfigurationDeviceModel

namespace ocpp::v16 {

// ----------------------------------------------------------------------------
// Protected methods

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalAllowChargingProfileWithoutStartSchedule(const std::string& value) {
    return set_value_check(isBool, *storage, keys::valid_keys::AllowChargingProfileWithoutStartSchedule, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCentralSystemURI(const std::string& value) {
    EVLOG_warning << "CentralSystemURI changed to: " << value;
    auto result = set_value(*storage, keys::valid_keys::CentralSystemURI, value);
    if (result == SetResult::Accepted) {
        result = SetResult::RebootRequired;
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCompositeScheduleDefaultLimitAmps(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::CompositeScheduleDefaultLimitAmps, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCompositeScheduleDefaultLimitWatts(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::CompositeScheduleDefaultLimitWatts, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCompositeScheduleDefaultNumberPhases(const std::string& value) {
    auto check = [](const std::string& value) {
        bool result{false};
        try {
            const auto [valid, num_phases] = is_positive_integer(value);
            result = valid;
            if (num_phases <= 0 or num_phases > 3) {
                result = false;
            }
        } catch (const std::invalid_argument& e) {
        } catch (const std::out_of_range& e) {
        }
        return result;
    };
    return set_value_check(check, *storage, keys::valid_keys::CompositeScheduleDefaultNumberPhases, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalConnectorEvseIds(const std::string& value) {
    return set_value_check(areValidEvseIds, *storage, keys::valid_keys::ConnectorEvseIds, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalIgnoredProfilePurposesOffline(const std::string& value) {
    SetResult result{SetResult::UnknownVariable};

    if (key_exists(*storage, keys::valid_keys::IgnoredProfilePurposesOffline)) {
        // check the purposes are valid
        const auto purposes = utils::from_csl(value);
        bool invalid{false};
        for (const auto& purpose : purposes) {
            try {
                conversions::string_to_charging_profile_purpose_type(purpose);
            } catch (const StringToEnumException& e) {
                EVLOG_warning << "Could not convert element of IgnoredProfilePurposesOffline: " << purpose;
                invalid = true;
                result = SetResult::Rejected;
            }
        }

        if (!invalid) {
            result = set_value(*storage, keys::valid_keys::IgnoredProfilePurposesOffline, utils::to_csl(purposes));
        }
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalOcspRequestInterval(const std::string& value) {
    auto check = [](const std::string& value) {
        bool result{false};
        try {
            const auto [valid, ocsp_request_interval] = is_positive_integer(value);
            result = valid;
            if (ocsp_request_interval < OCSP_REQUEST_INTERVAL_MIN) {
                result = false;
            }
        } catch (const std::invalid_argument& e) {
        } catch (const std::out_of_range& e) {
        }
        return result;
    };
    return set_value(check, *storage, keys::valid_keys::OcspRequestInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalRetryBackoffRandomRange(const std::string& value) {
    return set_value(*storage, keys::valid_keys::RetryBackoffRandomRange, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalRetryBackoffRepeatTimes(const std::string& value) {
    return set_value(*storage, keys::valid_keys::RetryBackoffRepeatTimes, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalRetryBackoffWaitMinimum(const std::string& value) {
    return set_value(*storage, keys::valid_keys::RetryBackoffWaitMinimum, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalSeccLeafSubjectCommonName(const std::string& value) {
    SetResult result{SetResult::Rejected};
    if (value.size() >= SECC_LEAF_SUBJECT_COMMON_NAME_MIN_LENGTH &&
        value.size() <= SECC_LEAF_SUBJECT_COMMON_NAME_MAX_LENGTH) {
        result = set_value_check(*storage, keys::valid_keys::SeccLeafSubjectCommonName, value);
    } else {
        EVLOG_warning << "Attempt to set SeccLeafSubjectCommonName with invalid number of characters";
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalSeccLeafSubjectCountry(const std::string& value) {
    SetResult result{SetResult::Rejected};
    if (value.size() == SECC_LEAF_SUBJECT_COUNTRY_LENGTH) {
        result = set_value_check(*storage, keys::valid_keys::SeccLeafSubjectCountry, value);
    } else {
        EVLOG_warning << "Attempt to set SeccLeafSubjectCountry with invalid number of characters";
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalSeccLeafSubjectOrganization(const std::string& value) {
    SetResult result{SetResult::Rejected};
    if (value.size() <= SECC_LEAF_SUBJECT_ORGANIZATION_MAX_LENGTH) {
        result = set_value_check(*storage, keys::valid_keys::SeccLeafSubjectOrganization, value);
    } else {
        EVLOG_warning << "Attempt to set SeccLeafSubjectOrganization with invalid number of characters";
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalStopTransactionIfUnlockNotSupported(const std::string& value) {
    return set_value_check(isBool, *storage, keys::valid_keys::StopTransactionIfUnlockNotSupported, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalSupplyVoltage(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::SupplyVoltage, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalVerifyCsmsAllowWildcards(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::VerifyCsmsAllowWildcards, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalWaitForStopTransactionsOnResetTimeout(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::WaitForStopTransactionsOnResetTimeout, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalAllowOfflineTxForUnknownId(const std::string& value) {
    return set_value_check(isBool, *storage, keys::valid_keys::AllowOfflineTxForUnknownId, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalAuthorizationCacheEnabled(const std::string& value) {
    return set_value_check(isBool, *storage, keys::valid_keys::AuthorizationCacheEnabled, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalAuthorizeRemoteTxRequests(const std::string& value) {
    return set_value(*storage, keys::valid_keys::AuthorizeRemoteTxRequests, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalBlinkRepeat(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::BlinkRepeat, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalClockAlignedDataInterval(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::ClockAlignedDataInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalConnectionTimeOut(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::ConnectionTimeOut, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalConnectorPhaseRotation(const std::string& value) {
    auto check = [this](const auto& v) { return isConnectorPhaseRotationValid(getNumberOfConnectors(), v); };
    return set_value(check, *storage, keys::valid_keys::ConnectorPhaseRotation, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalHeartbeatInterval(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::HeartbeatInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalLightIntensity(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::LightIntensity, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalLocalAuthorizeOffline(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::LocalAuthorizeOffline, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalLocalPreAuthorize(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::LocalPreAuthorize, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalMaxEnergyOnInvalidId(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::MaxEnergyOnInvalidId, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalMeterValuesAlignedData(const std::string& value) {
    SetResult result{SetResult::Rejected};

    if (isValidSupportedMeasurands(value)) {
        result = set_value(*storage, keys::valid_keys::MeterValuesAlignedData, value);
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalMeterValuesSampledData(const std::string& value) {
    SetResult result{SetResult::Rejected};

    if (isValidSupportedMeasurands(value)) {
        result = set_value(*storage, keys::valid_keys::MeterValuesSampledData, value);
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalMeterValueSampleInterval(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::MeterValueSampleInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalMinimumStatusDuration(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::MinimumStatusDuration, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalResetRetries(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::ResetRetries, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalStopTransactionOnInvalidId(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::StopTransactionOnInvalidId, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalStopTxnAlignedData(const std::string& value) {
    SetResult result{SetResult::Rejected};

    if (isValidSupportedMeasurands(value)) {
        result = set_value(*storage, keys::valid_keys::StopTxnAlignedData, value);
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalStopTxnSampledData(const std::string& value) {
    SetResult result{SetResult::Rejected};

    if (isValidSupportedMeasurands(value)) {
        result = set_value(*storage, keys::valid_keys::StopTxnSampledData, value);
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalTransactionMessageAttempts(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::TransactionMessageAttempts, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalTransactionMessageRetryInterval(const std::string& value) {
    return set_value(isPositiveInteger, *storage, keys::valid_keys::TransactionMessageRetryInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalUnlockConnectorOnEVSideDisconnect(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::UnlockConnectorOnEVSideDisconnect, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalWebsocketPingInterval(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::WebSocketPingInterval, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalAuthorizationKey(const std::string& value) {
    SetResult result{SetResult::Rejected};

    // TODO(piet): SecurityLog entry

    std::string str;
    if (isHexNotation(value)) {
        str = hexToString(value);
    } else {
        str = value;
    }

    if (str.size() >= AUTHORIZATION_KEY_MIN_LENGTH) {
        result = set_value(*storage, keys::valid_keys::AuthorizationKey, str);
    } else {
        EVLOG_warning << "Attempt to change AuthorizationKey to value with < 8 characters";
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCpoName(const std::string& value) {
    return set_value(*storage, keys::valid_keys::CpoName, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalDisableSecurityEventNotifications(const std::string& value) {
    return set_value_check(*storage, keys::valid_keys::DisableSecurityEventNotifications, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalSecurityProfile(const std::string& value) {
    // TODO(piet): add boundaries for value of security profile
    return set_value_check(*storage, keys::valid_keys::SecurityProfile, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalISO15118CertificateManagementEnabled(const std::string& value) {
    return set_value(*storage, keys::valid_keys::ISO15118CertificateManagementEnabled, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalLocalAuthListEnabled(const std::string& value) {
    SetResult result{SetResult::UnknownVariable};
    if (supported_feature_profiles.find(SupportedFeatureProfiles::LocalAuthListManagement) !=
        supported_feature_profiles.end()) {
        result = set_value(isBool, *storage, keys::valid_keys::LocalAuthListEnabled, value);
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalContractValidationOffline(const std::string& value) {
    return set_value(*storage, keys::valid_keys::ContractValidationOffline, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCentralContractValidationAllowed(const std::string& value) {
    return set_value_check(*storage, keys::valid_keys::CentralContractValidationAllowed, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCertSigningRepeatTimes(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::CertSigningRepeatTimes, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCertSigningWaitMinimum(const std::string& value) {
    return set_value_check(isPositiveInteger, *storage, keys::valid_keys::CertSigningWaitMinimum, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalISO15118PnCEnabled(const std::string& value) {
    return set_value(*storage, keys::valid_keys::ISO15118PnCEnabled, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalCustomIdleFeeAfterStop(const std::string& value) {
    return set_value(isBool, *storage, keys::valid_keys::CustomIdleFeeAfterStop, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalDefaultPrice(const std::string& value) {
    SetResult result{SetResult::Rejected};
    try {
        json default_price = json::object();
        default_price = json::parse(value);

        // perform schema validation on value
        json test_value;
        test_value["CustomDisplayCostAndPrice"] = true;
        test_value["DefaultPrice"] = default_price;

        if (validate("CostAndPrice.json", test_value)) {
            result = set_value(*storage, keys::valid_keys::DefaultPrice, value);
        } else {
            EVLOG_error << "DefaultPrice is invalid: " << value;
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Default price json not correct, can not store default price : " << e.what();
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalDefaultPriceText(const std::string& key, const std::string& value) {
    SetResult result{SetResult::Rejected};
    const auto default_prices = utils::split_string(',', key);
    std::string language;
    bool found{false};

    if (default_prices.size() == 2) {
        // Second value is language.
        language = default_prices.at(1);
        // Check if language is allowed. It should be in the list of config item multi language supported languages
        const auto supported_languages = getMultiLanguageSupportedLanguages();
        if (supported_languages) {
            const auto languages = utils::split_string(',', supported_languages.value());
            const auto& found_it =
                std::find_if(languages.begin(), languages.end(), [&language](const std::string& supported_language) {
                    return trim_string(supported_language) == trim_string(language);
                });

            if (found_it != languages.end()) {
                found = true;
            } else {
                EVLOG_error << "Can not set default price text for language '" << language
                            << "', because the language is currently not supported in this charging station. Supported "
                               "languages: "
                            << supported_languages.value();
            }
        } else {
            EVLOG_error << "Can not set a default price text for language '" << language
                        << "', because the config item for multi language support is not set in the config.";
        }
    } else {
        EVLOG_error << "Configuration DefaultPriceText is set, but does not contain a language (Configuration should "
                       "be something like 'DefaultPriceText,en', but is "
                    << key << ").";
    }

    if (found) {
        // add/update existing
        json default_price_json = json::object();

        // DefaultPriceText is a JSON string
        const auto& default_price_text = get_optional<std::string>(*storage, keys::valid_keys::DefaultPriceText);
        if (default_price_text) {
            try {
                default_price_json = json::parse(default_price_text.value());
            } catch (const std::exception& ex) {
                EVLOG_error << "Default price text json not correct, can not fetch default price text : " << ex.what();
            }
        }

        try {
            auto value_json = json::parse(value);

            // priceText is mandatory
            if (value_json.contains("priceText")) {
                value_json["language"] = language;

                if (!default_price_json.contains("priceTexts")) {
                    default_price_json["priceTexts"] = json::array();
                }

                auto& price_texts = default_price_json.at("priceTexts");

                if (price_texts.is_array()) {
                    bool language_found = false;

                    // update the information if it exists
                    for (auto& price_text : price_texts.items()) {
                        if (price_text.value().at("language").get<std::string>() == language) {
                            price_text.value() = value_json;
                            language_found = true;
                            break;
                        }
                    }
                    if (!language_found) {
                        // add new info
                        default_price_json["priceTexts"].push_back(value_json);
                    }

                    // perform schema validation on default_price
                    json test_value;
                    test_value["CustomDisplayCostAndPrice"] = true;
                    test_value["DefaultPriceText"] = default_price_json;

                    if (validate("CostAndPrice.json", test_value)) {
                        result = set_value(*storage, keys::valid_keys::DefaultPriceText, default_price_json.dump(2));
                    } else {
                        EVLOG_error << "DefaultPriceText value is invalid: " << value;
                    }
                } else {
                    EVLOG_error << "Error while setting default price: 'priceTexts' is not an array";
                }

            } else {
                EVLOG_error << "Configuration DefaultPriceText is set, but does not contain 'priceText'";
            }
        } catch (const std::exception& ex) {
            EVLOG_error << "Configuration DefaultPriceText not JSON : " << ex.what();
        }
    }

    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalDisplayTimeOffset(const std::string& value) {
    return set_value(isTimeOffset, *storage, keys::valid_keys::TimeOffset, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalLanguage(const std::string& value) {
    return set_value(*storage, keys::valid_keys::Language, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalNextTimeOffsetTransitionDateTime(const std::string& value) {
    SetResult result{SetResult::Rejected};
    DateTime dt(value);
    if (dt.to_time_point() > date::utc_clock::now()) {
        result = set_value(*storage, keys::valid_keys::NextTimeOffsetTransitionDateTime, value);
    }
    return result;
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalTimeOffsetNextTransition(const std::string& value) {
    return set_value(isTimeOffset, *storage, keys::valid_keys::TimeOffsetNextTransition, value);
}

ChargePointConfigurationDeviceModel::SetResult
ChargePointConfigurationDeviceModel::setInternalWaitForSetUserPriceTimeout(const std::string& value) {
    auto check = [](const std::string& value) {
        bool result{false};
        try {
            const auto [valid, timeout] = is_positive_integer(value);
            result = valid;
            if (timeout > MAX_WAIT_FOR_SET_USER_PRICE_TIMEOUT_MS) {
                result = false;
            }
        } catch (const std::invalid_argument& e) {
        } catch (const std::out_of_range& e) {
        }
        return result;
    };
    return set_value_check(check, *storage, keys::valid_keys::WaitForSetUserPriceTimeout, value);
}

// ----------------------------------------------------------------------------
// Public methods

ChargePointConfigurationDeviceModel::ChargePointConfigurationDeviceModel(
    const std::string_view& ocpp_main_path, std::unique_ptr<v2::DeviceModelInterface> device_model_interface) :
    ChargePointConfigurationBase(ocpp_main_path), storage(std::move(device_model_interface)) {
    const auto profiles = get_optional<std::string>(*storage, keys::valid_keys::SupportedFeatureProfiles);
    const auto measurands = get_optional<std::string>(*storage, keys::valid_keys::SupportedMeasurands);
    ProfilesSet initial;

    // TODO(james-ctc): check how to determine this for v2
    // get from the device model e.g. perhaps:
    // CustomizationCtrlr, ISO15118Ctrlr, TariffCostCtrlr

#if 0
    // If supported add to initial set

    if (config.contains("PnC")) {
        // add PnC behind the scenes as supported feature profile
        initial.insert(conversions::string_to_supported_feature_profiles("PnC"));
    }

    if (config.contains("CostAndPrice")) {
        // Add California Pricing Requirements behind the scenes as supported feature profile
        initial.insert(conversions::string_to_supported_feature_profiles("CostAndPrice"));
    }

    if (config.contains(custom_component)) {
        // add Custom behind the scenes as supported feature profile
        initial.insert(conversions::string_to_supported_feature_profiles(custom_component));
    }
#else
    // TODO(james-ctc): remove these - just added for unit tests
    initial.insert(conversions::string_to_supported_feature_profiles("PnC"));
    initial.insert(conversions::string_to_supported_feature_profiles("CostAndPrice"));
#endif

    initialise(initial, profiles, measurands);
}

// ----------------------------------------------------------------------------
// UserConfig and Internal

std::string ChargePointConfigurationDeviceModel::getChargeBoxSerialNumber() {
    return get_value<std::string>(*storage, keys::valid_keys::ChargeBoxSerialNumber);
}

std::optional<CiString<25>> ChargePointConfigurationDeviceModel::getChargePointSerialNumber() {
    return get_optional<std::string>(*storage, keys::valid_keys::ChargePointSerialNumber);
}

CiString<50> ChargePointConfigurationDeviceModel::getFirmwareVersion() {
    return get_value<std::string>(*storage, keys::valid_keys::FirmwareVersion);
}

std::optional<CiString<20>> ChargePointConfigurationDeviceModel::getICCID() {
    return get_optional<std::string>(*storage, keys::valid_keys::ICCID);
}

std::optional<CiString<20>> ChargePointConfigurationDeviceModel::getIMSI() {
    return get_optional<std::string>(*storage, keys::valid_keys::IMSI);
}

std::optional<CiString<25>> ChargePointConfigurationDeviceModel::getMeterSerialNumber() {
    return get_optional<std::string>(*storage, keys::valid_keys::MeterSerialNumber);
}

std::optional<CiString<25>> ChargePointConfigurationDeviceModel::getMeterType() {
    return get_optional<std::string>(*storage, keys::valid_keys::MeterType);
}

KeyValue ChargePointConfigurationDeviceModel::getChargeBoxSerialNumberKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargeBoxSerialNumber);
}

KeyValue ChargePointConfigurationDeviceModel::getFirmwareVersionKeyValue() {
    return get_key_value(*storage, keys::valid_keys::FirmwareVersion);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getChargePointSerialNumberKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ChargePointSerialNumber);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getICCIDKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ICCID);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getIMSIKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::IMSI);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMeterSerialNumberKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MeterSerialNumber);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMeterTypeKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MeterType);
}

void ChargePointConfigurationDeviceModel::setChargepointInformation(
    const std::string& chargePointVendor, const std::string& chargePointModel,
    const std::optional<std::string>& chargePointSerialNumber, const std::optional<std::string>& chargeBoxSerialNumber,
    const std::optional<std::string>& firmwareVersion) {
    set_value(*storage, keys::valid_keys::ChargePointVendor, chargePointVendor);
    set_value(*storage, keys::valid_keys::ChargePointModel, chargePointModel);
    set_value(*storage, keys::valid_keys::ChargePointSerialNumber, chargePointSerialNumber);
    set_value(*storage, keys::valid_keys::ChargeBoxSerialNumber, chargeBoxSerialNumber);
    set_value(*storage, keys::valid_keys::FirmwareVersion, firmwareVersion);
}

void ChargePointConfigurationDeviceModel::setChargepointMeterInformation(
    const std::optional<std::string>& meterSerialNumber, const std::optional<std::string>& meterType) {
    set_value(*storage, keys::valid_keys::MeterSerialNumber, meterSerialNumber);
    set_value(*storage, keys::valid_keys::MeterType, meterType);
}

void ChargePointConfigurationDeviceModel::setChargepointModemInformation(const std::optional<std::string>& ICCID,
                                                                         const std::optional<std::string>& IMSI) {
    set_value(*storage, keys::valid_keys::ICCID, ICCID);
    set_value(*storage, keys::valid_keys::IMSI, IMSI);
}

// ----------------------------------------------------------------------------
// Internal

std::string ChargePointConfigurationDeviceModel::getCentralSystemURI() {
    return get_value<std::string>(*storage, keys::valid_keys::CentralSystemURI);
}

std::string ChargePointConfigurationDeviceModel::getChargePointId() {
    return get_value<std::string>(*storage, keys::valid_keys::ChargePointId);
}

std::string ChargePointConfigurationDeviceModel::getSupportedCiphers12() {
    return get_value<std::string>(*storage, keys::valid_keys::SupportedCiphers12);
}

std::string ChargePointConfigurationDeviceModel::getSupportedCiphers13() {
    return get_value<std::string>(*storage, keys::valid_keys::SupportedCiphers13);
}

std::string ChargePointConfigurationDeviceModel::getSupportedMeasurands() {
    return get_value<std::string>(*storage, keys::valid_keys::SupportedMeasurands);
}

std::string ChargePointConfigurationDeviceModel::getTLSKeylogFile() {
    return get_value<std::string>(*storage, keys::valid_keys::TLSKeylogFile);
}

std::string ChargePointConfigurationDeviceModel::getWebsocketPingPayload() {
    return get_value<std::string>(*storage, keys::valid_keys::WebsocketPingPayload);
}

CiString<20> ChargePointConfigurationDeviceModel::getChargePointModel() {
    return get_value<std::string>(*storage, keys::valid_keys::ChargePointModel);
}

CiString<20> ChargePointConfigurationDeviceModel::getChargePointVendor() {
    return get_value<std::string>(*storage, keys::valid_keys::ChargePointVendor);
}

bool ChargePointConfigurationDeviceModel::getAuthorizeConnectorZeroOnConnectorOne() {
    bool result{false};
    if (getNumberOfConnectors() == 1) {
        get_value(result, *storage, keys::valid_keys::AuthorizeConnectorZeroOnConnectorOne);
    }
    return result;
}

bool ChargePointConfigurationDeviceModel::getEnableTLSKeylog() {
    return get_value<bool>(*storage, keys::valid_keys::EnableTLSKeylog);
}

bool ChargePointConfigurationDeviceModel::getLogMessages() {
    return get_value<bool>(*storage, keys::valid_keys::LogMessages);
}

bool ChargePointConfigurationDeviceModel::getLogMessagesRaw() {
    return get_value<bool>(*storage, keys::valid_keys::LogMessagesRaw);
}

bool ChargePointConfigurationDeviceModel::getLogRotation() {
    return get_value<bool>(*storage, keys::valid_keys::LogRotation);
}

bool ChargePointConfigurationDeviceModel::getLogRotationDateSuffix() {
    return get_value<bool>(*storage, keys::valid_keys::LogRotationDateSuffix);
}

bool ChargePointConfigurationDeviceModel::getStopTransactionIfUnlockNotSupported() {
    return get_value<bool>(*storage, keys::valid_keys::StopTransactionIfUnlockNotSupported);
}

bool ChargePointConfigurationDeviceModel::getUseSslDefaultVerifyPaths() {
    return get_value<bool>(*storage, keys::valid_keys::UseSslDefaultVerifyPaths);
}

bool ChargePointConfigurationDeviceModel::getUseTPM() {
    return get_value<bool>(*storage, keys::valid_keys::UseTPM);
}

bool ChargePointConfigurationDeviceModel::getUseTPMSeccLeafCertificate() {
    return get_value<bool>(*storage, keys::valid_keys::UseTPMSeccLeafCertificate);
}

bool ChargePointConfigurationDeviceModel::getVerifyCsmsAllowWildcards() {
    return get_value<bool>(*storage, keys::valid_keys::VerifyCsmsAllowWildcards);
}

bool ChargePointConfigurationDeviceModel::getVerifyCsmsCommonName() {
    return get_value<bool>(*storage, keys::valid_keys::VerifyCsmsCommonName);
}

int ChargePointConfigurationDeviceModel::getMaxMessageSize() {
    return get_value<int>(*storage, keys::valid_keys::MaxMessageSize);
}

std::int32_t ChargePointConfigurationDeviceModel::getMaxCompositeScheduleDuration() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::MaxCompositeScheduleDuration);
}

std::int32_t ChargePointConfigurationDeviceModel::getOcspRequestInterval() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::OcspRequestInterval);
}

std::int32_t ChargePointConfigurationDeviceModel::getRetryBackoffRandomRange() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::RetryBackoffRandomRange);
}

std::int32_t ChargePointConfigurationDeviceModel::getRetryBackoffRepeatTimes() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::RetryBackoffRepeatTimes);
}

std::int32_t ChargePointConfigurationDeviceModel::getRetryBackoffWaitMinimum() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::RetryBackoffWaitMinimum);
}

std::int32_t ChargePointConfigurationDeviceModel::getWaitForStopTransactionsOnResetTimeout() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::WaitForStopTransactionsOnResetTimeout);
}

std::int32_t ChargePointConfigurationDeviceModel::getWebsocketPongTimeout() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::WebsocketPongTimeout);
}

std::uint64_t ChargePointConfigurationDeviceModel::getLogRotationMaximumFileCount() {
    return get_value<std::uint64_t>(*storage, keys::valid_keys::LogRotationMaximumFileCount);
}

std::uint64_t ChargePointConfigurationDeviceModel::getLogRotationMaximumFileSize() {
    return get_value<std::uint64_t>(*storage, keys::valid_keys::LogRotationMaximumFileSize);
}

std::vector<std::string> ChargePointConfigurationDeviceModel::getLogMessagesFormat() {
    std::string csl;
    get_value(csl, *storage, keys::valid_keys::LogMessagesFormat);
    return utils::from_csl(csl);
}

std::vector<ChargingProfilePurposeType> ChargePointConfigurationDeviceModel::getIgnoredProfilePurposesOffline() {
    std::optional<std::string> get_result;
    get_value(get_result, *storage, keys::valid_keys::IgnoredProfilePurposesOffline);
    std::vector<ChargingProfilePurposeType> purpose_types;

    if (get_result) {
        const auto purposes = utils::from_csl(get_result.value());

        for (const auto& purpose : purposes) {
            try {
                purpose_types.push_back(conversions::string_to_charging_profile_purpose_type(purpose));
            } catch (const StringToEnumException& e) {
                EVLOG_warning << "Could not convert element of IgnoredProfilePurposesOffline: " << purpose;
            }
        }
    }

    return purpose_types;
}

std::vector<ChargingProfilePurposeType> ChargePointConfigurationDeviceModel::getSupportedChargingProfilePurposeTypes() {
    std::string get_result;
    get_value(get_result, *storage, keys::valid_keys::SupportedChargingProfilePurposeTypes);
    const auto purposes = utils::from_csl(get_result);

    std::vector<ChargingProfilePurposeType> supported_purpose_types;
    for (const auto& purpose : purposes) {
        try {
            supported_purpose_types.push_back(conversions::string_to_charging_profile_purpose_type(purpose));
        } catch (const StringToEnumException& e) {
            EVLOG_warning << "Could not convert element of SupportedChargingProfilePurposeTypes: " << purpose;
        }
    }
    return supported_purpose_types;
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getConnectorEvseIds() {
    return get_optional<std::string>(*storage, keys::valid_keys::ConnectorEvseIds);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getHostName() {
    return get_optional<std::string>(*storage, keys::valid_keys::HostName);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getIFace() {
    return get_optional<std::string>(*storage, keys::valid_keys::IFace);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getMessageTypesDiscardForQueueing() {
    return get_optional<std::string>(*storage, keys::valid_keys::MessageTypesDiscardForQueueing);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getSeccLeafSubjectCommonName() {
    return get_optional<std::string>(*storage, keys::valid_keys::SeccLeafSubjectCommonName);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getSeccLeafSubjectCountry() {
    return get_optional<std::string>(*storage, keys::valid_keys::SeccLeafSubjectCountry);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getSeccLeafSubjectOrganization() {
    return get_optional<std::string>(*storage, keys::valid_keys::SeccLeafSubjectOrganization);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getAllowChargingProfileWithoutStartSchedule() {
    return get_optional<bool>(*storage, keys::valid_keys::AllowChargingProfileWithoutStartSchedule);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getAllowOfflineTxForUnknownId() {
    return get_optional<bool>(*storage, keys::valid_keys::AllowOfflineTxForUnknownId);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getQueueAllMessages() {
    return get_optional<bool>(*storage, keys::valid_keys::QueueAllMessages);
}

std::optional<int> ChargePointConfigurationDeviceModel::getMessageQueueSizeThreshold() {
    return get_optional<int>(*storage, keys::valid_keys::MessageQueueSizeThreshold);
}

std::optional<int32_t> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultLimitAmps() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CompositeScheduleDefaultLimitAmps);
}

std::optional<int32_t> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultLimitWatts() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CompositeScheduleDefaultLimitWatts);
}

std::optional<int32_t> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultNumberPhases() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CompositeScheduleDefaultNumberPhases);
}

std::optional<int32_t> ChargePointConfigurationDeviceModel::getSupplyVoltage() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::SupplyVoltage);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getPublicKeyKeyValue(const std::uint32_t connector_id) {
    std::optional<KeyValue> result;
    const auto max = getNumberOfConnectors();
    if ((connector_id > max) || (connector_id < 1)) {
        EVLOG_warning << "Cannot get MeterPublicKey for connector " << connector_id
                      << ", because the connector id does not exist.";
    } else {
        auto key = meterPublicKeyString(connector_id);
        auto value = get_optional<std::string>(*storage, "Internal", key);
        if (value) {
            KeyValue kv;
            kv.key = std::move(key);
            kv.readonly = true;
            kv.value = std::move(value);
            result = std::move(kv);
        }
    }
    return result;
}

KeyValue ChargePointConfigurationDeviceModel::getAuthorizeConnectorZeroOnConnectorOneKeyValue() {
    return get_key_value(*storage, keys::valid_keys::AuthorizeConnectorZeroOnConnectorOne);
}

KeyValue ChargePointConfigurationDeviceModel::getCentralSystemURIKeyValue() {
    auto kv = get_key_value(*storage, keys::valid_keys::CentralSystemURI);
    // this may be changeable so check the device model rather than known_keys
    kv.readonly = isReadOnly(*storage, "Internal", "CentralSystemURI").value_or(true);
    return kv;
}

KeyValue ChargePointConfigurationDeviceModel::getChargePointIdKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargePointId);
}

KeyValue ChargePointConfigurationDeviceModel::getChargePointModelKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargePointModel);
}

KeyValue ChargePointConfigurationDeviceModel::getChargePointVendorKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargePointVendor);
}

KeyValue ChargePointConfigurationDeviceModel::getEnableTLSKeylogKeyValue() {
    return get_key_value(*storage, keys::valid_keys::EnableTLSKeylog);
}

KeyValue ChargePointConfigurationDeviceModel::getLogMessagesFormatKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogMessagesFormat);
}

KeyValue ChargePointConfigurationDeviceModel::getLogMessagesKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogMessages);
}

KeyValue ChargePointConfigurationDeviceModel::getLogMessagesRawKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogMessagesRaw);
}

KeyValue ChargePointConfigurationDeviceModel::getLogRotationDateSuffixKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogRotationDateSuffix);
}

KeyValue ChargePointConfigurationDeviceModel::getLogRotationKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogRotation);
}

KeyValue ChargePointConfigurationDeviceModel::getLogRotationMaximumFileCountKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogRotationMaximumFileCount);
}

KeyValue ChargePointConfigurationDeviceModel::getLogRotationMaximumFileSizeKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LogRotationMaximumFileSize);
}

KeyValue ChargePointConfigurationDeviceModel::getMaxCompositeScheduleDurationKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MaxCompositeScheduleDuration);
}

KeyValue ChargePointConfigurationDeviceModel::getMaxMessageSizeKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MaxMessageSize);
}

KeyValue ChargePointConfigurationDeviceModel::getOcspRequestIntervalKeyValue() {
    return get_key_value(*storage, keys::valid_keys::OcspRequestInterval);
}

KeyValue ChargePointConfigurationDeviceModel::getRetryBackoffRandomRangeKeyValue() {
    return get_key_value(*storage, keys::valid_keys::RetryBackoffRandomRange);
}

KeyValue ChargePointConfigurationDeviceModel::getRetryBackoffRepeatTimesKeyValue() {
    return get_key_value(*storage, keys::valid_keys::RetryBackoffRepeatTimes);
}

KeyValue ChargePointConfigurationDeviceModel::getRetryBackoffWaitMinimumKeyValue() {
    return get_key_value(*storage, keys::valid_keys::RetryBackoffWaitMinimum);
}

KeyValue ChargePointConfigurationDeviceModel::getStopTransactionIfUnlockNotSupportedKeyValue() {
    return get_key_value(*storage, keys::valid_keys::StopTransactionIfUnlockNotSupported);
}

KeyValue ChargePointConfigurationDeviceModel::getSupportedChargingProfilePurposeTypesKeyValue() {
    return get_key_value(*storage, keys::valid_keys::SupportedChargingProfilePurposeTypes);
}

KeyValue ChargePointConfigurationDeviceModel::getSupportedCiphers12KeyValue() {
    return get_key_value(*storage, keys::valid_keys::SupportedCiphers12);
}

KeyValue ChargePointConfigurationDeviceModel::getSupportedCiphers13KeyValue() {
    return get_key_value(*storage, keys::valid_keys::SupportedCiphers13);
}

KeyValue ChargePointConfigurationDeviceModel::getSupportedMeasurandsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::SupportedMeasurands);
}

KeyValue ChargePointConfigurationDeviceModel::getTLSKeylogFileKeyValue() {
    return get_key_value(*storage, keys::valid_keys::TLSKeylogFile);
}

KeyValue ChargePointConfigurationDeviceModel::getUseSslDefaultVerifyPathsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::UseSslDefaultVerifyPaths);
}

KeyValue ChargePointConfigurationDeviceModel::getUseTPMKeyValue() {
    return get_key_value(*storage, keys::valid_keys::UseTPM);
}

KeyValue ChargePointConfigurationDeviceModel::getUseTPMSeccLeafCertificateKeyValue() {
    return get_key_value(*storage, keys::valid_keys::UseTPMSeccLeafCertificate);
}

KeyValue ChargePointConfigurationDeviceModel::getVerifyCsmsAllowWildcardsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::VerifyCsmsAllowWildcards);
}

KeyValue ChargePointConfigurationDeviceModel::getVerifyCsmsCommonNameKeyValue() {
    return get_key_value(*storage, keys::valid_keys::VerifyCsmsCommonName);
}

KeyValue ChargePointConfigurationDeviceModel::getWaitForStopTransactionsOnResetTimeoutKeyValue() {
    return get_key_value(*storage, keys::valid_keys::WaitForStopTransactionsOnResetTimeout);
}

KeyValue ChargePointConfigurationDeviceModel::getWebsocketPingPayloadKeyValue() {
    return get_key_value(*storage, keys::valid_keys::WebsocketPingPayload);
}

KeyValue ChargePointConfigurationDeviceModel::getWebsocketPongTimeoutKeyValue() {
    return get_key_value(*storage, keys::valid_keys::WebsocketPongTimeout);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getAllowChargingProfileWithoutStartScheduleKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::AllowChargingProfileWithoutStartSchedule);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getAllowOfflineTxForUnknownIdKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::AllowOfflineTxForUnknownId);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultLimitAmpsKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CompositeScheduleDefaultLimitAmps);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultLimitWattsKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CompositeScheduleDefaultLimitWatts);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCompositeScheduleDefaultNumberPhasesKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CompositeScheduleDefaultNumberPhases);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getConnectorEvseIdsKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ConnectorEvseIds);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getHostNameKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::HostName);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getIFaceKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::IFace);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getIgnoredProfilePurposesOfflineKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::IgnoredProfilePurposesOffline);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMessageTypesDiscardForQueueingKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MessageTypesDiscardForQueueing);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMessageQueueSizeThresholdKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MessageQueueSizeThreshold);
}

std::optional<std::vector<KeyValue>> ChargePointConfigurationDeviceModel::getAllMeterPublicKeyKeyValues() {
    std::optional<std::vector<KeyValue>> result;
    std::vector<KeyValue> list;

    const auto max = getNumberOfConnectors();

    for (std::uint32_t i = 0; i <= max; i++) {
        auto res = getPublicKeyKeyValue(i);
        if (res) {
            list.push_back(std::move(res.value()));
        }
    }

    if (!list.empty()) {
        result = std::move(list);
    }

    return result;
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getQueueAllMessagesKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::QueueAllMessages);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSeccLeafSubjectCommonNameKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SeccLeafSubjectCommonName);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSeccLeafSubjectCountryKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SeccLeafSubjectCountry);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSeccLeafSubjectOrganizationKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SeccLeafSubjectOrganization);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSupplyVoltageKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SupplyVoltage);
}

void ChargePointConfigurationDeviceModel::setAllowChargingProfileWithoutStartSchedule(bool allow) {
    setInternalAllowChargingProfileWithoutStartSchedule(to_string(allow));
}

void ChargePointConfigurationDeviceModel::setAllowOfflineTxForUnknownId(bool enabled) {
    setInternalAllowOfflineTxForUnknownId(to_string(enabled));
}

void ChargePointConfigurationDeviceModel::setCentralSystemURI(const std::string& centralSystemUri) {
    setInternalCentralSystemURI(centralSystemUri);
}

void ChargePointConfigurationDeviceModel::setCompositeScheduleDefaultLimitAmps(std::int32_t limit_amps) {
    setInternalCompositeScheduleDefaultLimitAmps(std::to_string(limit_amps));
}

void ChargePointConfigurationDeviceModel::setCompositeScheduleDefaultLimitWatts(std::int32_t limit_watts) {
    setInternalCompositeScheduleDefaultLimitWatts(std::to_string(limit_watts));
}

void ChargePointConfigurationDeviceModel::setCompositeScheduleDefaultNumberPhases(std::int32_t number_phases) {
    setInternalCompositeScheduleDefaultNumberPhases(std::to_string(number_phases));
}

void ChargePointConfigurationDeviceModel::setConnectorEvseIds(const std::string& connector_evse_ids) {
    setInternalConnectorEvseIds(connector_evse_ids);
}

bool ChargePointConfigurationDeviceModel::setIgnoredProfilePurposesOffline(
    const std::string& ignored_profile_purposes_offline) {
    return setInternalIgnoredProfilePurposesOffline(ignored_profile_purposes_offline) == SetResult::Accepted;
}

bool ChargePointConfigurationDeviceModel::setMeterPublicKey(const std::int32_t connector_id,
                                                            const std::string& public_key_pem) {
    bool result{false};
    if (connector_id > getNumberOfConnectors() || connector_id < 1) {
        EVLOG_warning << "Cannot set MeterPublicKey for connector " << connector_id
                      << ", because the connector id does not exist.";
    } else {
        const auto key = meterPublicKeyString(connector_id);
        const auto res = set_value(*storage, "Internal", key, public_key_pem);
        result = res == SetResult::Accepted;
    }
    return result;
}

void ChargePointConfigurationDeviceModel::setOcspRequestInterval(const std::int32_t ocsp_request_interval) {
    setInternalOcspRequestInterval(std::to_string(ocsp_request_interval));
}

void ChargePointConfigurationDeviceModel::setRetryBackoffRandomRange(std::int32_t retry_backoff_random_range) {
    setInternalRetryBackoffRandomRange(std::to_string(retry_backoff_random_range));
}

void ChargePointConfigurationDeviceModel::setRetryBackoffRepeatTimes(std::int32_t retry_backoff_repeat_times) {
    setInternalRetryBackoffRepeatTimes(std::to_string(retry_backoff_repeat_times));
}

void ChargePointConfigurationDeviceModel::setRetryBackoffWaitMinimum(std::int32_t retry_backoff_wait_minimum) {
    setInternalRetryBackoffWaitMinimum(std::to_string(retry_backoff_wait_minimum));
}

void ChargePointConfigurationDeviceModel::setSeccLeafSubjectCommonName(
    const std::string& secc_leaf_subject_common_name) {
    setInternalSeccLeafSubjectCommonName(secc_leaf_subject_common_name);
}

void ChargePointConfigurationDeviceModel::setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country) {
    setInternalSeccLeafSubjectCountry(secc_leaf_subject_country);
}

void ChargePointConfigurationDeviceModel::setSeccLeafSubjectOrganization(
    const std::string& secc_leaf_subject_organization) {
    setInternalSeccLeafSubjectOrganization(secc_leaf_subject_organization);
}

void ChargePointConfigurationDeviceModel::setStopTransactionIfUnlockNotSupported(
    bool stop_transaction_if_unlock_not_supported) {
    setInternalStopTransactionIfUnlockNotSupported(to_string(stop_transaction_if_unlock_not_supported));
}

void ChargePointConfigurationDeviceModel::setSupplyVoltage(std::int32_t supply_voltage) {
    setInternalSupplyVoltage(std::to_string(supply_voltage));
}

void ChargePointConfigurationDeviceModel::setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards) {
    setInternalVerifyCsmsAllowWildcards(to_string(verify_csms_allow_wildcards));
}

void ChargePointConfigurationDeviceModel::setWaitForStopTransactionsOnResetTimeout(
    const int32_t wait_for_stop_transactions_on_reset_timeout) {
    setInternalWaitForStopTransactionsOnResetTimeout(std::to_string(wait_for_stop_transactions_on_reset_timeout));
}

// ----------------------------------------------------------------------------
// Core

std::string ChargePointConfigurationDeviceModel::getConnectorPhaseRotation() {
    return get_value<std::string>(*storage, keys::valid_keys::ConnectorPhaseRotation);
}

std::string ChargePointConfigurationDeviceModel::getMeterValuesAlignedData() {
    return get_value<std::string>(*storage, keys::valid_keys::MeterValuesAlignedData);
}

std::string ChargePointConfigurationDeviceModel::getMeterValuesSampledData() {
    return get_value<std::string>(*storage, keys::valid_keys::MeterValuesSampledData);
}

std::string ChargePointConfigurationDeviceModel::getStopTxnAlignedData() {
    return get_value<std::string>(*storage, keys::valid_keys::StopTxnAlignedData);
}

std::string ChargePointConfigurationDeviceModel::getStopTxnSampledData() {
    return get_value<std::string>(*storage, keys::valid_keys::StopTxnSampledData);
}

std::string ChargePointConfigurationDeviceModel::getSupportedFeatureProfiles() {
    return get_value<std::string>(*storage, keys::valid_keys::SupportedFeatureProfiles);
}

bool ChargePointConfigurationDeviceModel::getAuthorizeRemoteTxRequests() {
    return get_value<bool>(*storage, keys::valid_keys::AuthorizeRemoteTxRequests);
}

bool ChargePointConfigurationDeviceModel::getLocalAuthorizeOffline() {
    return get_value<bool>(*storage, keys::valid_keys::LocalAuthorizeOffline);
}

bool ChargePointConfigurationDeviceModel::getLocalPreAuthorize() {
    return get_value<bool>(*storage, keys::valid_keys::LocalPreAuthorize);
}

bool ChargePointConfigurationDeviceModel::getStopTransactionOnInvalidId() {
    return get_value<bool>(*storage, keys::valid_keys::StopTransactionOnInvalidId);
}

bool ChargePointConfigurationDeviceModel::getUnlockConnectorOnEVSideDisconnect() {
    return get_value<bool>(*storage, keys::valid_keys::UnlockConnectorOnEVSideDisconnect);
}

std::int32_t ChargePointConfigurationDeviceModel::getClockAlignedDataInterval() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::ClockAlignedDataInterval);
}

std::int32_t ChargePointConfigurationDeviceModel::getConnectionTimeOut() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::ConnectionTimeOut);
}

std::int32_t ChargePointConfigurationDeviceModel::getGetConfigurationMaxKeys() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::GetConfigurationMaxKeys);
}

std::int32_t ChargePointConfigurationDeviceModel::getHeartbeatInterval() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::HeartbeatInterval);
}

std::int32_t ChargePointConfigurationDeviceModel::getMeterValueSampleInterval() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::MeterValueSampleInterval);
}

std::int32_t ChargePointConfigurationDeviceModel::getNumberOfConnectors() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::NumberOfConnectors);
}

std::int32_t ChargePointConfigurationDeviceModel::getResetRetries() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::ResetRetries);
}

std::int32_t ChargePointConfigurationDeviceModel::getTransactionMessageAttempts() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::TransactionMessageAttempts);
}

std::int32_t ChargePointConfigurationDeviceModel::getTransactionMessageRetryInterval() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::TransactionMessageRetryInterval);
}

std::vector<MeasurandWithPhase> ChargePointConfigurationDeviceModel::getMeterValuesAlignedDataVector() {
    std::vector<MeasurandWithPhase> result;
    auto vec = csvToMeasurandWithPhaseVector(getMeterValuesAlignedData());
    if (vec) {
        result = std::move(vec.value());
    }
    return result;
}

std::vector<MeasurandWithPhase> ChargePointConfigurationDeviceModel::getMeterValuesSampledDataVector() {
    std::vector<MeasurandWithPhase> result;
    auto vec = csvToMeasurandWithPhaseVector(getMeterValuesSampledData());
    if (vec) {
        result = std::move(vec.value());
    }
    return result;
}

std::optional<bool> ChargePointConfigurationDeviceModel::getAuthorizationCacheEnabled() {
    return get_optional<bool>(*storage, keys::valid_keys::AuthorizationCacheEnabled);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getReserveConnectorZeroSupported() {
    return get_optional<bool>(*storage, keys::valid_keys::ReserveConnectorZeroSupported);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getStopTransactionOnEVSideDisconnect() {
    return get_optional<bool>(*storage, keys::valid_keys::StopTransactionOnEVSideDisconnect);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getBlinkRepeat() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::BlinkRepeat);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getConnectorPhaseRotationMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::ConnectorPhaseRotationMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getLightIntensity() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::LightIntensity);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getMaxEnergyOnInvalidId() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::MaxEnergyOnInvalidId);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getMeterValuesAlignedDataMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::MeterValuesAlignedDataMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getMeterValuesSampledDataMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::MeterValuesSampledDataMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getMinimumStatusDuration() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::MinimumStatusDuration);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getStopTxnAlignedDataMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::StopTxnAlignedDataMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getStopTxnSampledDataMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::StopTxnSampledDataMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getSupportedFeatureProfilesMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::SupportedFeatureProfilesMaxLength);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getWebsocketPingInterval() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::WebSocketPingInterval);
}

std::set<SupportedFeatureProfiles> ChargePointConfigurationDeviceModel::getSupportedFeatureProfilesSet() {
    return supported_feature_profiles;
}

KeyValue ChargePointConfigurationDeviceModel::getAuthorizeRemoteTxRequestsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::AuthorizeRemoteTxRequests);
}

KeyValue ChargePointConfigurationDeviceModel::getClockAlignedDataIntervalKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ClockAlignedDataInterval);
}

KeyValue ChargePointConfigurationDeviceModel::getConnectionTimeOutKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ConnectionTimeOut);
}

KeyValue ChargePointConfigurationDeviceModel::getConnectorPhaseRotationKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ConnectorPhaseRotation);
}

KeyValue ChargePointConfigurationDeviceModel::getGetConfigurationMaxKeysKeyValue() {
    return get_key_value(*storage, keys::valid_keys::GetConfigurationMaxKeys);
}

KeyValue ChargePointConfigurationDeviceModel::getHeartbeatIntervalKeyValue() {
    return get_key_value(*storage, keys::valid_keys::HeartbeatInterval);
}

KeyValue ChargePointConfigurationDeviceModel::getLocalAuthorizeOfflineKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LocalAuthorizeOffline);
}

KeyValue ChargePointConfigurationDeviceModel::getLocalPreAuthorizeKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LocalPreAuthorize);
}

KeyValue ChargePointConfigurationDeviceModel::getMeterValuesAlignedDataKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MeterValuesAlignedData);
}

KeyValue ChargePointConfigurationDeviceModel::getMeterValueSampleIntervalKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MeterValueSampleInterval);
}

KeyValue ChargePointConfigurationDeviceModel::getMeterValuesSampledDataKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MeterValuesSampledData);
}

KeyValue ChargePointConfigurationDeviceModel::getNumberOfConnectorsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::NumberOfConnectors);
}

KeyValue ChargePointConfigurationDeviceModel::getResetRetriesKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ResetRetries);
}

KeyValue ChargePointConfigurationDeviceModel::getStopTransactionOnInvalidIdKeyValue() {
    return get_key_value(*storage, keys::valid_keys::StopTransactionOnInvalidId);
}

KeyValue ChargePointConfigurationDeviceModel::getStopTxnAlignedDataKeyValue() {
    return get_key_value(*storage, keys::valid_keys::StopTxnAlignedData);
}

KeyValue ChargePointConfigurationDeviceModel::getStopTxnSampledDataKeyValue() {
    return get_key_value(*storage, keys::valid_keys::StopTxnSampledData);
}

KeyValue ChargePointConfigurationDeviceModel::getSupportedFeatureProfilesKeyValue() {
    return get_key_value(*storage, keys::valid_keys::SupportedFeatureProfiles);
}

KeyValue ChargePointConfigurationDeviceModel::getTransactionMessageAttemptsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::TransactionMessageAttempts);
}

KeyValue ChargePointConfigurationDeviceModel::getTransactionMessageRetryIntervalKeyValue() {
    return get_key_value(*storage, keys::valid_keys::TransactionMessageRetryInterval);
}

KeyValue ChargePointConfigurationDeviceModel::getUnlockConnectorOnEVSideDisconnectKeyValue() {
    return get_key_value(*storage, keys::valid_keys::UnlockConnectorOnEVSideDisconnect);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getAuthorizationCacheEnabledKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::AuthorizationCacheEnabled);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getBlinkRepeatKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::BlinkRepeat);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getConnectorPhaseRotationMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ConnectorPhaseRotationMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getLightIntensityKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::LightIntensity);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMaxEnergyOnInvalidIdKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MaxEnergyOnInvalidId);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMeterValuesAlignedDataMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MeterValuesAlignedDataMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMeterValuesSampledDataMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MeterValuesSampledDataMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMinimumStatusDurationKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::MinimumStatusDuration);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getReserveConnectorZeroSupportedKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ReserveConnectorZeroSupported);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getStopTransactionOnEVSideDisconnectKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::StopTransactionOnEVSideDisconnect);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getStopTxnAlignedDataMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::StopTxnAlignedDataMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getStopTxnSampledDataMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::StopTxnSampledDataMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSupportedFeatureProfilesMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SupportedFeatureProfilesMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getWebsocketPingIntervalKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::WebSocketPingInterval);
}

void ChargePointConfigurationDeviceModel::setAuthorizationCacheEnabled(bool enabled) {
    setInternalAuthorizationCacheEnabled(to_string(enabled));
}

void ChargePointConfigurationDeviceModel::setAuthorizeRemoteTxRequests(bool enabled) {
    setInternalAuthorizeRemoteTxRequests(to_string(enabled));
}

void ChargePointConfigurationDeviceModel::setBlinkRepeat(std::int32_t blink_repeat) {
    setInternalBlinkRepeat(std::to_string(blink_repeat));
}

void ChargePointConfigurationDeviceModel::setClockAlignedDataInterval(std::int32_t interval) {
    setInternalClockAlignedDataInterval(std::to_string(interval));
}

void ChargePointConfigurationDeviceModel::setConnectionTimeOut(std::int32_t timeout) {
    setInternalConnectionTimeOut(std::to_string(timeout));
}

void ChargePointConfigurationDeviceModel::setConnectorPhaseRotation(const std::string& connector_phase_rotation) {
    setInternalConnectorPhaseRotation(connector_phase_rotation);
}

void ChargePointConfigurationDeviceModel::setHeartbeatInterval(std::int32_t interval) {
    setInternalHeartbeatInterval(std::to_string(interval));
}

void ChargePointConfigurationDeviceModel::setLightIntensity(std::int32_t light_intensity) {
    setInternalLightIntensity(std::to_string(light_intensity));
}

void ChargePointConfigurationDeviceModel::setLocalAuthorizeOffline(bool local_authorize_offline) {
    setInternalLocalAuthorizeOffline(to_string(local_authorize_offline));
}

void ChargePointConfigurationDeviceModel::setLocalPreAuthorize(bool local_pre_authorize) {
    setInternalLocalPreAuthorize(to_string(local_pre_authorize));
}

void ChargePointConfigurationDeviceModel::setMaxEnergyOnInvalidId(std::int32_t max_energy) {
    setInternalMaxEnergyOnInvalidId(std::to_string(max_energy));
}

bool ChargePointConfigurationDeviceModel::setMeterValuesAlignedData(const std::string& meter_values_aligned_data) {
    return setInternalMeterValuesAlignedData(meter_values_aligned_data) == SetResult::Accepted;
}

bool ChargePointConfigurationDeviceModel::setMeterValuesSampledData(const std::string& meter_values_sampled_data) {
    return setInternalMeterValuesSampledData(meter_values_sampled_data) == SetResult::Accepted;
}

void ChargePointConfigurationDeviceModel::setMeterValueSampleInterval(std::int32_t interval) {
    setInternalMeterValueSampleInterval(std::to_string(interval));
}

void ChargePointConfigurationDeviceModel::setMinimumStatusDuration(std::int32_t minimum_status_duration) {
    setInternalMinimumStatusDuration(std::to_string(minimum_status_duration));
}

void ChargePointConfigurationDeviceModel::setResetRetries(std::int32_t retries) {
    setInternalResetRetries(std::to_string(retries));
}

void ChargePointConfigurationDeviceModel::setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) {
    setInternalStopTransactionOnInvalidId(to_string(stop_transaction_on_invalid_id));
}

bool ChargePointConfigurationDeviceModel::setStopTxnAlignedData(const std::string& stop_txn_aligned_data) {
    return setInternalStopTxnAlignedData(stop_txn_aligned_data) == SetResult::Accepted;
}

bool ChargePointConfigurationDeviceModel::setStopTxnSampledData(const std::string& stop_txn_sampled_data) {
    return setInternalStopTxnSampledData(stop_txn_sampled_data) == SetResult::Accepted;
}

void ChargePointConfigurationDeviceModel::setTransactionMessageAttempts(std::int32_t attempts) {
    setInternalTransactionMessageAttempts(std::to_string(attempts));
}

void ChargePointConfigurationDeviceModel::setTransactionMessageRetryInterval(std::int32_t retry_interval) {
    setInternalTransactionMessageRetryInterval(std::to_string(retry_interval));
}

void ChargePointConfigurationDeviceModel::setUnlockConnectorOnEVSideDisconnect(
    bool unlock_connector_on_ev_side_disconnect) {
    setInternalUnlockConnectorOnEVSideDisconnect(to_string(unlock_connector_on_ev_side_disconnect));
}

void ChargePointConfigurationDeviceModel::setWebsocketPingInterval(std::int32_t websocket_ping_interval) {
    setInternalWebsocketPingInterval(std::to_string(websocket_ping_interval));
}

// ----------------------------------------------------------------------------
// Firmware Management

std::optional<std::string> ChargePointConfigurationDeviceModel::getSupportedFileTransferProtocols() {
    return get_optional<std::string>(*storage, keys::valid_keys::SupportedFileTransferProtocols);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getSupportedFileTransferProtocolsKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SupportedFileTransferProtocols);
}

// ----------------------------------------------------------------------------
// Smart Charging

std::string ChargePointConfigurationDeviceModel::getChargingScheduleAllowedChargingRateUnit() {
    return get_value<std::string>(*storage, keys::valid_keys::ChargingScheduleAllowedChargingRateUnit);
}

std::int32_t ChargePointConfigurationDeviceModel::getChargeProfileMaxStackLevel() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::ChargeProfileMaxStackLevel);
}

std::int32_t ChargePointConfigurationDeviceModel::getChargingScheduleMaxPeriods() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::ChargingScheduleMaxPeriods);
}

std::int32_t ChargePointConfigurationDeviceModel::getMaxChargingProfilesInstalled() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::MaxChargingProfilesInstalled);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getConnectorSwitch3to1PhaseSupported() {
    return get_optional<bool>(*storage, keys::valid_keys::ConnectorSwitch3to1PhaseSupported);
}

std::vector<ChargingRateUnit> ChargePointConfigurationDeviceModel::getChargingScheduleAllowedChargingRateUnitVector() {
    std::vector<ChargingRateUnit> result;
    const auto csl = get_optional<std::string>(*storage, keys::valid_keys::ChargingScheduleAllowedChargingRateUnit);
    if (csl) {
        const auto components = utils::from_csl(csl.value());
        for (const auto& component : components) {
            if (component == "Current") {
                result.push_back(ChargingRateUnit::A);
            } else if (component == "Power") {
                result.push_back(ChargingRateUnit::W);
            }
        }
    }
    return result;
}

KeyValue ChargePointConfigurationDeviceModel::getChargeProfileMaxStackLevelKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargeProfileMaxStackLevel);
}

KeyValue ChargePointConfigurationDeviceModel::getChargingScheduleAllowedChargingRateUnitKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargingScheduleAllowedChargingRateUnit);
}

KeyValue ChargePointConfigurationDeviceModel::getChargingScheduleMaxPeriodsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ChargingScheduleMaxPeriods);
}

KeyValue ChargePointConfigurationDeviceModel::getMaxChargingProfilesInstalledKeyValue() {
    return get_key_value(*storage, keys::valid_keys::MaxChargingProfilesInstalled);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getConnectorSwitch3to1PhaseSupportedKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::ConnectorSwitch3to1PhaseSupported);
}

// ----------------------------------------------------------------------------
// Security

bool ChargePointConfigurationDeviceModel::getDisableSecurityEventNotifications() {
    return get_value<bool>(*storage, keys::valid_keys::DisableSecurityEventNotifications);
}

std::int32_t ChargePointConfigurationDeviceModel::getSecurityProfile() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::SecurityProfile);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getAuthorizationKey() {
    return get_optional<std::string>(*storage, keys::valid_keys::AuthorizationKey);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getCpoName() {
    return get_optional<std::string>(*storage, keys::valid_keys::CpoName);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getAdditionalRootCertificateCheck() {
    return get_optional<bool>(*storage, keys::valid_keys::AdditionalRootCertificateCheck);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getCertificateSignedMaxChainSize() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CertificateSignedMaxChainSize);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getCertificateStoreMaxLength() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CertificateStoreMaxLength);
}

KeyValue ChargePointConfigurationDeviceModel::getDisableSecurityEventNotificationsKeyValue() {
    return get_key_value(*storage, keys::valid_keys::DisableSecurityEventNotifications);
}

KeyValue ChargePointConfigurationDeviceModel::getSecurityProfileKeyValue() {
    return get_key_value(*storage, keys::valid_keys::SecurityProfile);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getAdditionalRootCertificateCheckKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::AdditionalRootCertificateCheck);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getAuthorizationKeyKeyValue() {
    // AuthorizationKey is writeOnly so we return a dummy when a value is set
    // a KeyValue is always returned
    const auto key = keys::valid_keys::AuthorizationKey;
    v16::KeyValue kv;
    kv.key = std::move(std::string{v16::keys::convert(key)});
    kv.readonly = v16::keys::is_readonly(key);
    if (key_exists(*storage, key)) {
        kv.value = "DummyAuthorizationKey";
    }
    return kv;
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCertificateSignedMaxChainSizeKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CertificateSignedMaxChainSize);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCertificateStoreMaxLengthKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CertificateStoreMaxLength);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCpoNameKeyValue() {
    // a KeyValue is always returned
    const auto key = keys::valid_keys::CpoName;
    v16::KeyValue kv;
    kv.key = std::move(std::string{v16::keys::convert(key)});
    kv.readonly = v16::keys::is_readonly(key);
    kv.value = get_optional<std::string>(*storage, key);
    return kv;
}

void ChargePointConfigurationDeviceModel::setAuthorizationKey(const std::string& authorization_key) {
    setInternalAuthorizationKey(authorization_key);
}

void ChargePointConfigurationDeviceModel::setCpoName(const std::string& cpo_name) {
    setInternalCpoName(cpo_name);
}

void ChargePointConfigurationDeviceModel::setDisableSecurityEventNotifications(
    bool disable_security_event_notifications) {
    setInternalDisableSecurityEventNotifications(to_string(disable_security_event_notifications));
}

void ChargePointConfigurationDeviceModel::setSecurityProfile(std::int32_t security_profile) {
    setInternalSecurityProfile(std::to_string(security_profile));
}

// ----------------------------------------------------------------------------
// Local Auth List Management

bool ChargePointConfigurationDeviceModel::getLocalAuthListEnabled() {
    const auto get_result = get_optional<bool>(*storage, keys::valid_keys::LocalAuthListEnabled);
    return get_result.value_or(false);
}

std::int32_t ChargePointConfigurationDeviceModel::getLocalAuthListMaxLength() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::LocalAuthListMaxLength);
}

std::int32_t ChargePointConfigurationDeviceModel::getSendLocalListMaxLength() {
    return get_value<std::int32_t>(*storage, keys::valid_keys::SendLocalListMaxLength);
}

KeyValue ChargePointConfigurationDeviceModel::getLocalAuthListEnabledKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LocalAuthListEnabled);
}

KeyValue ChargePointConfigurationDeviceModel::getLocalAuthListMaxLengthKeyValue() {
    return get_key_value(*storage, keys::valid_keys::LocalAuthListMaxLength);
}

KeyValue ChargePointConfigurationDeviceModel::getSendLocalListMaxLengthKeyValue() {
    return get_key_value(*storage, keys::valid_keys::SendLocalListMaxLength);
}

void ChargePointConfigurationDeviceModel::setLocalAuthListEnabled(bool local_auth_list_enabled) {
    setInternalLocalAuthListEnabled(to_string(local_auth_list_enabled));
}

// ----------------------------------------------------------------------------
// PnC

bool ChargePointConfigurationDeviceModel::getContractValidationOffline() {
    return get_value<bool>(*storage, keys::valid_keys::ContractValidationOffline);
}

bool ChargePointConfigurationDeviceModel::getISO15118CertificateManagementEnabled() {
    return get_value<bool>(*storage, keys::valid_keys::ISO15118CertificateManagementEnabled);
}

bool ChargePointConfigurationDeviceModel::getISO15118PnCEnabled() {
    return get_value<bool>(*storage, keys::valid_keys::ISO15118PnCEnabled);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getCentralContractValidationAllowed() {
    return get_optional<bool>(*storage, keys::valid_keys::CentralContractValidationAllowed);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getCertSigningRepeatTimes() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CertSigningRepeatTimes);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getCertSigningWaitMinimum() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::CertSigningWaitMinimum);
}

KeyValue ChargePointConfigurationDeviceModel::getContractValidationOfflineKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ContractValidationOffline);
}

KeyValue ChargePointConfigurationDeviceModel::getISO15118CertificateManagementEnabledKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ISO15118CertificateManagementEnabled);
}

KeyValue ChargePointConfigurationDeviceModel::getISO15118PnCEnabledKeyValue() {
    return get_key_value(*storage, keys::valid_keys::ISO15118PnCEnabled);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCentralContractValidationAllowedKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CentralContractValidationAllowed);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCertSigningRepeatTimesKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CertSigningRepeatTimes);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCertSigningWaitMinimumKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CertSigningWaitMinimum);
}

void ChargePointConfigurationDeviceModel::setContractValidationOffline(bool contract_validation_offline) {
    setInternalContractValidationOffline(to_string(contract_validation_offline));
}

void ChargePointConfigurationDeviceModel::setCentralContractValidationAllowed(
    bool central_contract_validation_allowed) {
    setInternalCentralContractValidationAllowed(to_string(central_contract_validation_allowed));
}

void ChargePointConfigurationDeviceModel::setCertSigningRepeatTimes(std::int32_t cert_signing_repeat_times) {
    setInternalCertSigningRepeatTimes(std::to_string(cert_signing_repeat_times));
}

void ChargePointConfigurationDeviceModel::setCertSigningWaitMinimum(std::int32_t cert_signing_wait_minimum) {
    setInternalCertSigningWaitMinimum(std::to_string(cert_signing_wait_minimum));
}

void ChargePointConfigurationDeviceModel::setISO15118CertificateManagementEnabled(
    bool iso15118_certificate_management_enabled) {
    setInternalISO15118CertificateManagementEnabled(to_string(iso15118_certificate_management_enabled));
}

void ChargePointConfigurationDeviceModel::setISO15118PnCEnabled(bool iso15118_pnc_enabled) {
    setInternalISO15118PnCEnabled(to_string(iso15118_pnc_enabled));
}

// ----------------------------------------------------------------------------
// California Pricing

bool ChargePointConfigurationDeviceModel::getCustomDisplayCostAndPriceEnabled() {
    const auto get_result = get_optional<bool>(*storage, keys::valid_keys::CustomDisplayCostAndPrice);
    return get_result.value_or(false);
}

TariffMessage ChargePointConfigurationDeviceModel::getDefaultTariffMessage(bool offline) {
    // DefaultPrice and DefaultPriceText are JSON strings

    TariffMessage tariff_message;
    const auto& default_price = get_optional<std::string>(*storage, keys::valid_keys::DefaultPrice);
    const auto& default_price_text = get_optional<std::string>(*storage, keys::valid_keys::DefaultPriceText);
    const std::string_view key = offline ? "priceTextOffline" : "priceText";

    if (default_price) {
        try {
            const auto default_price_json = json::parse(default_price.value());
            if (default_price_json.contains(key)) {
                DisplayMessageContent content;
                content.message = default_price_json.at(key);
                content.language = getLanguage();
                tariff_message.message.push_back(content);
            }
        } catch (const std::exception& ex) {
            EVLOG_error << "Default price json not correct, can not fetch default price : " << ex.what();
        }
    }

    if (default_price_text) {
        try {
            const auto default_price_text_json = json::parse(default_price_text.value());
            if (default_price_text_json.contains("priceTexts")) {
                for (const auto& item : default_price_text_json.at("priceTexts")) {
                    if (item.contains(key) && item.contains("language")) {
                        DisplayMessageContent content;
                        content.message = item.at(key);
                        content.language = item.at("language");
                        tariff_message.message.push_back(content);
                    }
                }
            }
        } catch (const std::exception& ex) {
            EVLOG_error << "Default price text json not correct, can not fetch default price text : " << ex.what();
        }
    }

    if (!default_price && !default_price_text) {
        EVLOG_warning << "No CostAndPrice configuration found, returning empty TariffMessage.";
    } else if (tariff_message.message.empty()) {
        EVLOG_warning << "No tariff message found in CostAndPrice configuration, returning empty TariffMessage.";
    }

    return tariff_message;
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getDefaultPrice() {
    return get_optional<std::string>(*storage, keys::valid_keys::DefaultPrice);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getDefaultPriceText(const std::string& language) {
    // DefaultPriceText is a JSON string

    const auto& default_price_text = get_optional<std::string>(*storage, keys::valid_keys::DefaultPriceText);
    std::optional<std::string> result;

    if (default_price_text) {
        try {
            const auto default_price_text_json = json::parse(default_price_text.value());
            json json_result = json::object();

            if (default_price_text_json.contains("priceTexts")) {
                for (const auto& price_text : default_price_text_json.at("priceTexts").items()) {
                    if (language == price_text.value().at("language").get<std::string>()) {
                        // Language found.
                        json_result["priceText"] = price_text.value().at("priceText");
                        if (price_text.value().contains("priceTextOffline")) {
                            json_result["priceTextOffline"] = price_text.value().at("priceTextOffline");
                        }
                        result = json_result.dump(2);
                        break;
                    }
                }
            }
        } catch (const std::exception& ex) {
            EVLOG_error << "Default price text json not correct, can not fetch default price text : " << ex.what();
        }
    }

    return result;
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getDisplayTimeOffset() {
    return get_optional<std::string>(*storage, keys::valid_keys::TimeOffset);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getLanguage() {
    return get_optional<std::string>(*storage, keys::valid_keys::Language);
}
std::optional<std::string> ChargePointConfigurationDeviceModel::getMultiLanguageSupportedLanguages() {
    return get_optional<std::string>(*storage, keys::valid_keys::SupportedLanguages);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getNextTimeOffsetTransitionDateTime() {
    return get_optional<std::string>(*storage, keys::valid_keys::NextTimeOffsetTransitionDateTime);
}

std::optional<std::string> ChargePointConfigurationDeviceModel::getTimeOffsetNextTransition() {
    return get_optional<std::string>(*storage, keys::valid_keys::TimeOffsetNextTransition);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getCustomIdleFeeAfterStop() {
    return get_optional<bool>(*storage, keys::valid_keys::CustomIdleFeeAfterStop);
}

std::optional<bool> ChargePointConfigurationDeviceModel::getCustomMultiLanguageMessagesEnabled() {
    return get_optional<bool>(*storage, keys::valid_keys::CustomMultiLanguageMessages);
}

std::optional<std::int32_t> ChargePointConfigurationDeviceModel::getWaitForSetUserPriceTimeout() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::WaitForSetUserPriceTimeout);
}

std::optional<std::uint32_t> ChargePointConfigurationDeviceModel::getPriceNumberOfDecimalsForCostValues() {
    return get_optional<std::int32_t>(*storage, keys::valid_keys::NumberOfDecimalsForCostValues);
}

KeyValue ChargePointConfigurationDeviceModel::getCustomDisplayCostAndPriceEnabledKeyValue() {
    return get_key_value(*storage, keys::valid_keys::CustomDisplayCostAndPrice);
}

KeyValue ChargePointConfigurationDeviceModel::getDefaultPriceTextKeyValue(const std::string& language) {
    KeyValue result;
    result.key = "DefaultPriceText," + language;
    result.readonly = false;
    const auto get_result = getDefaultPriceText(language);
    if (get_result) {
        result.value = get_result.value();
    } else {
        // It's a bit odd to return an empty string here, but it must be possible to set a default price text for a
        // new language. But since the 'set' function for configurations first performs a 'get' and does not continue
        // if it receives a nullopt, we can better just return an empty string so the 'set' function can continue.
        // Resolving it differently required more complexer code, this was the easiest way to do it.
        result.value = "";
    }
    return result;
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCustomIdleFeeAfterStopKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CustomIdleFeeAfterStop);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCustomMultiLanguageMessagesEnabledKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::CustomMultiLanguageMessages);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getDefaultPriceKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::DefaultPrice);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getDisplayTimeOffsetKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::TimeOffset);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getLanguageKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::Language);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getMultiLanguageSupportedLanguagesKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::SupportedLanguages);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getNextTimeOffsetTransitionDateTimeKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::NextTimeOffsetTransitionDateTime);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getPriceNumberOfDecimalsForCostValuesKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::NumberOfDecimalsForCostValues);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getTimeOffsetNextTransitionKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::TimeOffsetNextTransition);
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getWaitForSetUserPriceTimeoutKeyValue() {
    return get_key_value_optional(*storage, keys::valid_keys::WaitForSetUserPriceTimeout);
}

std::optional<std::vector<KeyValue>> ChargePointConfigurationDeviceModel::getAllDefaultPriceTextKeyValues() {
    std::optional<std::vector<KeyValue>> result;

    // DefaultPriceText is a JSON string
    const auto& default_price_text = get_optional<std::string>(*storage, keys::valid_keys::DefaultPriceText);

    if (default_price_text) {
        std::vector<KeyValue> key_values;
        try {
            const auto default_price_text_json = json::parse(default_price_text.value());
            if (default_price_text_json.contains("priceTexts")) {
                for (auto& price_text : default_price_text_json.at("priceTexts").items()) {
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
            }
        } catch (const std::exception& ex) {
            EVLOG_error << "Default price text json not correct, can not fetch default price text : " << ex.what();
        }

        if (!key_values.empty()) {
            result = std::move(key_values);
        }
    }

    return result;
}

void ChargePointConfigurationDeviceModel::setCustomIdleFeeAfterStop(bool value) {
    setInternalCustomIdleFeeAfterStop(to_string(value));
}

ConfigurationStatus ChargePointConfigurationDeviceModel::setDefaultPrice(const std::string& value) {
    return convert(setInternalDefaultPrice(value));
}

ConfigurationStatus ChargePointConfigurationDeviceModel::setDefaultPriceText(const CiString<50>& key,
                                                                             const CiString<500>& value) {
    return convert(setInternalDefaultPriceText(key, value));
}

ConfigurationStatus ChargePointConfigurationDeviceModel::setDisplayTimeOffset(const std::string& offset) {
    return convert(setInternalDisplayTimeOffset(offset));
}

void ChargePointConfigurationDeviceModel::setLanguage(const std::string& language) {
    setInternalLanguage(language);
}

ConfigurationStatus
ChargePointConfigurationDeviceModel::setNextTimeOffsetTransitionDateTime(const std::string& date_time) {
    return convert(setInternalNextTimeOffsetTransitionDateTime(date_time));
}

ConfigurationStatus ChargePointConfigurationDeviceModel::setTimeOffsetNextTransition(const std::string& offset) {
    return convert(setInternalTimeOffsetNextTransition(offset));
}

void ChargePointConfigurationDeviceModel::setWaitForSetUserPriceTimeout(std::int32_t wait_for_set_user_price_timeout) {
    setInternalWaitForSetUserPriceTimeout(std::to_string(wait_for_set_user_price_timeout));
}

// ----------------------------------------------------------------------------
// Custom

std::optional<KeyValue> ChargePointConfigurationDeviceModel::getCustomKeyValue(const CiString<50>& key) {
    std::string sv_key{key};
    return get_key_value_optional(*storage, sv_key);
}

ConfigurationStatus ChargePointConfigurationDeviceModel::setCustomKey(const CiString<50>& key,
                                                                      const CiString<500>& value, bool force) {
    const std::string sv_key{key};
    auto result = ConfigurationStatus::Rejected;

    const auto exists_ro = isReadOnly(*storage, custom_component, sv_key);
    if (exists_ro) {
        // the key exists (not allowed to create keys)
        const auto ro = exists_ro.value();

        if (!ro || force) {
            try {
                // validation is performed by the device model implementation
                const auto res = set_value(*storage, sv_key, std::string{value});
                result = convert(res);
            } catch (const std::exception& e) {
                EVLOG_warning << "Could not set custom configuration key: " << e.what();
            }
        }
    }

    return result;
}

std::optional<KeyValue> ChargePointConfigurationDeviceModel::get(const CiString<50>& key) {
    std::optional<KeyValue> result;
    const std::string key_str{key};
    const auto sv_key_opt = keys::convert(key_str);
    bool get_value{true};

    if (sv_key_opt) {
        const auto sv_key = sv_key_opt.value();

        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::FirmwareManagement)) {
            get_value = false;
        }
        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::PnC)) {
            get_value = false;
        }
        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::SmartCharging)) {
            get_value = false;
        }
        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::Security)) {
            get_value = false;
        }
        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::LocalAuthListManagement)) {
            get_value = false;
        }
        if (ignore_key(sv_key, supported_feature_profiles, SupportedFeatureProfiles::CostAndPrice)) {
            get_value = false;
        }

        // TODO(james-ctc): check for SupportedFeatureProfiles::Custom from device model

        if (keys::is_hidden(sv_key)) {
            // we should not return an AuthorizationKey because it's write only
            get_value = false;
        }
    } else {
        if (supported_feature_profiles.find(SupportedFeatureProfiles::CostAndPrice) !=
            supported_feature_profiles.end()) {
            // check keys starting DefaultPriceText
            if (key_str.find("DefaultPriceText") == 0) {
                if (getCustomMultiLanguageMessagesEnabled().value_or(false)) {
                    const auto lang = utils::split_string(',', key_str);
                    if (lang.size() == 2) {
                        result = getDefaultPriceTextKeyValue(lang.at(1));
                    }
                }
                get_value = false;
            }
        }
        // check keys starting MeterPublicKey[
        if (key_str.find("MeterPublicKey[") == 0) {
            auto id = extractConnectorId(key_str);
            if (id) {
                result = getPublicKeyKeyValue(id.value());
            }
            get_value = false;
        }
    }

    if (get_value) {
        if (sv_key_opt) {
            // known key
            result = get_key_value_optional(*storage, sv_key_opt.value());
        } else {
            // custom key
            result = get_key_value_optional(*storage, key_str);
        }
    }

    return result;
}

std::vector<KeyValue> ChargePointConfigurationDeviceModel::get_all_key_value() {
    std::vector<KeyValue> all;
    const auto report = storage->get_base_report_data(v2::ReportBaseEnum::ConfigurationInventory);
    for (const auto& entry : report) {
        const auto& component = entry.component;
        const auto& variable = entry.variable;
        try {
            const auto feature = conversions::string_to_supported_feature_profiles(component.name);
            if (const auto it = supported_feature_profiles.find(feature); it != supported_feature_profiles.end()) {
                auto kv = get(variable.name);
                if (kv) {
                    all.push_back(std::move(kv.value()));
                }
            }
        } catch (std::exception& ex) {
            EVLOG_error << "Device model '" << component.name << "' not supported: " << ex.what();
        }
    }
    return all;
}

std::optional<ConfigurationStatus> ChargePointConfigurationDeviceModel::set(const CiString<50>& key,
                                                                            const CiString<500>& value) {
    std::optional<ConfigurationStatus> result;
    const std::string key_str{key};
    const std::string value_str{value};
    const auto sv_key_opt = keys::convert(key_str);

    if (sv_key_opt) {
        const auto sv_key = sv_key_opt.value();

        switch (sv_key) {
        case keys::valid_keys::AllowChargingProfileWithoutStartSchedule:
            result = convert(setInternalAllowChargingProfileWithoutStartSchedule(value_str));
            break;
        case keys::valid_keys::AllowOfflineTxForUnknownId:
            result = convert(setInternalAllowOfflineTxForUnknownId(value_str));
            break;
        case keys::valid_keys::CentralSystemURI:
            result = convert(setInternalCentralSystemURI(value_str));
            break;
        case keys::valid_keys::CompositeScheduleDefaultLimitAmps:
            result = convert(setInternalCompositeScheduleDefaultLimitAmps(value_str));
            break;
        case keys::valid_keys::CompositeScheduleDefaultLimitWatts:
            result = convert(setInternalCompositeScheduleDefaultLimitWatts(value_str));
            break;
        case keys::valid_keys::CompositeScheduleDefaultNumberPhases:
            result = convert(setInternalCompositeScheduleDefaultNumberPhases(value_str));
            break;
        case keys::valid_keys::ConnectorEvseIds:
            result = convert(setInternalConnectorEvseIds(value_str));
            break;
        case keys::valid_keys::IgnoredProfilePurposesOffline:
            result = convert(setInternalIgnoredProfilePurposesOffline(value_str));
            break;
        case keys::valid_keys::OcspRequestInterval:
            result = convert(setInternalOcspRequestInterval(value_str));
            break;
        case keys::valid_keys::RetryBackoffRandomRange:
            result = convert(setInternalRetryBackoffRandomRange(value_str));
            break;
        case keys::valid_keys::RetryBackoffRepeatTimes:
            result = convert(setInternalRetryBackoffRepeatTimes(value_str));
            break;
        case keys::valid_keys::RetryBackoffWaitMinimum:
            result = convert(setInternalRetryBackoffWaitMinimum(value_str));
            break;
        case keys::valid_keys::SeccLeafSubjectCommonName:
            result = convert(setInternalSeccLeafSubjectCommonName(value_str));
            break;
        case keys::valid_keys::SeccLeafSubjectCountry:
            result = convert(setInternalSeccLeafSubjectCountry(value_str));
            break;
        case keys::valid_keys::SeccLeafSubjectOrganization:
            result = convert(setInternalSeccLeafSubjectOrganization(value_str));
            break;
        case keys::valid_keys::StopTransactionIfUnlockNotSupported:
            result = convert(setInternalStopTransactionIfUnlockNotSupported(value_str));
            break;
        case keys::valid_keys::SupplyVoltage:
            result = convert(setInternalSupplyVoltage(value_str));
            break;
        case keys::valid_keys::VerifyCsmsAllowWildcards:
            result = convert(setInternalVerifyCsmsAllowWildcards(value_str));
            break;
        case keys::valid_keys::WaitForStopTransactionsOnResetTimeout:
            result = convert(setInternalWaitForStopTransactionsOnResetTimeout(value_str));
            break;
        case keys::valid_keys::AuthorizationCacheEnabled:
            result = convert(setInternalAuthorizationCacheEnabled(value_str));
            break;
        case keys::valid_keys::AuthorizeRemoteTxRequests:
            result = convert(setInternalAuthorizeRemoteTxRequests(value_str));
            break;
        case keys::valid_keys::BlinkRepeat:
            result = convert(setInternalBlinkRepeat(value_str));
            break;
        case keys::valid_keys::ClockAlignedDataInterval:
            result = convert(setInternalClockAlignedDataInterval(value_str));
            break;
        case keys::valid_keys::ConnectionTimeOut:
            result = convert(setInternalConnectionTimeOut(value_str));
            break;
        case keys::valid_keys::ConnectorPhaseRotation:
            result = convert(setInternalConnectorPhaseRotation(value_str));
            break;
        case keys::valid_keys::HeartbeatInterval:
            result = convert(setInternalHeartbeatInterval(value_str));
            break;
        case keys::valid_keys::LightIntensity:
            result = convert(setInternalLightIntensity(value_str));
            break;
        case keys::valid_keys::LocalAuthorizeOffline:
            result = convert(setInternalLocalAuthorizeOffline(value_str));
            break;
        case keys::valid_keys::LocalPreAuthorize:
            result = convert(setInternalLocalPreAuthorize(value_str));
            break;
        case keys::valid_keys::MaxEnergyOnInvalidId:
            result = convert(setInternalMaxEnergyOnInvalidId(value_str));
            break;
        case keys::valid_keys::MeterValuesAlignedData:
            result = convert(setInternalMeterValuesAlignedData(value_str));
            break;
        case keys::valid_keys::MeterValuesSampledData:
            result = convert(setInternalMeterValuesSampledData(value_str));
            break;
        case keys::valid_keys::MeterValueSampleInterval:
            result = convert(setInternalMeterValueSampleInterval(value_str));
            break;
        case keys::valid_keys::MinimumStatusDuration:
            result = convert(setInternalMinimumStatusDuration(value_str));
            break;
        case keys::valid_keys::ResetRetries:
            result = convert(setInternalResetRetries(value_str));
            break;
        case keys::valid_keys::StopTransactionOnInvalidId:
            result = convert(setInternalStopTransactionOnInvalidId(value_str));
            break;
        case keys::valid_keys::StopTxnAlignedData:
            result = convert(setInternalStopTxnAlignedData(value_str));
            break;
        case keys::valid_keys::StopTxnSampledData:
            result = convert(setInternalStopTxnSampledData(value_str));
            break;
        case keys::valid_keys::TransactionMessageAttempts:
            result = convert(setInternalTransactionMessageAttempts(value_str));
            break;
        case keys::valid_keys::TransactionMessageRetryInterval:
            result = convert(setInternalTransactionMessageRetryInterval(value_str));
            break;
        case keys::valid_keys::UnlockConnectorOnEVSideDisconnect:
            result = convert(setInternalUnlockConnectorOnEVSideDisconnect(value_str));
            break;
        case keys::valid_keys::WebSocketPingInterval:
            result = convert(setInternalWebsocketPingInterval(value_str));
            break;
        case keys::valid_keys::AuthorizationKey:
            result = convert(setInternalAuthorizationKey(value_str));
            break;
        case keys::valid_keys::CpoName:
            result = convert(setInternalCpoName(value_str));
            break;
        case keys::valid_keys::DisableSecurityEventNotifications:
            result = convert(setInternalDisableSecurityEventNotifications(value_str));
            break;
        case keys::valid_keys::SecurityProfile:
            result = convert(setInternalSecurityProfile(value_str));
            break;
        case keys::valid_keys::ISO15118CertificateManagementEnabled:
            result = convert(setInternalISO15118CertificateManagementEnabled(value_str));
            break;
        case keys::valid_keys::LocalAuthListEnabled:
            result = convert(setInternalLocalAuthListEnabled(value_str));
            break;
        case keys::valid_keys::ContractValidationOffline:
            result = convert(setInternalContractValidationOffline(value_str));
            break;
        case keys::valid_keys::CentralContractValidationAllowed:
            result = convert(setInternalCentralContractValidationAllowed(value_str));
            break;
        case keys::valid_keys::CertSigningRepeatTimes:
            result = convert(setInternalCertSigningRepeatTimes(value_str));
            break;
        case keys::valid_keys::CertSigningWaitMinimum:
            result = convert(setInternalCertSigningWaitMinimum(value_str));
            break;
        case keys::valid_keys::ISO15118PnCEnabled:
            result = convert(setInternalISO15118PnCEnabled(value_str));
            break;
        case keys::valid_keys::CustomIdleFeeAfterStop:
            result = convert(setInternalCustomIdleFeeAfterStop(value_str));
            break;
        case keys::valid_keys::DefaultPrice:
            result = convert(setInternalDefaultPrice(value_str));
            break;
        case keys::valid_keys::DefaultPriceText:
            // should never match - language expected: DefaultPriceText,de
            EVLOG_error << R"(ChargePointConfiguration::set("DefaultPriceText", )" << value << R"(") not supported)";
            result = ConfigurationStatus::NotSupported;
            break;
        case keys::valid_keys::MeterPublicKeys:
            // should never match - connector ID expected: MeterPublicKey[1]
            EVLOG_error << R"(ChargePointConfiguration::set("MeterPublicKey", )" << value << R"(") not supported)";
            result = ConfigurationStatus::NotSupported;
            break;
        case keys::valid_keys::TimeOffset:
            result = convert(setInternalDisplayTimeOffset(value_str));
            break;
        case keys::valid_keys::Language:
            result = convert(setInternalLanguage(value_str));
            break;
        case keys::valid_keys::NextTimeOffsetTransitionDateTime:
            result = convert(setInternalNextTimeOffsetTransitionDateTime(value_str));
            break;
        case keys::valid_keys::TimeOffsetNextTransition:
            result = convert(setInternalTimeOffsetNextTransition(value_str));
            break;
        case keys::valid_keys::WaitForSetUserPriceTimeout:
            result = convert(setInternalWaitForSetUserPriceTimeout(value_str));
            break;

        case keys::valid_keys::ChargeBoxSerialNumber:
        case keys::valid_keys::ChargePointModel:
        case keys::valid_keys::ChargePointSerialNumber:
        case keys::valid_keys::ChargePointVendor:
        case keys::valid_keys::FirmwareVersion:
        case keys::valid_keys::ICCID:
        case keys::valid_keys::IMSI:
        case keys::valid_keys::MeterSerialNumber:
            // these are not setable via OCPP  - std::nullopt expected
            break;

        case keys::valid_keys::EnableTLSKeylog:
        case keys::valid_keys::LogRotation:
        case keys::valid_keys::LogRotationDateSuffix:
        case keys::valid_keys::LogRotationMaximumFileCount:
        case keys::valid_keys::LogRotationMaximumFileSize:
        case keys::valid_keys::TLSKeylogFile:
        case keys::valid_keys::UseTPM:
        case keys::valid_keys::UseTPMSeccLeafCertificate:
            // hidden keys - std::nullopt expected
            break;

        case keys::valid_keys::ConnectorPhaseRotationMaxLength:
        case keys::valid_keys::GetConfigurationMaxKeys:
        case keys::valid_keys::MeterValuesAlignedDataMaxLength:
        case keys::valid_keys::MeterValuesSampledDataMaxLength:
        case keys::valid_keys::NumberOfConnectors:
        case keys::valid_keys::StopTransactionOnEVSideDisconnect:
        case keys::valid_keys::StopTxnAlignedDataMaxLength:
        case keys::valid_keys::StopTxnSampledDataMaxLength:
        case keys::valid_keys::SupportedFeatureProfiles:
        case keys::valid_keys::SupportedFeatureProfilesMaxLength:
        case keys::valid_keys::CustomDisplayCostAndPrice:
        case keys::valid_keys::CustomMultiLanguageMessages:
        case keys::valid_keys::NumberOfDecimalsForCostValues:
        case keys::valid_keys::SupportedLanguages:
        case keys::valid_keys::SupportedFileTransferProtocols:
        case keys::valid_keys::AuthorizeConnectorZeroOnConnectorOne:
        case keys::valid_keys::ChargePointId:
        case keys::valid_keys::HostName:
        case keys::valid_keys::IFace:
        case keys::valid_keys::LogMessages:
        case keys::valid_keys::LogMessagesFormat:
        case keys::valid_keys::LogMessagesRaw:
        case keys::valid_keys::MaxCompositeScheduleDuration:
        case keys::valid_keys::MaxMessageSize:
        case keys::valid_keys::MessageQueueSizeThreshold:
        case keys::valid_keys::MessageTypesDiscardForQueueing:
        case keys::valid_keys::MeterType:
        case keys::valid_keys::QueueAllMessages:
        case keys::valid_keys::SupportedChargingProfilePurposeTypes:
        case keys::valid_keys::SupportedCiphers12:
        case keys::valid_keys::SupportedCiphers13:
        case keys::valid_keys::SupportedMeasurands:
        case keys::valid_keys::UseSslDefaultVerifyPaths:
        case keys::valid_keys::VerifyCsmsCommonName:
        case keys::valid_keys::WebsocketPingPayload:
        case keys::valid_keys::WebsocketPongTimeout:
        case keys::valid_keys::LocalAuthListMaxLength:
        case keys::valid_keys::SendLocalListMaxLength:
        case keys::valid_keys::ReserveConnectorZeroSupported:
        case keys::valid_keys::AdditionalRootCertificateCheck:
        case keys::valid_keys::CertificateSignedMaxChainSize:
        case keys::valid_keys::CertificateStoreMaxLength:
        case keys::valid_keys::ChargeProfileMaxStackLevel:
        case keys::valid_keys::ChargingScheduleAllowedChargingRateUnit:
        case keys::valid_keys::ChargingScheduleMaxPeriods:
        case keys::valid_keys::ConnectorSwitch3to1PhaseSupported:
        case keys::valid_keys::MaxChargingProfilesInstalled:
        default:
            // std::nullopt expected
            break;
        }
    } else {
        if (key_str.find("DefaultPriceText") == 0) {
            if (supported_feature_profiles.find(SupportedFeatureProfiles::CostAndPrice) !=
                supported_feature_profiles.end()) {
                // check keys starting DefaultPriceText
                result = setDefaultPriceText(key, value);
            } else {
                result = ConfigurationStatus::NotSupported;
            }
        } else if (key_str.find("MeterPublicKey[") == 0) {
            // not setable
        } else {
            // custom key
            const auto exists_ro = isReadOnly(*storage, custom_component, key_str);
            if (!exists_ro.value_or(true)) {
                // key exists and is not read-only
                const auto res = set_value(*storage, key_str, value_str);
                result = convert(res);
            }
        }
    }

    return result;
}

std::set<MessageType> ChargePointConfigurationDeviceModel::getSupportedMessageTypesSending() {
    return supported_message_types_sending;
}

std::set<MessageType> ChargePointConfigurationDeviceModel::getSupportedMessageTypesReceiving() {
    return supported_message_types_receiving;
}

} // namespace ocpp::v16
