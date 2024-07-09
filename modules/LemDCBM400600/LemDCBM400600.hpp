// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef LEM_DCBM400600_HPP
#define LEM_DCBM400600_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/powermeter/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ip_address;
    int port;
    std::string meter_tls_certificate;
    std::string ntp_server_1_ip_addr;
    int ntp_server_1_port;
    std::string ntp_server_2_ip_addr;
    int ntp_server_2_port;
    std::string meter_timezone;
    std::string meter_dst;
    int resilience_initial_connection_retries;
    int resilience_initial_connection_retry_delay;
    int resilience_transaction_request_retries;
    int resilience_transaction_request_retry_delay;
    int cable_id;
    int tariff_id;
    int SC;
    std::string UV;
    std::string UD;
};

class LemDCBM400600 : public Everest::ModuleBase {
public:
    LemDCBM400600() = delete;
    LemDCBM400600(const ModuleInfo& info, std::unique_ptr<powermeterImplBase> p_main, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), config(config){};

    const std::unique_ptr<powermeterImplBase> p_main;
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
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // LEM_DCBM400600_HPP
