// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/dc_pre_charge.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_PreChargeReqType& in, DC_PreChargeRequest& out) {
    convert(in.DC_EVStatus, out.ev_status);
    convert(in.EVTargetVoltage, out.ev_target_voltage);
    convert(in.EVTargetCurrent, out.ev_target_current);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_PreChargeReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<DC_PreChargeRequest>(in, header);
}

template <> void convert(const DC_PreChargeResponse& in, struct iso2_PreChargeResType& out) {
    init_iso2_PreChargeResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    convert(in.evse_status, out.DC_EVSEStatus);
    convert(in.evse_present_voltage, out.EVSEPresentVoltage);
}

template <> int serialize_to_exi(const DC_PreChargeResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.PreChargeRes);
    convert(in, doc.V2G_Message.Body.PreChargeRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_PreChargeResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
