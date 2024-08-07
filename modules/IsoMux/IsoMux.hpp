// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ISO_MUX_HPP
#define ISO_MUX_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ISO15118_charger/Interface.hpp>
#include <generated/interfaces/evse_security/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "v2g_ctx.hpp"
#include <tls.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string device;
    std::string tls_security;
    bool tls_key_logging;
    int tls_timeout;
    int proxy_port_iso2;
    int proxy_port_iso20;
};

class IsoMux : public Everest::ModuleBase {
public:
    IsoMux() = delete;
    IsoMux(const ModuleInfo& info, std::unique_ptr<ISO15118_chargerImplBase> p_charger,
           std::unique_ptr<evse_securityIntf> r_security, std::unique_ptr<ISO15118_chargerIntf> r_iso2,
           std::unique_ptr<ISO15118_chargerIntf> r_iso20, Conf& config) :
        ModuleBase(info),
        p_charger(std::move(p_charger)),
        r_security(std::move(r_security)),
        r_iso2(std::move(r_iso2)),
        r_iso20(std::move(r_iso20)),
        config(config){};

    const std::unique_ptr<ISO15118_chargerImplBase> p_charger;
    const std::unique_ptr<evse_securityIntf> r_security;
    const std::unique_ptr<ISO15118_chargerIntf> r_iso2;
    const std::unique_ptr<ISO15118_chargerIntf> r_iso20;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    ~IsoMux();
    bool selected_iso20();
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

    tls::Server tls_server;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // ISO_MUX_HPP
