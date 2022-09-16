// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "config.h"
#include <stdio.h>
#include <string.h>

#include "evSerial.h"
#include <unistd.h>

#include "hi2lo.pb.h"
#include "lo2hi.pb.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sigslot/signal.hpp>

volatile bool sw_version_received = false;
std::string currentYetiVersion;

std::string getVersionFromFile(const std::string& fn) {
    std::vector<std::string> results;
    boost::split(results, fn, boost::is_any_of("_"));
    if (results.size() >= 2) {
        return results[1];
    } else
        return std::string();
}

bool parseVersion(const std::string& v, unsigned int& major, unsigned int& minor, unsigned int& nrcommits) {
    std::vector<std::string> results;
    boost::split(results, v, boost::is_any_of(".-"));

    if (results.size() == 0)
        return false;

    if (results.size() >= 1) {
        major = std::stoi(results[0]);
    } else {
        major = 0;
    }

    if (results.size() >= 2) {
        minor = std::stoi(results[1]);
    } else {
        minor = 0;
    }

    if (results.size() >= 3) {
        nrcommits = std::stoi(results[2]);
    } else {
        nrcommits = 0;
    }

    return true;
}

/*
 compares two version strings of the format
 Major.Minor[-commits since tag], e.g. 0.4 with 0.4-3

 returns:

 -1 if v1<v2
  0 if v1==v2
 +1 if v1>v2

 -2 on error (e.g. string not parsable)

*/
int compareVersionStrings(const std::string& v1, const std::string& v2) {
    unsigned int v1_major, v1_minor, v1_nrcommits;
    unsigned int v2_major, v2_minor, v2_nrcommits;
    if (parseVersion(v1, v1_major, v1_minor, v1_nrcommits) && parseVersion(v2, v2_major, v2_minor, v2_nrcommits)) {
        if (v1_major < v2_major)
            return -1;
        if (v1_major > v2_major)
            return 1;

        if (v1_minor < v2_minor)
            return -1;
        if (v1_minor > v2_minor)
            return 1;

        if (v1_nrcommits < v2_nrcommits)
            return -1;
        if (v1_nrcommits > v2_nrcommits)
            return 1;

        return 0;

    } else
        return -2;
}

void recvKeepAliveLo(KeepAliveLo s) {
    printf("Current Yeti SW Version: %s (Protocol %i.%0.2i)\n", s.sw_version_string, s.protocol_version_major,
           s.protocol_version_minor);
    currentYetiVersion = s.sw_version_string;
    sw_version_received = true;
}

void help() {
    printf("\nUsage: ./yeti_fwupdate /dev/ttyXXX firmware.bin [--force]\n\n");
    printf("This tool uses stm32flash (version 0.6 and above) which needs to be installed.\n");
    printf("Installs Yeti FW over UART if the version is higher then the one currently installed. Use --force to downgrade.\n");
}

int main(int argc, char* argv[]) {
    printf("Yeti ROM Bootloader Firmware Updater %i.%i\n", yeti_fwupdate_VERSION_MAJOR, yeti_fwupdate_VERSION_MINOR);
    if (!(argc ==3 || argc ==4)) {
        help();
        exit(0);
    }

    bool force_update = false;
    if (argc ==4 && strcmp(argv[3], "--force")==0) {
	force_update = true;
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
            usleep(100);
        }

        // Check if the version is older then the update file, then install...
        std::string availableYetiVersion = getVersionFromFile(std::string(filename));

        if (availableYetiVersion.empty()) {
            std::cout << "Cannot parse version from filename " << std::string(filename) << std::endl;
        } else {
            std::cout << "Bundled YetiFW version " << availableYetiVersion << std::endl;

            // Check if version of update file is higher than currently installed version
            if (compareVersionStrings(availableYetiVersion, currentYetiVersion) == 1 || force_update) {
                std::cout << "Currently installed: " << currentYetiVersion << " - Installing " << availableYetiVersion
                          << std::endl;

                printf("\nRebooting Yeti in ROM Bootloader mode...\n");
                // send some dummy commands to make sure protocol is in sync
                p->setMaxCurrent(6.);
                p->setMaxCurrent(6.);
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
                std::cout << "Skipping installation." << std::endl;
	    }
        }
    } else {
        printf("Cannot open device \"%s\"\n", device);
        delete p;
    }
    return 0;
}
