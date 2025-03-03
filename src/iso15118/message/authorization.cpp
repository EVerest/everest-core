// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/authorization.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_AuthorizationReqType& in, AuthorizationRequest& out) {
    convert(in.Header, out.header);

    out.selected_authorization_service = static_cast<datatypes::Authorization>(in.SelectedAuthorizationService);
    if (in.EIM_AReqAuthorizationMode_isUsed) {
        out.authorization_mode.emplace<datatypes::EIM_ASReqAuthorizationMode>();
    } else if (in.PnC_AReqAuthorizationMode_isUsed) {

        auto& pnc_out = out.authorization_mode.emplace<datatypes::PnC_ASReqAuthorizationMode>();

        pnc_out.id = CB2CPP_STRING(in.PnC_AReqAuthorizationMode.Id);
        CB2CPP_BYTES(in.PnC_AReqAuthorizationMode.GenChallenge, pnc_out.gen_challenge);
        // Todo(sl): Adding certificate
    }
}

template <> void convert(const struct iso20_AuthorizationResType& in, AuthorizationResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    cb_convert_enum(in.EVSEProcessing, out.evse_processing);

    convert(in.Header, out.header);
}

template <> void convert(const AuthorizationRequest& in, iso20_AuthorizationReqType& out) {
    init_iso20_AuthorizationReqType(&out);

    convert(in.header, out.Header);

    cb_convert_enum(in.selected_authorization_service, out.SelectedAuthorizationService);

    // Todo(rb): add pnc
    out.EIM_AReqAuthorizationMode_isUsed = true;
}

template <> void convert(const AuthorizationResponse& in, iso20_AuthorizationResType& out) {
    init_iso20_AuthorizationResType(&out);

    out.ResponseCode = static_cast<iso20_responseCodeType>(in.response_code);
    out.EVSEProcessing = static_cast<iso20_processingType>(in.evse_processing);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_AuthorizationReqType& in) {
    va.insert_type<AuthorizationRequest>(in);
};

template <> void insert_type(VariantAccess& va, const struct iso20_AuthorizationResType& in) {
    va.insert_type<AuthorizationResponse>(in);
};

template <> int serialize_to_exi(const AuthorizationResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.AuthorizationRes);

    convert(in, doc.AuthorizationRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const AuthorizationRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.AuthorizationReq);

    convert(in, doc.AuthorizationReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const AuthorizationResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const AuthorizationRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
