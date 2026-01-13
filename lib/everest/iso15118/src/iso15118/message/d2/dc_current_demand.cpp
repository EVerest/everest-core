// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/dc_current_demand.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_CurrentDemandReqType& in, DC_CurrentDemandRequest& out) {
    convert(in.DC_EVStatus, out.ev_status);
    convert(in.EVTargetCurrent, out.ev_target_current);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumCurrentLimit, out.ev_maximum_current_limit);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumVoltageLimit, out.ev_maximum_voltage_limit);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumPowerLimit, out.ev_maximum_power_limit);
    CB2CPP_ASSIGN_IF_USED(in.BulkChargingComplete, out.bulk_charging_complete);
    out.charging_complete = in.ChargingComplete;
    CB2CPP_CONVERT_IF_USED(in.RemainingTimeToFullSoC, out.remaining_time_to_full_soc);
    CB2CPP_CONVERT_IF_USED(in.RemainingTimeToBulkSoC, out.remaining_time_to_bulk_soc);
    convert(in.EVTargetVoltage, out.ev_target_voltage);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_CurrentDemandReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<DC_CurrentDemandRequest>(in, header);
}

template <> void convert(const DC_CurrentDemandResponse& in, struct iso2_CurrentDemandResType& out) {
    init_iso2_CurrentDemandResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    convert(in.evse_status, out.DC_EVSEStatus);
    convert(in.evse_present_voltage, out.EVSEPresentVoltage);
    convert(in.evse_present_current, out.EVSEPresentCurrent);
    out.EVSECurrentLimitAchieved = in.evse_current_limit_achieved;
    out.EVSEVoltageLimitAchieved = in.evse_voltage_limit_achieved;
    out.EVSEPowerLimitAchieved = in.evse_power_limit_achieved;
    CPP2CB_CONVERT_IF_USED(in.evse_maximum_voltage_limit, out.EVSEMaximumVoltageLimit);
    CPP2CB_CONVERT_IF_USED(in.evse_maximum_current_limit, out.EVSEMaximumCurrentLimit);
    CPP2CB_CONVERT_IF_USED(in.evse_maximum_power_limit, out.EVSEMaximumPowerLimit);
    CPP2CB_STRING(in.evse_id, out.EVSEID);
    out.SAScheduleTupleID = in.sa_schedule_tuple_id;
    CPP2CB_CONVERT_IF_USED(in.meter_info, out.MeterInfo);
    CPP2CB_ASSIGN_IF_USED(in.receipt_required, out.ReceiptRequired);
}

template <> int serialize_to_exi(const DC_CurrentDemandResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.CurrentDemandRes);
    convert(in, doc.V2G_Message.Body.CurrentDemandRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_CurrentDemandResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
