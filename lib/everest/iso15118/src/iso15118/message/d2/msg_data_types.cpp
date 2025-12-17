// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest

#include <iso15118/message/d2/msg_data_types.hpp>

#include <iso15118/detail/cb_exi.hpp>

namespace iso15118::d2::msg {

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

void convert(const iso2_DC_EVStatusType& in, data_types::DC_EVStatus& out) {
    out.ev_ready = in.EVReady;
    cb_convert_enum(in.EVErrorCode, out.ev_error_code);
    out.ev_ress_soc = in.EVRESSSOC;
}

void convert(const data_types::AC_EVSEStatus& in, iso2_AC_EVSEStatusType& out) {
    init_iso2_AC_EVSEStatusType(&out);
    cb_convert_enum(in.evse_notification, out.EVSENotification);
    out.NotificationMaxDelay = in.notification_max_delay;
    out.RCD = in.rcd;
}

void convert(const data_types::DC_EVSEStatus& in, iso2_DC_EVSEStatusType& out) {
    init_iso2_DC_EVSEStatusType(&out);
    cb_convert_enum(in.evse_notification, out.EVSENotification);
    out.NotificationMaxDelay = in.notification_max_delay;
    if (in.evse_isolation_status) {
        cb_convert_enum(in.evse_isolation_status.value(), out.EVSEIsolationStatus);
        CB_SET_USED(out.EVSEIsolationStatus);
    }
    cb_convert_enum(in.evse_status_code, out.EVSEStatusCode);
}

} // namespace iso15118::d2::msg
