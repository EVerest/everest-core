// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::iso15118_charger {

enum class CertificateActionEnum {
    Install,
    Update,
};

enum class EnergyTransferMode {
    AC_single_phase_core,
    AC_two_phase,
    AC_three_phase_core,
    DC_core,
    DC_extended,
    DC_combo_core,
    DC_unique,
    DC,
    AC_BPT,
    AC_BPT_DER,
    AC_DER,
    DC_BPT,
    DC_ACDP,
    DC_ACDP_BPT,
    WPT,
    MCS,
    MCS_BPT,
};

enum class Status {
    Accepted,
    Failed,
};

enum class HashAlgorithm {
    SHA256,
    SHA384,
    SHA512,
};

struct RequestExiStreamSchema {
    std::string exi_request;
    std::string iso15118_schema_version;
    CertificateActionEnum certificate_action;
};

struct ResponseExiStreamStatus {
    Status status;
    CertificateActionEnum certificate_action;
    std::optional<std::string> exi_response;
};

struct CertificateHashDataInfo {
    HashAlgorithm hashAlgorithm;
    std::string issuerNameHash;
    std::string issuerKeyHash;
    std::string serialNumber;
    std::string responderURL;
};

struct EnergyTransferModeList {
    std::vector<EnergyTransferMode> modes;
};

} // namespace everest::lib::API::V1_0::types::iso15118_charger
