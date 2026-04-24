// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "PN532Serial.h"
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string.h>
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

void PN532Serial::run() {
    readThreadHandle = std::thread(&PN532Serial::readThread, this);
}

void PN532Serial::readThread() {
    uint8_t buf[2048];
    size_t n = 0;
    ssize_t first_start_code_offset;

    while (true) {
        if (readThreadHandle.shouldExit()) {
            break;
        }
        auto c = read(fd, &buf[n], sizeof(buf) - n);
        if (c == -1) {
            EVLOG_error << "Read failed: " << strerror(errno) << " (" << errno << ")";
            break;
        }
        if (c == 0) {
            continue;
        }

        if (this->debug) {
            EVLOG_info << "Received: " << hexdump(buf + n, c);
        }

        n += c;

    restart_buffer_analysis:
        EVLOG_verbose << "buffer content (size: " << n << "): " << hexdump(buf, n);

        // packet minimum size is 6 byte for ACK or NACK, other frames are larger
        if (n < 6) {
            continue;
        }

        first_start_code_offset = -1;

        // we cannot assume that we receive the whole response in one read call
        // because of different driver/interfaces, so we have to scan for well-known
        // patterns or check whether it looks like a valid frame after a start code
        // pattern; if we find one, we can delete it from the buffer and move the
        // left-over buffer content to the front, then we must restart analyzing
        // to be sure to handle any following frame immediately and don't wait
        // until the next read call runs/returns
        for (ssize_t i = 0; i <= n - 6; ++i) {
            // check for ACK or NACKs
            if (memcmp(&buf[i], ACK_FRAME.data(), ACK_FRAME.size()) == 0) {
                EVLOG_debug << "Received ACK";
                n = n - i - ACK_FRAME.size();
                memmove(buf, buf + i + ACK_FRAME.size(), n);
                goto restart_buffer_analysis;
            }
            if (memcmp(&buf[i], NACK_FRAME.data(), NACK_FRAME.size()) == 0) {
                EVLOG_debug << "Received NACK";
                n = n - i - NACK_FRAME.size();
                memmove(buf, buf + i + NACK_FRAME.size(), n);
                goto restart_buffer_analysis;
            }

            // check for start_code
            if (memcmp(&buf[i], START_CODE.data(), START_CODE.size()) == 0) {
                if (first_start_code_offset == -1) {
                    first_start_code_offset = i;
                }

                auto len = buf[i + START_CODE.size()];
                auto lcs = buf[i + START_CODE.size() + 1];

                if (((len + lcs) & 0xff) != 0) {
                    EVLOG_verbose << "package length checksum mismatch";
                    continue;
                }

                // offset + start code size + len and lcs byte + data length + dcs and postamble
                if (i + START_CODE.size() + 2 + len + 2 > n) {
                    EVLOG_verbose << "frame still too short or length bogus";
                    continue;
                }

                // accesses beyond i are safe below since we ensure that above
                auto tfi = buf[i + START_CODE.size() + 2];
                if (tfi != PN532_TO_HOST) {
                    EVLOG_verbose << "frame is not a response";
                    continue;
                }

                if (buf[i + START_CODE.size() + 2 + len + 1] != 0x00) {
                    EVLOG_verbose << "postamble check failed";
                    continue;
                }

                auto dcs_compl = 0;
                for (size_t j = 0; j < len; ++j) {
                    dcs_compl += buf[i + START_CODE.size() + 2 + j];
                }
                auto dcs = buf[i + START_CODE.size() + 2 + len];
                if (((dcs_compl + dcs) & 0xff) != 0) {
                    EVLOG_verbose << "package data checksum does not match";
                    continue;
                }

                // all checks so far indicate that frame/packet looks good
                command_code = buf[i + START_CODE.size() + 2 + 1];
                data = std::vector<uint8_t>(&buf[i + START_CODE.size() + 2 + 1 + 1],
                                            &buf[i + START_CODE.size() + 2 + len]);
                parseData();

                // discard processed data, the processed size is:
                // offset (incl. preamble) + start code size + len and lcs byte + data length + dcs and postamble
                auto processed_size = i + START_CODE.size() + 1 + 1 + data.size() + 1 + 1;
                n = n - processed_size;
                memmove(buf, buf + processed_size, n);
                goto restart_buffer_analysis;
            }
        }

        if (first_start_code_offset == -1) {
            // no start code found so far: continue with outer loop and receive more data
        } else {
            // use first start code offset found and assume garbage before -> discard this
            n -= first_start_code_offset;
            memmove(buf, buf + first_start_code_offset, n);
        }
    }
}

void PN532Serial::parseData() {
    if (command_code == COMMAND_GET_FIRMWARE_VERSION_RESPONSE && this->get_firmware_version_promise) {
        PN532Response response;
        if (data.size() == 4) {
            EVLOG_debug << "Sending ACK";
            serialWrite(ACK_FRAME);

            response.valid = true;
            response.message = std::make_shared<FirmwareVersion>(
                data.at(FIRMWARE_VERSION_IC_POS), data.at(FIRMWARE_VERSION_VER_POS), data.at(FIRMWARE_VERSION_REV_POS),
                data.at(FIRMWARE_VERSION_SUPPORT_POS));

            this->get_firmware_version_promise->set_value(response);
        } else {
            EVLOG_debug << "Sending NACK";
            serialWrite(NACK_FRAME);
        }
    } else if (command_code == COMMAND_SAM_CONFIGURATION_RESPONSE && this->configure_sam_promise) {
        EVLOG_debug << "Sending ACK";
        serialWrite(ACK_FRAME);
        this->configure_sam_promise->set_value(true);
    } else if (command_code == COMMAND_IN_LIST_PASSIVE_TARGET_RESPONSE && this->in_list_passive_target_promise) {
        // always send ACK for now
        EVLOG_debug << "Sending ACK";
        serialWrite(ACK_FRAME);
        parseInListPassiveTargetResponse();
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
                    nfcid_stream << hexdump(target_data.nfcid);
                    nfcid_stream << " (length: " << std::dec << target_data.nfcid.size() << ")";
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
                        ats_stream << hexdump(target_data.ats);
                        ats_stream << " (length: " << std::dec << target_data.ats.size() << ")";
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
        COMMAND_SAM_CONFIGURATION, // command code
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

bool PN532Serial::serialWrite(const std::vector<uint8_t>& data) {
    auto rv = write(fd, data.data(), data.size());

    if (rv != data.size()) {
        EVLOG_error << "Write failed: " << strerror(errno) << " (" << errno << ")";
        return false;
    }

    return true;
}

bool PN532Serial::serialWriteCommand(const std::vector<uint8_t>& data) {
    // calculate packet length checksum
    uint8_t lcs = data.size() + 1;
    lcs = (uint8_t)-lcs;

    // calculate data check sum
    uint8_t dcs = HOST_TO_PN532;
    for (auto element : data) {
        dcs += element;
    }
    dcs = (uint8_t)-dcs;

    std::vector<uint8_t> serial_data;
    // size: preamble + start code + LEN + LCS + TFI + data + DCS + postamble
    serial_data.reserve(1 + START_CODE.size() + 1 + 1 + 1 + data.size() + 1 + 1);
    serial_data.push_back(PREAMBLE);
    serial_data.insert(serial_data.end(), START_CODE.begin(), START_CODE.end());
    serial_data.push_back(data.size() + 1);
    serial_data.push_back(lcs);
    serial_data.push_back(HOST_TO_PN532);
    serial_data.insert(serial_data.end(), data.begin(), data.end());
    serial_data.push_back(dcs);
    serial_data.push_back(POSTAMBLE);

    if (this->debug) {
        EVLOG_info << "Sending bytes: " << hexdump(serial_data);
    }

    return this->serialWrite(serial_data);
}

bool PN532Serial::reset() {
    return this->serialWrite(
        {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
}

void PN532Serial::enableDebug() {
    this->debug = true;
}
