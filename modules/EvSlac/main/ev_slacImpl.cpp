// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ev_slacImpl.hpp"

#include <fmt/core.h>

#include <slac/io.hpp>

#include "fsm_controller.hpp"

static std::promise<void> module_ready;
// FIXME (aw): this is ugly, but due to the design of the auto-generated module skeleton ..
static std::unique_ptr<FSMController> fsm_ctrl{nullptr};

namespace module {
namespace main {

void ev_slacImpl::init() {
    // setup evse fsm thread
    std::thread(&ev_slacImpl::run, this).detach();
}

void ev_slacImpl::ready() {
    module_ready.set_value();
}

void ev_slacImpl::run() {
    // wait until ready
    module_ready.get_future().get();

    // initialize slac i/o
    SlacIO slac_io;
    try {
        slac_io.init(config.device);
    } catch (const std::exception& e) {
        EVLOG_error << fmt::format("Couldn't open device {} for SLAC communication. Reason: {}", config.device,
                                   e.what());
        raise_error(
            error_factory->create_error("generic/CommunicationFault", "", "Could not open device " + config.device));
        return;
    }

    // setup callbacks
    slac::fsm::ev::ContextCallbacks callbacks;
    callbacks.send_raw_slac = [&slac_io](slac::messages::HomeplugMessage& msg) { slac_io.send(msg); };

    callbacks.signal_state = [this](const std::string& value) {
        if (value == "MATCHED") {
            publish_dlink_ready(true);
        } else if (value == "UNMATCHED") {
            publish_dlink_ready(false);
        }
        publish_state(value);
    };

    callbacks.log = [](const std::string& text) { EVLOG_info << "EvSlac: " << text; };

    auto fsm_ctx = slac::fsm::ev::Context(callbacks);
    // fsm_ctx.slac_config.set_key_timeout_ms = config.set_key_timeout_ms;
    // fsm_ctx.slac_config.ac_mode_five_percent = config.ac_mode_five_percent;
    // fsm_ctx.slac_config.sounding_atten_adjustment = config.sounding_attenuation_adjustment;
    // fsm_ctx.slac_config.generate_nmk();

    fsm_ctrl = std::make_unique<FSMController>(fsm_ctx);

    slac_io.run([](slac::messages::HomeplugMessage& msg) { fsm_ctrl->signal_new_slac_message(msg); });

    fsm_ctrl->run();
}

void ev_slacImpl::handle_reset() {
    fsm_ctrl->signal_reset();
}

bool ev_slacImpl::handle_trigger_matching() {
    fsm_ctrl->signal_trigger_matching();
    return true;
}

} // namespace main
} // namespace module
