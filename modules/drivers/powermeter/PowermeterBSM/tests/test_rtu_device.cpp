// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "test_helper.hpp"

#include "lib/sunspec_base.hpp" // helper stuff for sunspec model definition / handling

#include "lib/BSMSnapshotModel.hpp"
#include "lib/sunspec_models.hpp" // sunspec models

#include "lib/known_model.hpp"
#include "lib/transport.hpp"

#include <modbus/modbus_client.hpp>

#include <chrono>
#include <fstream>
#include <thread>

using namespace std::chrono_literals;
using namespace std::string_literals;

// we are only using function codes read / write multiple registers
using ModbusUnitId = protocol_related_types::ModbusUnitId;

struct TestBSMPowerMeter : public ::testing::Test {

    std::string serial_device_name{"/dev/ttyUSB0"};
    everest::connection::SerialDeviceConfiguration cfg;
    everest::connection::SerialDeviceLogToStream serial_device;
    everest::connection::RTUConnection rtu_connection;
    everest::modbus::ModbusRTUClient modbus_client;

    std::string current_test_name;
    std::string current_test_suite;
    std::fstream logstream;

    const ModbusUnitId unit_id = 42;
    const protocol_related_types::ModbusRegisterAddress default_sunspec_base_address{40000};

    TestBSMPowerMeter() :
        cfg(serial_device_name), serial_device(cfg), rtu_connection(serial_device), modbus_client(rtu_connection){};

    void SetUp() override {

        current_test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        current_test_suite = ::testing::UnitTest::GetInstance()->current_test_suite()->name();
        std::cout << __PRETTY_FUNCTION__ << " " << current_test_suite << " " << current_test_name << "\n";
        std::string logfile_name = current_test_suite + "_" + current_test_name + ".log";
        logstream.open(logfile_name, std::fstream::out);
        serial_device.set_stream(&logstream);
    }

    void TearDown() override {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        logstream.close();
    }
};

TEST_F(TestBSMPowerMeter, DriverBaseCheckSunspecDevice) {

    // verify that a sunspec compatilbe device is connected
    // throws if no device found
    ASSERT_NO_THROW(test_helper::get_sunspec_device_base_register(modbus_client, unit_id));
}

TEST_F(TestBSMPowerMeter, DriverBaseCheckBSMWS36A) {

    // verify that a BSM-WS36A-H01-1311-0000 device is connected
    protocol_related_types::ModbusRegisterAddress base_address =
        test_helper::get_sunspec_device_base_register(serial_device, unit_id);

    transport::AbstractDataTransport::Spt fetcher_spt =
        std::make_shared<transport::ModbusTransport>(serial_device_name, unit_id, base_address);

    {
        transport::DataVector dv = fetcher_spt->fetch(3_sma, calc_model_size_in_register(sunspec_model::Common::Model));
        sunspec_model::Common common(dv);
        EXPECT_EQ(common.Mn(), "BAUER Electronic");
        EXPECT_EQ(common.Md(), "BSM-WS36A-H01-1311-0000");
    }

    {
        transport::DataVector dv = fetcher_spt->fetch(known_model::Sunspec_Common);
        sunspec_model::Common common(dv);
        EXPECT_EQ(common.Mn(), "BAUER Electronic");
        EXPECT_EQ(common.Md(), "BSM-WS36A-H01-1311-0000");
    }
}

TEST_F(TestBSMPowerMeter, ReadBSMSnapshot) {

    transport::AbstractDataTransport::Spt transport_spt =
        std::make_shared<transport::ModbusTransport>(serial_device_name, unit_id, default_sunspec_base_address);

    for (std::size_t counter = 0; counter < 10; counter++) {

        ASSERT_TRUE(transport_spt->trigger_snapshot_generation_BSM());
        transport::DataVector dv = transport_spt->fetch(known_model::BSM_CurrentSnapshot);
        bsm::SignedSnapshot snapshot(dv);

        std::cout << std::dec;

        model_to_stream(std::cout, snapshot,
                        {5, // watt
                         8, // response counter
                         9, // Operation seconds
                         3} // Total Wh Exp
        );

        std::this_thread::sleep_for(5000ms);
    }
}

TEST_F(TestBSMPowerMeter, ReadBSM_OCMFSnapshot) {

    transport::AbstractDataTransport::Spt transport_spt =
        std::make_shared<transport::ModbusTransport>(serial_device_name, unit_id, default_sunspec_base_address);

    for (std::size_t counter = 0; counter < 10; counter++) {

        ASSERT_TRUE(transport_spt->trigger_snapshot_generation_BSM_OCMF());
        transport::DataVector dv = transport_spt->fetch(known_model::BSM_OCMF_CurrentSnapshot);
        bsm::SignedOCMFSnapshot snapshot(dv);

        std::cout << std::dec;

        // { "ID", PointType::uint16 },
        // { "L", PointType::uint16 },
        // { "Type", PointType::enum16 },
        // { "St", PointType::enum16 },
        // { "O", PointType::string , 496 }

        model_to_stream(std::cout, snapshot,
                        {
                            0, // ID
                            1, // L
                            2, // type
                            3, // status
                            4, // OCMF result string
                        });

        std::this_thread::sleep_for(5000ms);
    }
}
