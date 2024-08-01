// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include <slac/fsm/ev/states/others.hpp>

#include <chrono>
#include <cstring>
#include <random>
#include <thread>

#include <endian.h>

#include "timing_helper.hpp"

#include <slac/fsm/ev/states/sounding.hpp>

namespace slac::fsm::ev {

void ResetState::enter() {
    ctx.signal_state("UNMATCHED");
    ctx.log_info("Entered Reset state");
}

FSMSimpleState::HandleEventReturnType ResetState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::TRIGGER_MATCHING) {
        if (ctx.chip_reset.enabled) {
            // If chip reset is enabled in config, go to ResetChipState and from there to InitSlacState
            return sa.create_simple<ResetChipState>(ctx);
        } else {
            // If chip reset is disabled, go to InitSlacState directly
            return sa.create_simple<InitSlacState>(ctx);
        }
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetState::callback() {
    ctx.log_info("Called callback of ResetState");

    return {};
}

void ResetChipState::enter() {
    ctx.log_info("Entered HW Chip Reset state");
}

FSMSimpleState::HandleEventReturnType ResetChipState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (check_for_valid_reset_conf()) {
            return sa.create_simple<InitSlacState>(ctx);
        } else {
            return sa.PASS_ON;
        }
    } else if (ev == Event::SUCCESS) {
        return sa.create_simple<InitSlacState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::CallbackReturnType ResetChipState::callback() {
    if (sub_state == SubState::DELAY) {
        sub_state = SubState::SEND_RESET;
        return ctx.chip_reset.delay_ms;

    } else if (sub_state == SubState::SEND_RESET) {

        if (ctx.modem_vendor == ModemVendor::Qualcomm) {
            slac::messages::qualcomm::cm_reset_device_req reset_req;
            ctx.log_info("Resetting HW Chip using RS_DEV.REQ");
            ctx.send_slac_message(ctx.plc_mac, reset_req);
            sub_state = SubState::DONE;
            return ctx.chip_reset.timeout_ms;

        } else if (ctx.modem_vendor == ModemVendor::Lumissil) {
            slac::messages::lumissil::nscm_reset_device_req reset_req;
            ctx.log_info("Resetting HW Chip using NSCM_RESET_DEVICE.REQ");
            sub_state = SubState::DONE;
            ctx.send_slac_message(ctx.plc_mac, reset_req);
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

bool ResetChipState::check_for_valid_reset_conf() {
    const auto mmtype = ctx.slac_message.get_mmtype();
    if (mmtype != (slac::defs::qualcomm::MMTYPE_CM_RESET_DEVICE | slac::defs::MMTYPE_MODE_CNF)) {
        // unexpected message
        ctx.log_info("Received non-expected SLAC message");
        return false;
    } else {
        ctx.log_info("Received RS_DEV.CNF");
        return true;
    }
}

FSMSimpleState::HandleEventReturnType InitSlacState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (check_for_valid_parm_conf()) {
            ctx.signal_state("MATCHING");
            return sa.create_simple<SoundingState>(ctx, SessionParamaters{run_id, ctx.slac_message.get_src_mac()});
        }

        return sa.HANDLED_INTERNALLY;
    } else if (ev == Event::FAILED) {
        return sa.create_simple<FailedState>(ctx);
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void InitSlacState::enter() {

    if (ctx.chip_reset.enabled == true and ctx.modem_vendor == ModemVendor::Lumissil) {
        std::this_thread::sleep_for(
            std::chrono::seconds(10)); // FIXME we need to find out when Lumissil is ready after reset
    }

    ctx.log_info("Entered init state");

    // generate random run_id
    std::random_device rnd_dev;
    std::mt19937 rng(rnd_dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist256(0, 255);

    for (auto& id : this->run_id) {
        id = dist256(rng);
    }
}

FSMSimpleState::CallbackReturnType InitSlacState::callback() {
    if (num_of_tries == 0) {
        return send_parm_req();
    }

    // did already send a parm req, check for timeout

    const auto now = std::chrono::steady_clock::now();
    const auto time_left = milliseconds_left(now, next_timeout);

    if (time_left > 0) {
        // still have time
        return time_left;
    }

    // timeout, check if we still have retries
    if (num_of_tries < 100) {
        // FIXME (aw): the norm says num_of_tries < slac::defs::C_EV_MATCH_RETRY
        // but doesn't seem to work in 'real life'
        return send_parm_req();
    }

    // no retries left fail
    return Event::FAILED;
}

int InitSlacState::send_parm_req() {
    slac::messages::cm_slac_parm_req msg;
    msg.application_type = 0x0;
    msg.security_type = 0x0;
    memcpy(msg.run_id, run_id, sizeof(msg.run_id));

    const uint8_t broadcast_mac[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ctx.send_slac_message(broadcast_mac, msg);

    num_of_tries++;

    next_timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(slac::defs::TT_MATCH_RESPONSE_MS);

    return slac::defs::TT_MATCH_RESPONSE_MS;
}

bool InitSlacState::check_for_valid_parm_conf() {
    const auto mmtype = ctx.slac_message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SLAC_PARAM | slac::defs::MMTYPE_MODE_CNF)) {
        return false;
    }

    // correct message type
    const auto& parm_cnf = ctx.slac_message.get_payload<slac::messages::cm_slac_parm_cnf>();

    // it is not clear, whether we should use the m-sound target anyhow ... or if we should really validate any of the
    // fields except the run id
    const auto same_run_id = (memcmp(parm_cnf.run_id, run_id, sizeof(run_id)) == 0);

    return same_run_id;
}

MatchRequestState::MatchRequestState(Context& ctx, SessionParamaters session_parameters_) :
    FSMSimpleState(ctx), session_parameters(std::move(session_parameters_)) {
}

FSMSimpleState::HandleEventReturnType MatchRequestState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        const auto nmk = check_for_valid_match_req_conf();
        if (nmk) {
            return sa.create_simple<JoinNetworkState>(ctx, nmk);
        }

        return sa.HANDLED_INTERNALLY;
    } else if (ev == Event::FAILED) {
        return sa.create_simple<FailedState>(ctx);
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void MatchRequestState::enter() {
    ctx.log_info("Entered MatchRequestState state");
}

FSMSimpleState::CallbackReturnType MatchRequestState::callback() {
    if (num_of_tries == 0) {
        return send_match_req();
    }

    // did already send a parm req, check for timeout

    const auto now = std::chrono::steady_clock::now();
    const auto time_left = milliseconds_left(now, next_timeout);

    if (time_left > 0) {
        // still have time
        return time_left;
    }

    // timeout, check if we still have retries
    if (num_of_tries < slac::defs::C_EV_MATCH_RETRY) {
        return send_match_req();
    }

    // no retries left fail
    return Event::FAILED;
}

const uint8_t* MatchRequestState::check_for_valid_match_req_conf() {
    if (num_of_tries == 0) {
        return nullptr;
    }

    const auto mmtype = ctx.slac_message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SLAC_MATCH | slac::defs::MMTYPE_MODE_CNF)) {
        return nullptr;
    }

    // correct message type
    const auto& match_cnf = ctx.slac_message.get_payload<slac::messages::cm_slac_match_cnf>();

    const auto run_id_match =
        (memcmp(session_parameters.run_id, match_cnf.run_id, sizeof(session_parameters.run_id)) == 0);

    if (run_id_match) {
        return match_cnf.nmk;
    } else {
        return nullptr;
    }
}

int MatchRequestState::send_match_req() {
    slac::messages::cm_slac_match_req msg;
    msg.application_type = 0x0;
    msg.security_type = 0x0;
    msg.mvf_length = htole16(0x3e); // FIXME (aw) fixed constant
    memset(msg.pev_id, 0, sizeof(msg.pev_id));
    memcpy(msg.pev_mac, ctx.plc_mac, sizeof(msg.pev_mac));
    memset(msg.evse_id, 0, sizeof(msg.evse_id));
    memcpy(msg.evse_mac, session_parameters.evse_mac, sizeof(msg.evse_mac));
    memcpy(msg.run_id, session_parameters.run_id, sizeof(msg.run_id));
    memset(msg._reserved, 0, sizeof(msg._reserved));

    ctx.send_slac_message(session_parameters.evse_mac, msg);

    num_of_tries++;

    next_timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(slac::defs::TT_MATCH_RESPONSE_MS);

    return slac::defs::TT_MATCH_RESPONSE_MS;
}

JoinNetworkState::JoinNetworkState(Context& ctx, const uint8_t* nmk_) : FSMSimpleState(ctx) {
    memcpy(nmk, nmk_, sizeof(nmk));
}

FSMSimpleState::HandleEventReturnType JoinNetworkState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::SLAC_MESSAGE) {
        if (check_for_valid_set_key_conf()) {
            // FIXME (aw): later on, we also need to distinguish between set_key failed
            return sa.create_simple<MatchedState>(ctx);
        }

        return sa.HANDLED_INTERNALLY;
    } else if (ev == Event::FAILED) {
        return sa.create_simple<FailedState>(ctx);
    } else if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else {
        return sa.PASS_ON;
    }
}

void JoinNetworkState::enter() {
    ctx.log_info("Entered JoinNetwork state");

    slac::messages::cm_set_key_req msg;
    msg.key_type = slac::defs::CM_SET_KEY_REQ_KEY_TYPE_NMK;
    msg.my_nonce = 0xAAAAAAAA;
    msg.your_nonce = 0x00000000;
    msg.pid = slac::defs::CM_SET_KEY_REQ_PID_HLE;
    msg.prn = htole16(slac::defs::CM_SET_KEY_REQ_PRN_UNUSED);
    msg.pmn = slac::defs::CM_SET_KEY_REQ_PMN_UNUSED;
    msg.cco_capability = slac::defs::CM_SET_KEY_REQ_CCO_CAP_NONE;
    slac::utils::generate_nid_from_nmk(msg.nid, nmk);
    msg.new_eks = slac::defs::CM_SET_KEY_REQ_PEKS_NMK_KNOWN_TO_STA;
    memcpy(msg.new_key, nmk, sizeof(msg.new_key));

    ctx.send_slac_message(ctx.plc_mac, msg);

    timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(SET_KEY_TIMEOUT_MS);
}

FSMSimpleState::CallbackReturnType JoinNetworkState::callback() {
    const auto now = std::chrono::steady_clock::now();
    const auto time_left = milliseconds_left(now, timeout);

    if (time_left > 0) {
        // still have time
        return time_left;
    }

    // we reached the set key timeout
    return Event::FAILED;
}

bool JoinNetworkState::check_for_valid_set_key_conf() {
    const auto mmtype = ctx.slac_message.get_mmtype();
    if (mmtype != (slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF)) {
        return false;
    }

    // correct message type
    const auto& set_key_cnf = ctx.slac_message.get_payload<slac::messages::cm_set_key_cnf>();

    // FIXME (aw): validation of the message?

    return true;
}

FSMSimpleState::HandleEventReturnType MatchedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else if (ev == Event::SLAC_MESSAGE) {
        return sa.HANDLED_INTERNALLY;
    } else {
        return sa.PASS_ON;
    }
}

FSMSimpleState::HandleEventReturnType FailedState::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::RESET) {
        return sa.create_simple<ResetState>(ctx);
    } else if (ev == Event::SLAC_MESSAGE) {
        return sa.HANDLED_INTERNALLY;
    } else {
        return sa.PASS_ON;
    }
}

} // namespace slac::fsm::ev
