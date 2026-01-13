// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/cable_check.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_CableCheckReqType& in, CableCheckRequest& out) {
    convert(in.DC_EVStatus, out.ev_status);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_CableCheckReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<CableCheckRequest>(in, header);
}

template <> void convert(const CableCheckResponse& in, struct iso2_CableCheckResType& out) {
    init_iso2_CableCheckResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    convert(in.evse_status, out.DC_EVSEStatus);
    cb_convert_enum(in.evse_processing, out.EVSEProcessing);
}

template <> int serialize_to_exi(const CableCheckResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.CableCheckRes);
    convert(in, doc.V2G_Message.Body.CableCheckRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const CableCheckResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
