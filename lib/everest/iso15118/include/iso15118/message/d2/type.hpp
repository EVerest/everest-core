// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/io/stream_view.hpp>

namespace iso15118::d2::msg {

enum class Type {
    None,
    SupportedAppProtocolReq,
    SupportedAppProtocolRes,
    SessionSetupReq,
    SessionSetupRes,
    ServiceDiscoveryReq,
    ServiceDiscoveryRes,
    ServiceDetailReq,
    ServiceDetailRes,
    PaymentServiceSelectionReq,
    PaymentServiceSelectionRes,
    AuthorizationReq,
    AuthorizationRes,
    ChargeParameterDiscoveryReq,
    ChargeParameterDiscoveryRes,
    PowerDeliveryReq,
    PowerDeliveryRes,
    CableCheckReq,
    CableCheckRes,
    PreChargeReq,
    PreChargeRes,
    CurrentDemandReq,
    CurrentDemandRes,
    WeldingDetectionReq,
    WeldingDetectionRes
};

template <typename T> struct TypeTrait {
    static const Type type = Type::None;
};

template <typename InType, typename OutType> void convert(const InType&, OutType&);

template <typename MessageType> size_t serialize(const MessageType&, const io::StreamOutputView&);

//
// definitions of type traits
//
#ifdef CREATE_TYPE_TRAIT
#define CREATE_TYPE_TRAIT_PUSHED CREATE_TYPE_TRAIT
#endif

#define CREATE_TYPE_TRAIT(struct_name, enum_name)                                                                      \
    struct struct_name;                                                                                                \
    template <> struct TypeTrait<struct_name> {                                                                        \
        static const Type type = Type::enum_name;                                                                      \
    }

CREATE_TYPE_TRAIT(SupportedAppProtocolRequest, SupportedAppProtocolReq);
CREATE_TYPE_TRAIT(SupportedAppProtocolResponse, SupportedAppProtocolRes);
CREATE_TYPE_TRAIT(SessionSetupRequest, SessionSetupReq);
CREATE_TYPE_TRAIT(SessionSetupResponse, SessionSetupRes);
CREATE_TYPE_TRAIT(ServiceDiscoveryRequest, ServiceDiscoveryReq);
CREATE_TYPE_TRAIT(ServiceDiscoveryResponse, ServiceDiscoveryRes);
CREATE_TYPE_TRAIT(ServiceDetailRequest, ServiceDetailReq);
CREATE_TYPE_TRAIT(ServiceDetailResponse, ServiceDetailRes);
CREATE_TYPE_TRAIT(PaymentServiceSelectionRequest, PaymentServiceSelectionReq);
CREATE_TYPE_TRAIT(PaymentServiceSelectionResponse, PaymentServiceSelectionRes);
CREATE_TYPE_TRAIT(AuthorizationRequest, AuthorizationReq);
CREATE_TYPE_TRAIT(AuthorizationResponse, AuthorizationRes);
CREATE_TYPE_TRAIT(ChargeParameterDiscoveryRequest, ChargeParameterDiscoveryReq);
CREATE_TYPE_TRAIT(ChargeParameterDiscoveryResponse, ChargeParameterDiscoveryRes);
CREATE_TYPE_TRAIT(PowerDeliveryRequest, PowerDeliveryReq);
CREATE_TYPE_TRAIT(PowerDeliveryResponse, PowerDeliveryRes);
CREATE_TYPE_TRAIT(DC_CableCheckRequest, CableCheckReq);
CREATE_TYPE_TRAIT(DC_CableCheckResponse, CableCheckRes);
CREATE_TYPE_TRAIT(DC_PreChargeRequest, PreChargeReq);
CREATE_TYPE_TRAIT(DC_PreChargeResponse, PreChargeRes);
CREATE_TYPE_TRAIT(DC_CurrentDemandRequest, CurrentDemandReq);
CREATE_TYPE_TRAIT(DC_CurrentDemandResponse, CurrentDemandRes);
CREATE_TYPE_TRAIT(DC_WeldingDetectionRequest, WeldingDetectionReq);
CREATE_TYPE_TRAIT(DC_WeldingDetectionResponse, WeldingDetectionRes);

#ifdef CREATE_TYPE_TRAIT_PUSHED
#define CREATE_TYPE_TRAIT CREATE_TYPE_TRAIT_PUSHED
#else
#undef CREATE_TYPE_TRAIT
#endif

} // namespace iso15118::d2::msg
