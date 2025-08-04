// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <cmath>

#include <iso15118/message/common_types.hpp>

#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/message/variant.hpp>

#include <cbv2g/iso_20/iso20_AC_Datatypes.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Datatypes.h>
#include <cbv2g/iso_20/iso20_DC_Datatypes.h>

namespace iso15118::message_20 {

template <typename cb_HeaderType> void convert(const cb_HeaderType& in, Header& out) {

    std::copy(in.SessionID.bytes, in.SessionID.bytes + in.SessionID.bytesLen, out.session_id.begin());
    out.timestamp = in.TimeStamp;

    // Todo(sl): missing signature
}

template void convert(const struct iso20_MessageHeaderType& in, Header& out);
template void convert(const struct iso20_dc_MessageHeaderType& in, Header& out);
template void convert(const struct iso20_ac_MessageHeaderType& in, Header& out);

template <typename cb_HeaderType> void convert_header(const Header& in, cb_HeaderType& out) {
    out.TimeStamp = in.timestamp;
    std::copy(in.session_id.begin(), in.session_id.end(), out.SessionID.bytes);

    // FIXME (aw): this should be fixed 8
    out.SessionID.bytesLen = 8;
}

template <> void convert(const Header& in, iso20_MessageHeaderType& out) {
    init_iso20_MessageHeaderType(&out);
    convert_header(in, out);
}

template <> void convert(const Header& in, iso20_dc_MessageHeaderType& out) {
    init_iso20_dc_MessageHeaderType(&out);
    convert_header(in, out);
}

template <> void convert(const Header& in, iso20_ac_MessageHeaderType& out) {
    init_iso20_ac_MessageHeaderType(&out);
    convert_header(in, out);
}

template <typename cb_RationalNumberType>
void convert(const cb_RationalNumberType& in, datatypes::RationalNumber& out) {
    out.exponent = in.Exponent;
    out.value = in.Value;
}

template void convert(const struct iso20_ac_RationalNumberType& in, datatypes::RationalNumber& out);
template void convert(const struct iso20_dc_RationalNumberType& in, datatypes::RationalNumber& out);
template void convert(const struct iso20_RationalNumberType& in, datatypes::RationalNumber& out);

template <typename cb_RationalNumberType>
void convert(const datatypes::RationalNumber& in, cb_RationalNumberType& out) {
    out.Exponent = in.exponent;
    out.Value = in.value;
}

template void convert(const datatypes::RationalNumber& in, struct iso20_ac_RationalNumberType& out);
template void convert(const datatypes::RationalNumber& in, struct iso20_dc_RationalNumberType& out);
template void convert(const datatypes::RationalNumber& in, struct iso20_RationalNumberType& out);

template <> void convert(const datatypes::EvseStatus& in, struct iso20_dc_EVSEStatusType& out) {
    out.NotificationMaxDelay = in.notification_max_delay;
    cb_convert_enum(in.notification, out.EVSENotification);
}

template <> void convert(const struct iso20_dc_EVSEStatusType& in, datatypes::EvseStatus& out) {
    cb_convert_enum(in.EVSENotification, out.notification);
    out.notification_max_delay = in.NotificationMaxDelay;
}

template <> void convert(const datatypes::EvseStatus& in, struct iso20_ac_EVSEStatusType& out) {
    out.NotificationMaxDelay = in.notification_max_delay;
    cb_convert_enum(in.notification, out.EVSENotification);
}

template <> void convert(const struct iso20_ac_EVSEStatusType& in, datatypes::EvseStatus& out) {
    cb_convert_enum(in.EVSENotification, out.notification);
    out.notification_max_delay = in.NotificationMaxDelay;
}

template <typename cb_MeterInfoType> void convert_meterinfo(const datatypes::MeterInfo& in, cb_MeterInfoType& out) {

    CPP2CB_STRING(in.meter_id, out.MeterID);
    out.ChargedEnergyReadingWh = in.charged_energy_reading_wh;

    CPP2CB_ASSIGN_IF_USED(in.bpt_discharged_energy_reading_wh, out.BPT_DischargedEnergyReadingWh);
    CPP2CB_ASSIGN_IF_USED(in.capacitive_energy_reading_varh, out.CapacitiveEnergyReadingVARh);
    CPP2CB_ASSIGN_IF_USED(in.bpt_inductive_energy_reading_varh, out.BPT_InductiveEnergyReadingVARh);

    if (in.meter_signature) {
        CPP2CB_BYTES(in.meter_signature.value(), out.MeterSignature);
        CB_SET_USED(out.MeterSignature);
    }

    CPP2CB_ASSIGN_IF_USED(in.meter_status, out.MeterStatus);
    CPP2CB_ASSIGN_IF_USED(in.meter_timestamp, out.MeterTimestamp);
}

template <> void convert(const datatypes::MeterInfo& in, iso20_dc_MeterInfoType& out) {
    init_iso20_dc_MeterInfoType(&out);
    convert_meterinfo(in, out);
}

template <> void convert(const datatypes::MeterInfo& in, iso20_ac_MeterInfoType& out) {
    init_iso20_ac_MeterInfoType(&out);
    convert_meterinfo(in, out);
}

template <typename cb_MeterInfoType>
void convert_meterinfo_inverse(const cb_MeterInfoType& in, datatypes::MeterInfo& out) {
    out.meter_id = CB2CPP_STRING(in.MeterID);
    out.charged_energy_reading_wh = in.ChargedEnergyReadingWh;
    CB2CPP_ASSIGN_IF_USED(in.BPT_DischargedEnergyReadingWh, out.bpt_discharged_energy_reading_wh);
    CB2CPP_ASSIGN_IF_USED(in.BPT_InductiveEnergyReadingVARh, out.bpt_inductive_energy_reading_varh);
    CB2CPP_ASSIGN_IF_USED(in.CapacitiveEnergyReadingVARh, out.capacitive_energy_reading_varh);
    CB2CPP_BYTES_IF_USED(in.MeterSignature, out.meter_signature);
    CB2CPP_ASSIGN_IF_USED(in.MeterStatus, out.meter_status);
    CB2CPP_ASSIGN_IF_USED(in.MeterTimestamp, out.meter_timestamp);
}

template <> void convert(const iso20_dc_MeterInfoType& in, datatypes::MeterInfo& out) {
    convert_meterinfo_inverse(in, out);
}

template <> void convert(const iso20_ac_MeterInfoType& in, datatypes::MeterInfo& out) {
    convert_meterinfo_inverse(in, out);
}

template <> void convert(const datatypes::EvseStatus& in, iso20_EVSEStatusType& out) {
    out.NotificationMaxDelay = in.notification_max_delay;
    cb_convert_enum(in.notification, out.EVSENotification);
}

template <> void convert(const struct iso20_EVSEStatusType& in, datatypes::EvseStatus& out) {
    cb_convert_enum(in.EVSENotification, out.notification);
    out.notification_max_delay = in.NotificationMaxDelay;
}

template <> void convert(const struct iso20_PowerScheduleEntryType& in, datatypes::PowerScheduleEntry& out) {
    out.duration = in.Duration;
    convert(in.Power, out.power);
    CB2CPP_CONVERT_IF_USED(in.Power_L2, out.power_l2);
    CB2CPP_CONVERT_IF_USED(in.Power_L3, out.power_l3);
}

namespace datatypes {

float from_RationalNumber(const RationalNumber& in) {
    return in.value * pow(10, in.exponent);
}

RationalNumber from_float(float in) {
    RationalNumber out;
    if (in == 0.0) {
        out.exponent = 0;
        out.value = 0;
        return out;
    }
    out.exponent = static_cast<int8_t>(floor(log10(fabs(in))));
    out.exponent -= 3; // add 3 digits of precision
    out.value = static_cast<int16_t>(in * pow(10, -out.exponent));
    return out;
}

std::string from_Protocol(const Protocol& in) {

    switch (in) {
    case Protocol::Ftp:
        return std::string("ftp");
    case Protocol::Http:
        return std::string("http");
    case Protocol::Https:
        return std::string("https");
    }
    return "";
}

std::string from_control_mode(const ControlMode& in) {
    switch (in) {
    case ControlMode::Scheduled:
        return "Scheduled";
    case ControlMode::Dynamic:
        return "Dynamic";
    }
    return "";
}

std::string from_mobility_needs_mode(const MobilityNeedsMode& in) {
    switch (in) {
    case MobilityNeedsMode::ProvidedByEvcc:
        return "ProvidedByEvcc";
    case MobilityNeedsMode::ProvidedBySecc:
        return "ProvidedBySecc";
    }
    return "";
}

} // namespace datatypes

} // namespace iso15118::message_20
