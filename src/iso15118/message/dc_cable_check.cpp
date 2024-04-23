// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/dc_cable_check.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_dc_DC_CableCheckReqType& in, DC_CableCheckRequest& out) {
    convert(in.Header, out.header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_CableCheckReqType& in) {
    va.insert_type<DC_CableCheckRequest>(in);
}

template <> void convert(const DC_CableCheckResponse& in, struct iso20_dc_DC_CableCheckResType& out) {
    init_iso20_dc_DC_CableCheckResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.processing, out.EVSEProcessing);
}

template <> int serialize_to_exi(const DC_CableCheckResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_CableCheckRes);

    convert(in, doc.DC_CableCheckRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_CableCheckResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
