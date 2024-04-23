// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/session_stop.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_SessionStopReqType& in, SessionStopRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.ChargingSession, out.charging_session);
    if (in.EVTerminationCode_isUsed) {
        out.ev_termination_code = CB2CPP_STRING(in.EVTerminationCode);
    }
    if (in.EVTerminationExplanation_isUsed) {
        out.ev_termination_explanation = CB2CPP_STRING(in.EVTerminationExplanation);
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_SessionStopReqType& in) {
    va.insert_type<SessionStopRequest>(in);
}

template <> void convert(const SessionStopResponse& in, struct iso20_SessionStopResType& out) {
    init_iso20_SessionStopResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
}

template <> int serialize_to_exi(const SessionStopResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.SessionStopRes);

    convert(in, doc.SessionStopRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const SessionStopResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20