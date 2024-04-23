// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/dc_pre_charge.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_dc_DC_PreChargeReqType& in, DC_PreChargeRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.EVProcessing, out.processing);
    convert(in.EVPresentVoltage, out.present_voltage);
    convert(in.EVTargetVoltage, out.target_voltage);
}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_PreChargeReqType& in) {
    va.insert_type<DC_PreChargeRequest>(in);
}

template <> void convert(const DC_PreChargeResponse& in, struct iso20_dc_DC_PreChargeResType& out) {
    init_iso20_dc_DC_PreChargeResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    convert(in.present_voltage, out.EVSEPresentVoltage);
}

template <> int serialize_to_exi(const DC_PreChargeResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_PreChargeRes);

    convert(in, doc.DC_PreChargeRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_PreChargeResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
