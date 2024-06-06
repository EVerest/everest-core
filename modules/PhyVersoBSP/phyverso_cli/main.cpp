// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <stdio.h>
#include <string.h>

#include "evSerial.h"
#include <unistd.h>

#include "phyverso.pb.h"
#include <sigslot/signal.hpp>

std::atomic_bool sw_version_received = false;

void help() {
    printf("\nUsage: ./phyverso_cli /dev/ttyXXX /path/to/config.json\n\n");
}

constexpr auto REFERENCE_VOLTAGE = 3.3;
constexpr auto NUMBER_OF_BITS = 12;
constexpr auto VOLTAGE_TO_TEMPERATURE_SLOPE = -31.0;
constexpr auto VOLTAGE_TO_TEMPERATURE_OFFSET = 92.8;

/// @brief Converts the raw reading to temperature.
float get_temp(int raw) {
    float voltage = (static_cast<float>(raw) / ((1 << NUMBER_OF_BITS) - 1)) * REFERENCE_VOLTAGE;
    return VOLTAGE_TO_TEMPERATURE_SLOPE * voltage + VOLTAGE_TO_TEMPERATURE_OFFSET;
}

int main(int argc, char* argv[]) {
    int selected_connector = 1;

    printf("-- Phyverso CLI tool --\n");

    printf("Use the following keys to send packets:\n");
    printf("A or a: set AC coil on or off\n");
    printf("D or d: set DC coil on or off\n");
    printf("L or l: motorlock lock or unlock\n");
    printf("R or r: hard or soft reset\n");
    printf("V: send keep alive/get version\n");
    printf("1: use connector 1\n");
    printf("2: use connector 2\n\n");

    printf("0: PWM F (0%% DC, -12V)\n");
    printf("5: PWM 5%%\n");
    printf("6: PWM 10%%\n");
    printf("7: PWM 80%%\n");
    printf("8: PWM 97%%\n");
    printf("9: PWM X1 (100%%)\n");
    printf("9: PWM X1 (100%%)\n");

    printf("U or u: Fan1 50%% or OFF (if your fan supports 0%% duty cycle)\n");
    printf("I or i: Fan2 ON or 20%%\n");

    if (argc != 3) {
        help();
        exit(0);
    }
    const char* device = argv[1];
    const char* config_path = argv[2];

    evConfig verso_config;
    if (!verso_config.open_file(config_path)) {
        printf("Could not open config file \"%s\"\n", config_path);
        return -1;
    }

    evSerial p;

    if (!p.open_device(device, 115200)) {
        printf("Cannot open device \"%s\"\n", device);
    } else {
        p.run();

        p.signal_config_request.connect([&]() {
            printf("Received config request\n");
            p.send_config(verso_config);
            printf("Sent config packet\n");
        });

        p.signal_keep_alive.connect([](KeepAlive s) {
            printf(">> KeepAlive: phyverso MCU SW Version: %s, Hardware %i/rev %i, MCU Timestamp %i\n",
                   s.sw_version_string, s.hw_type, s.hw_revision, s.time_stamp);
            sw_version_received = true;
        });

        p.signal_set_coil_state_response.connect([](int connector, CoilState s) {
            if (s.coil_state)
                printf(">> Connector %i, Coil %d: Relais CLOSED\n", connector, s.coil_type);
            else
                printf(">> Connector %i, Coil %d: Relais OPEN\n", connector, s.coil_type);
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

        p.signal_temperature.connect([](Temperature t) {
            printf("Temperatures reported: ");
            for (size_t i = 0; i < t.temp_count; ++i) {
                printf("[T_%zu]: %.1f\t", i, get_temp(t.temp[i]));
            }
            printf("\n");
        });

        p.signal_opaque_data.connect([](int connector, const std::vector<int32_t>& data) {
            printf("Received data from connector %i\n", connector);
        });

        while (true) {
            char c = getc(stdin);
            switch (c) {
            case 'A':
                printf("Setting AC coil to ON\n");
                p.set_coil_state_request(selected_connector, CoilType_COIL_AC, true);
                break;
            case 'a':
                printf("Setting AC coil to OFF\n");
                p.set_coil_state_request(selected_connector, CoilType_COIL_AC, false);
                break;
            case 'D':
                printf("Setting DC coil to ON\n");
                p.set_coil_state_request(selected_connector, CoilType_COIL_DC1, true);
                break;
            case 'd':
                printf("Setting DC coil to OFF\n");
                p.set_coil_state_request(selected_connector, CoilType_COIL_DC1, false);
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
            case 'R':
                printf("Hard reset\n");
                p.reset(1);
                break;
            case 'V':
                printf("Sending keep alive\n");
                p.keep_alive();
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
                printf("Set 0%% PWM\n");
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
            case 'U':
                printf("Set fan1 to 50%%\n");
                p.set_fan_state(0, true, 500);
                break;
            case 'u':
                printf("Set fan1 to OFF\n");
                p.set_fan_state(0, false, 500); // example for setting fan off via enable param
                // check if your fan supports full OFF on PWM 0% duty (also check PWM on oscilloscope)
                // some PWM fans wont turn fully off without switching 12V supply
                break;
            case 'I':
                printf("Set fan2 to ON\n");
                p.set_fan_state(1, true, 1000);
                break;
            case 'i':
                printf("Set fan2 to 20%%\n");
                p.set_fan_state(1, true, 200);
                break;
            }
        }
    }
    return 0;
}
