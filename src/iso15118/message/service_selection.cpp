// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/service_selection.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_ServiceSelectionReqType& in, ServiceSelectionRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.SelectedEnergyTransferService.ServiceID, out.selected_energy_transfer_service.service_id);
    out.selected_energy_transfer_service.parameter_set_id = in.SelectedEnergyTransferService.ParameterSetID;

    if (in.SelectedVASList_isUsed == true) {
        out.selected_vas_list.value().reserve(in.SelectedVASList.SelectedService.arrayLen);
        auto& vas_list_out = *out.selected_vas_list;

        for (size_t i = 0; i < in.SelectedVASList.SelectedService.arrayLen; i++) {
            const auto& item_in = in.SelectedVASList.SelectedService.array[i];
            auto& item_out = vas_list_out.emplace_back();

            cb_convert_enum(item_in.ServiceID, item_out.service_id);
            item_out.parameter_set_id = item_in.ParameterSetID;
        }
    }
}

template <> void convert(const ServiceSelectionResponse& in, iso20_ServiceSelectionResType& out) {
    init_iso20_ServiceSelectionResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceSelectionReqType& in) {
    va.insert_type<ServiceSelectionRequest>(in);
}

template <> int serialize_to_exi(const ServiceSelectionResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceSelectionRes);

    convert(in, doc.ServiceSelectionRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceSelectionResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
