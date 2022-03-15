// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

#include <future>

#include <fmt/core.h>
#include <fsm/specialization/sync/posix.hpp>
#include <slac/channel.hpp>

#include "evse_fsm.hpp"
#include "slac_io.hpp"

void fsm_logging_callback(const std::string& msg) {
    EVLOG(debug) << fmt::format("SlacFSM: {}", msg);
}

static std::mutex comm_mtx;
static std::promise<void> module_ready;
static fsm::sync::PosixController<EvseFSM::StateHandleType, &fsm_logging_callback> fsm_ctrl;
static std::condition_variable new_event_cv;
static bool new_event;

// void notify() {
//     new_event = true;
//     new_event_cv.notify_one();
// }

bool send_event(const EvseFSM::EventInfoType::BaseType& event) {
    std::lock_guard<std::mutex> lck(comm_mtx);
    new_event = true;

    auto ret = fsm_ctrl.submit_event(event);

    new_event_cv.notify_one();
    return ret;
}

namespace module {
namespace main {

void slacImpl::init() {
    // initialize static variables
    new_event = false;

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

    EvseFSM fsm(slac_io);

    EVLOG(info) << "Starting the SLAC state machine";

    bool matched = false;
    auto cur_state_id = fsm.get_initial_state().id.id;
    fsm_ctrl.reset(fsm.get_initial_state());

    slac_io.run([this](slac::messages::HomeplugMessage& msg) {
        if (send_event(EventSlacMessage(msg))) {
            return;
        }

        EVLOG(info) << fmt::format("No SLAC message handler for current state: {}", fsm_ctrl.current_state()->id.name);
    });

    std::unique_lock<std::mutex> feed_lck(comm_mtx);
    int next_timeout_in_ms = 0;

    while (true) {
        do {
            // run the next cycle on the state machine
            next_timeout_in_ms = fsm_ctrl.feed();

            // FIXME (aw): we probably want to react on all state changes
        } while (next_timeout_in_ms == 0);

        // FIXME (aw): this simple logic should be implemented in the fsm lib
        bool changed_state = false;
        changed_state = (cur_state_id != fsm_ctrl.current_state()->id.id);
        cur_state_id = fsm_ctrl.current_state()->id.id;

        if (changed_state) {
            switch (cur_state_id) {
            case State::Matching:
                publish_state("MATCHING");
                break;
            case State::Matched:
                matched = true;
                publish_dlink_ready(true);
                publish_state("MATCHED");
                break;
            case State::SignalError:
                publish_request_error_routine();
                break;
            case State::Idle:
                publish_state("UNMATCHED");
                break;
            case State::ReceivedSlacMatch:
                // FIXME (aw): we could publish a var, that SLAC should
                //             be in principle available now
                // FIMXE (aw): this needs to be implemented in a higher level ->
                //             stipulate link establishment if appropriate
                fsm_ctrl.submit_event(EventLinkDetected());
                break;
            default:
                break;
            }
        }

        if (matched && (cur_state_id != State::Matched)) {
            matched = false;
            publish_dlink_ready(false);
        }

        // FIXME (aw): handle disconnection / un-matching etc.

        if (next_timeout_in_ms < 0) {
            new_event_cv.wait(feed_lck, [] { return new_event; });
        } else {
            new_event_cv.wait_for(feed_lck, std::chrono::milliseconds(next_timeout_in_ms), [] { return new_event; });
        }

        if (new_event) { // the if might be not necessary
            new_event = false;
        }
    }
}

void slacImpl::handle_reset(bool& enable) {
    // FIXME (aw): the enable could be used for power saving etc, but it is not implemented yet
    send_event(EventReset());
};

bool slacImpl::handle_enter_bcd() {
    return send_event(EventEnterBCD());
};

bool slacImpl::handle_leave_bcd() {
    return send_event(EventLeaveBCD());
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
