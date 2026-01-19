// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/metering_receipt.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_MeteringReceiptReqType& in, MeteringReceiptRequest& out) {
    CB2CPP_STRING_IF_USED(in.Id, out.id);
    CB2CPP_BYTES(in.SessionID, out.session_id);
    CB2CPP_ASSIGN_IF_USED(in.SAScheduleTupleID, out.sa_schedule_tuple_id);
    convert(in.MeterInfo, out.meter_info);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_MeteringReceiptReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<MeteringReceiptRequest>(in, header);
}

template <> void convert(const MeteringReceiptResponse& in, struct iso2_MeteringReceiptResType& out) {
    init_iso2_MeteringReceiptResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    CPP2CB_CONVERT_IF_USED(in.ac_evse_status, out.AC_EVSEStatus);
    CPP2CB_CONVERT_IF_USED(in.dc_evse_status, out.DC_EVSEStatus);
}

template <> int serialize_to_exi(const MeteringReceiptResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.MeteringReceiptRes);
    convert(in, doc.V2G_Message.Body.MeteringReceiptRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const MeteringReceiptResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
