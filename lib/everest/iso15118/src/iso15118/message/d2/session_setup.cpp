// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/session_setup.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_SessionSetupReqType& in, SessionSetupRequest& out) {
    std::copy(in.EVCCID.bytes, in.EVCCID.bytes + in.EVCCID.bytesLen, out.evcc_id.begin());
}

template <>
void insert_type(VariantAccess& va, const struct iso2_SessionSetupReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<SessionSetupRequest>(in, header);
}

template <> void convert(const SessionSetupResponse& in, struct iso2_SessionSetupResType& out) {
    init_iso2_SessionSetupResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    CPP2CB_STRING(in.evse_id, out.EVSEID);
    CPP2CB_ASSIGN_IF_USED(in.timestamp, out.EVSETimeStamp);
}

template <> int serialize_to_exi(const SessionSetupResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.SessionSetupRes);
    convert(in, doc.V2G_Message.Body.SessionSetupRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const SessionSetupResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
