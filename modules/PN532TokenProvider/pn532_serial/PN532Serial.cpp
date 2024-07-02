// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "PN532Serial.h"
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

#include <everest/logging.hpp>

constexpr size_t FIRMWARE_VERSION_IC_POS = 0;
constexpr size_t FIRMWARE_VERSION_VER_POS = 1;
constexpr size_t FIRMWARE_VERSION_REV_POS = 2;
constexpr size_t FIRMWARE_VERSION_SUPPORT_POS = 3;

constexpr size_t IN_LIST_PASSIVE_TARGET_NBTG = 0;
constexpr size_t IN_LIST_PASSIVE_TARGET_TG = 1;
constexpr size_t IN_LIST_PASSIVE_TARGET_SENS_RES_MSB = 2;
constexpr size_t IN_LIST_PASSIVE_TARGET_SENS_RES_LSB = 3;
constexpr size_t IN_LIST_PASSIVE_TARGET_SEL_RES = 4;
constexpr size_t IN_LIST_PASSIVE_TARGET_NFCID_LENGTH = 5;

// command codes
constexpr uint8_t COMMAND_GET_FIRMWARE_VERSION = 0x02;
constexpr uint8_t COMMAND_GET_FIRMWARE_VERSION_RESPONSE = 0x03;
constexpr uint8_t COMMAND_SAM_CONFIGURATION = 0x14;
constexpr uint8_t COMMAND_SAM_CONFIGURATION_RESPONSE = 0x15;
constexpr uint8_t COMMAND_IN_LIST_PASSIVE_TARGET = 0x4a;
constexpr uint8_t COMMAND_IN_LIST_PASSIVE_TARGET_RESPONSE = 0x4b;

PN532Serial::PN532Serial() {
}

PN532Serial::~PN532Serial() {
}

void PN532Serial::run() {
    readThreadHandle = std::thread(&PN532Serial::readThread, this);
}

void PN532Serial::resetDataRead() {
    start_of_packet = 0;
    packet_length = 0;
    preamble_start_seen = false;
    preamble_seen = false;
    first_data = true;
    data_length_checksum_valid = false;
    tfi = 0;
    command_code = 0;
    data.clear();
}

void PN532Serial::readThread() {
    uint8_t buf[2048];

    resetDataRead();

    while (true) {
        if (readThreadHandle.shouldExit()) {
            break;
        }
        auto n = read(fd, buf, sizeof buf);
        if (n == 0) {
            continue;
        }

        if (this->debug) {
            std::stringstream data_stream;
            for (ssize_t i = 0; i < n; i++) {
                data_stream << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)buf[i]
                            << " ";
            }

            EVLOG_info << "Received bytes: " << data_stream.str();
        }
        for (ssize_t i = 0; i < n; i++) {
            if (!preamble_seen) {
                if (preamble_start_seen && buf[i] == 0xff) {
                    preamble_seen = true;
                    start_of_packet = i + 1;
                    if (start_of_packet + 1 >= n) {
                        if (this->debug) {
                            EVLOG_warning << "Packet is not long enough to be parsed";
                        }
                        continue;
                    } else {
                        packet_length = buf[start_of_packet];
                        auto packet_length_checksum = buf[start_of_packet + 1];
                        auto checksum = (packet_length + packet_length_checksum) & 0x00ff;
                        if (checksum == 0) {
                            // data length checkum is valid
                            data_length_checksum_valid = true;
                        } else {
                            // can be a valid ACK frame
                        }
                    }
                    break;
                } else {
                    preamble_start_seen = false;
                }

                if (!preamble_start_seen && buf[i] == 0x00) {
                    preamble_start_seen = true;
                }
            } else {
                data.push_back(buf[i]);
            }
        }

        if (packet_length == 0) {
            if (start_of_packet + 1 < n) {
                if (buf[start_of_packet + 1] == 0xff) {
                    // ACK FRAME
                    // TODO keep track of acks
                }
            } else {
                if (this->debug) {
                    EVLOG_warning << "Packet of length 0 received that is not a ACK Frame.";
                }
                resetDataRead();
            }
        } else {
            if (!data_length_checksum_valid) {
                if (this->debug) {
                    EVLOG_warning << "Data length checksum invalid";
                }
                resetDataRead();
                continue;
            }

            // normal packet
            if (first_data) {
                tfi = buf[start_of_packet + 2];
                command_code = buf[start_of_packet + 3];
                first_data = false;
                for (ssize_t i = start_of_packet + 4; i < n; i++) {
                    data.push_back(buf[i]);
                }
            }

            if (data.size() < packet_length) {
                // not enough bytes received for a complete message yet
                continue;
            } else {
                parseData();
            }
        }
        resetDataRead();
    }
}

void PN532Serial::parseData() {
    if (data.back() == 0x00 && data.size() >= 2) {
        // last byte is 0x00 (postamble) removing it
        data.pop_back();
        auto packet_data_checksum = data.back();
        data.pop_back();

        uint8_t sum = tfi + command_code + packet_data_checksum;
        for (auto element : data) {
            sum += element;
        }
        if (this->debug) {
            EVLOG_info << "----- Parsing data -----";
            std::stringstream data_stream;
            EVLOG_info << std::right << std::hex << "Packet data checksum: 0x" << std::setfill('0') << std::setw(2)
                       << (int)packet_data_checksum << " checksum result: 0x" << std::setfill('0') << std::setw(2)
                       << (int)sum;
            for (auto& element : data) {
                data_stream << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)element
                            << " ";
            }
            data_stream << "(length: " << std::dec << data.size() << ")";

            EVLOG_info << "Raw data: " << data_stream.str();
        }

        if (sum == 0x00) {
            if (this->debug) {
                EVLOG_info << "Packet data checksum valid, command code: 0x" << std::setfill('0') << std::setw(2)
                           << std::right << std::hex << (int)command_code;
            }

            if (command_code == COMMAND_GET_FIRMWARE_VERSION_RESPONSE && this->get_firmware_version_promise) {
                PN532Response response;
                if (data.size() == 4) {
                    response.valid = true;
                    response.message = std::make_shared<FirmwareVersion>(
                        data.at(FIRMWARE_VERSION_IC_POS), data.at(FIRMWARE_VERSION_VER_POS),
                        data.at(FIRMWARE_VERSION_REV_POS), data.at(FIRMWARE_VERSION_SUPPORT_POS));
                }
                this->get_firmware_version_promise->set_value(response);
            } else if (command_code == COMMAND_SAM_CONFIGURATION_RESPONSE && this->configure_sam_promise) {
                this->configure_sam_promise->set_value(true);
            } else if (command_code == COMMAND_IN_LIST_PASSIVE_TARGET_RESPONSE &&
                       this->in_list_passive_target_promise) {
                parseInListPassiveTargetResponse();
            }
        }

    } else {
        if (this->debug) {
            EVLOG_warning << "Last byte is NOT 0x00, something went wrong...";
            std::stringstream data_stream;
            for (auto& element : data) {
                data_stream << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)element
                            << " ";
            }
            data_stream << "(length: " << std::dec << data.size() << ")";

            EVLOG_info << "Raw data: " << data_stream.str();
        }
    }
}

void PN532Serial::parseInListPassiveTargetResponse() {
    PN532Response response;
    auto in_list_passive_target_response = std::make_shared<InListPassiveTargetResponse>();
    TargetData target_data;
    if (data.size() >= 6) {
        in_list_passive_target_response->nbtg = data.at(IN_LIST_PASSIVE_TARGET_NBTG);
        if (in_list_passive_target_response->nbtg == 1) {
            target_data.tg = data.at(IN_LIST_PASSIVE_TARGET_TG);
            target_data.sens_res_msb = data.at(IN_LIST_PASSIVE_TARGET_SENS_RES_MSB);
            target_data.sens_res_lsb = data.at(IN_LIST_PASSIVE_TARGET_SENS_RES_LSB);
            target_data.sens_res = (target_data.sens_res_msb << 8) + target_data.sens_res_lsb;
            target_data.sel_res = data.at(IN_LIST_PASSIVE_TARGET_SEL_RES);
            target_data.nfcid_length = data.at(IN_LIST_PASSIVE_TARGET_NFCID_LENGTH);

            if (this->debug) {
                EVLOG_info << "Target data: " << std::right << std::hex << "\ntg: 0x" << std::setfill('0')
                           << std::setw(2) << (int)target_data.tg << "\nsens_res_msb: 0x" << std::setfill('0')
                           << std::setw(2) << (int)target_data.sens_res_msb << "\nsens_res_lsb: 0x" << std::setfill('0')
                           << std::setw(2) << (int)target_data.sens_res_lsb << "\nsens_res: 0x" << std::setfill('0')
                           << std::setw(4) << (int)target_data.sens_res << "\nsel_res: 0x" << std::setfill('0')
                           << std::setw(2) << (int)target_data.sel_res << "\nnfcid_length: 0x" << std::setfill('0')
                           << std::setw(2) << (int)target_data.nfcid_length;
            }

            if (data.size() >= 6 + target_data.nfcid_length) {
                response.valid = true;
                for (ssize_t i = 6; i < 6 + target_data.nfcid_length; i++) {
                    target_data.nfcid.push_back(data.at(i));
                }

                if (this->debug) {
                    std::stringstream nfcid_stream;

                    for (auto& element : target_data.nfcid) {
                        nfcid_stream << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex
                                     << (int)element << " ";
                    }
                    nfcid_stream << "(length: " << std::dec << target_data.nfcid.size() << ")";

                    EVLOG_info << "NFCID: " << nfcid_stream.str();
                }

                if (data.size() >= 6 + target_data.nfcid_length + 1) {
                    auto ats_length = data.at(6 + target_data.nfcid_length);

                    if (data.size() >= 6 + target_data.nfcid_length + ats_length) {
                        for (size_t i = 6 + target_data.nfcid_length; i < 6 + target_data.nfcid_length + ats_length;
                             i++) {
                            target_data.ats.push_back(data.at(i));
                        }
                    }

                    if (this->debug) {
                        std::stringstream ats_stream;

                        for (auto& element : target_data.ats) {
                            ats_stream << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex
                                       << (int)element << " ";
                        }
                        ats_stream << "(length: " << std::dec << target_data.ats.size() << ")";

                        EVLOG_info << "ATS: " << ats_stream.str();
                    }
                }
            }

            in_list_passive_target_response->target_data.push_back(target_data);
        }
    }

    response.message = in_list_passive_target_response;
    this->in_list_passive_target_promise->set_value(response);
}

std::future<bool> PN532Serial::configureSAM() {
    this->configure_sam_promise = std::make_unique<std::promise<bool>>();
    this->serialWriteCommand({
        COMMAND_SAM_CONFIGURATION, // command codee
        0x01,                      // normal mode
        0x00,                      // no timeout
        0x01                       // P70_IRQ pin driven by PN532
    });
    return this->configure_sam_promise->get_future();
}

std::future<PN532Response> PN532Serial::getFirmwareVersion() {
    this->get_firmware_version_promise = std::make_unique<std::promise<PN532Response>>();
    this->serialWriteCommand({COMMAND_GET_FIRMWARE_VERSION});
    return this->get_firmware_version_promise->get_future();
}

std::future<PN532Response> PN532Serial::inListPassiveTarget() {
    this->in_list_passive_target_promise = std::make_unique<std::promise<PN532Response>>();
    this->serialWriteCommand({
        COMMAND_IN_LIST_PASSIVE_TARGET, // command code
        0x01,                           // one target
        0x00                            // 105 kbps type A (ISO/IEC1443 Type A)
    });
    return this->in_list_passive_target_promise->get_future();
}

bool PN532Serial::serialWrite(std::vector<uint8_t> data) {
    write(fd, data.data(), data.size());

    return true;
}

bool PN532Serial::serialWriteCommand(std::vector<uint8_t> data) {
    std::vector<uint8_t> data_copy = data;
    data_copy.push_back(host_to_pn532);
    uint8_t sum = 0;
    for (auto element : data_copy) {
        sum += element;
    }
    uint8_t data_length_checksum = (~data_copy.size() & 0xFF) + 0x01;
    uint8_t inverse = (~sum & 0xFF) + 0x01;

    if (inverse > 255) {
        inverse = inverse - 255;
    }

    std::vector<uint8_t> serial_data = preamble;
    serial_data.push_back(data_copy.size());
    serial_data.push_back(data_length_checksum);
    serial_data.push_back(host_to_pn532);

    serial_data.insert(serial_data.end(), data.begin(), data.end());
    serial_data.push_back(inverse);
    serial_data.push_back(postamble);

    return this->serialWrite(serial_data);
}

bool PN532Serial::reset() {
    bool success = true;
    this->serialWrite({0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

    return success;
}

void PN532Serial::enableDebug() {
    this->debug = true;
}
