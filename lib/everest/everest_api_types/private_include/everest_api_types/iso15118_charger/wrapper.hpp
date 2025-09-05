// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/iso15118_charger/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/iso15118.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::iso15118_charger {

using CertificateActionEnum_Internal = ::types::iso15118::CertificateActionEnum;
using CertificateActionEnum_External = CertificateActionEnum;

CertificateActionEnum_Internal to_internal_api(CertificateActionEnum_External const& val);
CertificateActionEnum_External to_external_api(CertificateActionEnum_Internal const& val);

using Status_Internal = ::types::iso15118::Status;
using Status_External = Status;

Status_Internal to_internal_api(Status_External const& val);
Status_External to_external_api(Status_Internal const& val);

using HashAlgorithm_Internal = ::types::iso15118::HashAlgorithm;
using HashAlgorithm_External = HashAlgorithm;

HashAlgorithm_Internal to_internal_api(HashAlgorithm_External const& val);
HashAlgorithm_External to_external_api(HashAlgorithm_Internal const& val);

using RequestExiStreamSchema_Internal = ::types::iso15118::RequestExiStreamSchema;
using RequestExiStreamSchema_External = RequestExiStreamSchema;

RequestExiStreamSchema_Internal to_internal_api(RequestExiStreamSchema_External const& val);
RequestExiStreamSchema_External to_external_api(RequestExiStreamSchema_Internal const& val);

using ResponseExiStreamStatus_Internal = ::types::iso15118::ResponseExiStreamStatus;
using ResponseExiStreamStatus_External = ResponseExiStreamStatus;

ResponseExiStreamStatus_Internal to_internal_api(ResponseExiStreamStatus_External const& val);
ResponseExiStreamStatus_External to_external_api(ResponseExiStreamStatus_Internal const& val);

using CertificateHashDataInfo_Internal = ::types::iso15118::CertificateHashDataInfo;
using CertificateHashDataInfo_External = CertificateHashDataInfo;

CertificateHashDataInfo_Internal to_internal_api(CertificateHashDataInfo_External const& val);
CertificateHashDataInfo_External to_external_api(CertificateHashDataInfo_Internal const& val);

} // namespace everest::lib::API::V1_0::types::iso15118_charger
