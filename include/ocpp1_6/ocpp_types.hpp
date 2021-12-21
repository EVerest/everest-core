// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_OCPP_TYPES_HPP
#define OCPP1_6_OCPP_TYPES_HPP

#include <map>
#include <string>

#include <boost/optional.hpp>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

struct IdTagInfo {
    AuthorizationStatus status;
    boost::optional<DateTime> expiryDate;
    boost::optional<CiString20Type> parentIdTag;

    friend void to_json(json& j, const IdTagInfo& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::authorization_status_to_string(k.status)},
        };
        // the optional parts of the message
        if (k.expiryDate) {
            j["expiryDate"] = k.expiryDate.value().to_rfc3339();
        }
        if (k.parentIdTag) {
            j["parentIdTag"] = k.parentIdTag.value();
        }
    }

    friend void from_json(const json& j, IdTagInfo& k) {
        // the required parts of the message
        k.status = conversions::string_to_authorization_status(j.at("status"));

        // the optional parts of the message
        if (j.contains("expiryDate")) {
            k.expiryDate.emplace(j.at("expiryDate"));
        }
        if (j.contains("parentIdTag")) {
            k.parentIdTag.emplace(j.at("parentIdTag"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const IdTagInfo& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ChargingSchedulePeriod {
    int32_t startPeriod;
    float limit;
    boost::optional<int32_t> numberPhases;

    friend void to_json(json& j, const ChargingSchedulePeriod& k) {
        // the required parts of the message
        j = json{
            {"startPeriod", k.startPeriod},
            {"limit", k.limit},
        };
        // the optional parts of the message
        if (k.numberPhases) {
            j["numberPhases"] = k.numberPhases.value();
        }
    }

    friend void from_json(const json& j, ChargingSchedulePeriod& k) {
        // the required parts of the message
        k.startPeriod = j.at("startPeriod");
        k.limit = j.at("limit");

        // the optional parts of the message
        if (j.contains("numberPhases")) {
            k.numberPhases.emplace(j.at("numberPhases"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const ChargingSchedulePeriod& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ChargingSchedule {
    ChargingRateUnit chargingRateUnit;
    std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
    boost::optional<int32_t> duration;
    boost::optional<DateTime> startSchedule;
    boost::optional<float> minChargingRate;

    friend void to_json(json& j, const ChargingSchedule& k) {
        // the required parts of the message
        j = json{
            {"chargingRateUnit", conversions::charging_rate_unit_to_string(k.chargingRateUnit)},
            {"chargingSchedulePeriod", k.chargingSchedulePeriod},
        };
        // the optional parts of the message
        if (k.duration) {
            j["duration"] = k.duration.value();
        }
        if (k.startSchedule) {
            j["startSchedule"] = k.startSchedule.value().to_rfc3339();
        }
        if (k.minChargingRate) {
            j["minChargingRate"] = k.minChargingRate.value();
        }
    }

    friend void from_json(const json& j, ChargingSchedule& k) {
        // the required parts of the message
        k.chargingRateUnit = conversions::string_to_charging_rate_unit(j.at("chargingRateUnit"));
        for (auto val : j.at("chargingSchedulePeriod")) {
            k.chargingSchedulePeriod.push_back(val);
        }

        // the optional parts of the message
        if (j.contains("duration")) {
            k.duration.emplace(j.at("duration"));
        }
        if (j.contains("startSchedule")) {
            k.startSchedule.emplace(j.at("startSchedule"));
        }
        if (j.contains("minChargingRate")) {
            k.minChargingRate.emplace(j.at("minChargingRate"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct KeyValue {
    CiString50Type key;
    bool readonly;
    boost::optional<CiString500Type> value;

    friend void to_json(json& j, const KeyValue& k) {
        // the required parts of the message
        j = json{
            {"key", k.key},
            {"readonly", k.readonly},
        };
        // the optional parts of the message
        if (k.value) {
            j["value"] = k.value.value();
        }
    }

    friend void from_json(const json& j, KeyValue& k) {
        // the required parts of the message
        k.key = j.at("key");
        k.readonly = j.at("readonly");

        // the optional parts of the message
        if (j.contains("value")) {
            k.value.emplace(j.at("value"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const KeyValue& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct SampledValue {
    std::string value;
    boost::optional<ReadingContext> context;
    boost::optional<ValueFormat> format;
    boost::optional<Measurand> measurand;
    boost::optional<Phase> phase;
    boost::optional<Location> location;
    boost::optional<UnitOfMeasure> unit;

    friend void to_json(json& j, const SampledValue& k) {
        // the required parts of the message
        j = json{
            {"value", k.value},
        };
        // the optional parts of the message
        if (k.context) {
            j["context"] = conversions::reading_context_to_string(k.context.value());
        }
        if (k.format) {
            j["format"] = conversions::value_format_to_string(k.format.value());
        }
        if (k.measurand) {
            j["measurand"] = conversions::measurand_to_string(k.measurand.value());
        }
        if (k.phase) {
            j["phase"] = conversions::phase_to_string(k.phase.value());
        }
        if (k.location) {
            j["location"] = conversions::location_to_string(k.location.value());
        }
        if (k.unit) {
            j["unit"] = conversions::unit_of_measure_to_string(k.unit.value());
        }
    }

    friend void from_json(const json& j, SampledValue& k) {
        // the required parts of the message
        k.value = j.at("value");

        // the optional parts of the message
        if (j.contains("context")) {
            k.context.emplace(j.at("context"));
        }
        if (j.contains("format")) {
            k.format.emplace(j.at("format"));
        }
        if (j.contains("measurand")) {
            k.measurand.emplace(j.at("measurand"));
        }
        if (j.contains("phase")) {
            k.phase.emplace(j.at("phase"));
        }
        if (j.contains("location")) {
            k.location.emplace(j.at("location"));
        }
        if (j.contains("unit")) {
            k.unit.emplace(j.at("unit"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const SampledValue& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct MeterValue {
    DateTime timestamp;
    std::vector<SampledValue> sampledValue;

    friend void to_json(json& j, const MeterValue& k) {
        // the required parts of the message
        j = json{
            {"timestamp", k.timestamp.to_rfc3339()},
            {"sampledValue", k.sampledValue},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, MeterValue& k) {
        // the required parts of the message
        k.timestamp = DateTime(std::string(j.at("timestamp")));
        ;
        for (auto val : j.at("sampledValue")) {
            k.sampledValue.push_back(val);
        }

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const MeterValue& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ChargingProfile {
    int32_t chargingProfileId;
    int32_t stackLevel;
    ChargingProfilePurposeType chargingProfilePurpose;
    ChargingProfileKindType chargingProfileKind;
    ChargingSchedule chargingSchedule;
    boost::optional<int32_t> transactionId;
    boost::optional<RecurrencyKindType> recurrencyKind;
    boost::optional<DateTime> validFrom;
    boost::optional<DateTime> validTo;

    friend void to_json(json& j, const ChargingProfile& k) {
        // the required parts of the message
        j = json{
            {"chargingProfileId", k.chargingProfileId},
            {"stackLevel", k.stackLevel},
            {"chargingProfilePurpose", conversions::charging_profile_purpose_type_to_string(k.chargingProfilePurpose)},
            {"chargingProfileKind", conversions::charging_profile_kind_type_to_string(k.chargingProfileKind)},
            {"chargingSchedule", k.chargingSchedule},
        };
        // the optional parts of the message
        if (k.transactionId) {
            j["transactionId"] = k.transactionId.value();
        }
        if (k.recurrencyKind) {
            j["recurrencyKind"] = conversions::recurrency_kind_type_to_string(k.recurrencyKind.value());
        }
        if (k.validFrom) {
            j["validFrom"] = k.validFrom.value().to_rfc3339();
        }
        if (k.validTo) {
            j["validTo"] = k.validTo.value().to_rfc3339();
        }
    }

    friend void from_json(const json& j, ChargingProfile& k) {
        // the required parts of the message
        k.chargingProfileId = j.at("chargingProfileId");
        k.stackLevel = j.at("stackLevel");
        k.chargingProfilePurpose = conversions::string_to_charging_profile_purpose_type(j.at("chargingProfilePurpose"));
        k.chargingProfileKind = conversions::string_to_charging_profile_kind_type(j.at("chargingProfileKind"));
        k.chargingSchedule = j.at("chargingSchedule");

        // the optional parts of the message
        if (j.contains("transactionId")) {
            k.transactionId.emplace(j.at("transactionId"));
        }
        if (j.contains("recurrencyKind")) {
            k.recurrencyKind.emplace(j.at("recurrencyKind"));
        }
        if (j.contains("validFrom")) {
            k.validFrom.emplace(j.at("validFrom"));
        }
        if (j.contains("validTo")) {
            k.validTo.emplace(j.at("validTo"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const ChargingProfile& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct LocalAuthorizationList {
    CiString20Type idTag;
    boost::optional<IdTagInfo> idTagInfo;

    friend void to_json(json& j, const LocalAuthorizationList& k) {
        // the required parts of the message
        j = json{
            {"idTag", k.idTag},
        };
        // the optional parts of the message
        if (k.idTagInfo) {
            j["idTagInfo"] = k.idTagInfo.value();
        }
    }

    friend void from_json(const json& j, LocalAuthorizationList& k) {
        // the required parts of the message
        k.idTag = j.at("idTag");

        // the optional parts of the message
        if (j.contains("idTagInfo")) {
            k.idTagInfo.emplace(j.at("idTagInfo"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const LocalAuthorizationList& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct TransactionData {
    DateTime timestamp;
    std::vector<SampledValue> sampledValue;

    friend void to_json(json& j, const TransactionData& k) {
        // the required parts of the message
        j = json{
            {"timestamp", k.timestamp.to_rfc3339()},
            {"sampledValue", k.sampledValue},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, TransactionData& k) {
        // the required parts of the message
        k.timestamp = DateTime(std::string(j.at("timestamp")));
        ;
        for (auto val : j.at("sampledValue")) {
            k.sampledValue.push_back(val);
        }

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const TransactionData& k) {
        os << json(k).dump(4);
        return os;
    }
};
} // namespace ocpp1_6

#endif // OCPP1_6_OCPP_TYPES_HPP
