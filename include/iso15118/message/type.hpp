// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/io/stream_view.hpp>

namespace iso15118::message_20 {

enum class Type {
    None,
    SupportedAppProtocolReq,
    SupportedAppProtocolRes,
    SessionSetupReq,
    SessionSetupRes,
    AuthorizationSetupReq,
    AuthorizationSetupRes,
    AuthorizationReq,
    AuthorizationRes,
    ServiceDiscoveryReq,
    ServiceDiscoveryRes,
    ServiceDetailReq,
    ServiceDetailRes,
    ServiceSelectionReq,
    ServiceSelectionRes,
    DC_ChargeParameterDiscoveryReq,
    DC_ChargeParameterDiscoveryRes,
    ScheduleExchangeReq,
    ScheduleExchangeRes,
    DC_CableCheckReq,
    DC_CableCheckRes,
    DC_PreChargeReq,
    DC_PreChargeRes,
    PowerDeliveryReq,
    PowerDeliveryRes,
    DC_ChargeLoopReq,
    DC_ChargeLoopRes,
    DC_WeldingDetectionReq,
    DC_WeldingDetectionRes,
    SessionStopReq,
    SessionStopRes,
    AC_ChargeParameterDiscoveryReq,
    AC_ChargeParameterDiscoveryRes,
    AC_ChargeLoopReq,
    AC_ChargeLoopRes,
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
CREATE_TYPE_TRAIT(SessionSetupRequest, SessionSetupReq);
CREATE_TYPE_TRAIT(AuthorizationSetupRequest, AuthorizationSetupReq);
CREATE_TYPE_TRAIT(AuthorizationRequest, AuthorizationReq);
CREATE_TYPE_TRAIT(ServiceDiscoveryRequest, ServiceDiscoveryReq);
CREATE_TYPE_TRAIT(ServiceDetailRequest, ServiceDetailReq);
CREATE_TYPE_TRAIT(ServiceSelectionRequest, ServiceSelectionReq);
CREATE_TYPE_TRAIT(DC_ChargeParameterDiscoveryRequest, DC_ChargeParameterDiscoveryReq);
CREATE_TYPE_TRAIT(ScheduleExchangeRequest, ScheduleExchangeReq);
CREATE_TYPE_TRAIT(DC_CableCheckRequest, DC_CableCheckReq);
CREATE_TYPE_TRAIT(DC_PreChargeRequest, DC_PreChargeReq);
CREATE_TYPE_TRAIT(PowerDeliveryRequest, PowerDeliveryReq);
CREATE_TYPE_TRAIT(DC_ChargeLoopRequest, DC_ChargeLoopReq);
CREATE_TYPE_TRAIT(DC_WeldingDetectionRequest, DC_WeldingDetectionReq);
CREATE_TYPE_TRAIT(SessionStopRequest, SessionStopReq);
CREATE_TYPE_TRAIT(AC_ChargeParameterDiscoveryRequest, AC_ChargeParameterDiscoveryReq);
CREATE_TYPE_TRAIT(AC_ChargeLoopRequest, AC_ChargeLoopReq);

#ifdef CREATE_TYPE_TRAIT_PUSHED
#define CREATE_TYPE_TRAIT CREATE_TYPE_TRAIT_PUSHED
#else
#undef CREATE_TYPE_TRAIT
#endif

} // namespace iso15118::message_20
