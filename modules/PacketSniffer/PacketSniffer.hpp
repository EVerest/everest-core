// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef PACKET_SNIFFER_HPP
#define PACKET_SNIFFER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_manager/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <pcap.h>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string device;
    std::string session_logging_path;
};

class PacketSniffer : public Everest::ModuleBase {
public:
    PacketSniffer() = delete;
    PacketSniffer(const ModuleInfo& info, std::unique_ptr<emptyImplBase> p_main,
                  std::unique_ptr<evse_managerIntf> r_evse_manager, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_evse_manager(std::move(r_evse_manager)), config(config){};

    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<evse_managerIntf> r_evse_manager;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
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
    void capture(const std::string& logpath, const std::string& session_id);
    pcap_t* p_handle{nullptr};
    pcap_dumper_t* pdumpfile{nullptr};
    char errbuf[PCAP_ERRBUF_SIZE];
    std::atomic_bool capturing_stopped{false};
    std::atomic_bool already_started{false};
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // PACKET_SNIFFER_HPP
