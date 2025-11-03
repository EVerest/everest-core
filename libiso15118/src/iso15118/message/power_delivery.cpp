// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/power_delivery.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

// Begin conversion for deserializing a PowerDeliveryRequest (EVSEside)
template <>
void convert(const struct iso20_Scheduled_EVPPTControlModeType& in, datatypes::Scheduled_EVPPTControlMode& out) {
    if (in.PowerToleranceAcceptance_isUsed) {
        out.power_tolerance_acceptance = static_cast<datatypes::PowerToleranceAcceptance>(in.PowerToleranceAcceptance);
    }
    out.selected_schedule = in.SelectedScheduleTupleID;
}

template <> void convert(const struct iso20_EVPowerProfileType& in, datatypes::PowerProfile& out) {
    out.time_anchor = in.TimeAnchor;

    if (in.Dynamic_EVPPTControlMode_isUsed) {
        out.control_mode.emplace<datatypes::Dynamic_EVPPTControlMode>();
        // NOTE (aw): nothing more to do here because Dynamic_EVPPTControlMode is empty
    } else if (in.Scheduled_EVPPTControlMode_isUsed) {
        auto& cm = out.control_mode.emplace<datatypes::Scheduled_EVPPTControlMode>();
        convert(in.Scheduled_EVPPTControlMode, cm);
    } else {
        throw std::runtime_error("PowerProfile control mode not defined");
    }

    auto& entries_in = in.EVPowerProfileEntries.EVPowerProfileEntry;
    out.entries.reserve(entries_in.arrayLen);
    for (auto i = 0; i < entries_in.arrayLen; ++i) {
        auto& entry_out = out.entries.emplace_back();
        const auto& entry_in = entries_in.array[i];
        convert(entry_in, entry_out);
    }
}

void convert(const iso20_channelSelectionType in, datatypes::ChannelSelection& out) {
    cb_convert_enum(in, out);
}

template <> void convert(const struct iso20_PowerDeliveryReqType& in, PowerDeliveryRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.EVProcessing, out.processing);
    cb_convert_enum(in.ChargeProgress, out.charge_progress);

    CB2CPP_CONVERT_IF_USED(in.EVPowerProfile, out.power_profile);
    CB2CPP_CONVERT_IF_USED(in.BPT_ChannelSelection, out.channel_selection);
}
// End conversion for deserializing a PowerDeliveryRequest (EVSEside)

// Begin conversion for deserializing a PowerDeliveryResponse (EVside)
template <> void convert(const struct iso20_PowerDeliveryResType& in, PowerDeliveryResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    CB2CPP_CONVERT_IF_USED(in.EVSEStatus, out.status);

    convert(in.Header, out.header);
}
// End conversion for deserializing a PowerDeliveryResponse (EVside)

// Begin conversion for serializing a PowerDeliveryResponse (EVSEside)
template <> void convert(const datatypes::PowerToleranceAcceptance& in, iso20_powerToleranceAcceptanceType& out) {
    cb_convert_enum(in, out);
}

template <> void convert(const datatypes::Scheduled_EVPPTControlMode& in, iso20_Scheduled_EVPPTControlModeType& out) {
    init_iso20_Scheduled_EVPPTControlModeType(&out);

    CPP2CB_CONVERT_IF_USED(in.power_tolerance_acceptance, out.PowerToleranceAcceptance);
    out.SelectedScheduleTupleID = in.selected_schedule;
}

template <> void convert(const datatypes::PowerScheduleEntry& in, iso20_PowerScheduleEntryType& out) {
    init_iso20_PowerScheduleEntryType(&out);

    out.Duration = in.duration;
    convert(in.power, out.Power);
    CPP2CB_CONVERT_IF_USED(in.power_l2, out.Power_L2);
    CPP2CB_CONVERT_IF_USED(in.power_l3, out.Power_L3);
}

template <> void convert(const datatypes::PowerProfile& in, iso20_EVPowerProfileType& out) {
    init_iso20_EVPowerProfileType(&out);

    out.TimeAnchor = in.time_anchor;

    if (std::holds_alternative<datatypes::Dynamic_EVPPTControlMode>(in.control_mode)) {
        out.Dynamic_EVPPTControlMode_isUsed = true;
    } else if (std::holds_alternative<datatypes::Scheduled_EVPPTControlMode>(in.control_mode)) {
        out.Scheduled_EVPPTControlMode_isUsed = true;
        convert(std::get<datatypes::Scheduled_EVPPTControlMode>(in.control_mode), out.Scheduled_EVPPTControlMode);
    } else {
        throw std::runtime_error("PowerProfile control mode not defined");
    }

    auto& entries_out = out.EVPowerProfileEntries.EVPowerProfileEntry;
    entries_out.arrayLen = static_cast<uint32_t>(in.entries.size());
    for (size_t i = 0; i < in.entries.size(); ++i) {
        convert(in.entries[i], entries_out.array[i]);
    }
}

template <> void convert(const PowerDeliveryResponse& in, iso20_PowerDeliveryResType& out) {
    init_iso20_PowerDeliveryResType(&out);

    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    CPP2CB_CONVERT_IF_USED(in.status, out.EVSEStatus);
}

template <> void insert_type(VariantAccess& va, const struct iso20_PowerDeliveryResType& in) {
    va.insert_type<PowerDeliveryResponse>(in);
};

template <> int serialize_to_exi(const PowerDeliveryResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.PowerDeliveryRes);

    convert(in, doc.PowerDeliveryRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const PowerDeliveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a PowerDeliveryResponse (EVSEside)

// Begin conversion for serializing a PowerDeliveryRequest (EVside)
template <> void convert(const datatypes::ChannelSelection& in, iso20_channelSelectionType& out) {
    cb_convert_enum(in, out);
}

template <> void convert(const PowerDeliveryRequest& in, iso20_PowerDeliveryReqType& out) {
    init_iso20_PowerDeliveryReqType(&out);

    cb_convert_enum(in.charge_progress, out.ChargeProgress);
    cb_convert_enum(in.processing, out.EVProcessing);
    CPP2CB_CONVERT_IF_USED(in.power_profile, out.EVPowerProfile);
    CPP2CB_CONVERT_IF_USED(in.channel_selection, out.BPT_ChannelSelection);

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_PowerDeliveryReqType& in) {
    va.insert_type<PowerDeliveryRequest>(in);
};

template <> int serialize_to_exi(const PowerDeliveryRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.PowerDeliveryReq);

    convert(in, doc.PowerDeliveryReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const PowerDeliveryRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a PowerDeliveryRequest (EVside)

} // namespace iso15118::message_20
