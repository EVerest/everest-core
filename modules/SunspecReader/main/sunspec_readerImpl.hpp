// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SUNSPEC_READER_IMPL_HPP
#define MAIN_SUNSPEC_READER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include <generated/sunspec_reader/Implementation.hpp>

#include "../SunspecReader.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <date/date.h>
#include <date/tz.h>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

#include <sunspec/reader.hpp>
#include <sunspec/sunspec_device_mapping.hpp>
#include <utils/thread.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string ip_address;
    int port;
    int unit;
    std::string query;
    int read_interval;
};

class sunspec_readerImpl : public sunspec_readerImplBase {
public:
    sunspec_readerImpl() = delete;
    sunspec_readerImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<SunspecReader>& mod, Conf& config) :
        sunspec_readerImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // no commands defined for this interface

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<SunspecReader>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    void run_read_loop();

    std::unique_ptr<everest::connection::TCPConnection> tcp_connection;
    std::unique_ptr<everest::modbus::ModbusTCPClient> mb_client;
    std::unique_ptr<everest::sunspec::SunspecDeviceMapping> sdm;
    std::unique_ptr<everest::sunspec::SunspecReader<double>> reader;
    Everest::Thread read_loop_thread;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SUNSPEC_READER_IMPL_HPP
