// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

// FIXME (aw): how to streamline this with type.hpp?

#include <iso15118/io/sdp.hpp>

namespace iso15118::message_20 {

template <typename T> struct PayloadTypeTrait;

//
// definitions of type traits
//
#ifdef CREATE_TYPE_TRAIT
#define CREATE_TYPE_TRAIT_PUSHED CREATE_TYPE_TRAIT
#endif

#define CREATE_TYPE_TRAIT(struct_name, payload_type)                                                                   \
    struct struct_name;                                                                                                \
    template <> struct PayloadTypeTrait<struct_name> {                                                                 \
        static const io::v2gtp::PayloadType type = io::v2gtp::PayloadType::payload_type;                               \
    }

CREATE_TYPE_TRAIT(SupportedAppProtocolResponse, SAP);
CREATE_TYPE_TRAIT(SessionSetupResponse, Part20Main);
CREATE_TYPE_TRAIT(AuthorizationSetupResponse, Part20Main);
CREATE_TYPE_TRAIT(AuthorizationResponse, Part20Main);
CREATE_TYPE_TRAIT(ServiceDiscoveryResponse, Part20Main);
CREATE_TYPE_TRAIT(ServiceDetailResponse, Part20Main);
CREATE_TYPE_TRAIT(ServiceSelectionResponse, Part20Main);
CREATE_TYPE_TRAIT(DC_ChargeParameterDiscoveryResponse, Part20DC);
CREATE_TYPE_TRAIT(ScheduleExchangeResponse, Part20Main);
CREATE_TYPE_TRAIT(DC_CableCheckResponse, Part20DC);
CREATE_TYPE_TRAIT(DC_PreChargeResponse, Part20DC);
CREATE_TYPE_TRAIT(PowerDeliveryResponse, Part20Main);
CREATE_TYPE_TRAIT(DC_ChargeLoopResponse, Part20DC);
CREATE_TYPE_TRAIT(DC_WeldingDetectionResponse, Part20DC);
CREATE_TYPE_TRAIT(SessionStopResponse, Part20Main);
CREATE_TYPE_TRAIT(AC_ChargeParameterDiscoveryResponse, Part20AC);
CREATE_TYPE_TRAIT(AC_ChargeLoopResponse, Part20AC);

#ifdef CREATE_TYPE_TRAIT_PUSHED
#define CREATE_TYPE_TRAIT CREATE_TYPE_TRAIT_PUSHED
#else
#undef CREATE_TYPE_TRAIT
#endif

} // namespace iso15118::message_20
