/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef YETI_SERIAL
#define YETI_SERIAL

#include <stdint.h>
#include <termios.h>
#include "evThread.h"
#include <sigslot/signal.hpp>
#include "lo2hi.pb.h"
#include "hi2lo.pb.h"

class evSerial {

  public:
    evSerial();
    ~evSerial();

    bool openDevice(const char* device, int baud);

    void readThread();
    void run();

    void setMaxCurrent(float c);
    void setThreePhases(bool n);
    void enableRCD(bool e);
    void setHasVentilation(bool v);
    void setCountryCode(const char* iso3166_alpha2_code);
    void setControlMode(InterfaceControlMode s);
    void setAuth(const char* userid);
    void enable();
    void disable();
    void switchThreePhasesWhileCharging(bool n);
    void pauseCharging();
    void resumeCharging();
    void restart();
    bool reset();
    void firmwareUpdate(bool rom);
    void keepAlive();

    void setPWM(uint32_t mode, float dc);
    void allowPowerOn(bool p);
    void forceUnlock();

    void enableSimulation(bool s);
    void setSimulationData(SimulationData s);


    sigslot::signal<StateUpdate> signalStateUpdate;
    sigslot::signal<DebugUpdate> signalDebugUpdate;
    sigslot::signal<KeepAliveLo> signalKeepAliveLo;
    sigslot::signal<PowerMeter> signalPowerMeter;
    sigslot::signal<SimulationFeedback> signalSimulationFeedback;
    sigslot::signal<Event> signalEvent;
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
    evThread readThreadHandle;
    evThread timeoutDetectionThreadHandle;

    bool linkWrite(HiToLo *m);
    volatile bool reset_done_flag;
    volatile bool forced_reset;

    bool serial_timed_out();
    void timeoutDetectionThread();
    std::chrono::system_clock::time_point last_keep_alive_lo_timestamp;
};

#endif
