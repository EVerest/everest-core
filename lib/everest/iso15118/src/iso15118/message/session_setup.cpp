// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/session_setup.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Datatypes.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

//
// conversions
//
template <> void convert(const struct iso20_SessionSetupReqType& in, SessionSetupRequest& out) {
    convert(in.Header, out.header);
    out.evccid = CB2CPP_STRING(in.EVCCID);
}

template <> void convert(const struct iso20_SessionSetupResType& in, SessionSetupResponse& out) {
    convert(in.Header, out.header);
    out.evseid = CB2CPP_STRING(in.EVSEID);
}

template <> void convert(const SessionSetupResponse& in, iso20_SessionSetupResType& out) {
    init_iso20_SessionSetupResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    CPP2CB_STRING(in.evseid, out.EVSEID);

    convert(in.header, out.Header);
}

template <> void convert(const SessionSetupRequest& in, iso20_SessionSetupReqType& out) {
    init_iso20_SessionSetupReqType(&out);

    CPP2CB_STRING(in.evccid, out.EVCCID);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_SessionSetupReqType& in) {
    va.insert_type<SessionSetupRequest>(in);
};

template <> void insert_type(VariantAccess& va, const struct iso20_SessionSetupResType& in) {
    va.insert_type<SessionSetupResponse>(in);
};

template <> int serialize_to_exi(const SessionSetupResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);
    CB_SET_USED(doc.SessionSetupRes);

    convert(in, doc.SessionSetupRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const SessionSetupRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);
    CB_SET_USED(doc.SessionSetupReq);

    convert(in, doc.SessionSetupReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const SessionSetupResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const SessionSetupRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
