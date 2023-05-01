// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2023 Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

#include <future>

#include <fmt/core.h>
#include <slac/channel.hpp>

#include "fsm_controller.hpp"
#include "slac_io.hpp"

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
    ContextCallbacks callbacks;
    callbacks.send_raw_slac = [&slac_io](slac::messages::HomeplugMessage& msg) { slac_io.send(msg); };

    callbacks.signal_dlink_ready = [this](bool value) { publish_dlink_ready(value); };

    callbacks.signal_state = [this](const std::string& value) { publish_state(value); };

    callbacks.signal_error_routine_request = [this]() { publish_request_error_routine(boost::blank()); };

    callbacks.log = [](const std::string& text) { EVLOG_debug << text; };

    if (config.publish_mac_on_first_parm_req) {
        callbacks.signal_ev_mac_address_parm_req = [this](const std::string& mac) { publish_ev_mac_address(mac); };
    }

    if (config.publish_mac_on_match_cnf) {
        callbacks.signal_ev_mac_address_match_cnf = [this](const std::string& mac) { publish_ev_mac_address(mac); };
    }

    auto fsm_ctx = Context(callbacks);
    fsm_ctx.slac_config.set_key_timeout_ms = config.set_key_timeout_ms;
    fsm_ctx.slac_config.ac_mode_five_percent = config.ac_mode_five_percent;
    fsm_ctx.slac_config.sounding_atten_adjustment = config.sounding_attenuation_adjustment;
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
    EVLOG_AND_THROW(Everest::EverestInternalError("dlink_terminate() not implemented yet"));
    return false;
};

bool slacImpl::handle_dlink_error() {
    EVLOG_AND_THROW(Everest::EverestInternalError("dlink_error() not implemented yet"));
    return false;
};

bool slacImpl::handle_dlink_pause() {
    EVLOG_AND_THROW(Everest::EverestInternalError("dlink_pause() not implemented yet"));
    return false;
};

} // namespace main
} // namespace module
