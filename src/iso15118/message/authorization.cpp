// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/authorization.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_AuthorizationReqType& in, AuthorizationRequest& out) {
    convert(in.Header, out.header);

    out.selected_authorization_service = static_cast<Authorization>(in.SelectedAuthorizationService);
    if (in.EIM_AReqAuthorizationMode_isUsed) {
        out.eim_as_req_authorization_mode.emplace();
    } else if (in.PnC_AReqAuthorizationMode_isUsed) {

        auto& pnc_out = out.pnc_as_req_authorization_mode.emplace();

        pnc_out.id = CB2CPP_STRING(in.PnC_AReqAuthorizationMode.Id);
        pnc_out.gen_challenge.reserve(in.PnC_AReqAuthorizationMode.GenChallenge.bytesLen);
        pnc_out.gen_challenge.insert(
            pnc_out.gen_challenge.end(), &in.PnC_AReqAuthorizationMode.GenChallenge.bytes[0],
            &in.PnC_AReqAuthorizationMode.GenChallenge.bytes[in.PnC_AReqAuthorizationMode.GenChallenge.bytesLen]);
        // Todo(sl): Adding certificate
    }
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

template <> int serialize_to_exi(const AuthorizationResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.AuthorizationRes);

    convert(in, doc.AuthorizationRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const AuthorizationResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
