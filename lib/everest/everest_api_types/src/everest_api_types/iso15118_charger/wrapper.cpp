// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "iso15118_charger/wrapper.hpp"
#include "iso15118_charger/API.hpp"
#include <optional>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace iso15118_charger;
template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    if (src) {
        return std::make_optional(to_internal_api(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    if (src) {
        return std::make_optional(to_external_api(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}

} // namespace

namespace iso15118_charger {

CertificateActionEnum_Internal to_internal_api(CertificateActionEnum_External const& val) {
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

CertificateActionEnum_External to_external_api(CertificateActionEnum_Internal const& val) {
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

EnergyTransferMode_Internal to_internal_api(EnergyTransferMode_External const& val) {
    using SrcT = EnergyTransferMode_External;
    using TarT = EnergyTransferMode_Internal;

    switch (val) {
    case SrcT::AC_single_phase_core:
        return TarT::AC_single_phase_core;
    case SrcT::AC_two_phase:
        return TarT::AC_two_phase;
    case SrcT::AC_three_phase_core:
        return TarT::AC_three_phase_core;
    case SrcT::DC_core:
        return TarT::DC_core;
    case SrcT::DC_extended:
        return TarT::DC_extended;
    case SrcT::DC_combo_core:
        return TarT::DC_combo_core;
    case SrcT::DC_unique:
        return TarT::DC_unique;
    case SrcT::DC:
        return TarT::DC;
    case SrcT::AC_BPT:
        return TarT::AC_BPT;
    case SrcT::AC_BPT_DER:
        return TarT::AC_BPT_DER;
    case SrcT::AC_DER:
        return TarT::AC_DER;
    case SrcT::DC_BPT:
        return TarT::DC_BPT;
    case SrcT::DC_ACDP:
        return TarT::DC_ACDP;
    case SrcT::DC_ACDP_BPT:
        return TarT::DC_ACDP_BPT;
    case SrcT::WPT:
        return TarT::WPT;
    case SrcT::MCS:
        return TarT::MCS;
    case SrcT::MCS_BPT:
        return TarT::MCS_BPT;
    }

    throw std::out_of_range("Unexpected value for EnergyTransferMode_External");
}

EnergyTransferMode_External to_external_api(EnergyTransferMode_Internal const& val) {
    using SrcT = EnergyTransferMode_Internal;
    using TarT = EnergyTransferMode_External;

    switch (val) {
    case SrcT::AC_single_phase_core:
        return TarT::AC_single_phase_core;
    case SrcT::AC_two_phase:
        return TarT::AC_two_phase;
    case SrcT::AC_three_phase_core:
        return TarT::AC_three_phase_core;
    case SrcT::DC_core:
        return TarT::DC_core;
    case SrcT::DC_extended:
        return TarT::DC_extended;
    case SrcT::DC_combo_core:
        return TarT::DC_combo_core;
    case SrcT::DC_unique:
        return TarT::DC_unique;
    case SrcT::DC:
        return TarT::DC;
    case SrcT::AC_BPT:
        return TarT::AC_BPT;
    case SrcT::AC_BPT_DER:
        return TarT::AC_BPT_DER;
    case SrcT::AC_DER:
        return TarT::AC_DER;
    case SrcT::DC_BPT:
        return TarT::DC_BPT;
    case SrcT::DC_ACDP:
        return TarT::DC_ACDP;
    case SrcT::DC_ACDP_BPT:
        return TarT::DC_ACDP_BPT;
    case SrcT::WPT:
        return TarT::WPT;
    case SrcT::MCS:
        return TarT::MCS;
    case SrcT::MCS_BPT:
        return TarT::MCS_BPT;
    }

    throw std::out_of_range("Unexpected value for EnergyTransferMode_Internal");
}

Status_Internal to_internal_api(Status_External const& val) {
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

Status_External to_external_api(Status_Internal const& val) {
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

RequestExiStreamSchema_Internal to_internal_api(RequestExiStreamSchema_External const& val) {
    RequestExiStreamSchema_Internal result;
    result.exi_request = val.exi_request;
    result.iso15118_schema_version = val.iso15118_schema_version;
    result.certificate_action = to_internal_api(val.certificate_action);
    return result;
}

RequestExiStreamSchema_External to_external_api(RequestExiStreamSchema_Internal const& val) {
    RequestExiStreamSchema_External result;
    result.exi_request = val.exi_request;
    result.iso15118_schema_version = val.iso15118_schema_version;
    result.certificate_action = to_external_api(val.certificate_action);
    return result;
}

ResponseExiStreamStatus_Internal to_internal_api(ResponseExiStreamStatus_External const& val) {
    ResponseExiStreamStatus_Internal result;
    result.status = to_internal_api(val.status);
    result.certificate_action = to_internal_api(val.certificate_action);
    result.exi_response = val.exi_response;
    return result;
}

ResponseExiStreamStatus_External to_external_api(ResponseExiStreamStatus_Internal const& val) {
    ResponseExiStreamStatus_External result;
    result.status = to_external_api(val.status);
    result.certificate_action = to_external_api(val.certificate_action);
    result.exi_response = val.exi_response;
    return result;
}

EnergyTransferModeList_Internal to_internal_api(EnergyTransferModeList_External const& val) {
    EnergyTransferModeList_Internal result;
    result = vecToInternal(val.modes);
    return result;
}

EnergyTransferModeList_External to_external_api(EnergyTransferModeList_Internal const& val) {
    EnergyTransferModeList_External result;
    result.modes = vecToExternal(val);
    return result;
}

} // namespace iso15118_charger
} // namespace everest::lib::API::V1_0::types
