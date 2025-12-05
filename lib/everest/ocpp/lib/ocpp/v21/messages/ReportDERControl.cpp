// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/ReportDERControl.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string ReportDERControlRequest::get_type() const {
    return "ReportDERControl";
}

void to_json(json& j, const ReportDERControlRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.curve) {
        j["curve"] = json::array();
        for (auto val : k.curve.value()) {
            j["curve"].push_back(val);
        }
    }
    if (k.enterService) {
        j["enterService"] = json::array();
        for (auto val : k.enterService.value()) {
            j["enterService"].push_back(val);
        }
    }
    if (k.fixedPFAbsorb) {
        j["fixedPFAbsorb"] = json::array();
        for (auto val : k.fixedPFAbsorb.value()) {
            j["fixedPFAbsorb"].push_back(val);
        }
    }
    if (k.fixedPFInject) {
        j["fixedPFInject"] = json::array();
        for (auto val : k.fixedPFInject.value()) {
            j["fixedPFInject"].push_back(val);
        }
    }
    if (k.fixedVar) {
        j["fixedVar"] = json::array();
        for (auto val : k.fixedVar.value()) {
            j["fixedVar"].push_back(val);
        }
    }
    if (k.freqDroop) {
        j["freqDroop"] = json::array();
        for (auto val : k.freqDroop.value()) {
            j["freqDroop"].push_back(val);
        }
    }
    if (k.gradient) {
        j["gradient"] = json::array();
        for (auto val : k.gradient.value()) {
            j["gradient"].push_back(val);
        }
    }
    if (k.limitMaxDischarge) {
        j["limitMaxDischarge"] = json::array();
        for (auto val : k.limitMaxDischarge.value()) {
            j["limitMaxDischarge"].push_back(val);
        }
    }
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ReportDERControlRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("curve")) {
        json arr = j.at("curve");
        std::vector<DERCurveGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.curve.emplace(vec);
    }
    if (j.contains("enterService")) {
        json arr = j.at("enterService");
        std::vector<EnterServiceGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.enterService.emplace(vec);
    }
    if (j.contains("fixedPFAbsorb")) {
        json arr = j.at("fixedPFAbsorb");
        std::vector<FixedPFGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.fixedPFAbsorb.emplace(vec);
    }
    if (j.contains("fixedPFInject")) {
        json arr = j.at("fixedPFInject");
        std::vector<FixedPFGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.fixedPFInject.emplace(vec);
    }
    if (j.contains("fixedVar")) {
        json arr = j.at("fixedVar");
        std::vector<FixedVarGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.fixedVar.emplace(vec);
    }
    if (j.contains("freqDroop")) {
        json arr = j.at("freqDroop");
        std::vector<FreqDroopGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.freqDroop.emplace(vec);
    }
    if (j.contains("gradient")) {
        json arr = j.at("gradient");
        std::vector<GradientGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.gradient.emplace(vec);
    }
    if (j.contains("limitMaxDischarge")) {
        json arr = j.at("limitMaxDischarge");
        std::vector<LimitMaxDischargeGet> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.limitMaxDischarge.emplace(vec);
    }
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ReportDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the ReportDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const ReportDERControlRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ReportDERControlResponse::get_type() const {
    return "ReportDERControlResponse";
}

void to_json(json& j, const ReportDERControlResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ReportDERControlResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ReportDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the ReportDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const ReportDERControlResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
