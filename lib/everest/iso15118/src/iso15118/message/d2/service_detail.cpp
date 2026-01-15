// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/service_detail.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const data_types::ServiceParameterList& in, struct iso2_ServiceParameterListType& out) {
    int parameter_set_index = 0;
    for (auto const& parameter_set : in) {
        if (parameter_set_index >= iso2_ParameterSetType_5_ARRAY_SIZE) {
            // TODO(kd): Should we raise exception here? I've seen that in -20 there is often no bounds checking at all.
            // Example: src/iso15118/message/service_detail.cpp:274
            break;
        }

        auto& out_parameter_set = out.ParameterSet.array[parameter_set_index++];
        out_parameter_set.ParameterSetID = parameter_set.parameter_set_id;

        int parameter_idx = 0;
        for (auto const& parameter : parameter_set.parameter) {
            if (parameter_idx >= iso2_ParameterType_16_ARRAY_SIZE) {
                // TODO(kd): Should we raise exception here? I've seen that in -20 there is often no bounds checking at
                // all. Example: src/iso15118/message/service_detail.cpp:274
                break;
            }
            auto& out_param = out_parameter_set.Parameter.array[parameter_idx++];
            CPP2CB_STRING(parameter.name, out_param.Name);

            // TODO(kd): This is a bit naive implementation. Alternatively we could do an if statement using has_value()
            // for each field. This way we can prevent errors but also we will introduce a priority between field types.
            CPP2CB_ASSIGN_IF_USED(parameter.boolValue, out_param.boolValue);
            CPP2CB_ASSIGN_IF_USED(parameter.byteValue, out_param.byteValue);
            CPP2CB_ASSIGN_IF_USED(parameter.shortValue, out_param.shortValue);
            CPP2CB_ASSIGN_IF_USED(parameter.intValue, out_param.intValue);
            CPP2CB_CONVERT_IF_USED(parameter.physicalValue, out_param.physicalValue);
            CPP2CB_STRING_IF_USED(parameter.stringValue, out_param.stringValue);
        }
        out_parameter_set.Parameter.arrayLen = parameter_set.parameter.size();
    }
    out.ParameterSet.arrayLen = in.size();
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
