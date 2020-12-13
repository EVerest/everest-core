#include "ISO15118_3_SLACImpl.hpp"

#include <sys/ioctl.h>

extern "C" {
#include <open-plc-utils/tools/flags.h>
#include <open-plc-utils/tools/memory.h>
#include <open-plc-utils/tools/permissions.h>
}

// FIXME (aw): remove defines ...
#define EVSE_STATE_UNAVAILABLE 0
#define EVSE_STATE_UNOCCUPIED  1
#define EVSE_STATE_UNMATCHED   2
#define EVSE_STATE_MATCHED     3

extern struct channel channel;

namespace module {
namespace main {

static std::string stateToString(unsigned int state) {
    switch (state) {
    case 0:
        return "UNAVAILABLE";
    case 1:
        return "UNOCCUPIED";
    case 2:
        return "UNMATCHED";
    case 3:
        return "MATCHED";
    default:
        // FIXME (aw): throw exception?
        return "";
    }
}

void ISO15118_3_SLACImpl::init() {
    memset(&session, 0, sizeof(session));
    memset(&message, 0, sizeof(message));

    strcpy(ifname, config.device.c_str());
    channel.ifname = ifname;
    EVLOG(info) << "Using ethernet device " << channel.ifname << std::endl;

    channel.timeout = config.timeout;

    initchannel(&channel);
    desuid();
    openchannel(&channel);
    slac_initialize(&session);
    identifier(&session, &channel);

    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, channel.ifname, sizeof(ifr.ifr_name));
    // set MTU to minimum value of 1280 for ipv6 as RFC1981 does not really work here
    ifr.ifr_mtu = 1280;
    if (ioctl(channel.fd, SIOCSIFMTU, (caddr_t)&ifr) < 0) {
        EVLOG(error) << "ERROR: Cannot set MTU on device " << channel.ifname << std::endl;
        exit(EXIT_FAILURE);
    }

    if (evse_cm_set_key(&session, &channel, &message))
        slac_debug(&session, 1, __func__, "Can't set key.");
    sleep(session.settletime);
}

void ISO15118_3_SLACImpl::ready() {
    slacThread = std::thread(&ISO15118_3_SLACImpl::run, this);
    stateUpdateThread = std::thread(&ISO15118_3_SLACImpl::runStateUpdate, this);
}

void ISO15118_3_SLACImpl::handle_DLINK_TERMINATE() {

    std::lock_guard<std::mutex> lock(sessionStatusMutex);
    // FIXME (aw): whats that?
    session.state == EVSE_STATE_UNOCCUPIED;

    // this should probably be session_set_state(EVSE_STATE_UNOCCUPIED);
};

void ISO15118_3_SLACImpl::handle_DLINK_ERROR(){
    // your code for cmd DLINK_ERROR goes here
};

void ISO15118_3_SLACImpl::handle_DLINK_PAUSE(){
    // your code for cmd DLINK_PAUSE goes here
};

void ISO15118_3_SLACImpl::runStateUpdate() {
    unsigned int state = 0;

    while (!stateUpdateThread.shouldExit()) {
        auto state = get_session_state();
        publish_state(stateToString(state));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void ISO15118_3_SLACImpl::run() {
    auto state = get_session_state();

    while (state) {
        if (slacThread.shouldExit())
            return;

        if (state == EVSE_STATE_UNOCCUPIED) {
            publish_state(stateToString(state));
            publish_DLINK_READY(false);
            UnoccupiedState(&session, &channel, &message);
        } else if (state == EVSE_STATE_UNMATCHED) {
            publish_state(stateToString(state));
            publish_DLINK_READY(false);
            UnmatchedState(&session, &channel, &message);
        } else if (state == EVSE_STATE_MATCHED) {
            publish_state(stateToString(state));
            publish_DLINK_READY(true);
            MatchedState(&session, &channel, &message);
        }

        state = get_session_state();
    }
    closechannel(&channel);
}

void ISO15118_3_SLACImpl::slac_initialize(struct session* session) {
    session->next = session->prev = session;
    hexencode(session->EVSE_ID, sizeof(session->EVSE_ID), config.sid.c_str());
    hexencode(session->NMK, sizeof(session->NMK), config.nmk.c_str());
    hexencode(session->NID, sizeof(session->NID), config.nid.c_str());
    session->NUM_SOUNDS = config.number_of_sounds;
    session->TIME_OUT = config.time_to_sound;
    session->RESP_TYPE = 1;
    session->chargetime = 2;
    session->settletime = 10;
    memcpy(session->original_nmk, session->NMK, sizeof(session->original_nmk));
    memcpy(session->original_nid, session->NID, sizeof(session->original_nid));
    set_session_state(EVSE_STATE_UNOCCUPIED);
    slac_session(session);
    return;
}

/*====================================================================*
 *
 *   signed identifier (struct session * session, struct channel * channel);
 *
 *   copy channel host address to session EVSE MAC address; set session
 *   EVSE identifier to zeros;
 *
 *--------------------------------------------------------------------*/

signed int ISO15118_3_SLACImpl::identifier(struct session* session, struct channel* channel) {
    memcpy(session->EVSE_MAC, channel->host, sizeof(session->EVSE_MAC));
    return 0;
}

/*====================================================================*
 *
 *   void UnoccupiedState (struct session * session, struct channel * channel, struct message * message);
 *
 *--------------------------------------------------------------------*/

void ISO15118_3_SLACImpl::UnoccupiedState(struct session* session, struct channel* channel, struct message* message) {
    slac_session(session);
    slac_debug(session, 0, __func__, "Listening ...");
    while (evse_cm_slac_param(session, channel, message))
        ;

    set_session_state(EVSE_STATE_UNMATCHED);

    return;
}

/*====================================================================*
 *
 *   void MatchingState (struct session * session, struct channel * channel, struct message * message);
 *
 *   the cm_start_atten_char message establishes msound count and
 *   timeout;
 *
 *--------------------------------------------------------------------*/

void ISO15118_3_SLACImpl::UnmatchedState(struct session* session, struct channel* channel, struct message* message) {
    slac_session(session);
    slac_debug(session, 0, __func__, "Sounding ...");
    if (evse_cm_start_atten_char(session, channel, message)) {
        set_session_state(EVSE_STATE_UNOCCUPIED);
        return;
    }
    if (evse_cm_mnbc_sound(session, channel, message)) {
        set_session_state(EVSE_STATE_UNOCCUPIED);
        return;
    }
    if (evse_cm_atten_char(session, channel, message)) {
        set_session_state(EVSE_STATE_UNOCCUPIED);
        return;
    }
    if (_allset(session->flags, SLAC_SOUNDONLY)) {
        set_session_state(EVSE_STATE_UNAVAILABLE);
        return;
    }

    slac_debug(session, 0, __func__, "Matching ...");

    if (evse_cm_slac_match(session, channel, message)) {
        set_session_state(EVSE_STATE_UNOCCUPIED);
        return;
    }

    set_session_state(EVSE_STATE_MATCHED);

    return;
}

/*====================================================================*
 *
 *   void MatchedState (struct session * session, struct channel * channel, struct message * message);
 *
 *--------------------------------------------------------------------*/

void ISO15118_3_SLACImpl::MatchedState(struct session* session, struct channel* channel, struct message* message) {
    /*slac_debug (session, 0, __func__, "Connecting ...");
    slac_debug (session, 0, __func__, "waiting for pev to settle ...");
    sleep (1);
    slac_debug (session, 0, __func__, "Charging (%d) ...\n\n", session->counter++);
    sleep (2);
    slac_debug (session, 0, __func__, "Disconnecting ...");

    slac_debug (session, 0, __func__, "waiting for pev to settle ...");
    sleep (1);*/

    // session->state = EVSE_STATE_UNOCCUPIED;

    auto state = get_session_state();

    sleep(10);
    session->state = EVSE_STATE_UNOCCUPIED;
    return;

    while (state == EVSE_STATE_MATCHED) {
        state = get_session_state();
        usleep(100);
    }
    return;
}

void ISO15118_3_SLACImpl::set_session_state(unsigned int state) {
    std::lock_guard<std::mutex> lock(sessionStatusMutex);
    session.state = state;
}

unsigned int ISO15118_3_SLACImpl::get_session_state() {
    std::lock_guard<std::mutex> lock(sessionStatusMutex);
    return session.state;
}

} // namespace main
} // namespace module
