// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <stdio.h>

#include <cstring>
#include <goose-ethernet/driver.hpp>
#include <goose-ethernet/frame.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return 1;
    }

    goose_ethernet::EthernetInterface interface(argv[1]);
    const std::uint8_t* mac = interface.get_mac_address();

    goose_ethernet::EthernetFrame frame;
    frame.destination[0] = 0x00;
    frame.destination[1] = 0x00;
    frame.destination[2] = 0x00;
    frame.destination[3] = 0x00;
    frame.destination[4] = 0x00;
    frame.destination[5] = 0x00;
    memcpy(frame.source, mac, 6);
    frame.ethertype = 0x88B8;
    frame.payload.resize(46);
    // appid
    frame.payload[0] = 0xff;
    frame.payload[1] = 0x00;

    interface.send_packet(frame);

    while (1) {
        auto recveive = interface.receive_packet();
        if (!recveive.has_value()) {
            continue;
        }

        auto recv = recveive.value();

        printf("Received packet from: ");
        for (size_t i = 0; i < 6; i++) {
            printf("%02x ", recv.source[i]);
        }
        printf("\n");

        printf("EtherType: %04x\n", recv.ethertype);

        printf("Received packet payload: ");
        for (size_t i = 0; i < recv.payload.size(); i++) {
            printf("%02x ", recv.payload[i]);
        }
        printf("\n");
    }

    return 0;
}
