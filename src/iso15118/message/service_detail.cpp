// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/service_detail.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

// todo(sl): refactor in header file
// default
ServiceDetailResponse::ParameterSet::ParameterSet() {
    id = 0;
    parameter.push_back({
        "Connector",                            // name
        static_cast<int32_t>(DcConnector::Core) // value
    });
    parameter.push_back({
        "ControlMode",                               // name
        static_cast<int32_t>(ControlMode::Scheduled) // value
    });
    parameter.push_back({
        "MobilityNeedsMode",                                    // name
        static_cast<int32_t>(MobilityNeedsMode::ProvidedByEvcc) // value
    });
    parameter.push_back({
        "Pricing",                               // name
        static_cast<int32_t>(Pricing::NoPricing) // value
    });
}

ServiceDetailResponse::ParameterSet::ParameterSet(uint16_t _id, const DcParameterList& list) {
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
    if (list.control_mode == message_20::ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(message_20::MobilityNeedsMode::ProvidedByEvcc);
    } else {
        mobility.value = static_cast<int32_t>(list.mobility_needs_mode);
    }
    // Pricing
    auto& pricing = parameter.emplace_back();
    pricing.name = "Pricing";
    pricing.value = static_cast<int32_t>(list.pricing);
}

ServiceDetailResponse::ParameterSet::ParameterSet(uint16_t _id, const DcBptParameterList& list) {
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
    if (list.control_mode == message_20::ControlMode::Scheduled) {
        mobility.value = static_cast<int32_t>(message_20::MobilityNeedsMode::ProvidedByEvcc);
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

ServiceDetailResponse::ParameterSet::ParameterSet(uint16_t _id, const InternetParameterList& list) {
    id = _id;

    auto& protocol = parameter.emplace_back();
    protocol.name = "Protocol";
    protocol.value = message_20::from_Protocol(list.protocol);

    auto& port = parameter.emplace_back();
    port.name = "Port";
    port.value = static_cast<int32_t>(list.port);
}

ServiceDetailResponse::ParameterSet::ParameterSet(uint16_t _id, const ParkingParameterList& list) {
    id = _id;
    auto& intended_service = parameter.emplace_back();
    intended_service.name = "IntendedService";
    intended_service.value = static_cast<int32_t>(list.intended_service);
    auto& parking_status = parameter.emplace_back();
    parking_status.name = "ParkingStatusType";
    parking_status.value = static_cast<int32_t>(list.parking_status);
}

template <> void convert(const struct iso20_ServiceDetailReqType& in, ServiceDetailRequest& out) {
    convert(in.Header, out.header);
    out.service = static_cast<ServiceCategory>(in.ServiceID);
}

struct ParamterValueVisitor {
    ParamterValueVisitor(iso20_ParameterType& parameter_) : parameter(parameter_){};
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
    void operator()(const message_20::RationalNumber& in) {
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

    cb_convert_enum(in.service, out.ServiceID);

    uint8_t index = 0;
    for (auto const& in_parameter_set : in.service_parameter_list) {
        auto& out_paramater_set = out.ServiceParameterList.ParameterSet.array[index++];
        out_paramater_set.ParameterSetID = in_parameter_set.id;

        uint8_t t = 0;
        for (auto const& in_parameter : in_parameter_set.parameter) {
            auto& out_parameter = out_paramater_set.Parameter.array[t++];
            CPP2CB_STRING(in_parameter.name, out_parameter.Name);
            std::visit(ParamterValueVisitor(out_parameter), in_parameter.value);
        }
        out_paramater_set.Parameter.arrayLen = in_parameter_set.parameter.size();
    }

    out.ServiceParameterList.ParameterSet.arrayLen = in.service_parameter_list.size();
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceDetailReqType& in) {
    va.insert_type<ServiceDetailRequest>(in);
}

template <> int serialize_to_exi(const ServiceDetailResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceDetailRes);

    convert(in, doc.ServiceDetailRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceDetailResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
