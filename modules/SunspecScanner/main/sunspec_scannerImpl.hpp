// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SUNSPEC_SCANNER_IMPL_HPP
#define MAIN_SUNSPEC_SCANNER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/sunspec_scanner/Implementation.hpp>

#include "../SunspecScanner.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <stdexcept>

#include <connection/connection.hpp>
#include <connection/exceptions.hpp>
#include <sunspec/reader.hpp>
#include <sunspec/sunspec_device_mapping.hpp>
#include <utils/thread.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class sunspec_scannerImpl : public sunspec_scannerImplBase {
public:
    sunspec_scannerImpl() = delete;
    sunspec_scannerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<SunspecScanner>& mod, Conf& config) :
        sunspec_scannerImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual Object handle_scan_unit(std::string& ip_address, int& port, int& unit) override;
    virtual Object handle_scan_port(std::string& ip_address, int& port) override;
    virtual Object handle_scan_device(std::string& ip_address) override;
    virtual Object handle_scan_network() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<SunspecScanner>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SUNSPEC_SCANNER_IMPL_HPP
