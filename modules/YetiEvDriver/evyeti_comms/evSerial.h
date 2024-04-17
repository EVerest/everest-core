// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef YETI_SERIAL
#define YETI_SERIAL

#include <stdint.h>
#include <termios.h>
#include <utils/thread.hpp>
#include <sigslot/signal.hpp>
#include "lo2hi.pb.h"
#include "hi2lo.pb.h"
#include <date/date.h>
#include <date/tz.h>

class evSerial {

  public:
    evSerial();
    ~evSerial();

    bool openDevice(const char* device, int baud);

    void readThread();
    void run();

    void enable();
    void disable();
    void firmwareUpdate(bool rom);
    void keepAlive();

    bool reset(const int reset_pin);

    void setBCDE(uint32_t mode);
    void allowPowerOn(bool p);

    sigslot::signal<KeepAliveLo> signalKeepAliveLo;
    sigslot::signal<Event> signalEvent;
    sigslot::signal<Measurements> signalMeasurements;
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
    size_t cobsEncode(const void *data, size_t length, uint8_t *buffer);
    uint8_t msg[2048];
    uint8_t code;
    uint8_t block;
    uint8_t *decode;
    uint32_t crc32(uint8_t *buf, int len);

    // Read thread for serial port
    Everest::Thread readThreadHandle;
    Everest::Thread timeoutDetectionThreadHandle;

    bool linkWrite(HiToLo *m);
    volatile bool reset_done_flag;
    volatile bool forced_reset;

    bool serial_timed_out();
    void timeoutDetectionThread();
    std::chrono::time_point<date::utc_clock> last_keep_alive_lo_timestamp;
};

#endif
