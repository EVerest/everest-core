// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/service_detail.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

namespace datatypes {
ParameterSet::ParameterSet(uint16_t _id, const DcParameterList& list) {
    id = _id;
    // Connector
    auto& connector = parameter.emplace_back();
    connector.name = "Connector";
    connector.value = static_cast<int32_t>(list.connector);
    // ControlMode
    auto& control_mode = parameter.emplace_back();
    control_mode.name = "ControlMode";
    control_mode.value = static_cast<int32_t>(list.control_mode);
    // MobilityNeedsMode
    auto& mobility = parameter.emplace_back();
    mobility.name = "MobilityNeedsMode";
    if (list.control_mode == ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(MobilityNeedsMode::ProvidedByEvcc);
    } else {
        mobility.value = static_cast<int32_t>(list.mobility_needs_mode);
    }
    // Pricing
    auto& pricing = parameter.emplace_back();
    pricing.name = "Pricing";
    pricing.value = static_cast<int32_t>(list.pricing);
}

ParameterSet::ParameterSet(uint16_t _id, const DcBptParameterList& list) {
    id = _id;
    // Todo(sl): Refactor because of duplicate code
    // Connector
    auto& connector = parameter.emplace_back();
    connector.name = "Connector";
    connector.value = static_cast<int32_t>(list.connector);
    // ControlMode
    auto& control_mode = parameter.emplace_back();
    control_mode.name = "ControlMode";
    control_mode.value = static_cast<int32_t>(list.control_mode);
    // MobilityNeedsMode
    auto& mobility = parameter.emplace_back();
    mobility.name = "MobilityNeedsMode";
    if (list.control_mode == ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(MobilityNeedsMode::ProvidedByEvcc);
    } else {
        mobility.value = static_cast<int32_t>(list.mobility_needs_mode);
    }
    // Pricing
    auto& pricing = parameter.emplace_back();
    pricing.name = "Pricing";
    pricing.value = static_cast<int32_t>(list.pricing);
    // BPTChannel
    auto& channel = parameter.emplace_back();
    channel.name = "BPTChannel";
    channel.value = static_cast<int32_t>(list.bpt_channel);
    // GeneratorMode
    auto& generator_mode = parameter.emplace_back();
    generator_mode.name = "GeneratorMode";
    generator_mode.value = static_cast<int32_t>(list.generator_mode);
}

ParameterSet::ParameterSet(uint16_t _id, const McsParameterList& list) {
    id = _id;
    // Connector
    auto& connector = parameter.emplace_back();
    connector.name = "Connector";
    connector.value = static_cast<int32_t>(list.connector);
    // ControlMode
    auto& control_mode = parameter.emplace_back();
    control_mode.name = "ControlMode";
    control_mode.value = static_cast<int32_t>(list.control_mode);
    // MobilityNeedsMode
    auto& mobility = parameter.emplace_back();
    mobility.name = "MobilityNeedsMode";
    if (list.control_mode == ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(MobilityNeedsMode::ProvidedByEvcc);
    } else {
        mobility.value = static_cast<int32_t>(list.mobility_needs_mode);
    }
    // Pricing
    auto& pricing = parameter.emplace_back();
    pricing.name = "Pricing";
    pricing.value = static_cast<int32_t>(list.pricing);
}

ParameterSet::ParameterSet(uint16_t _id, const McsBptParameterList& list) {
    id = _id;
    // Todo(sl): Refactor because of duplicate code
    // Connector
    auto& connector = parameter.emplace_back();
    connector.name = "Connector";
    connector.value = static_cast<int32_t>(list.connector);
    // ControlMode
    auto& control_mode = parameter.emplace_back();
    control_mode.name = "ControlMode";
    control_mode.value = static_cast<int32_t>(list.control_mode);
    // MobilityNeedsMode
    auto& mobility = parameter.emplace_back();
    mobility.name = "MobilityNeedsMode";
    if (list.control_mode == ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(MobilityNeedsMode::ProvidedByEvcc);
    } else {
        mobility.value = static_cast<int32_t>(list.mobility_needs_mode);
    }
    // Pricing
    auto& pricing = parameter.emplace_back();
    pricing.name = "Pricing";
    pricing.value = static_cast<int32_t>(list.pricing);
    // BPTChannel
    auto& channel = parameter.emplace_back();
    channel.name = "BPTChannel";
    channel.value = static_cast<int32_t>(list.bpt_channel);
    // GeneratorMode
    auto& generator_mode = parameter.emplace_back();
    generator_mode.name = "GeneratorMode";
    generator_mode.value = static_cast<int32_t>(list.generator_mode);
}

ParameterSet::ParameterSet(uint16_t _id, const InternetParameterList& list) {
    id = _id;

    auto& protocol = parameter.emplace_back();
    protocol.name = "Protocol";
    protocol.value = from_Protocol(list.protocol);

    auto& port = parameter.emplace_back();
    port.name = "Port";
    port.value = static_cast<int32_t>(list.port);
}

ParameterSet::ParameterSet(uint16_t _id, const ParkingParameterList& list) {
    id = _id;
    auto& intended_service = parameter.emplace_back();
    intended_service.name = "IntendedService";
    intended_service.value = static_cast<int32_t>(list.intended_service);
    auto& parking_status = parameter.emplace_back();
    parking_status.name = "ParkingStatusType";
    parking_status.value = static_cast<int32_t>(list.parking_status);
}

} // namespace datatypes

template <> void convert(const struct iso20_ServiceDetailReqType& in, ServiceDetailRequest& out) {
    convert(in.Header, out.header);
    out.service = in.ServiceID;
}

template <> void convert(const struct iso20_ServiceDetailResType& in, ServiceDetailResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    out.service = in.ServiceID;
    out.service_parameter_list.clear();
    for (uint8_t i = 0; i < in.ServiceParameterList.ParameterSet.arrayLen; i++) {
        const auto& in_parameter_set = in.ServiceParameterList.ParameterSet.array[i];
        datatypes::ParameterSet out_parameter_set;
        out_parameter_set.id = in_parameter_set.ParameterSetID;

        out_parameter_set.parameter.clear();
        for (uint8_t t = 0; t < in_parameter_set.Parameter.arrayLen; t++) {
            const auto& in_parameter = in_parameter_set.Parameter.array[t];
            datatypes::Parameter out_parameter;

            out_parameter.name = CB2CPP_STRING(in_parameter.Name);
            if (in_parameter.boolValue_isUsed) {
                out_parameter.value = in_parameter.boolValue;
            } else if (in_parameter.byteValue_isUsed) {
                out_parameter.value = in_parameter.byteValue;
            } else if (in_parameter.shortValue_isUsed) {
                out_parameter.value = in_parameter.shortValue;
            } else if (in_parameter.intValue_isUsed) {
                out_parameter.value = in_parameter.intValue;
            } else if (in_parameter.finiteString_isUsed) {
                out_parameter.value = CB2CPP_STRING(in_parameter.finiteString);
            } else if (in_parameter.rationalNumber_isUsed) {
                out_parameter.value =
                    datatypes::RationalNumber{in_parameter.rationalNumber.Value, in_parameter.rationalNumber.Exponent};
            }
            out_parameter_set.parameter.push_back(out_parameter);
        }
        out.service_parameter_list.push_back(out_parameter_set);
    }

    convert(in.Header, out.header);
}

template <> void convert(const ServiceDetailRequest& in, iso20_ServiceDetailReqType& out) {
    init_iso20_ServiceDetailReqType(&out);

    out.ServiceID = in.service;

    convert(in.header, out.Header);
}

struct ParameterValueVisitor {
    ParameterValueVisitor(iso20_ParameterType& parameter_) : parameter(parameter_){};
    void operator()(const bool& in) {
        CB_SET_USED(parameter.boolValue);
        parameter.boolValue = in;
    }
    void operator()(const int8_t& in) {
        CB_SET_USED(parameter.byteValue);
        parameter.byteValue = in;
    }
    void operator()(const int16_t& in) {
        CB_SET_USED(parameter.shortValue);
        parameter.shortValue = in;
    }
    void operator()(const int32_t& in) {
        CB_SET_USED(parameter.intValue);
        parameter.intValue = in;
    }
    void operator()(const std::string& in) {
        CB_SET_USED(parameter.finiteString);
        CPP2CB_STRING(in, parameter.finiteString);
    }
    void operator()(const message_20::datatypes::RationalNumber& in) {
        CB_SET_USED(parameter.rationalNumber);
        parameter.rationalNumber.Exponent = in.exponent;
        parameter.rationalNumber.Value = in.value;
    }

private:
    iso20_ParameterType& parameter;
};

template <> void convert(const ServiceDetailResponse& in, iso20_ServiceDetailResType& out) {
    init_iso20_ServiceDetailResType(&out);

    convert(in.header, out.Header);

    cb_convert_enum(in.response_code, out.ResponseCode);

    out.ServiceID = in.service;

    uint8_t index = 0;
    for (auto const& in_parameter_set : in.service_parameter_list) {
        auto& out_parameter_set = out.ServiceParameterList.ParameterSet.array[index++];
        out_parameter_set.ParameterSetID = in_parameter_set.id;

        uint8_t t = 0;
        for (auto const& in_parameter : in_parameter_set.parameter) {
            auto& out_parameter = out_parameter_set.Parameter.array[t++];
            init_iso20_ParameterType(&out_parameter);

            CPP2CB_STRING(in_parameter.name, out_parameter.Name);
            std::visit(ParameterValueVisitor(out_parameter), in_parameter.value);
        }
        out_parameter_set.Parameter.arrayLen = in_parameter_set.parameter.size();
    }

    out.ServiceParameterList.ParameterSet.arrayLen = in.service_parameter_list.size();
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceDetailReqType& in) {
    va.insert_type<ServiceDetailRequest>(in);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceDetailResType& in) {
    va.insert_type<ServiceDetailResponse>(in);
};

template <> int serialize_to_exi(const ServiceDetailResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceDetailRes);

    convert(in, doc.ServiceDetailRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> int serialize_to_exi(const ServiceDetailRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceDetailReq);

    convert(in, doc.ServiceDetailReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceDetailResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

template <> size_t serialize(const ServiceDetailRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
