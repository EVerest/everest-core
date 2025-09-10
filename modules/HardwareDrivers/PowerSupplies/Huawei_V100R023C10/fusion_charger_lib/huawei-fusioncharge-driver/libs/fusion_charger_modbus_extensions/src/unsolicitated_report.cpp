// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/extensions/unsolicitated_report.hpp>

#include "modbus-server/frames.hpp"

using namespace fusion_charger::modbus_extensions;
using namespace modbus_server::pdu;

std::vector<uint8_t> UnsolicitatedReportRequest::Segment::to_vec() const {
    if (registers.size() != registers_count * 2) {
        throw EncodingError("UnsolicitatedReportRequest::Segment",
                            "'registers' size must be equal to 'registers_count * 2'");
    }

    std::vector<uint8_t> ret;
    ret.push_back(registers_start >> 8);
    ret.push_back(registers_start & 0xFF);
    ret.push_back(registers_count >> 8);
    ret.push_back(registers_count & 0xFF);

    ret.insert(ret.end(), registers.begin(), registers.end());

    return ret;
}

std::vector<uint8_t> UnsolicitatedReportRequest::Device::to_vec() const {
    std::vector<uint8_t> ret;
    ret.push_back(location >> 8);
    ret.push_back(location & 0xFF);

    uint16_t number_of_segments = segments.size();
    ret.push_back(number_of_segments >> 8);
    ret.push_back(number_of_segments & 0xFF);

    for (const auto& segment : segments) {
        auto segment_vec = segment.to_vec();
        ret.insert(ret.end(), segment_vec.begin(), segment_vec.end());
    }

    return ret;
}

void UnsolicitatedReportRequest::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x41) {
        throw DecodingError(generic, "UnsolicitatedReportRequest", "Invalid function code");
    }

    if (generic.data.size() < 6) {
        throw DecodingError(generic, "UnsolicitatedReportRequest", "Invalid data size (expected at least 6 bytes)");
    }

    uint8_t subfunction_code = generic.data[0];
    uint16_t data_length = generic.data[1] << 8 | generic.data[2];
    uint8_t reporting_type = generic.data[3];
    uint16_t number_of_devices = generic.data[4] << 8 | generic.data[5];

    if (subfunction_code != 0x91) {
        throw DecodingError(generic, "UnsolicitatedReportRequest", "Invalid subfunction code");
    }

    if (data_length != generic.data.size() - 3) {
        throw DecodingError(generic, "UnsolicitatedReportRequest", "Invalid data length in header");
    }

    this->response_required = (reporting_type & 0x80) != 0;

    auto rest_data = std::vector<uint8_t>(generic.data.begin() + 6, generic.data.end());
    auto devices = std::vector<Device>();
    for (size_t i = 0; i < number_of_devices; i++) {
        auto device = Device();

        if (rest_data.size() < 4) {
            throw DecodingError(generic, "UnsolicitatedReportRequest",
                                "Invalid data size (expected at least 4 bytes for device)");
        }

        device.location = (rest_data[0] << 8) | rest_data[1];
        uint16_t number_of_segments = (rest_data[2] << 8) | rest_data[3];

        rest_data.erase(rest_data.begin(), rest_data.begin() + 4);

        for (int j = 0; j < number_of_segments; j++) {
            auto segment = Segment();

            if (rest_data.size() < 6) {
                throw DecodingError(generic, "UnsolicitatedReportRequest",
                                    "Invalid data size (expected at least 6 bytes for segment)");
            }

            segment.registers_start = (rest_data[0] << 8) | rest_data[1];
            segment.registers_count = (rest_data[2] << 8) | rest_data[3];

            rest_data.erase(rest_data.begin(), rest_data.begin() + 4);

            if (segment.registers_count * 2 > rest_data.size()) {
                throw DecodingError(generic, "UnsolicitatedReportRequest",
                                    "Invalid data size (expected at least " +
                                        std::to_string(segment.registers_count * 2 + 4) + " bytes for segment)");
            }

            segment.registers =
                std::vector<uint8_t>(rest_data.begin(), rest_data.begin() + segment.registers_count * 2);

            rest_data.erase(rest_data.begin(), rest_data.begin() + segment.registers_count * 2);

            device.segments.push_back(segment);
        }

        devices.push_back(device);
    }

    this->devices = devices;
}

GenericPDU UnsolicitatedReportRequest::to_generic() const {
    // this is the "Device 1" ... "Device N" part
    std::vector<uint8_t> devices_vec;
    for (const auto& device : devices) {
        auto device_vec = device.to_vec();
        devices_vec.insert(devices_vec.end(), device_vec.begin(), device_vec.end());
    }

    // "Data length" in pdf
    uint16_t size_in_header = devices_vec.size() + 1 + 2; // +1 for reporting type and +2 for number of devices

    uint16_t number_of_devices = devices.size();

    GenericPDU generic;
    generic.function_code = 0x41;
    generic.data.push_back(0x91); // subfunction code
    generic.data.push_back(size_in_header >> 8);
    generic.data.push_back(size_in_header & 0xFF);
    generic.data.push_back(response_required ? 0x80 : 0x00); // todo: bit 7, is this correct? or should it be 0x01?
    generic.data.push_back(number_of_devices >> 8);
    generic.data.push_back(number_of_devices & 0xFF);
    generic.data.insert(generic.data.end(), devices_vec.begin(), devices_vec.end());

    return generic;
}

void UnsolicitatedReportResponse::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x41) {
        throw DecodingError(generic, "UnsolicitatedReportResponse", "Invalid function code");
    }

    // todo: in the pdf there are only 5 bytes specified but the length is given
    // in 16 bit???

    if (generic.data.size() != 5) {
        throw DecodingError(generic, "UnsolicitatedReportResponse", "Invalid data size (expected 5 bytes)");
    }

    if (generic.data[0] != 0x91) {
        throw DecodingError(generic, "UnsolicitatedReportResponse", "Invalid subfunction code");
    }

    // todo: check if this is correct; we have no real world example of this...
    // uint16_t size_in_header = (generic.data[1] << 8) | generic.data[2];
    // if (size_in_header != 2) {
    //   throw DecodingError(generic, "UnsolicitatedReportResponse",
    //                       "Invalid data size in header (expected 1)");
    // }

    // todo: check if this is correct; we have no real world example of this...
    success = generic.data[3] == 0x00 && generic.data[4] == 0x00;
}

GenericPDU UnsolicitatedReportResponse ::to_generic() const {
    GenericPDU generic;
    generic.function_code = 0x41;
    generic.data.push_back(0x91);
    // todo: check if longer frames are possible
    generic.data.push_back(0x00);
    generic.data.push_back(0x01);
    generic.data.push_back(0x00);
    generic.data.push_back(success ? 0x00 : 0x01);
    return generic;
}

void UnsolicitatedReportRequest::defragment() {
    for (auto& device : this->devices) {
        device.defragment();
    }
}

void UnsolicitatedReportRequest::Device::defragment() {
    bool defragmented = false;

    while (!defragmented) {
        bool round_defragmented = false;
        for (size_t i = 0; i < segments.size(); i++) {
            // check if another segment is directly after this one
            for (size_t j = 0; j < segments.size(); j++) {
                if (i == j)
                    continue;

                if (segments[i].registers_start + segments[i].registers_count == segments[j].registers_start) {
                    // merge segments
                    segments[i].registers_count += segments[j].registers_count;
                    segments[i].registers.insert(segments[i].registers.end(), segments[j].registers.begin(),
                                                 segments[j].registers.end());
                    segments.erase(segments.begin() + j);
                    round_defragmented = true;
                    break;
                }
            }

            if (round_defragmented)
                break;
        }

        // if we didn't have to defragment this round, we are done
        if (!round_defragmented) {
            defragmented = true;
        }
    }
}
