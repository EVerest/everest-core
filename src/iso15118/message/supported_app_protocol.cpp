// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/supported_app_protocol.hpp>

#include <type_traits>

#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/app_handshake/appHand_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct appHand_supportedAppProtocolReq& in, SupportedAppProtocolRequest& out) {
    const auto& ap_in = in.AppProtocol;
    out.app_protocol.reserve(ap_in.arrayLen);

    for (size_t i = 0; i < ap_in.arrayLen; ++i) {
        const auto& item_in = ap_in.array[i];
        auto& item_out = out.app_protocol.emplace_back();

        item_out.protocol_namespace = CB2CPP_STRING(item_in.ProtocolNamespace);
        item_out.version_number_major = item_in.VersionNumberMajor;
        item_out.version_number_minor = item_in.VersionNumberMinor;
        item_out.schema_id = item_in.SchemaID;
        item_out.priority = item_in.Priority;
    }
}

template <> void convert(const SupportedAppProtocolRequest& in, struct appHand_supportedAppProtocolReq& out) {
    init_appHand_supportedAppProtocolReq(&out);

    auto& ap_out = out.AppProtocol;
    // FIXME (aw): check size constraints
    ap_out.arrayLen = in.app_protocol.size();

    for (size_t i = 0; i < in.app_protocol.size(); ++i) {
        const auto& item_in = in.app_protocol[i];
        auto& item_out = ap_out.array[i];

        CPP2CB_STRING(item_in.protocol_namespace, item_out.ProtocolNamespace);
        item_out.VersionNumberMajor = item_in.version_number_major;
        item_out.VersionNumberMinor = item_in.version_number_minor;
        item_out.SchemaID = item_in.schema_id;
        item_out.Priority = item_in.priority;
    }
}

template <> void convert(const SupportedAppProtocolResponse& in, struct appHand_supportedAppProtocolRes& out) {
    init_appHand_supportedAppProtocolRes(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    if (in.schema_id) {
        out.SchemaID = *in.schema_id;
        CB_SET_USED(out.SchemaID);
    }
}

template <> void insert_type(VariantAccess& va, const struct appHand_supportedAppProtocolReq& in) {
    va.insert_type<SupportedAppProtocolRequest>(in);
};

template <> int serialize_to_exi(const SupportedAppProtocolResponse& in, exi_bitstream_t& out) {
    appHand_exiDocument doc;
    init_appHand_exiDocument(&doc);

    convert(in, doc.supportedAppProtocolRes);

    CB_SET_USED(doc.supportedAppProtocolRes);

    return encode_appHand_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const SupportedAppProtocolRequest& in, exi_bitstream_t& out) {
    appHand_exiDocument doc;
    init_appHand_exiDocument(&doc);

    convert(in, doc.supportedAppProtocolReq);

    CB_SET_USED(doc.supportedAppProtocolReq);

    return encode_appHand_exiDocument(&out, &doc);
}

template <> size_t serialize(const SupportedAppProtocolResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const SupportedAppProtocolRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
