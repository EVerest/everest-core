#ifndef MAIN_ISO15118_3_SLAC_IMPL_HPP
#define MAIN_ISO15118_3_SLAC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 0.0.1
//

#include <generated/ISO15118_3_SLAC/Implementation.hpp>

#include "../SLAC_openplcutils.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <utils/thread.hpp>

#include <mutex>

extern "C" {
#include <open-plc-utils/slac/slac.h>
}
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string device;
    std::string sid;
    std::string nmk;
    std::string nid;
    int timeout;
    int time_to_sound;
    int number_of_sounds;
};

class ISO15118_3_SLACImpl : public ISO15118_3_SLACImplBase {
public:
    ISO15118_3_SLACImpl() = delete;
    ISO15118_3_SLACImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<SLAC_openplcutils>& mod, Conf& config) :
        ISO15118_3_SLACImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_DLINK_TERMINATE() override;
    virtual void handle_DLINK_ERROR() override;
    virtual void handle_DLINK_PAUSE() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<SLAC_openplcutils>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    void run();
    void runStateUpdate();

    // FIXME (aw): we don't need to pass session, channel and message at all ...
    void slac_initialize(struct session* session);
    signed int identifier(struct session* session, struct channel* channel);
    void set_session_state(unsigned int state);
    unsigned int get_session_state();
    void UnoccupiedState(struct session* session, struct channel* channel, struct message* message);
    void UnmatchedState(struct session* session, struct channel* channel, struct message* message);
    void MatchedState(struct session* session, struct channel* channel, struct message* message);

    Everest::Thread slacThread{};
    Everest::Thread stateUpdateThread{};

    struct session session;
    struct std::mutex sessionStatusMutex;
    struct message message;
    char ifname[50];
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_ISO15118_3_SLAC_IMPL_HPP
