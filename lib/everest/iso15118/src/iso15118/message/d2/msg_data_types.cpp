// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#include <cmath>

#include <iso15118/message/d2/msg_data_types.hpp>

#include <iso15118/detail/cb_exi.hpp>

namespace iso15118::d2::msg {

namespace data_types {

float from_PhysicalValue(const PhysicalValue& in) {
    return in.value * pow(10, in.multiplier);
}

PhysicalValue from_float(const float in, const data_types::UnitSymbol unit) {
    PhysicalValue out;
    out.unit = unit;
    if (in == 0.0) {
        out.multiplier = 0;
        out.value = 0;
        return out;
    }
    out.multiplier = static_cast<int8_t>(floor(log10(fabs(in))));
    out.multiplier -= 3; // add 3 digits of precision
    out.value = static_cast<int16_t>(in * pow(10, -out.multiplier));
    return out;
}

}; // namespace data_types

void convert(const iso2_NotificationType& in, data_types::Notification& out) {
    cb_convert_enum(in.FaultCode, out.fault_code);
    CB2CPP_STRING_IF_USED(in.FaultMsg, out.fault_msg);
}

void convert(const struct iso2_MessageHeaderType& in, Header& out) {
    CB2CPP_BYTES(in.SessionID, out.session_id);
    CB2CPP_CONVERT_IF_USED(in.Notification, out.notification);
}

void convert(const data_types::Notification& in, struct iso2_NotificationType& out) {
    cb_convert_enum(in.fault_code, out.FaultCode);
    CPP2CB_STRING_IF_USED(in.fault_msg, out.FaultMsg);
}

void convert(const Header& in, struct iso2_MessageHeaderType& out) {
    init_iso2_MessageHeaderType(&out);
    CPP2CB_BYTES(in.session_id, out.SessionID);
    CPP2CB_CONVERT_IF_USED(in.notification, out.Notification);
}

template <> void convert(const iso2_MeterInfoType& in, data_types::MeterInfo& out) {
    out.meter_id = CB2CPP_STRING(in.MeterID);
    CB2CPP_ASSIGN_IF_USED(in.MeterReading, out.meter_reading);
    CB2CPP_BYTES_IF_USED(in.SigMeterReading, out.sig_meter_reading);
    CB2CPP_ASSIGN_IF_USED(in.MeterStatus, out.meter_status);
    CB2CPP_ASSIGN_IF_USED(in.TMeter, out.t_meter);
}

template <> void convert(const data_types::MeterInfo& in, iso2_MeterInfoType& out) {
    init_iso2_MeterInfoType(&out);

    CPP2CB_STRING(in.meter_id, out.MeterID);
    CPP2CB_ASSIGN_IF_USED(in.meter_reading, out.MeterReading);
    if (in.sig_meter_reading) {
        CPP2CB_BYTES(in.sig_meter_reading.value(), out.SigMeterReading);
        CB_SET_USED(out.SigMeterReading);
    }
    CPP2CB_ASSIGN_IF_USED(in.meter_status, out.MeterStatus);
    CPP2CB_ASSIGN_IF_USED(in.t_meter, out.TMeter);
}

template <> void convert(const iso2_DC_EVStatusType& in, data_types::DcEvStatus& out) {
    out.ev_ready = in.EVReady;
    cb_convert_enum(in.EVErrorCode, out.ev_error_code);
    out.ev_ress_soc = in.EVRESSSOC;
}

template <> void convert(const data_types::AcEvseStatus& in, iso2_AC_EVSEStatusType& out) {
    init_iso2_AC_EVSEStatusType(&out);
    cb_convert_enum(in.evse_notification, out.EVSENotification);
    out.NotificationMaxDelay = in.notification_max_delay;
    out.RCD = in.rcd;
}

template <> void convert(const data_types::DcEvseStatus& in, iso2_DC_EVSEStatusType& out) {
    init_iso2_DC_EVSEStatusType(&out);
    cb_convert_enum(in.evse_notification, out.EVSENotification);
    out.NotificationMaxDelay = in.notification_max_delay;
    if (in.evse_isolation_status) {
        cb_convert_enum(in.evse_isolation_status.value(), out.EVSEIsolationStatus);
        CB_SET_USED(out.EVSEIsolationStatus);
    }
    cb_convert_enum(in.evse_status_code, out.EVSEStatusCode);
}

template <> void convert(const iso2_PhysicalValueType& in, data_types::PhysicalValue& out) {
    out.multiplier = in.Multiplier;
    out.value = in.Value;
    cb_convert_enum(in.Unit, out.unit);
}

template <> void convert(const data_types::PhysicalValue& in, iso2_PhysicalValueType& out) {
    out.Multiplier = in.multiplier;
    out.Value = in.value;
    cb_convert_enum(in.unit, out.Unit);
}

} // namespace iso15118::d2::msg
