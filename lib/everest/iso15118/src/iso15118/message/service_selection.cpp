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
        auto& vas_list_out = out.selected_vas_list.emplace();
        vas_list_out.reserve(in.SelectedVASList.SelectedService.arrayLen);

        for (size_t i = 0; i < in.SelectedVASList.SelectedService.arrayLen; i++) {
            const auto& item_in = in.SelectedVASList.SelectedService.array[i];
            auto& item_out = vas_list_out.emplace_back();

            item_out.service_id = item_in.ServiceID;
            item_out.parameter_set_id = item_in.ParameterSetID;
        }
    }
}

template <> void convert(const struct iso20_ServiceSelectionResType& in, ServiceSelectionResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    convert(in.Header, out.header);
}

template <> void convert(const ServiceSelectionRequest& in, iso20_ServiceSelectionReqType& out) {
    init_iso20_ServiceSelectionReqType(&out);

    cb_convert_enum(in.selected_energy_transfer_service.service_id, out.SelectedEnergyTransferService.ServiceID);
    out.SelectedEnergyTransferService.ParameterSetID = in.selected_energy_transfer_service.parameter_set_id;

    if (in.selected_vas_list.has_value()) {
        out.SelectedVASList_isUsed = true;
        out.SelectedVASList.SelectedService.arrayLen = in.selected_vas_list->size();
        for (size_t i = 0; i < in.selected_vas_list->size(); i++) {
            const auto& item_in = in.selected_vas_list->at(i);
            auto& item_out = out.SelectedVASList.SelectedService.array[i];
            item_out.ServiceID = item_in.service_id;
            item_out.ParameterSetID = item_in.parameter_set_id;
        }
    } else {
        out.SelectedVASList_isUsed = false;
    }
    convert(in.header, out.Header);
}

template <> void convert(const ServiceSelectionResponse& in, iso20_ServiceSelectionResType& out) {
    init_iso20_ServiceSelectionResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceSelectionReqType& in) {
    va.insert_type<ServiceSelectionRequest>(in);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceSelectionResType& in) {
    va.insert_type<ServiceSelectionResponse>(in);
};

template <> int serialize_to_exi(const ServiceSelectionResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceSelectionRes);

    convert(in, doc.ServiceSelectionRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const ServiceSelectionRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceSelectionReq);

    convert(in, doc.ServiceSelectionReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceSelectionResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const ServiceSelectionRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
