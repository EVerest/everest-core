// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/variant.hpp>

#include <cassert>
#include <string>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/app_handshake/appHand_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Decoder.h>

using PayloadType = iso15118::io::v2gtp::PayloadType;

namespace iso15118::message_20 {

static void handle_sap(VariantAccess& va) {
    appHand_exiDocument doc;

    const auto decode_status = decode_appHand_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_appHand_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.supportedAppProtocolReq_isUsed) {
        insert_type(va, doc.supportedAppProtocolReq);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_main(VariantAccess& va) {
    iso20_exiDocument doc;

    const auto decode_status = decode_iso20_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.SessionSetupReq_isUsed) {
        insert_type(va, doc.SessionSetupReq);
    } else if (doc.SessionSetupRes_isUsed) {
        insert_type(va, doc.SessionSetupRes);
    } else if (doc.AuthorizationSetupReq_isUsed) {
        insert_type(va, doc.AuthorizationSetupReq);
    } else if (doc.AuthorizationSetupRes_isUsed) {
        insert_type(va, doc.AuthorizationSetupRes);
    } else if (doc.AuthorizationReq_isUsed) {
        insert_type(va, doc.AuthorizationReq);
    } else if (doc.AuthorizationRes_isUsed) {
        insert_type(va, doc.AuthorizationRes);
    } else if (doc.ServiceDiscoveryReq_isUsed) {
        insert_type(va, doc.ServiceDiscoveryReq);
    } else if (doc.ServiceDiscoveryRes_isUsed) {
        insert_type(va, doc.ServiceDiscoveryRes);
    } else if (doc.ServiceDetailReq_isUsed) {
        insert_type(va, doc.ServiceDetailReq);
    } else if (doc.ServiceDetailRes_isUsed) {
        insert_type(va, doc.ServiceDetailRes);
    } else if (doc.ServiceSelectionReq_isUsed) {
        insert_type(va, doc.ServiceSelectionReq);
    } else if (doc.ServiceSelectionRes_isUsed) {
        insert_type(va, doc.ServiceSelectionRes);
    } else if (doc.ScheduleExchangeReq_isUsed) {
        insert_type(va, doc.ScheduleExchangeReq);
    } else if (doc.ScheduleExchangeRes_isUsed) {
        insert_type(va, doc.ScheduleExchangeRes);
    } else if (doc.PowerDeliveryReq_isUsed) {
        insert_type(va, doc.PowerDeliveryReq);
    } else if (doc.SessionStopReq_isUsed) {
        insert_type(va, doc.SessionStopReq);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_dc(VariantAccess& va) {
    iso20_dc_exiDocument doc;

    const auto decode_status = decode_iso20_dc_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_dc_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.DC_ChargeParameterDiscoveryReq_isUsed) {
        insert_type(va, doc.DC_ChargeParameterDiscoveryReq);
    } else if (doc.DC_CableCheckReq_isUsed) {
        insert_type(va, doc.DC_CableCheckReq);
    } else if (doc.DC_PreChargeReq_isUsed) {
        insert_type(va, doc.DC_PreChargeReq);
    } else if (doc.DC_ChargeLoopReq_isUsed) {
        insert_type(va, doc.DC_ChargeLoopReq);
    } else if (doc.DC_WeldingDetectionReq_isUsed) {
        insert_type(va, doc.DC_WeldingDetectionReq);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_ac(VariantAccess& va) {
    iso20_ac_exiDocument doc;

    const auto decode_status = decode_iso20_ac_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_dc_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.AC_ChargeParameterDiscoveryReq_isUsed) {
        insert_type(va, doc.AC_ChargeParameterDiscoveryReq);
    } else if (doc.AC_ChargeLoopReq_isUsed) {
        insert_type(va, doc.AC_ChargeLoopReq);
    } else {
        va.error = "chosen message type unhandled";
    }
}

Variant::Variant(io::v2gtp::PayloadType payload_type, const io::StreamInputView& buffer_view) {

    VariantAccess va{
        get_exi_input_stream(buffer_view), this->data, this->type, this->custom_deleter, this->error,
    };

    if (payload_type == PayloadType::SAP) {
        handle_sap(va);
    } else if (payload_type == PayloadType::Part20Main) {
        handle_main(va);
    } else if (payload_type == PayloadType::Part20DC) {
        handle_dc(va);
    } else if (payload_type == PayloadType::Part20AC) {
        handle_ac(va);
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

} // namespace iso15118::message_20
