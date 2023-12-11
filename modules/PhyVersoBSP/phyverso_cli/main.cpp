// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <stdio.h>
#include <string.h>

#include "evSerial.h"
#include <unistd.h>

#include "phyverso.pb.h"
#include <sigslot/signal.hpp>

std::atomic_bool sw_version_received = false;

void help() {
    printf("\nUsage: ./phyverso_cli /dev/ttyXXX\n\n");
}

int main(int argc, char* argv[]) {
    int selected_connector = 1;

    printf("-- Phyverso CLI tool --\n");

    printf("Use the following keys to send packets:\n");
    printf("A or a: allow_power_on true or false\n");
    printf("L or l: motorlock lock or unlock\n");
    printf("R or r: hard or soft reset\n");
    printf("1: use connector 1\n");
    printf("2: use connector 2\n\n");

    printf("0: PWM F (0%% DC, -12V)\n");
    printf("5: PWM 5%%\n");
    printf("6: PWM 10%%\n");
    printf("7: PWM 80%%\n");
    printf("8: PWM 97%%\n");
    printf("9: PWM X1 (100%%)\n");
    printf("9: PWM X1 (100%%)\n");

    if (argc != 2) {
        help();
        exit(0);
    }
    const char* device = argv[1];

    evSerial p;

    if (!p.open_device(device, 115200)) {
        printf("Cannot open device \"%s\"\n", device);
    } else {
        p.run();
        p.signal_keep_alive.connect([](KeepAlive s) {
            printf(">> KeepAlive: phyverso MCU SW Version: %s, Hardware %i/rev %i\n", s.sw_version_string, s.hw_type,
                   s.hw_revision);
            sw_version_received = true;
        });

        p.signal_relais_state.connect([](int connector, bool s) {
            if (s)
                printf(">> Connector %i: Relais CLOSED\n", connector);
            else
                printf(">> Connector %i: Relais OPEN\n", connector);
        });

        p.signal_telemetry.connect([](int connector, Telemetry t) {
            printf(">> Connector %i: CP Voltage %i %i\n", connector, t.cp_voltage_hi, t.cp_voltage_lo);
        });

        p.signal_cp_state.connect([](int connector, CpState s) {
            switch (s) {
            case CpState_STATE_A:
                printf(">> Connector %i: CP state A\n", connector);
                break;
            case CpState_STATE_B:
                printf(">> Connector %i: CP state B\n", connector);
                break;
            case CpState_STATE_C:
                printf(">> Connector %i: CP state C\n", connector);
                break;
            case CpState_STATE_D:
                printf(">> Connector %i: CP state D\n", connector);
                break;
            case CpState_STATE_E:
                printf(">> Connector %i: CP state E\n", connector);
                break;
            case CpState_STATE_F:
                printf(">> Connector %i: CP state F\n", connector);
                break;
            }
        });

        p.signal_pp_state.connect([](int connector, PpState s) {
            switch (s) {
            case PpState_STATE_NC:
                printf(">> Connector %i: PP state NC\n", connector);
                break;
            case PpState_STATE_13A:
                printf(">> Connector %i: PP state 13A\n", connector);
                break;
            case PpState_STATE_20A:
                printf(">> Connector %i: PP state 20A\n", connector);
                break;
            case PpState_STATE_32A:
                printf(">> Connector %i: PP state 32A\n", connector);
                break;
            case PpState_STATE_70A:
                printf(">> Connector %i: PP state 70A\n", connector);
                break;
            case PpState_STATE_FAULT:
                printf(">> Connector %i: PP state FAULT\n", connector);
                break;
            }
        });

        p.signal_fan_state.connect([](FanState s) {
            printf(">> Fan %i: EN=%s, Duty=%d RPM=%d\n", s.fan_id, (s.enabled ? "ON" : "OFF"), s.duty, s.rpm);
        });

        p.signal_lock_state.connect([](int connector, LockState s) {
            switch (s) {
            case LockState_UNDEFINED:
                printf(">> Connector %i: Lock State UNDEFINED\n", connector);
                break;
            case LockState_LOCKED:
                printf(">> Connector %i: Lock State Locked\n", connector);
                break;
            case LockState_UNLOCKED:
                printf(">> Connector %i: Lock State Unlocked\n", connector);
                break;
            }
        });

        while (true) {
            char c = getc(stdin);
            switch (c) {
            case 'A':
                printf("Setting allow_power_on to true\n");
                p.allow_power_on(selected_connector, true);
                break;
            case 'a':
                printf("Setting allow_power_on to false\n");
                p.allow_power_on(selected_connector, false);
                break;
            case 'L':
                printf("Locking connector\n");
                p.lock(selected_connector, true);
                break;
            case 'l':
                printf("Unlocking connector\n");
                p.lock(selected_connector, false);
                break;
            case 'r':
                printf("Soft reset\n");
                p.reset(-1);
                break;
            case '1':
                printf("Connector 1 selected.\n");
                selected_connector = 1;
                break;
            case '2':
                printf("Connector 2 selected.\n");
                selected_connector = 2;
                break;
            case '0':
                p.set_pwm(selected_connector, 0);
                break;
            case '5':
                printf("Set 5%% PWM\n");
                p.set_pwm(selected_connector, 500);
                break;
            case '6':
                printf("Set 10%% PWM\n");
                p.set_pwm(selected_connector, 1000);
                break;
            case '7':
                printf("Set 80%% PWM\n");
                p.set_pwm(selected_connector, 8000);
                break;
            case '8':
                printf("Set 97%% PWM\n");
                p.set_pwm(selected_connector, 9700);
                break;
            case '9':
                printf("Set 100%% PWM\n");
                p.set_pwm(selected_connector, 10000);
                break;
            }
        }
    }
    return 0;
}
