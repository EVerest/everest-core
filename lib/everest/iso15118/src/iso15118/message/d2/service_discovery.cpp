// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <cstddef>
#include <iso15118/message/d2/service_discovery.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const data_types::Service& in, struct iso2_ServiceType& out) {
    out.ServiceID = in.service_id;
    CPP2CB_STRING_IF_USED(in.service_name, out.ServiceName);
    cb_convert_enum(in.service_category, out.ServiceCategory);
    CPP2CB_STRING_IF_USED(in.service_scope, out.ServiceScope);
    out.FreeService = in.FreeService;
}

template <> void convert(const data_types::ChargeService& in, struct iso2_ChargeServiceType& out) {
    out.ServiceID = in.service_id;
    CPP2CB_STRING_IF_USED(in.service_name, out.ServiceName);
    cb_convert_enum(in.service_category, out.ServiceCategory);
    CPP2CB_STRING_IF_USED(in.service_scope, out.ServiceScope);
    out.FreeService = in.FreeService;

    const auto supported_energy_transfer_mode_length = std::min(
        static_cast<size_t>(iso2_EnergyTransferModeType_6_ARRAY_SIZE), in.supported_energy_transfer_mode.size());
    for (size_t i = 0; i < supported_energy_transfer_mode_length; i++) {
        cb_convert_enum(in.supported_energy_transfer_mode.at(i),
                        out.SupportedEnergyTransferMode.EnergyTransferMode.array[i]);
    }
    out.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen = supported_energy_transfer_mode_length;
}

template <> void convert(const struct iso2_ServiceDiscoveryReqType& in, ServiceDiscoveryRequest& out) {
    if (in.ServiceCategory_isUsed) {
        out.service_category.emplace();
        cb_convert_enum(in.ServiceCategory, out.service_category.value());
    }
    CB2CPP_STRING_IF_USED(in.ServiceScope, out.service_scope);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_ServiceDiscoveryReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<ServiceDiscoveryRequest>(in, header);
}

template <> void convert(const ServiceDiscoveryResponse& in, struct iso2_ServiceDiscoveryResType& out) {
    init_iso2_ServiceDiscoveryResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    const auto payment_option_length =
        std::min(static_cast<size_t>(iso2_paymentOptionType_2_ARRAY_SIZE), in.payment_option_list.size());
    for (size_t i = 0; i < payment_option_length; i++) {
        cb_convert_enum(in.payment_option_list.at(i), out.PaymentOptionList.PaymentOption.array[i]);
    }
    out.PaymentOptionList.PaymentOption.arrayLen = payment_option_length;

    convert(in.charge_service, out.ChargeService);

    if (in.service_list) {
        const auto service_list_length =
            std::min(static_cast<size_t>(iso2_ServiceType_8_ARRAY_SIZE), in.service_list->size());
        for (size_t i = 0; i < service_list_length; i++) {
            convert(in.service_list->at(i), out.ServiceList.Service.array[i]);
        }
        out.ServiceList.Service.arrayLen = service_list_length;
        CB_SET_USED(out.ServiceList);
    }
}

template <> int serialize_to_exi(const ServiceDiscoveryResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.ServiceDiscoveryRes);
    convert(in, doc.V2G_Message.Body.ServiceDiscoveryRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
