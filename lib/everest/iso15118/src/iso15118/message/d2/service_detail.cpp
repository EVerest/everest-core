// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <cstdint>
#include <iso15118/message/d2/service_detail.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>
#include <variant>

namespace iso15118::d2::msg {

template <> void convert(const data_types::ServiceParameterList& in, struct iso2_ServiceParameterListType& out) {
    const auto parameter_set_length = std::min(iso2_ParameterSetType_5_ARRAY_SIZE, (int)in.size());
    for (int i = 0; i < parameter_set_length; i++) {
        const auto& in_parameter_set = in.at(i);
        auto& out_parameter_set = out.ParameterSet.array[i];
        out_parameter_set.ParameterSetID = in_parameter_set.parameter_set_id;

        const auto parameter_length =
            std::min(iso2_ParameterType_16_ARRAY_SIZE, (int)in_parameter_set.parameter.size());
        for (int j = 0; j < parameter_length; j++) {
            const auto& in_param = in_parameter_set.parameter.at(j);
            auto& out_param = out_parameter_set.Parameter.array[j];
            CPP2CB_STRING(in_param.name, out_param.Name);

            if (std::holds_alternative<bool>(in_param.value)) {
                out_param.boolValue = std::get<bool>(in_param.value);
                out_param.boolValue_isUsed = true;
            } else if (std::holds_alternative<int8_t>(in_param.value)) {
                out_param.byteValue = std::get<int8_t>(in_param.value);
                out_param.byteValue_isUsed = true;
            } else if (std::holds_alternative<int16_t>(in_param.value)) {
                out_param.shortValue = std::get<int16_t>(in_param.value);
                out_param.shortValue_isUsed = true;
            } else if (std::holds_alternative<int32_t>(in_param.value)) {
                out_param.intValue = std::get<int32_t>(in_param.value);
                out_param.intValue_isUsed = true;
            } else if (std::holds_alternative<data_types::PhysicalValue>(in_param.value)) {
                convert(std::get<data_types::PhysicalValue>(in_param.value), out_param.physicalValue);
                out_param.physicalValue_isUsed = true;
            } else if (std::holds_alternative<std::string>(in_param.value)) {
                CPP2CB_STRING(std::get<std::string>(in_param.value), out_param.stringValue);
                out_param.stringValue_isUsed = true;
            }
        }
        out_parameter_set.Parameter.arrayLen = parameter_length;
    }
    out.ParameterSet.arrayLen = parameter_set_length;
}

template <> void convert(const struct iso2_ServiceDetailReqType& in, ServiceDetailRequest& out) {
    out.service_id = in.ServiceID;
}

template <>
void insert_type(VariantAccess& va, const struct iso2_ServiceDetailReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<ServiceDetailRequest>(in, header);
}

template <> void convert(const ServiceDetailResponse& in, struct iso2_ServiceDetailResType& out) {
    init_iso2_ServiceDetailResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    out.ServiceID = in.service_id;

    CPP2CB_CONVERT_IF_USED(in.service_parameter_list, out.ServiceParameterList);
}

template <> int serialize_to_exi(const ServiceDetailResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.ServiceDetailRes);
    convert(in, doc.V2G_Message.Body.ServiceDetailRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceDetailResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
