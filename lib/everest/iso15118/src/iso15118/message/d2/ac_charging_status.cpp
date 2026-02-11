// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/ac_charging_status.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_ChargingStatusReqType& in, AC_ChargingStatusRequest& out) {
    (void)in;
    (void)out;
}

template <>
void insert_type(VariantAccess& va, const struct iso2_ChargingStatusReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<AC_ChargingStatusRequest>(in, header);
}

template <> void convert(const AC_ChargingStatusResponse& in, struct iso2_ChargingStatusResType& out) {
    init_iso2_ChargingStatusResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    CPP2CB_STRING(in.evse_id, out.EVSEID);
    out.SAScheduleTupleID = in.sa_schedule_tuple_id;
    CPP2CB_CONVERT_IF_USED(in.evse_max_current, out.EVSEMaxCurrent);
    CPP2CB_CONVERT_IF_USED(in.meter_info, out.MeterInfo);
    CPP2CB_ASSIGN_IF_USED(in.receipt_required, out.ReceiptRequired);
    convert(in.ac_evse_status, out.AC_EVSEStatus);
}

template <> int serialize_to_exi(const AC_ChargingStatusResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.ChargingStatusRes);
    convert(in, doc.V2G_Message.Body.ChargingStatusRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const AC_ChargingStatusResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
