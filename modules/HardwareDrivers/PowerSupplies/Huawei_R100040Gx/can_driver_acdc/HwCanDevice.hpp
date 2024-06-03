// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef INFY_CAN_DEVICE_HPP
#define INFY_CAN_DEVICE_HPP

#include "CanDevice.hpp"
#include <atomic>
#include <linux/can.h>
#include <map>
#include <mutex>
#include <sigslot/signal.hpp>
#include <thread>

#include <generated/interfaces/power_supply_DC/Implementation.hpp>

class HwCanDevice : public CanDevice {
public:
    HwCanDevice(){

    };

    ~HwCanDevice();

    void run() {
        // spawn thread that requests some data periodically to keep the connection alive
        last_communication_received = std::chrono::steady_clock::now();
        exit_tx_thread = false;
        tx_thread_handle = std::thread(&HwCanDevice::tx_thread, this);
    };

    void set_module_address(const std::vector<uint8_t>& _module_addresses) {
        module_addresses = _module_addresses;
        // Update capabilities
        auto caps = get_capabilities();
        signal_capabilities(caps);
    };

    void set_module_autodetection() {
        module_auto_detection = true;
    };

    // Commands
    bool switch_on(bool on);
    bool set_voltage_current(float voltage, float current);

    int get_number_of_modules() {
        return module_addresses.size();
    }

    void request_module_info();

    // Data out
    sigslot::signal<float, float> signal_voltage_current;
    sigslot::signal<bool> signal_on_off; // true: on, false: off
    sigslot::signal<int, const std::string&> signal_serial_number;
    sigslot::signal<const types::power_supply_DC::Capabilities&> signal_capabilities;
    sigslot::signal<bool, const std::string&>
        signal_communication_error; // true for error, false means error was cleared, error description

    struct Telemetry {
        float voltage{0.};
        float current{0.};
    };

    float total_current{0.};

    std::map<uint8_t, Telemetry> telemetries;

    friend std::ostream& operator<<(std::ostream& out, const Telemetry& self);

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload);
    virtual void connection_established();

private:
    std::atomic_bool exit_tx_thread;
    std::thread tx_thread_handle;
    void tx_thread();

    void set_mode();

    bool tx(Huawei::Packet& packet);

    types::power_supply_DC::Capabilities get_capabilities();

    void send_to_all_modules(Huawei::Packet packet) {
        // Send to each module
        for (auto module_address : module_addresses) {
            packet.set_module_address(module_address);
            tx(packet);
        }
    };

    void send_to_broadcast(Huawei::Packet packet) {
        packet.set_module_address(Huawei::ADDR_BROADCAST);
        tx(packet);
    };

    std::vector<uint8_t> module_addresses{};
    std::vector<uint8_t> module_addresses_reported{};

    bool switch_on_nolock(bool on);
    bool set_voltage_current_nolock(float voltage, float current);

    std::atomic_bool module_auto_detection{false};

    float set_point_voltage{0.};
    float set_point_current{0.};

    std::atomic<float> requested_set_point_voltage{0.};
    std::atomic<float> requested_set_point_current{0.};

    bool is_on{false};
    std::atomic_bool requested_on{false};
    bool last_requested_on{false};

    std::chrono::time_point<std::chrono::steady_clock> last_communication_received;
    static constexpr std::chrono::seconds COMMS_TIMEOUT{2};

    std::unordered_map<uint8_t, std::string> module_serial_numbers;
};

#endif // INFY_CAN_DEVICE_HPP
