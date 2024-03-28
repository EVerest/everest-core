// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef YETI_SERIAL
#define YETI_SERIAL

#include "yeti.pb.h"
#include <date/date.h>
#include <date/tz.h>
#include <sigslot/signal.hpp>
#include <stdint.h>
#include <termios.h>
#include <utils/thread.hpp>

class evSerial {

public:
    evSerial();
    ~evSerial();

    bool openDevice(const char* device, int baud);

    void readThread();
    void run();

    bool reset(const std::string& reset_chip, const int reset_line);
    void firmwareUpdate(bool rom);
    void keepAlive();

    void setPWM(uint32_t dc);
    void allowPowerOn(bool p);
    void forceUnlock();

    sigslot::signal<KeepAliveLo> signalKeepAliveLo;
    sigslot::signal<PowerMeter> signalPowerMeter;
    sigslot::signal<CpState> signalCPState;
    sigslot::signal<PpState> signalPPState;
    sigslot::signal<ErrorFlags> signalErrorFlags;
    sigslot::signal<bool> signalRelaisState;
    sigslot::signal<bool> signalLockState;

    sigslot::signal<> signalSpuriousReset;
    sigslot::signal<> signalConnectionTimeout;

private:
    // Serial interface
    bool setSerialAttributes();
    int fd;
    int baud;

    // COBS de-/encoder
    void cobsDecodeReset();
    void handlePacket(uint8_t* buf, int len);
    void cobsDecode(uint8_t* buf, int len);
    void cobsDecodeByte(uint8_t byte);
    size_t cobsEncode(const void* data, size_t length, uint8_t* buffer);
    uint8_t msg[2048];
    uint8_t code;
    uint8_t block;
    uint8_t* decode;
    uint32_t crc32(uint8_t* buf, int len);

    // Read thread for serial port
    Everest::Thread readThreadHandle;
    Everest::Thread timeoutDetectionThreadHandle;

    bool linkWrite(EverestToMcu* m);
    volatile bool reset_done_flag;
    volatile bool forced_reset;

    bool serial_timed_out();
    void timeoutDetectionThread();
    std::chrono::time_point<date::utc_clock> last_keep_alive_lo_timestamp;
};

#endif
