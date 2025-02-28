// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/authorization_setup.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_AuthorizationSetupReqType& in, AuthorizationSetupRequest& out) {
    convert(in.Header, out.header);
}

template <> void convert(const struct iso20_AuthorizationSetupResType& in, AuthorizationSetupResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    out.authorization_services.resize(in.AuthorizationServices.arrayLen);

    uint8_t element = 0;
    for (auto const& service : in.AuthorizationServices.array) {
        cb_convert_enum(service, out.authorization_services[element++]);
    }

    out.certificate_installation_service = in.CertificateInstallationService;

    if (in.EIM_ASResAuthorizationMode_isUsed) {
        out.authorization_mode = datatypes::EIM_ASResAuthorizationMode{};
    } else if (in.PnC_ASResAuthorizationMode_isUsed) {
        auto& pnc_out = out.authorization_mode.emplace<datatypes::PnC_ASResAuthorizationMode>();
        CB2CPP_BYTES(in.PnC_ASResAuthorizationMode.GenChallenge, pnc_out.gen_challenge);

        // todo(sl): supported_providers missing
    }

    convert(in.Header, out.header);
}

template <> void convert(const AuthorizationSetupRequest& in, iso20_AuthorizationSetupReqType& out) {
    init_iso20_AuthorizationSetupReqType(&out);

    convert(in.header, out.Header);
}

struct AuthorizationModeVisitor {
    AuthorizationModeVisitor(iso20_AuthorizationSetupResType& out_) : out(out_){};
    void operator()([[maybe_unused]] const datatypes::EIM_ASResAuthorizationMode& in) {
        CB_SET_USED(out.EIM_ASResAuthorizationMode);
        init_iso20_EIM_ASResAuthorizationModeType(&out.EIM_ASResAuthorizationMode);
    }
    void operator()(const datatypes::PnC_ASResAuthorizationMode& in) {
        CB_SET_USED(out.PnC_ASResAuthorizationMode);
        init_iso20_PnC_ASResAuthorizationModeType(&out.PnC_ASResAuthorizationMode);
        CPP2CB_BYTES(in.gen_challenge, out.PnC_ASResAuthorizationMode.GenChallenge);
        // todo(sl): supported_providers missing
    }

private:
    iso20_AuthorizationSetupResType& out;
};

template <> void convert(const AuthorizationSetupResponse& in, iso20_AuthorizationSetupResType& out) {
    init_iso20_AuthorizationSetupResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    out.AuthorizationServices.arrayLen = in.authorization_services.size();

    uint8_t element = 0;
    for (auto const& service : in.authorization_services) {
        cb_convert_enum(service, out.AuthorizationServices.array[element++]);
    }

    out.CertificateInstallationService = in.certificate_installation_service;

    std::visit(AuthorizationModeVisitor(out), in.authorization_mode);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_AuthorizationSetupReqType& in) {
    va.insert_type<AuthorizationSetupRequest>(in);
};

template <> void insert_type(VariantAccess& va, const struct iso20_AuthorizationSetupResType& in) {
    va.insert_type<AuthorizationSetupResponse>(in);
};

template <> int serialize_to_exi(const AuthorizationSetupResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.AuthorizationSetupRes);

    convert(in, doc.AuthorizationSetupRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const AuthorizationSetupRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.AuthorizationSetupReq);

    convert(in, doc.AuthorizationSetupReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const AuthorizationSetupResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const AuthorizationSetupRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
