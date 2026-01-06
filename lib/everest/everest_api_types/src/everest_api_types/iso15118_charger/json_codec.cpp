// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "iso15118_charger/json_codec.hpp"
#include "iso15118_charger/API.hpp"
#include "iso15118_charger/codec.hpp"
#include "nlohmann/json.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::iso15118_charger {

void from_json(json const& j, CertificateActionEnum& k) {
    std::string s = j;
    if (s == "Install") {
        k = CertificateActionEnum::Install;
        return;
    }
    if (s == "Update") {
        k = CertificateActionEnum::Update;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type API_V1_0_TYPES_ISO15118_CHARGER_CertificateActionEnum");
}

void to_json(json& j, CertificateActionEnum const& k) noexcept {
    switch (k) {
    case CertificateActionEnum::Install:
        j = "Install";
        return;
    case CertificateActionEnum::Update:
        j = "Update";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::iso15118_charger::CertificateActionEnum";
}

void from_json(json const& j, EnergyTransferMode& k) {
    std::string s = j;
    if (s == "AC_single_phase_core") {
        k = EnergyTransferMode::AC_single_phase_core;
        return;
    }
    if (s == "AC_two_phase") {
        k = EnergyTransferMode::AC_two_phase;
        return;
    }
    if (s == "AC_three_phase_core") {
        k = EnergyTransferMode::AC_three_phase_core;
        return;
    }
    if (s == "DC_core") {
        k = EnergyTransferMode::DC_core;
        return;
    }
    if (s == "DC_extended") {
        k = EnergyTransferMode::DC_extended;
        return;
    }
    if (s == "DC_combo_core") {
        k = EnergyTransferMode::DC_combo_core;
        return;
    }
    if (s == "DC_unique") {
        k = EnergyTransferMode::DC_unique;
        return;
    }
    if (s == "DC") {
        k = EnergyTransferMode::DC;
        return;
    }
    if (s == "AC_BPT") {
        k = EnergyTransferMode::AC_BPT;
        return;
    }
    if (s == "AC_BPT_DER") {
        k = EnergyTransferMode::AC_BPT_DER;
        return;
    }
    if (s == "AC_DER") {
        k = EnergyTransferMode::AC_DER;
        return;
    }
    if (s == "DC_BPT") {
        k = EnergyTransferMode::DC_BPT;
        return;
    }
    if (s == "DC_ACDP") {
        k = EnergyTransferMode::DC_ACDP;
        return;
    }
    if (s == "DC_ACDP_BPT") {
        k = EnergyTransferMode::DC_ACDP_BPT;
        return;
    }
    if (s == "WPT") {
        k = EnergyTransferMode::WPT;
        return;
    }
    if (s == "MCS") {
        k = EnergyTransferMode::MCS;
        return;
    }
    if (s == "MCS_BPT") {
        k = EnergyTransferMode::MCS_BPT;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type API_V1_0_TYPES_ISO15118_CHARGER_EnergyTransferMode");
}

void to_json(json& j, EnergyTransferMode const& k) noexcept {
    switch (k) {
    case EnergyTransferMode::AC_single_phase_core:
        j = "AC_single_phase_core";
        return;
    case EnergyTransferMode::AC_two_phase:
        j = "AC_two_phase";
        return;
    case EnergyTransferMode::AC_three_phase_core:
        j = "AC_three_phase_core";
        return;
    case EnergyTransferMode::DC_core:
        j = "DC_core";
        return;
    case EnergyTransferMode::DC_extended:
        j = "DC_extended";
        return;
    case EnergyTransferMode::DC_combo_core:
        j = "DC_combo_core";
        return;
    case EnergyTransferMode::DC_unique:
        j = "DC_unique";
        return;
    case EnergyTransferMode::DC:
        j = "DC";
        return;
    case EnergyTransferMode::AC_BPT:
        j = "AC_BPT";
        return;
    case EnergyTransferMode::AC_BPT_DER:
        j = "AC_BPT_DER";
        return;
    case EnergyTransferMode::AC_DER:
        j = "AC_DER";
        return;
    case EnergyTransferMode::DC_BPT:
        j = "DC_BPT";
        return;
    case EnergyTransferMode::DC_ACDP:
        j = "DC_ACDP";
        return;
    case EnergyTransferMode::DC_ACDP_BPT:
        j = "DC_ACDP_BPT";
        return;
    case EnergyTransferMode::WPT:
        j = "WPT";
        return;
    case EnergyTransferMode::MCS:
        j = "MCS";
        return;
    case EnergyTransferMode::MCS_BPT:
        j = "MCS_BPT";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::iso15118_charger::EnergyTransferMode";
}

void from_json(json const& j, Status& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = Status::Accepted;
        return;
    }
    if (s == "Failed") {
        k = Status::Failed;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_ISO15118_CHARGER_Status");
}

void to_json(json& j, Status const& k) noexcept {
    switch (k) {
    case Status::Accepted:
        j = "Accepted";
        return;
    case Status::Failed:
        j = "Failed";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::iso15118_charger::Status";
}

void from_json(json const& j, HashAlgorithm& k) {
    std::string s = j;
    if (s == "SHA256") {
        k = HashAlgorithm::SHA256;
        return;
    }
    if (s == "SHA384") {
        k = HashAlgorithm::SHA384;
        return;
    }
    if (s == "SHA512") {
        k = HashAlgorithm::SHA512;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type API_V1_0_TYPES_ISO15118_CHARGER_HashAlgorithm");
}

void to_json(json& j, HashAlgorithm const& k) noexcept {
    switch (k) {
    case HashAlgorithm::SHA256:
        j = "SHA256";
        return;
    case HashAlgorithm::SHA384:
        j = "SHA384";
        return;
    case HashAlgorithm::SHA512:
        j = "SHA512";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::iso15118_charger::HashAlgorithm";
}

void from_json(const json& j, RequestExiStreamSchema& k) {
    k.exi_request = j.at("exi_request");
    k.iso15118_schema_version = j.at("iso15118_schema_version");
    k.certificate_action = j.at("certificate_action");
}

void to_json(json& j, RequestExiStreamSchema const& k) noexcept {
    j = json{
        {"exi_request", k.exi_request},
        {"iso15118_schema_version", k.iso15118_schema_version},
        {"certificate_action", k.certificate_action},
    };
}

void from_json(const json& j, ResponseExiStreamStatus& k) {
    k.status = j.at("status");
    k.certificate_action = j.at("certificate_action");
    if (j.contains("exi_response")) {
        k.exi_response.emplace(j.at("exi_response"));
    }
}

void to_json(json& j, ResponseExiStreamStatus const& k) noexcept {
    j = json{
        {"status", k.status},
        {"certificate_action", k.certificate_action},
    };
    if (k.exi_response) {
        j["exi_response"] = k.exi_response.value();
    }
}

void from_json(const json& j, CertificateHashDataInfo& k) {
    k.hashAlgorithm = j.at("hashAlgorithm");
    k.issuerNameHash = j.at("issuerNameHash");
    k.issuerKeyHash = j.at("issuerKeyHash");
    k.serialNumber = j.at("serialNumber");
    k.responderURL = j.at("responderURL");
}

void to_json(json& j, CertificateHashDataInfo const& k) noexcept {
    j = json{
        {"hashAlgorithm", k.hashAlgorithm}, {"issuerNameHash", k.issuerNameHash}, {"issuerKeyHash", k.issuerKeyHash},
        {"serialNumber", k.serialNumber},   {"responderURL", k.responderURL},
    };
}

void to_json(json& j, EnergyTransferModeList const& k) noexcept {
    j["modes"] = json::array();
    for (auto val : k.modes) {
        j["modes"].push_back(val);
    }
}

void from_json(const json& j, EnergyTransferModeList& k) {
    k.modes.clear();
    for (auto val : j.at("modes")) {
        k.modes.push_back(val);
    }
}

} // namespace everest::lib::API::V1_0::types::iso15118_charger
