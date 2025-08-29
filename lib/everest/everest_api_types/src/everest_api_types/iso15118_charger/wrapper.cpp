// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "iso15118_charger/wrapper.hpp"
#include "iso15118_charger/API.hpp"
#include "iso15118_charger/codec.hpp"
#include <optional>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace iso15118_charger;
template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(toInternalApi(src.value()))> {
    if (src) {
        return std::make_optional(toInternalApi(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(toExternalApi(src.value()))> {
    if (src) {
        return std::make_optional(toExternalApi(src.value()));
    }
    return std::nullopt;
}

} // namespace

namespace iso15118_charger {

CertificateActionEnum_Internal toInternalApi(CertificateActionEnum_External const& val) {
    using SrcT = CertificateActionEnum_External;
    using TarT = CertificateActionEnum_Internal;

    switch (val) {
    case SrcT::Install:
        return TarT::Install;
    case SrcT::Update:
        return TarT::Update;
    }

    throw std::out_of_range("Unexpected value for CertificateActionEnum_External");
}

CertificateActionEnum_External toExternalApi(CertificateActionEnum_Internal const& val) {
    using SrcT = CertificateActionEnum_Internal;
    using TarT = CertificateActionEnum_External;

    switch (val) {
    case SrcT::Install:
        return TarT::Install;
    case SrcT::Update:
        return TarT::Update;
    }

    throw std::out_of_range("Unexpected value for CertificateActionEnum_Internal");
}

Status_Internal toInternalApi(Status_External const& val) {
    using SrcT = Status_External;
    using TarT = Status_Internal;

    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Failed:
        return TarT::Failed;
    }

    throw std::out_of_range("Unexpected value for Status_External");
}

Status_External toExternalApi(Status_Internal const& val) {
    using SrcT = Status_Internal;
    using TarT = Status_External;

    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Failed:
        return TarT::Failed;
    }

    throw std::out_of_range("Unexpected value for Status_Internal");
}

RequestExiStreamSchema_Internal toInternalApi(RequestExiStreamSchema_External const& val) {
    RequestExiStreamSchema_Internal result;
    result.exi_request = val.exi_request;
    result.iso15118_schema_version = val.iso15118_schema_version;
    result.certificate_action = toInternalApi(val.certificate_action);
    return result;
}

RequestExiStreamSchema_External toExternalApi(RequestExiStreamSchema_Internal const& val) {
    RequestExiStreamSchema_External result;
    result.exi_request = val.exi_request;
    result.iso15118_schema_version = val.iso15118_schema_version;
    result.certificate_action = toExternalApi(val.certificate_action);
    return result;
}

ResponseExiStreamStatus_Internal toInternalApi(ResponseExiStreamStatus_External const& val) {
    ResponseExiStreamStatus_Internal result;
    result.status = toInternalApi(val.status);
    result.certificate_action = toInternalApi(val.certificate_action);
    result.exi_response = val.exi_response;
    return result;
}

ResponseExiStreamStatus_External toExternalApi(ResponseExiStreamStatus_Internal const& val) {
    ResponseExiStreamStatus_External result;
    result.status = toExternalApi(val.status);
    result.certificate_action = toExternalApi(val.certificate_action);
    result.exi_response = val.exi_response;
    return result;
}

} // namespace iso15118_charger
} // namespace everest::lib::API::V1_0::types
