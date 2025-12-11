// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/dc_charge_parameter_discovery.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace iso15118::message_20 {

using DC_ModeReq = datatypes::DC_CPDReqEnergyTransferMode;
using BPT_DC_ModeReq = datatypes::BPT_DC_CPDReqEnergyTransferMode;

using DC_ModeRes = datatypes::DC_CPDResEnergyTransferMode;
using BPT_DC_ModeRes = datatypes::BPT_DC_CPDResEnergyTransferMode;

// Begin conversion for deserializing a DCChargeParameterRequest (EVSEside)
template <typename InType> void convert(const InType& in, DC_ModeReq& out) {
    convert(in.EVMaximumChargePower, out.max_charge_power);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    convert(in.EVMaximumChargeCurrent, out.max_charge_current);
    convert(in.EVMinimumChargeCurrent, out.min_charge_current);
    convert(in.EVMaximumVoltage, out.max_voltage);
    convert(in.EVMinimumVoltage, out.min_voltage);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
}

template <> void convert(const struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType& in, BPT_DC_ModeReq& out) {
    convert(in, static_cast<DC_ModeReq&>(out));
    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    convert(in.EVMaximumDischargeCurrent, out.max_discharge_current);
    convert(in.EVMinimumDischargeCurrent, out.min_discharge_current);
}

template <>
void convert(const struct iso20_dc_DC_ChargeParameterDiscoveryReqType& in, DC_ChargeParameterDiscoveryRequest& out) {
    convert(in.Header, out.header);

    if (in.DC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<DC_ModeReq>();
        convert(in.DC_CPDReqEnergyTransferMode, mode_out);

    } else if (in.BPT_DC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_DC_ModeReq>();
        convert(in.BPT_DC_CPDReqEnergyTransferMode, mode_out);
    } else {
        // FIXME (aw): fail, should not happen!
    }
}
// End conversion for deserializing a DCChargeParameterRequest (EVSEside)

// Begin conversion for deserializing a DCChargeParameterResponse (EVside)
template <typename InType> void convert(const InType& in, DC_ModeRes& out) {
    convert(in.EVSEMaximumChargePower, out.max_charge_power);
    convert(in.EVSEMinimumChargePower, out.min_charge_power);
    convert(in.EVSEMaximumChargeCurrent, out.max_charge_current);
    convert(in.EVSEMinimumChargeCurrent, out.min_charge_current);
    convert(in.EVSEMaximumVoltage, out.max_voltage);
    convert(in.EVSEMinimumVoltage, out.min_voltage);
    CB2CPP_CONVERT_IF_USED(in.EVSEPowerRampLimitation, out.power_ramp_limit);
}

template <> void convert(const struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType& in, BPT_DC_ModeRes& out) {
    convert(in, static_cast<DC_ModeRes&>(out));
    convert(in.EVSEMaximumDischargePower, out.max_discharge_power);
    convert(in.EVSEMinimumDischargePower, out.min_discharge_power);
    convert(in.EVSEMaximumDischargeCurrent, out.max_discharge_current);
    convert(in.EVSEMinimumDischargeCurrent, out.min_discharge_current);
}

template <>
void convert(const struct iso20_dc_DC_ChargeParameterDiscoveryResType& in, DC_ChargeParameterDiscoveryResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    if (in.DC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<DC_ModeRes>();
        convert(in.DC_CPDResEnergyTransferMode, mode_out);
    } else if (in.BPT_DC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_DC_ModeRes>();
        convert(in.BPT_DC_CPDResEnergyTransferMode, mode_out);
    } else {
        // FIXME (SL): fail, should not happen!
    }
    convert(in.Header, out.header);
}
// End conversion for deserializing a DCChargeParameterResponse (EVside)

// Begin conversion for serializing a DCChargeParameterResponse (EVSEside)
struct ModeResponseVisitor {
    ModeResponseVisitor(iso20_dc_DC_ChargeParameterDiscoveryResType& res_) : res(res_){};
    void operator()(const DC_ModeRes& in) {
        init_iso20_dc_DC_CPDResEnergyTransferModeType(&res.DC_CPDResEnergyTransferMode);
        CB_SET_USED(res.DC_CPDResEnergyTransferMode);

        convert_common(in, res.DC_CPDResEnergyTransferMode);
    }

    void operator()(const BPT_DC_ModeRes& in) {
        init_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(&res.BPT_DC_CPDResEnergyTransferMode);
        CB_SET_USED(res.BPT_DC_CPDResEnergyTransferMode);

        auto& out = res.BPT_DC_CPDResEnergyTransferMode;

        convert_common(in, out);

        convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
        convert(in.min_discharge_power, out.EVSEMinimumDischargePower);
        convert(in.max_discharge_current, out.EVSEMaximumDischargeCurrent);
        convert(in.min_discharge_current, out.EVSEMinimumDischargeCurrent);
    }

    template <typename ModeResTypeIn, typename ModeResTypeOut>
    static void convert_common(const ModeResTypeIn& in, ModeResTypeOut& out) {
        convert(in.max_charge_power, out.EVSEMaximumChargePower);
        convert(in.min_charge_power, out.EVSEMinimumChargePower);
        convert(in.max_charge_current, out.EVSEMaximumChargeCurrent);
        convert(in.min_charge_current, out.EVSEMinimumChargeCurrent);
        convert(in.max_voltage, out.EVSEMaximumVoltage);
        convert(in.min_voltage, out.EVSEMinimumVoltage);
        CPP2CB_CONVERT_IF_USED(in.power_ramp_limit, out.EVSEPowerRampLimitation);
    }

private:
    iso20_dc_DC_ChargeParameterDiscoveryResType& res;
};

template <>
void convert(const DC_ChargeParameterDiscoveryResponse& in, struct iso20_dc_DC_ChargeParameterDiscoveryResType& out) {
    init_iso20_dc_DC_ChargeParameterDiscoveryResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    std::visit(ModeResponseVisitor(out), in.transfer_mode);
}

template <> int serialize_to_exi(const DC_ChargeParameterDiscoveryResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_ChargeParameterDiscoveryRes);

    convert(in, doc.DC_ChargeParameterDiscoveryRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_ChargeParameterDiscoveryResType& in) {
    va.insert_type<DC_ChargeParameterDiscoveryResponse>(in);
};

template <> size_t serialize(const DC_ChargeParameterDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a DCChargeParameterResponse (EVSEside)

// Begin conversion for serializing a DCChargeParameterRequest (EVside)

struct ModeRequestVisitor {
    ModeRequestVisitor(iso20_dc_DC_ChargeParameterDiscoveryReqType& req_) : req(req_){};
    void operator()(const DC_ModeReq& in) {
        init_iso20_dc_DC_CPDReqEnergyTransferModeType(&req.DC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.DC_CPDReqEnergyTransferMode);
        convert_common(in, req.DC_CPDReqEnergyTransferMode);
    }

    void operator()(const BPT_DC_ModeReq& in) {
        init_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(&req.BPT_DC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.BPT_DC_CPDReqEnergyTransferMode);
        auto& out = req.BPT_DC_CPDReqEnergyTransferMode;
        convert_common(in, out);
        convert(in.max_discharge_power, out.EVMaximumDischargePower);
        convert(in.min_discharge_power, out.EVMinimumDischargePower);
        convert(in.max_discharge_current, out.EVMaximumDischargeCurrent);
        convert(in.min_discharge_current, out.EVMinimumDischargeCurrent);
    }

    template <typename ModeReqTypeIn, typename ModeReqTypeOut>
    static void convert_common(const ModeReqTypeIn& in, ModeReqTypeOut& out) {
        convert(in.max_charge_power, out.EVMaximumChargePower);
        convert(in.min_charge_power, out.EVMinimumChargePower);
        convert(in.max_charge_current, out.EVMaximumChargeCurrent);
        convert(in.min_charge_current, out.EVMinimumChargeCurrent);
        convert(in.max_voltage, out.EVMaximumVoltage);
        convert(in.min_voltage, out.EVMinimumVoltage);
        CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    }

private:
    iso20_dc_DC_ChargeParameterDiscoveryReqType& req;
};

template <>
void convert(const DC_ChargeParameterDiscoveryRequest& in, iso20_dc_DC_ChargeParameterDiscoveryReqType& out) {
    init_iso20_dc_DC_ChargeParameterDiscoveryReqType(&out);
    std::visit(ModeRequestVisitor(out), in.transfer_mode);
    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_ChargeParameterDiscoveryReqType& in) {
    va.insert_type<DC_ChargeParameterDiscoveryRequest>(in);
}

template <> int serialize_to_exi(const DC_ChargeParameterDiscoveryRequest& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);
    CB_SET_USED(doc.DC_ChargeParameterDiscoveryReq);
    convert(in, doc.DC_ChargeParameterDiscoveryReq);
    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_ChargeParameterDiscoveryRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a DCChargeParameterRequest (EVside)

} // namespace iso15118::message_20
