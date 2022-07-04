// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef YETI_SERIAL
#define YETI_SERIAL

#include "evThread.h"
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <termios.h>
#include <vector>

enum class PN532Command {
    SAMConfiguration,
    GetFirmwareVersion
};

struct PN532Message {
    virtual ~PN532Message() = default;
};

struct PN532Response {
    bool valid = false;
    std::shared_ptr<PN532Message> message;
};

struct FirmwareVersion : public PN532Message {
    u_int8_t ic;
    u_int8_t ver; // TODO: also pre-parsed version string of this
    u_int8_t rev;
    u_int8_t support; // TODO: bool flags
};

struct TargetData {
    u_int8_t tg;
    u_int8_t sens_res_msb;
    u_int8_t sens_res_lsb;
    u_int16_t sens_res;
    u_int8_t sel_res;
    u_int8_t nfcid_length;
    std::vector<u_int8_t> nfcid;
    std::vector<u_int8_t> ats;

    std::string getNFCID() {
        std::stringstream ss;
        for (auto it = nfcid.begin(); it != nfcid.end(); it++) {
            int id = *it;
            ss << std::setfill('0') << std::setw(2) << std::hex << id;
        }
        return ss.str();
    };
};

struct InListPassiveTargetResponse : public PN532Message {
    u_int8_t nbtg = 0;
    std::vector<TargetData> target_data;
};

class evSerial {

public:
    evSerial();
    ~evSerial();

    bool openDevice(const char* device, int baud);

    void readThread();
    void run();
    bool reset();
    bool serialWrite(std::vector<uint8_t> data);
    bool serialWriteCommand(std::vector<uint8_t> data);
    std::future<bool> configureSAM();
    std::future<PN532Response> getFirmwareVersion();
    std::future<PN532Response> inListPassiveTarget();

private:
    std::vector<u_int8_t> preamble = {0x00, 0x00, 0xff};
    u_int8_t postamble = 0x00;
    uint8_t host_to_pn532 = 0xd4;
    std::unique_ptr<std::promise<bool>> configure_sam_promise;
    std::unique_ptr<std::promise<PN532Response>> get_firmware_version_promise;
    std::unique_ptr<std::promise<PN532Response>> in_list_passive_target_promise;
    size_t start_of_packet = 0;
    size_t packet_length = 0;
    size_t data_received = 0;
    bool preamble_start_seen = false;
    bool preamble_seen = false;
    bool first_data = true;
    bool data_length_checksum_valid = false;
    u_int8_t tfi = 0;
    u_int8_t command_code = 0;
    std::vector<u_int8_t> data;

    void resetDataRead();

    // Serial interface
    bool setSerialAttributes();
    int fd;
    int baud;

    // Read thread for serial port
    evThread readThreadHandle;
};

#endif
