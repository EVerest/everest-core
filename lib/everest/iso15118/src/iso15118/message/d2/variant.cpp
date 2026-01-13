// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/variant.hpp>

#include <cassert>
#include <string>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/app_handshake/appHand_Decoder.h>
#include <cbv2g/iso_2/iso2_msgDefDecoder.h>

using PayloadType = iso15118::io::v2gtp::PayloadType;

namespace iso15118::d2::msg {

namespace {

void handle_sap(VariantAccess& va) {
    appHand_exiDocument doc;

    const auto decode_status = decode_appHand_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_appHand_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.supportedAppProtocolReq_isUsed) {
        // insert_type(va, doc.supportedAppProtocolReq);
    } else {
        va.error = "chosen message type unhandled";
    }
}

void handle_v2g(VariantAccess& va) {
    iso2_exiDocument doc;

    const auto decode_status = decode_iso2_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        return;
    }

    if (doc.V2G_Message.Body.SessionSetupReq_isUsed) {
        insert_type(va, doc.V2G_Message.Body.SessionSetupReq, doc.V2G_Message.Header);
    } else if (doc.V2G_Message.Body.AuthorizationReq_isUsed) {
        insert_type(va, doc.V2G_Message.Body.AuthorizationReq, doc.V2G_Message.Header);
    } else {
        va.error = "chosen message type unhandled";
    }
}
} // namespace

Variant::Variant(io::v2gtp::PayloadType payload_type, const io::StreamInputView& buffer_view,
                 bool supported_app_protocol_msg) {

    VariantAccess va{
        get_exi_input_stream(buffer_view), this->data, this->type, this->custom_deleter, this->error,
    };

    if (payload_type == PayloadType::SAP) {
        if (supported_app_protocol_msg) {
            handle_sap(va);
        } else {
            handle_v2g(va);
        }
    } else {
        logf_warning("Unknown type");
    }

    if (data) {
        // in case data was set, make sure the custom deleter and the type were set!
        assert(custom_deleter != nullptr);
        assert(type != Type::None);
    } else {
        logf_error("Failed due to: %s\n", error.c_str());
    }
}

Variant::~Variant() {
    if (data) {
        custom_deleter(data);
    }
}

Type Variant::get_type() const {
    return type;
}

const std::string& Variant::get_error() const {
    return error;
}

} // namespace iso15118::d2::msg
