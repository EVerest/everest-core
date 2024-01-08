// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "evSerial.h"

#include <chrono>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sigslot/signal.hpp>

#include "umwc.pb.h"

using namespace std::chrono_literals;

volatile bool sw_version_received = false;

void recvKeepAliveLo(KeepAliveLo s) {
    printf("Current uMWC SW Version: %s (Protocol %i.%i)\n", s.sw_version_string, s.protocol_version_major,
           s.protocol_version_minor);
    sw_version_received = true;
}

void help() {
    printf("\nUsage: ./umwc_fwupdate /dev/ttyXXX firmware.bin\n\n");
    printf("This tool uses stm32flash (version 0.6 and above) which needs to be installed.\n");
}

int main(int argc, char* argv[]) {
    printf("uMWC ROM Bootloader Firmware Updater\n");
    if (argc != 3) {
        help();
        exit(0);
    }
    const char* device = argv[1];
    const char* filename = argv[2];

    evSerial* p = new evSerial();

    if (p->openDevice(device, 115200)) {
        // printf("Running\n");
        p->run();
        p->signalKeepAliveLo.connect(recvKeepAliveLo);
        while (true) {
            if (sw_version_received)
                break;
            std::this_thread::sleep_for(100us);
        }
        printf("\nRebooting uMWC in ROM Bootloader mode...\n");
        // send some dummy commands to make sure protocol is in sync
        p->enable(false);
        p->enable(false);
        // now reboot uC in boot loader mode
        p->firmwareUpdate(true);
        sleep(1);
        delete p;

        sleep(1);
        char cmd[1000];
        sprintf(cmd, "stm32flash -b 115200 %.100s -v -w %.100s -R", device, filename);
        // sprintf(cmd, "stm32flash -b115200 %.100s", device);
        printf("Executing %s ...\n", cmd);
        system(cmd);

        // printf ("Joining\n");
    } else {
        printf("Cannot open device \"%s\"\n", device);
        delete p;
    }
    return 0;
}
