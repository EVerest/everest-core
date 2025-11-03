// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/ac_charge_parameter_discovery.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Encoder.h>

namespace iso15118::message_20 {

using AC_ModeReq = datatypes::AC_CPDReqEnergyTransferMode;
using BPT_AC_ModeReq = datatypes::BPT_AC_CPDReqEnergyTransferMode;

using AC_ModeRes = datatypes::AC_CPDResEnergyTransferMode;
using BPT_AC_ModeRes = datatypes::BPT_AC_CPDResEnergyTransferMode;

// Begin conversion for deserializing an ACChargeParameterRequest (EVSEside)
template <typename InType> void convert(const InType& in, AC_ModeReq& out) {
    convert(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);
}

template <> void convert(const struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType& in, BPT_AC_ModeReq& out) {
    convert(in, static_cast<AC_ModeReq&>(out));
    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);
    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);
}

template <>
void convert(const struct iso20_ac_AC_ChargeParameterDiscoveryReqType& in, AC_ChargeParameterDiscoveryRequest& out) {
    convert(in.Header, out.header);
    if (in.AC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<AC_ModeReq>();
        convert(in.AC_CPDReqEnergyTransferMode, mode_out);
    } else if (in.BPT_AC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_AC_ModeReq>();
        convert(in.BPT_AC_CPDReqEnergyTransferMode, mode_out);
    } else {
        // FIXME (aw): fail, should not happen!
    }
}
// End conversion for deserializing an ACChargeParameterRequest (EVSEside)

// Begin conversion for deserializing an ACChargeParameterResponse (EVside)
template <typename InType> void convert(const InType& in, AC_ModeRes& out) {
    convert(in.EVSEMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVSEMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumChargePower_L3, out.min_charge_power_L3);
    convert(in.EVSENominalFrequency, out.nominal_frequency);
    CB2CPP_CONVERT_IF_USED(in.MaximumPowerAsymmetry, out.max_power_asymmetry);
    CB2CPP_CONVERT_IF_USED(in.EVSEPowerRampLimitation, out.power_ramp_limitation);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L3, out.present_active_power_L3);
}

template <> void convert(const iso20_ac_BPT_AC_CPDResEnergyTransferModeType& in, BPT_AC_ModeRes& out) {
    convert(in, static_cast<AC_ModeRes&>(out));
    convert(in.EVSEMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L3, out.max_discharge_power_L3);
    convert(in.EVSEMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumDischargePower_L3, out.min_discharge_power_L3);
}

template <>
void convert(const struct iso20_ac_AC_ChargeParameterDiscoveryResType& in, AC_ChargeParameterDiscoveryResponse& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.ResponseCode, out.response_code);

    if (in.AC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<AC_ModeRes>();
        convert(in.AC_CPDResEnergyTransferMode, mode_out);
    } else if (in.BPT_AC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_AC_ModeRes>();
        convert(in.BPT_AC_CPDResEnergyTransferMode, mode_out);
    } else {
        // FIXME (RB): fail, should not happen!
    }
}
// End conversion for deserializing an ACChargeParameterResponse (EVside)

// Begin conversion for serializing an ACChargeParameterResponse (EVSEside)
template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeParameterDiscoveryResType& in) {
    va.insert_type<AC_ChargeParameterDiscoveryResponse>(in);
}

struct ModeResponseVisitor {
public:
    ModeResponseVisitor(iso20_ac_AC_ChargeParameterDiscoveryResType& res_) : res(res_){};
    void operator()(const AC_ModeRes& in) {
        init_iso20_ac_AC_CPDResEnergyTransferModeType(&res.AC_CPDResEnergyTransferMode);

        CB_SET_USED(res.AC_CPDResEnergyTransferMode);
        convert_common(in, res.AC_CPDResEnergyTransferMode);
    }

    void operator()(const BPT_AC_ModeRes& in) {
        init_iso20_ac_BPT_AC_CPDResEnergyTransferModeType(&res.BPT_AC_CPDResEnergyTransferMode);

        CB_SET_USED(res.BPT_AC_CPDResEnergyTransferMode);

        auto& out = res.BPT_AC_CPDResEnergyTransferMode;
        convert_common(in, out);

        convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVSEMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVSEMaximumDischargePower_L3);

        convert(in.min_discharge_power, out.EVSEMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVSEMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVSEMinimumDischargePower_L3);
    }

    template <typename ModeResTypeIn, typename ModeResTypeOut>
    static void convert_common(const ModeResTypeIn& in, ModeResTypeOut& out) {
        convert(in.max_charge_power, out.EVSEMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVSEMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVSEMaximumChargePower_L3);
        convert(in.min_charge_power, out.EVSEMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVSEMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVSEMinimumChargePower_L3);
        convert(in.nominal_frequency, out.EVSENominalFrequency);
        CPP2CB_CONVERT_IF_USED(in.max_power_asymmetry, out.MaximumPowerAsymmetry);
        CPP2CB_CONVERT_IF_USED(in.power_ramp_limitation, out.EVSEPowerRampLimitation);
        CPP2CB_CONVERT_IF_USED(in.present_active_power, out.EVSEPresentActivePower);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVSEPresentActivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVSEPresentActivePower_L3);
    }

private:
    iso20_ac_AC_ChargeParameterDiscoveryResType& res;
};

template <>
void convert(const AC_ChargeParameterDiscoveryResponse& in, struct iso20_ac_AC_ChargeParameterDiscoveryResType& out) {

    init_iso20_ac_AC_ChargeParameterDiscoveryResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    std::visit(ModeResponseVisitor(out), in.transfer_mode);
}

template <> int serialize_to_exi(const AC_ChargeParameterDiscoveryResponse& in, exi_bitstream_t& out) {
    iso20_ac_exiDocument doc;

    init_iso20_ac_exiDocument(&doc);

    CB_SET_USED(doc.AC_ChargeParameterDiscoveryRes);

    convert(in, doc.AC_ChargeParameterDiscoveryRes);

    return encode_iso20_ac_exiDocument(&out, &doc);
}
template <> size_t serialize(const AC_ChargeParameterDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing an ACChargeParameterResponse (EVSEside)

// Begin conversion for serializing an ACChargeParameterRequest (EVside)
template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeParameterDiscoveryReqType& in) {
    va.insert_type<AC_ChargeParameterDiscoveryRequest>(in);
}

struct ModeRequestVisitor {
public:
    ModeRequestVisitor(iso20_ac_AC_ChargeParameterDiscoveryReqType& req_) : req(req_){};
    void operator()(const AC_ModeReq& in) {
        init_iso20_ac_AC_CPDReqEnergyTransferModeType(&req.AC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.AC_CPDReqEnergyTransferMode);
        convert_common(in, req.AC_CPDReqEnergyTransferMode);
    }

    void operator()(const BPT_AC_ModeReq& in) {
        init_iso20_ac_BPT_AC_CPDReqEnergyTransferModeType(&req.BPT_AC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.BPT_AC_CPDReqEnergyTransferMode);
        auto& out = req.BPT_AC_CPDReqEnergyTransferMode;
        convert_common(in, out);
        convert(in.max_discharge_power, out.EVMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L2, out.EVMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L3, out.EVMaximumDischargePower_L3);
        convert(in.min_discharge_power, out.EVMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L2, out.EVMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L3, out.EVMinimumDischargePower_L3);
    }

    template <typename ModeReqTypeIn, typename ModeReqTypeOut>
    static void convert_common(const ModeReqTypeIn& in, ModeReqTypeOut& out) {
        convert(in.max_charge_power, out.EVMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVMaximumChargePower_L3);
        convert(in.min_charge_power, out.EVMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVMinimumChargePower_L3);
    }

private:
    iso20_ac_AC_ChargeParameterDiscoveryReqType& req;
};

template <>
void convert(const AC_ChargeParameterDiscoveryRequest& in, struct iso20_ac_AC_ChargeParameterDiscoveryReqType& out) {
    init_iso20_ac_AC_ChargeParameterDiscoveryReqType(&out);
    convert(in.header, out.Header);
    std::visit(ModeRequestVisitor(out), in.transfer_mode);
}

template <> int serialize_to_exi(const AC_ChargeParameterDiscoveryRequest& in, exi_bitstream_t& out) {
    iso20_ac_exiDocument doc;

    init_iso20_ac_exiDocument(&doc);

    CB_SET_USED(doc.AC_ChargeParameterDiscoveryReq);

    convert(in, doc.AC_ChargeParameterDiscoveryReq);

    return encode_iso20_ac_exiDocument(&out, &doc);
}
template <> size_t serialize(const AC_ChargeParameterDiscoveryRequest& in, const io::StreamOutputView& out) {

    auto rv = serialize_helper(in, out);

    return rv;
}
// End conversion for serializing an ACChargeParameterRequest (EVside)
} // namespace iso15118::message_20
