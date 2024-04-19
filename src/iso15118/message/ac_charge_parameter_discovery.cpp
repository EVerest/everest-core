// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/ac_charge_parameter_discovery.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_AC_Decoder.h>
#include <exi/cb/iso20_AC_Encoder.h>

namespace iso15118::message_20 {

using AC_ModeReq = AC_ChargeParameterDiscoveryRequest::AC_CPDReqEnergyTransferMode;
using BPT_AC_ModeReq = AC_ChargeParameterDiscoveryRequest::BPT_AC_CPDReqEnergyTransferMode;

using AC_ModeRes = AC_ChargeParameterDiscoveryResponse::AC_CPDResEnergyTransferMode;
using BPT_AC_ModeRes = AC_ChargeParameterDiscoveryResponse::BPT_AC_CPDResEnergyTransferMode;

template <> void convert(const struct iso20_ac_AC_CPDReqEnergyTransferModeType& in, AC_ModeReq& out) {
    convert(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);
}

template <> void convert(const struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType& in, BPT_AC_ModeReq& out) {
    convert(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);

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
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeParameterDiscoveryReqType& in) {
    va.insert_type<AC_ChargeParameterDiscoveryRequest>(in);
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

} // namespace iso15118::message_20
