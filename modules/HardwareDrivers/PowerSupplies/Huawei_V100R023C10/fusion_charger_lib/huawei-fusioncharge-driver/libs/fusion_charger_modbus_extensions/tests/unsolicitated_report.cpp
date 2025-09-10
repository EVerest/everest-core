// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/modbus/extensions/unsolicitated_report.hpp>

using namespace fusion_charger::modbus_extensions;

TEST(UnsolicitatedReportRequest_Device, defragment_2_segments) {
    UnsolicitatedReportRequest::Device device;

    device.segments.push_back({0, 2, {0x00, 0x01, 0x00, 0x02}});
    device.segments.push_back({2, 2, {0x00, 0x03, 0x00, 0x04}});

    device.defragment();

    ASSERT_EQ(device.segments.size(), 1);
    ASSERT_EQ(device.segments[0].registers_start, 0);
    ASSERT_EQ(device.segments[0].registers_count, 4);
    ASSERT_EQ(device.segments[0].registers.size(), 8);
    ASSERT_EQ(device.segments[0].registers[0], 0x00);
    ASSERT_EQ(device.segments[0].registers[1], 0x01);
    ASSERT_EQ(device.segments[0].registers[2], 0x00);
    ASSERT_EQ(device.segments[0].registers[3], 0x02);
    ASSERT_EQ(device.segments[0].registers[4], 0x00);
    ASSERT_EQ(device.segments[0].registers[5], 0x03);
    ASSERT_EQ(device.segments[0].registers[6], 0x00);
    ASSERT_EQ(device.segments[0].registers[7], 0x04);

    device.segments.clear();

    device.segments.push_back({7, 2, {0x00, 0x01, 0x00, 0x02}});
    device.segments.push_back({4, 3, {0x00, 0x03, 0x00, 0x04, 0xde, 0xad}});

    device.defragment();
    ASSERT_EQ(device.segments.size(), 1);

    ASSERT_EQ(device.segments[0].registers_start, 4);
    ASSERT_EQ(device.segments[0].registers_count, 5);
    ASSERT_EQ(device.segments[0].registers.size(), 10);
    ASSERT_EQ(device.segments[0].registers[0], 0x00);
    ASSERT_EQ(device.segments[0].registers[1], 0x03);
    ASSERT_EQ(device.segments[0].registers[2], 0x00);
    ASSERT_EQ(device.segments[0].registers[3], 0x04);
    ASSERT_EQ(device.segments[0].registers[4], 0xde);
    ASSERT_EQ(device.segments[0].registers[5], 0xad);
    ASSERT_EQ(device.segments[0].registers[6], 0x00);
    ASSERT_EQ(device.segments[0].registers[7], 0x01);
    ASSERT_EQ(device.segments[0].registers[8], 0x00);
    ASSERT_EQ(device.segments[0].registers[9], 0x02);
}

TEST(UnsolicitatedReportRequest_Device, defragment_3_segments) {
    UnsolicitatedReportRequest::Device device;

    device.segments.push_back({0, 2, {0x00, 0x01, 0x00, 0x02}});
    device.segments.push_back({2, 2, {0x00, 0x03, 0x00, 0x04}});
    device.segments.push_back({4, 2, {0x00, 0x05, 0x00, 0x06}});

    device.defragment();

    ASSERT_EQ(device.segments.size(), 1);
    ASSERT_EQ(device.segments[0].registers_start, 0);
    ASSERT_EQ(device.segments[0].registers_count, 6);
    ASSERT_EQ(device.segments[0].registers.size(), 12);
    ASSERT_EQ(device.segments[0].registers[0], 0x00);
    ASSERT_EQ(device.segments[0].registers[1], 0x01);
    ASSERT_EQ(device.segments[0].registers[2], 0x00);
    ASSERT_EQ(device.segments[0].registers[3], 0x02);
    ASSERT_EQ(device.segments[0].registers[4], 0x00);
    ASSERT_EQ(device.segments[0].registers[5], 0x03);
    ASSERT_EQ(device.segments[0].registers[6], 0x00);
    ASSERT_EQ(device.segments[0].registers[7], 0x04);
    ASSERT_EQ(device.segments[0].registers[8], 0x00);
    ASSERT_EQ(device.segments[0].registers[9], 0x05);
    ASSERT_EQ(device.segments[0].registers[10], 0x00);
    ASSERT_EQ(device.segments[0].registers[11], 0x06);
}

TEST(UnsolicitatedReportRequest_Device, defragment_2_segments_in_pool_of_3) {
    UnsolicitatedReportRequest::Device device;

    device.segments.push_back({0, 2, {0x00, 0x01, 0x00, 0x02}});
    device.segments.push_back({2, 2, {0x00, 0x03, 0x00, 0x04}});
    device.segments.push_back({5, 2, {0x00, 0x05, 0x00, 0x06}});

    device.defragment();

    ASSERT_EQ(device.segments.size(), 2);
    ASSERT_EQ(device.segments[0].registers_start, 0);
    ASSERT_EQ(device.segments[0].registers_count, 4);
    ASSERT_EQ(device.segments[0].registers.size(), 8);
    ASSERT_EQ(device.segments[0].registers[0], 0x00);
    ASSERT_EQ(device.segments[0].registers[1], 0x01);
    ASSERT_EQ(device.segments[0].registers[2], 0x00);
    ASSERT_EQ(device.segments[0].registers[3], 0x02);
    ASSERT_EQ(device.segments[0].registers[4], 0x00);
    ASSERT_EQ(device.segments[0].registers[5], 0x03);
    ASSERT_EQ(device.segments[0].registers[6], 0x00);
    ASSERT_EQ(device.segments[0].registers[7], 0x04);

    ASSERT_EQ(device.segments[1].registers_start, 5);
    ASSERT_EQ(device.segments[1].registers_count, 2);
    ASSERT_EQ(device.segments[1].registers.size(), 4);
    ASSERT_EQ(device.segments[1].registers[0], 0x00);
    ASSERT_EQ(device.segments[1].registers[1], 0x05);
    ASSERT_EQ(device.segments[1].registers[2], 0x00);
    ASSERT_EQ(device.segments[1].registers[3], 0x06);
}

TEST(UnsolicitatedReportRequest_Device, defragment_doesnt_defragment_non_defragmentable) {
    UnsolicitatedReportRequest::Device device;

    device.segments.push_back({0, 2, {0x00, 0x01, 0x00, 0x02}});
    device.segments.push_back({3, 2, {0x00, 0x03, 0x00, 0x04}});

    device.defragment();

    ASSERT_EQ(device.segments.size(), 2);
}

TEST(UnsolicitatedReportRequest, positive_test) {
    UnsolicitatedReportRequest req;
    UnsolicitatedReportRequest::Device device1234;

    device1234.location = 0x1234;
    device1234.segments.push_back({0, 2, {0xba, 0xad, 0xca, 0xfe}});
    device1234.segments.push_back({2, 2, {0xbe, 0xef, 0xfe, 0xed}});
    device1234.segments.push_back({5, 1, {0xde, 0xad}});

    req.devices.push_back(device1234);
    req.response_required = true;

    auto generic = req.to_generic();

    ASSERT_EQ(generic.function_code, 0x41);
    ASSERT_EQ(generic.data[0], 0x91);
    // length
    ASSERT_EQ(generic.data[1], 0x00);
    ASSERT_EQ(generic.data[2], 29);
    // response required
    ASSERT_EQ(generic.data[3], 0x80);
    // number of devices
    ASSERT_EQ(generic.data[4], 0x00);
    ASSERT_EQ(generic.data[5], 0x01);
    // device 1
    ASSERT_EQ(generic.data[6], 0x12);
    ASSERT_EQ(generic.data[7], 0x34);
    // segment count
    ASSERT_EQ(generic.data[8], 0x00);
    ASSERT_EQ(generic.data[9], 0x03);
    // segment 1
    ASSERT_EQ(generic.data[10], 0x00);
    ASSERT_EQ(generic.data[11], 0x00);
    ASSERT_EQ(generic.data[12], 0x00);
    ASSERT_EQ(generic.data[13], 0x02);
    ASSERT_EQ(generic.data[14], 0xba);
    ASSERT_EQ(generic.data[15], 0xad);
    ASSERT_EQ(generic.data[16], 0xca);
    ASSERT_EQ(generic.data[17], 0xfe);
    // segment 2
    ASSERT_EQ(generic.data[18], 0x00);
    ASSERT_EQ(generic.data[19], 0x02);
    ASSERT_EQ(generic.data[20], 0x00);
    ASSERT_EQ(generic.data[21], 0x02);
    ASSERT_EQ(generic.data[22], 0xbe);
    ASSERT_EQ(generic.data[23], 0xef);
    ASSERT_EQ(generic.data[24], 0xfe);
    ASSERT_EQ(generic.data[25], 0xed);
    // segment 3
    ASSERT_EQ(generic.data[26], 0x00);
    ASSERT_EQ(generic.data[27], 0x05);
    ASSERT_EQ(generic.data[28], 0x00);
    ASSERT_EQ(generic.data[29], 0x01);
    ASSERT_EQ(generic.data[30], 0xde);
    ASSERT_EQ(generic.data[31], 0xad);
}

TEST(UnsolicitatedReportRequest, positive_test_defragmented) {
    UnsolicitatedReportRequest req;
    UnsolicitatedReportRequest::Device device1234;

    device1234.location = 0x1234;
    device1234.segments.push_back({0, 2, {0xba, 0xad, 0xca, 0xfe}});
    device1234.segments.push_back({2, 2, {0xbe, 0xef, 0xfe, 0xed}});
    device1234.segments.push_back({5, 1, {0xde, 0xad}});

    req.devices.push_back(device1234);
    req.response_required = true;
    req.defragment();

    auto generic = req.to_generic();

    ASSERT_EQ(generic.function_code, 0x41);
    ASSERT_EQ(generic.data[0], 0x91);
    // length
    ASSERT_EQ(generic.data[1], 0x00);
    ASSERT_EQ(generic.data[2], 25);
    // response required
    ASSERT_EQ(generic.data[3], 0x80);
    // number of devices
    ASSERT_EQ(generic.data[4], 0x00);
    ASSERT_EQ(generic.data[5], 0x01);
    // device 1
    ASSERT_EQ(generic.data[6], 0x12);
    ASSERT_EQ(generic.data[7], 0x34);
    // segment count
    ASSERT_EQ(generic.data[8], 0x00);
    ASSERT_EQ(generic.data[9], 0x02);
    // segment 1+2
    ASSERT_EQ(generic.data[10], 0x00);
    ASSERT_EQ(generic.data[11], 0x00);
    ASSERT_EQ(generic.data[12], 0x00);
    ASSERT_EQ(generic.data[13], 0x04);
    ASSERT_EQ(generic.data[14], 0xba);
    ASSERT_EQ(generic.data[15], 0xad);
    ASSERT_EQ(generic.data[16], 0xca);
    ASSERT_EQ(generic.data[17], 0xfe);
    ASSERT_EQ(generic.data[18], 0xbe);
    ASSERT_EQ(generic.data[19], 0xef);
    ASSERT_EQ(generic.data[20], 0xfe);
    ASSERT_EQ(generic.data[21], 0xed);
    // segment 3
    ASSERT_EQ(generic.data[22], 0x00);
    ASSERT_EQ(generic.data[23], 0x05);
    ASSERT_EQ(generic.data[24], 0x00);
    ASSERT_EQ(generic.data[25], 0x01);
    ASSERT_EQ(generic.data[26], 0xde);
    ASSERT_EQ(generic.data[27], 0xad);
}
