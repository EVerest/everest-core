// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::iso15118_charger {

enum class CertificateActionEnum {
    Install,
    Update,
};

enum class Status {
    Accepted,
    Failed,
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

} // namespace everest::lib::API::V1_0::types::iso15118_charger
