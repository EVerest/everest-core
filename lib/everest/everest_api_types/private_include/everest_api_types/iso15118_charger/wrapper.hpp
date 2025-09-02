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

CertificateActionEnum_Internal toInternalApi(CertificateActionEnum_External const& val);
CertificateActionEnum_External toExternalApi(CertificateActionEnum_Internal const& val);

using Status_Internal = ::types::iso15118::Status;
using Status_External = Status;

Status_Internal toInternalApi(Status_External const& val);
Status_External toExternalApi(Status_Internal const& val);

using RequestExiStreamSchema_Internal = ::types::iso15118::RequestExiStreamSchema;
using RequestExiStreamSchema_External = RequestExiStreamSchema;

RequestExiStreamSchema_Internal toInternalApi(RequestExiStreamSchema_External const& val);
RequestExiStreamSchema_External toExternalApi(RequestExiStreamSchema_Internal const& val);

using ResponseExiStreamStatus_Internal = ::types::iso15118::ResponseExiStreamStatus;
using ResponseExiStreamStatus_External = ResponseExiStreamStatus;

ResponseExiStreamStatus_Internal toInternalApi(ResponseExiStreamStatus_External const& val);
ResponseExiStreamStatus_External toExternalApi(ResponseExiStreamStatus_Internal const& val);

} // namespace everest::lib::API::V1_0::types::iso15118_charger
