// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef PN532_SERIAL
#define PN532_SERIAL

#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <termios.h>
#include <utils/serial.hpp>
#include <utils/thread.hpp>
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
    uint8_t ic;
    uint8_t ver; // TODO: also pre-parsed version string of this
    uint8_t rev;
    uint8_t support; // TODO: bool flags
    FirmwareVersion(uint8_t ic, uint8_t ver, uint8_t rev, uint8_t support) :
        ic(ic), ver(ver), rev(rev), support(support) {
    }
};

struct TargetData {
    uint8_t tg;
    uint8_t sens_res_msb;
    uint8_t sens_res_lsb;
    uint16_t sens_res;
    uint8_t sel_res;
    uint8_t nfcid_length;
    std::vector<uint8_t> nfcid;
    std::vector<uint8_t> ats;

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
    uint8_t nbtg = 0;
    std::vector<TargetData> target_data;
};

class PN532Serial : public Everest::Serial {

public:
    PN532Serial() = default;
    virtual ~PN532Serial() = default;

    void readThread();
    void run() override;
    bool reset();
    bool serialWrite(const std::vector<uint8_t>& data);
    bool serialWriteCommand(const std::vector<uint8_t>& data);
    std::future<bool> configureSAM();
    std::future<PN532Response> getFirmwareVersion();
    std::future<PN532Response> inListPassiveTarget();
    void enableDebug();

private:
    static constexpr uint8_t PREAMBLE = 0x00;
    static constexpr uint8_t POSTAMBLE = 0x00;
    static constexpr uint8_t HOST_TO_PN532 = 0xd4;
    static constexpr uint8_t PN532_TO_HOST = 0xd5;
    const std::vector<uint8_t> START_CODE{0x00, 0xff};
    const std::vector<uint8_t> ACK_FRAME{0x00, 0x00, 0xff, 0x00, 0xff, 0x00};
    const std::vector<uint8_t> NACK_FRAME{0x00, 0x00, 0xff, 0xff, 0x00, 0x00};
    std::unique_ptr<std::promise<bool>> configure_sam_promise;
    std::unique_ptr<std::promise<PN532Response>> get_firmware_version_promise;
    std::unique_ptr<std::promise<PN532Response>> in_list_passive_target_promise;
    uint8_t command_code = 0;
    std::vector<uint8_t> data;
    bool debug = false;

    void parseData();
    void parseInListPassiveTargetResponse();

    // Read thread for serial port
    Everest::Thread readThreadHandle;
};

#endif
