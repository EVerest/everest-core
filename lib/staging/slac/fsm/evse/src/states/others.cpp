// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <slac/fsm/evse/states/others.hpp>

#include <cstring>
#include <string_view>

#include <slac/fsm/evse/states/matching.hpp>

#include "../misc.hpp"

namespace slac::fsm::evse {

static auto create_cm_set_key_req(uint8_t const* session_nmk) {
    slac::messages::cm_set_key_req set_key_req;

    set_key_req.key_type = slac::defs::CM_SET_KEY_REQ_KEY_TYPE_NMK;
    set_key_req.my_nonce = 0xAAAAAAAA;
    set_key_req.your_nonce = 0x00000000;
    set_key_req.pid = slac::defs::CM_SET_KEY_REQ_PID_HLE;
    set_key_req.prn = htole16(slac::defs::CM_SET_KEY_REQ_PRN_UNUSED);
    set_key_req.pmn = slac::defs::CM_SET_KEY_REQ_PMN_UNUSED;
    set_key_req.cco_capability = slac::defs::CM_SET_KEY_REQ_CCO_CAP_NONE;
    slac::utils::generate_nid_from_nmk(set_key_req.nid, session_nmk);
    set_key_req.new_eks = slac::defs::CM_SET_KEY_REQ_PEKS_NMK_KNOWN_TO_STA;
    memcpy(set_key_req.new_key, session_nmk, sizeof(set_key_req.new_key));

    return set_key_req;
}

void ResetState::enter() {
    ctx.log_info("Entered Reset state");
}

FSMSimpleState::HandleEventReturnType ResetState::handle_event(AllocatorType& sa, Event ev) {
    const auto& cfg = ctx.slac_config;
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            if (cfg.chip_reset.enabled) {
                // If chip reset is enabled in config, go to ResetChipState and from there to IdleState
                return sa.create_simple<ResetChipState>(ctx);
            } else {
                // If chip reset is disabled, go to IdleState directly
                return sa.create_simple<IdleState>(ctx);
            }
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetState::callback() {
    const auto& cfg = ctx.slac_config;
    if (setup_has_been_send == false) {
        auto set_key_req = create_cm_set_key_req(cfg.session_nmk);

        ctx.log_info("New NMK key: " + format_nmk(cfg.session_nmk));

        ctx.send_slac_message(cfg.plc_peer_mac, set_key_req);

        setup_has_been_send = true;

        return cfg.set_key_timeout_ms;
    } else {
        ctx.log_info("CM_SET_KEY_REQ timeout - failed to setup NMK key");
        return {};
    }
}

bool ResetState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        // FIXME (aw): need to also deal with CM_VALIDATE.REQ
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    } else {
        ctx.log_info("Received CM_SET_KEY_CNF");
        return true;
    }
}

void ResetChipState::enter() {
    ctx.log_info("Entered HW Chip Reset state");
}

FSMSimpleState::HandleEventReturnType ResetChipState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return sa.create_simple<IdleState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::SUCCESS) {
        return sa.create_simple<IdleState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetChipState::callback() {
    const auto& cfg = ctx.slac_config;
    if (sub_state == SubState::DELAY) {
        sub_state = SubState::SEND_RESET;
        return cfg.chip_reset.delay_ms;

    } else if (sub_state == SubState::SEND_RESET) {

        if (ctx.modem_vendor == ModemVendor::Qualcomm) {
            slac::messages::qualcomm::cm_reset_device_req reset_req;
            ctx.log_info("Resetting HW Chip using RS_DEV.REQ");
            ctx.send_slac_message(cfg.plc_peer_mac, reset_req);
            sub_state = SubState::DONE;
            return cfg.chip_reset.timeout_ms;

        } else if (ctx.modem_vendor == ModemVendor::Lumissil) {
            slac::messages::lumissil::nscm_reset_device_req reset_req;
            ctx.log_info("Resetting HW Chip using NSCM_RESET_DEVICE.REQ");
            sub_state = SubState::DONE;
            ctx.send_slac_message(cfg.plc_peer_mac, reset_req);
            // CG5317 does not reply to the reset packet
            return Event::SUCCESS;

        } else {
            ctx.log_info("Chip reset not supported on this chip");
        }
    } else {
        ctx.log_info("Reset timeout, no response received - failed to reset the chip");
        return {};
    }
    return {};
}

bool ResetChipState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype != (slac::defs::qualcomm::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    } else {
        ctx.log_info("Received RS_DEV.CNF");
        return true;
    }
}

void IdleState::enter() {
    ctx.signal_state("UNMATCHED");
    ctx.log_info("Entered Idle state");
}

FSMSimpleState::HandleEventReturnType IdleState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::ENTER_BCD) {
        return sa.create_simple<MatchingState>(ctx);
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void MatchedState::enter() {
    ctx.signal_state("MATCHED");
    ctx.signal_dlink_ready(true);
    ctx.log_info("Entered Matched state");
}

FSMSimpleState::HandleEventReturnType MatchedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void MatchedState::leave() {
    ctx.signal_dlink_ready(false);
}

void FailedState::enter() {
    ctx.signal_error_routine_request();
    ctx.log_info("Entered Failed state");
}

FSMSimpleState::HandleEventReturnType FailedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void WaitForLinkState::enter() {
    ctx.log_info("Waiting for Link to be ready...");
    start_time = std::chrono::steady_clock::now();
}

FSMSimpleState::HandleEventReturnType WaitForLinkState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (handle_slac_message(ctx.slac_message_payload)) {
            return sa.create_simple<MatchedState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::RETRY_MATCHING) {
        ctx.log_info("Link could not be established, resetting...");
        // Notify higher layers to on CP signal
        return sa.create_simple<FailedState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType WaitForLinkState::callback() {
    const auto& cfg = ctx.slac_config;
    if (not link_status_req_sent) {

        if (ctx.modem_vendor == ModemVendor::Qualcomm) {
            slac::messages::qualcomm::link_status_req link_status_req;
            ctx.send_slac_message(cfg.plc_peer_mac, link_status_req);
            link_status_req_sent = true;
        } else if (ctx.modem_vendor == ModemVendor::Lumissil) {
            slac::messages::lumissil::nscm_get_d_link_status_req link_status_req;
            ctx.send_slac_message(cfg.plc_peer_mac, link_status_req);
            link_status_req_sent = true;
        } else {
            ctx.log_info("Link detection not supported on this chip");
        }

        return cfg.link_status.retry_ms;
    } else {
        // Did we timeout?
        if (std::chrono::steady_clock::now() - start_time > std::chrono::milliseconds(cfg.link_status.timeout_ms)) {
            return Event::RETRY_MATCHING;
        }
        // Link is confirmed not up yet, query again
        link_status_req_sent = false;
        return cfg.link_status.retry_ms;
    }
}

bool WaitForLinkState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();

    if (ctx.modem_vendor == ModemVendor::Qualcomm &&
        mmtype == (slac::defs::qualcomm::MMTYPE_LINK_STATUS | slac::defs::MMTYPE_MODE_CNF)) {
        const auto success = message.get_payload<slac::messages::qualcomm::link_status_cnf>().link_status == 0x01;
        return success;

    } else if (ctx.modem_vendor == ModemVendor::Lumissil &&
               mmtype == (slac::defs::lumissil::MMTYPE_NSCM_GET_D_LINK_STATUS | slac::defs::MMTYPE_MODE_CNF)) {
        const auto success =
            message.get_payload<slac::messages::lumissil::nscm_get_d_link_status_cnf>().link_status == 0x01;
        return success;

    } else {
        // unexpected message
        ctx.log_info("Received non-expected SLAC message of type " + format_mmtype(mmtype));
        return false;
    }
}

FSMSimpleState::HandleEventReturnType InitState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        handle_slac_message(ctx.slac_message_payload);
        return sa.PASS_ON;
    } else if (ev == Event::SUCCESS) {
        return sa.create_simple<ResetState>(ctx);
    }
    return sa.PASS_ON;
}

FSMSimpleState::CallbackReturnType InitState::callback() {
    const auto& cfg = ctx.slac_config;

    if (sub_state == SubState::QUALCOMM_OP_ATTR) {
        sub_state = SubState::LUMISSIL_GET_VERSION;
        slac::messages::qualcomm::op_attr_req op_attr_req;
        ctx.send_slac_message(cfg.plc_peer_mac, op_attr_req);
        return cfg.request_info_delay_ms;

    } else if (sub_state == SubState::LUMISSIL_GET_VERSION) {
        sub_state = SubState::DONE;
        slac::messages::lumissil::nscm_get_version_req version_req;
        ctx.send_slac_message(cfg.plc_peer_mac, version_req);
        return cfg.request_info_delay_ms;

    } else if (sub_state == SubState::DONE) {
        // the requested info may or may not be implemented by the chip,
        // so we ignore timeouts here.
        return Event::SUCCESS;
    }
    return {};
}

static std::string get_qualcomm_device_info(slac::messages::qualcomm::op_attr_cnf const& msg) {
    const auto get_string_view = [](auto const& raw) constexpr {
        static_assert(sizeof(uint8_t) == sizeof(char));
        return std::string_view(reinterpret_cast<char const*>(raw), sizeof(raw));
    };

    std::string result("Qualcomm PLC Device Attributes:");
    result += "\n  HW Platform: ";
    result += get_string_view(msg.hw_platform);
    result += "\n  SW Platform: ";
    result += get_string_view(msg.sw_platform);
    result += "\n  Firmware: ";
    result += ("\n  Build date: " + std::to_string(msg.version_major) + "." + std::to_string(msg.version_minor) + "." +
               std::to_string(msg.version_pib) + "-" + std::to_string(msg.version_build));

    result += "\n  ZC signal: ";

    // FIXME: no magic numbers
    const auto zc_signal = (msg.line_freq_zc >> 2) & 0x03;
    if (zc_signal == 0x01) {
        result += "Detected";
    } else if (zc_signal == 0x02) {
        result += "Missing";
    } else {
        result += ("Unknown (" + std::to_string(zc_signal) + ")");
    }

    result += "\n  Line frequency: ";

    const auto line_freq = (msg.line_freq_zc) & 0x03;
    if (line_freq == 0x01) {
        result += "50Hz";
    } else if (line_freq == 0x02) {
        result += "60Hz";
    } else {
        result += ("Unknown (" + std::to_string(line_freq) + ")");
    }

    return result;
}

static std::string get_lumissil_device_info(slac::messages::lumissil::nscm_get_version_cnf const& msg) {
    return "Lumissil PLC Device Firmware version: " + std::to_string(msg.version_major) + "." +
           std::to_string(msg.version_minor) + "." + std::to_string(msg.version_patch) + "." +
           std::to_string(msg.version_build);
}

void InitState::handle_slac_message(slac::messages::HomeplugMessage& message) {
    const auto mmtype = message.get_mmtype();
    if (mmtype == (slac::defs::qualcomm::MMTYPE_OP_ATTR | slac::defs::MMTYPE_MODE_CNF)) {
        const auto msg = message.get_payload<slac::messages::qualcomm::op_attr_cnf>();
        const auto device_info = get_qualcomm_device_info(msg);
        ctx.log_info(device_info);
        // This message is only supported on Qualcomm, so we can use it to detect the Vendor
        ctx.modem_vendor = ModemVendor::Qualcomm;

    } else if (mmtype == (slac::defs::lumissil::MMTYPE_NSCM_GET_VERSION | slac::defs::MMTYPE_MODE_CNF)) {
        const auto msg = message.get_payload<slac::messages::lumissil::nscm_get_version_cnf>();
        const auto device_info = get_lumissil_device_info(msg);
        ctx.log_info(device_info);
        // This message is only supported on Qualcomm, so we can use it to detect the Vendor
        ctx.modem_vendor = ModemVendor::Lumissil;
    }
}
} // namespace slac::fsm::evse
