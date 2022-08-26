// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "evSerial.h"
#include <fstream>
#include <iostream>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <unistd.h>

#include <thread>

evSerial::evSerial() {
    fd = 0;
    baud = 0;
}

evSerial::~evSerial() {
    if (fd)
        close(fd);
}

bool evSerial::openDevice(const char* device, int _baud) {

    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Serial: error %d opening %s: %s\n", errno, device, strerror(errno));
        return false;
    } // else printf ("Serial: opened %s as %i\n", device, fd);

    switch (_baud) {
    case 9600:
        baud = B9600;
        break;
    case 19200:
        baud = B19200;
        break;
    case 38400:
        baud = B38400;
        break;
    case 57600:
        baud = B57600;
        break;
    case 115200:
        baud = B115200;
        break;
    case 230400:
        baud = B230400;
        break;
    default:
        baud = 0;
        return false;
    }

    return setSerialAttributes();
}

bool evSerial::setSerialAttributes() {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        printf("Serial: error %d from tcgetattr\n", errno);
        return false;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
    tty.c_lflag = 0;     // no signaling chars, no echo,
                         // no canonical processing
    tty.c_oflag = 0;     // no remapping, no delays
    tty.c_cc[VMIN] = 0;  // read blocks
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    // printf ("Success setting tcsetattr\n");
    return true;
}

void evSerial::run() {
    readThreadHandle.handle = std::thread(&evSerial::readThread, this);
}

void evSerial::resetDataRead() {
    start_of_packet = 0;
    packet_length = 0;
    data_received = 0;
    preamble_start_seen = false;
    preamble_seen = false;
    first_data = true;
    data_length_checksum_valid = false;
    tfi = 0;
    command_code = 0;
    data.clear();
}

void evSerial::readThread() {
    uint8_t buf[2048];
    int n;

    resetDataRead();

    while (true) {
        if (readThreadHandle.shouldExit()) {
            break;
        }
        n = read(fd, buf, sizeof buf);
        if (n == 0) {
            continue;
        }

        for (size_t i = 0; i < n; i++) {
            if (!preamble_seen) {
                if (preamble_start_seen && buf[i] == 0xff) {
                    preamble_seen = true;
                    start_of_packet = i + 1;
                    if (start_of_packet + 1 >= n) {
                        printf("Packet is not long enough to be parsed\n");
                        continue;
                    } else {
                        packet_length = buf[start_of_packet];
                        auto packet_length_checksum = buf[start_of_packet + 1];
                        auto checksum = (packet_length + packet_length_checksum) & 0x00ff;
                        if (checksum == 0) {
                            // printf("data length checkum is valid, packet length: %02x\n", packet_length);
                            data_received = n - start_of_packet;
                            data_length_checksum_valid = true;
                        } else {
                            // can be a valid ACK frame
                            // printf("\nchecksum: %02x != 0\n", checksum);
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
                data_received += 1;
            }
        }

        if (packet_length == 0) {
            if (start_of_packet + 1 < n) {
                if (buf[start_of_packet + 1] == 0xff) {
                    // printf("ACK FRAME\n");
                    // TODO keep track of acks
                }
            } else {
                printf("Packet of length 0 received that is not a ACK Frame.");
                resetDataRead();
            }
        } else {
            if (!data_length_checksum_valid) {
                printf("Data length checksum invalid");
                resetDataRead();
                continue;
            }

            // normal packet
            if (first_data) {
                tfi = buf[start_of_packet + 2];
                command_code = buf[start_of_packet + 3];
                first_data = false;
                for (size_t i = start_of_packet + 4; i < n; i++) {
                    // printf("%02x ", buf[i]);
                    data.push_back(buf[i]);
                }
            }

            if (data_received < packet_length) {
                // not enough bytes received for a complete message yet
                continue;
            } else {
                if (data.back() == 0x00 && data.size() >= 2) {
                    // printf("last byte is 0x00 (postamble) removing...\n");
                    data.pop_back();
                    auto packet_data_checksum = data.back();
                    data.pop_back();

                    uint8_t sum = tfi + command_code + packet_data_checksum;
                    for (auto element : data) {
                        sum += element;
                    }
                    // printf("packet data checksum: %02x checksum result: %02x\n", packet_data_checksum, sum);

                    if (sum == 0x00) {
                        // printf("packet data checksum correct\n");
                        if (command_code == 0x03 && this->get_firmware_version_promise) {
                            PN532Response response;
                            if (data.size() == 4) {
                                response.valid = true;
                                auto firmware_version = std::make_shared<FirmwareVersion>();
                                firmware_version->ic = data.at(0);
                                firmware_version->ver = data.at(1);
                                firmware_version->rev = data.at(2);
                                firmware_version->support = data.at(3);
                                response.message = firmware_version;
                            }
                            this->get_firmware_version_promise->set_value(response);
                        } else if (command_code == 0x15 && this->configure_sam_promise) {
                            this->configure_sam_promise->set_value(true);
                        } else if (command_code == 0x4b && this->in_list_passive_target_promise) {
                            PN532Response response;
                            auto in_list_passive_target_response = std::make_shared<InListPassiveTargetResponse>();
                            TargetData target_data;
                            if (data.size() >= 6) {
                                auto nbtg = data.at(0);
                                in_list_passive_target_response->nbtg = nbtg;
                                if (nbtg == 1) {
                                    auto tg = data.at(1);
                                    auto sens_res_msb = data.at(2);
                                    auto sens_res_lsb = data.at(3);
                                    auto sel_res = data.at(4);
                                    auto nfcid_length = data.at(5);
                                    target_data.tg = tg;
                                    target_data.sens_res_msb = sens_res_msb;
                                    target_data.sens_res_lsb = sens_res_lsb;
                                    target_data.sens_res = (sens_res_msb << 8) + sens_res_lsb;
                                    target_data.sel_res = sel_res;
                                    target_data.nfcid_length = nfcid_length;

                                    if (data.size() >= 6 + nfcid_length + 1) {
                                        response.valid = true;
                                        for (size_t i = 6; i < 6 + nfcid_length; i++) {
                                            target_data.nfcid.push_back(data.at(i));
                                        }

                                        auto ats_length = data.at(6 + nfcid_length);

                                        if (data.size() >= 6 + nfcid_length + ats_length) {
                                            for (size_t i = 6 + nfcid_length; i < 6 + nfcid_length + ats_length; i++) {
                                                target_data.ats.push_back(data.at(i));
                                            }
                                        }
                                    }

                                    in_list_passive_target_response->target_data.push_back(target_data);
                                }
                            }

                            response.message = in_list_passive_target_response;
                            this->in_list_passive_target_promise->set_value(response);
                        }
                    }

                } else {
                    printf("last bit is NOT 0x00,something went wrong...\n");
                }
            }
        }
        resetDataRead();
    }
}

std::future<bool> evSerial::configureSAM() {
    this->configure_sam_promise = std::make_unique<std::promise<bool>>();
    this->serialWriteCommand({0x14, 0x01, 0x00, 0x01});
    return this->configure_sam_promise->get_future();
}

std::future<PN532Response> evSerial::getFirmwareVersion() {
    this->get_firmware_version_promise = std::make_unique<std::promise<PN532Response>>();
    this->serialWriteCommand({0x02});
    return this->get_firmware_version_promise->get_future();
}

std::future<PN532Response> evSerial::inListPassiveTarget() {
    this->in_list_passive_target_promise = std::make_unique<std::promise<PN532Response>>();
    this->serialWriteCommand({
        0x4a, // command code
        0x01, // one target
        0x00  // 105 kbps type A (ISO/IEC1443 Type A)
    });
    return this->in_list_passive_target_promise->get_future();
}

bool evSerial::serialWrite(std::vector<uint8_t> data) {
    write(fd, data.data(), data.size());

    return true;
}

bool evSerial::serialWriteCommand(std::vector<uint8_t> data) {
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

    std::vector<u_int8_t> serial_data = preamble;
    serial_data.push_back(data_copy.size());
    serial_data.push_back(data_length_checksum);
    serial_data.push_back(host_to_pn532);

    serial_data.insert(serial_data.end(), data.begin(), data.end());
    serial_data.push_back(inverse);
    serial_data.push_back(postamble);

    return this->serialWrite(serial_data);
}

bool evSerial::reset() {
    bool success = true;
    this->serialWrite({0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

    return success;
}
