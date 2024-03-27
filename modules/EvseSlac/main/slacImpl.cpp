// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2023 Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

#include <future>

#include <fmt/core.h>
#include <slac/channel.hpp>
#include <slac/io.hpp>

#include "fsm_controller.hpp"

static std::promise<void> module_ready;
// FIXME (aw): this is ugly, but due to the design of the auto-generated module skeleton ..
static std::unique_ptr<FSMController> fsm_ctrl{nullptr};

namespace module {
namespace main {

static std::string mac_to_ascii(const std::string& mac_binary) {
    if (mac_binary.size() < 6)
        return "";
    return fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", mac_binary[0], mac_binary[1], mac_binary[2],
                       mac_binary[3], mac_binary[4], mac_binary[5]);
}

void slacImpl::init() {
    // validate config settings
    if (config.evse_id.length() != slac::defs::STATION_ID_LEN) {
        EVLOG_AND_THROW(
            Everest::EverestConfigError(fmt::format("The EVSE id config needs to be exactly {} octets (got {}).",
                                                    slac::defs::STATION_ID_LEN, config.evse_id.length())));
    }

    if (config.nid.length() != slac::defs::NID_LEN) {
        EVLOG_AND_THROW(Everest::EverestConfigError(fmt::format(
            "The NID config needs to be exactly {} octets (got {}).", slac::defs::NID_LEN, config.nid.length())));
    }

    // setup evse fsm thread
    std::thread(&slacImpl::run, this).detach();
}

void slacImpl::ready() {
    // let the waiting run thread go
    module_ready.set_value();
}

void slacImpl::run() {
    // wait until ready
    module_ready.get_future().get();

    // initialize slac i/o
    SlacIO slac_io;
    try {
        slac_io.init(config.device);
    } catch (const std::exception& e) {
        EVLOG_AND_THROW(Everest::EverestBaseRuntimeError(
            fmt::format("Couldn't open device {} for SLAC communication. Reason: {}", config.device, e.what())));
    }

    // setup callbacks
    slac::fsm::evse::ContextCallbacks callbacks;
    callbacks.send_raw_slac = [&slac_io](slac::messages::HomeplugMessage& msg) { slac_io.send(msg); };

    callbacks.signal_dlink_ready = [this](bool value) { publish_dlink_ready(value); };

    callbacks.signal_state = [this](const std::string& value) { publish_state(value); };

    callbacks.signal_error_routine_request = [this]() { publish_request_error_routine(nullptr); };

    callbacks.log = [](const std::string& text) { EVLOG_info << text; };

    if (config.publish_mac_on_first_parm_req) {
        callbacks.signal_ev_mac_address_parm_req = [this](const std::string& mac) { publish_ev_mac_address(mac); };
    }

    if (config.publish_mac_on_match_cnf) {
        callbacks.signal_ev_mac_address_match_cnf = [this](const std::string& mac) { publish_ev_mac_address(mac); };
    }

    auto fsm_ctx = slac::fsm::evse::Context(callbacks);
    fsm_ctx.slac_config.set_key_timeout_ms = config.set_key_timeout_ms;
    fsm_ctx.slac_config.ac_mode_five_percent = config.ac_mode_five_percent;
    fsm_ctx.slac_config.sounding_atten_adjustment = config.sounding_attenuation_adjustment;

    fsm_ctx.slac_config.chip_reset.enabled = config.do_chip_reset;
    fsm_ctx.slac_config.chip_reset.delay_ms = config.chip_reset_delay_ms;
    fsm_ctx.slac_config.chip_reset.timeout_ms = config.chip_reset_timeout_ms;

    fsm_ctx.slac_config.link_status.do_detect = config.link_status_detection;
    fsm_ctx.slac_config.link_status.retry_ms = config.link_status_retry_ms;
    fsm_ctx.slac_config.link_status.timeout_ms = config.link_status_timeout_ms;
    fsm_ctx.slac_config.link_status.debug_simulate_failed_matching = config.debug_simulate_failed_matching;

    fsm_ctx.slac_config.generate_nmk();

    fsm_ctrl = std::make_unique<FSMController>(fsm_ctx);

    slac_io.run([](slac::messages::HomeplugMessage& msg) { fsm_ctrl->signal_new_slac_message(msg); });

    fsm_ctrl->run();
}

void slacImpl::handle_reset(bool& enable) {
    // FIXME (aw): the enable could be used for power saving etc, but it is not implemented yet
    // CC: as power saving is not implemented, we actually don't need to reset at beginning of session (enable=true): At
    // start of everest it is being reset once and then it is enough to reset at the end of each session. This saves
    // some hundreds of msecs at the beginning of the charging session as we do not need to set up keys. Then
    // EvseManager can switch on 5% PWM basically immediately as SLAC is already ready.
    if (!enable) {
        fsm_ctrl->signal_reset();
    }
};

bool slacImpl::handle_enter_bcd() {
    return fsm_ctrl->signal_enter_bcd();
};

bool slacImpl::handle_leave_bcd() {
    return fsm_ctrl->signal_leave_bcd();
};

bool slacImpl::handle_dlink_terminate() {
    // With receiving a D-LINK_TERMINATE.request from HLE, the communication node
    // shall leave the logical network within TP_match_leave. All parameters related
    // to the current link shall be set to the default value and shall change to the status "Unmatched".
    EVLOG_info << "D-LINK_TERMINATE.request received, leaving network.";
    fsm_ctrl->signal_reset();
    return true;
};

bool slacImpl::handle_dlink_error() {
    // The D-LINK_ERROR.request requests lower layers to terminate the data link and restart the matching
    // process by a control pilot transition through state E (on EVSE side this should be state F though)
    // CP signal is handled by EvseManager, so we just need to reset the SLAC state machine here.
    // DLINK_ERROR will be send from HLC layers when they detect that the connection is dead.
    EVLOG_warning << "D-LINK_ERROR.request received";
    fsm_ctrl->signal_reset();
    return true;
};

bool slacImpl::handle_dlink_pause() {
    // The D-LINK_PAUSE.request requests lower layers to enter a power saving mode. While being in this
    // mode, the state will be kept to "Matched".
    // So we don't need to do anything here as we do not support low power mode to power down the PLC modem.
    // This is optional in ISO15118-3.
    EVLOG_info << "D-LINK_PAUSE.request received. Staying in MATCHED, PLC chip stays powered on (low power mode "
                  "optional in -3)";
    return true;
};

} // namespace main
} // namespace module
