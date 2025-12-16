// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/authorization.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_AuthorizationReqType& in, AuthorizationRequest& out) {
    std::copy(in.Id.characters, in.Id.characters + in.Id.charactersLen, out.id.begin());
    if (in.GenChallenge_isUsed) {
        std::copy(in.GenChallenge.bytes, in.GenChallenge.bytes + in.GenChallenge.bytesLen,
                  out.gen_challenge.value().begin());
    }
}

template <>
void insert_type(VariantAccess& va, const struct iso2_AuthorizationReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<AuthorizationRequest>(in, header);
}

template <> void convert(const AuthorizationResponse& in, struct iso2_AuthorizationResType& out) {
    init_iso2_AuthorizationResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.evse_processing, out.EVSEProcessing);
}

template <> int serialize_to_exi(const AuthorizationResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.AuthorizationRes);
    convert(in, doc.V2G_Message.Body.AuthorizationRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const AuthorizationResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
