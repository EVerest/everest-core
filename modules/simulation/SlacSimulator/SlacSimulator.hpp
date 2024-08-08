// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SLAC_SIMULATOR_HPP
#define SLAC_SIMULATOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ev_slac/Implementation.hpp>
#include <generated/interfaces/slac/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include <atomic>
#include <mutex>
#include <utils/thread.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class SlacSimulator : public Everest::ModuleBase {
public:
    SlacSimulator() = delete;
    SlacSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<slacImplBase> p_evse,
                  std::unique_ptr<ev_slacImplBase> p_ev, Conf& config) :
        ModuleBase(info), mqtt(mqtt_provider), p_evse(std::move(p_evse)), p_ev(std::move(p_ev)), config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<slacImplBase> p_evse;
    const std::unique_ptr<ev_slacImplBase> p_ev;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    enum State {
        STATE_UNMATCHED,
        STATE_MATCHING,
        STATE_MATCHED
    };

    State state_evse = STATE_UNMATCHED;
    State state_ev = STATE_UNMATCHED;
    int cntmatching = 0;

    std::string state_to_string(State s) {
        switch (s) {
        case STATE_UNMATCHED:
            return "UNMATCHED";
        case STATE_MATCHING:
            return "MATCHING";
        case STATE_MATCHED:
            return "MATCHED";
        default:
            return "";
        }
    }

    void set_unmatched_ev();
    void set_matching_ev();
    void set_matched_ev();

    void set_unmatched_evse();
    void set_matching_evse();
    void set_matched_evse();

    // std::mutex slac_simulator_mutex;
    // Everest::Thread slac_simulator_thread_handle;
    void slac_simulator_worker(void);

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // SLAC_SIMULATOR_HPP
