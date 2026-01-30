// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/payment_service_selection.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_PaymentServiceSelectionReqType& in, PaymentServiceSelectionRequest& out) {
    cb_convert_enum(in.SelectedPaymentOption, out.selected_payment_option);

    out.selected_service_list.reserve(data_types::SelectedServiceListMaxLength);
    for (int i = 0; i < in.SelectedServiceList.SelectedService.arrayLen; i++) {
        const auto& in_service = in.SelectedServiceList.SelectedService.array[i];
        data_types::SelectedService service;
        service.service_id = in_service.ServiceID;
        if (in_service.ParameterSetID_isUsed) {
            service.parameter_set_id = in_service.ParameterSetID;
        }
        out.selected_service_list.push_back(service);
    }
}

template <>
void insert_type(VariantAccess& va, const struct iso2_PaymentServiceSelectionReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<PaymentServiceSelectionRequest>(in, header);
}

template <> void convert(const PaymentServiceSelectionResponse& in, struct iso2_PaymentServiceSelectionResType& out) {
    init_iso2_PaymentServiceSelectionResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
}

template <> int serialize_to_exi(const PaymentServiceSelectionResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.PaymentServiceSelectionRes);
    convert(in, doc.V2G_Message.Body.PaymentServiceSelectionRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const PaymentServiceSelectionResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
