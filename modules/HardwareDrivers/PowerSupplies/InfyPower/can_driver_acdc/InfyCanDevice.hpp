// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef INFY_CAN_DEVICE_HPP
#define INFY_CAN_DEVICE_HPP

#include "CanBus.hpp"
#include <chrono>
#include <linux/can.h>
#include <map>
#include <mutex>
#include <sigslot/signal.hpp>
#include <vector>

class InfyCanDevice : public CanBus {
public:
    InfyCanDevice();
    ~InfyCanDevice();

    enum class Error {
        OverVoltage,
        UnderVoltage,
        OverTemperature,
        FanFault,
        InputPhaseLoss,
        CommunicationFault,
        InternalFault,
        OverCurrent,
        InputVoltage
    };

    enum class OperatingMode {
        FIXED_ADDRESS,
        GROUP_DISCOVERY
    };

    void set_can_device(const std::string& dev);
    void set_config_values(const std::string& addrs, int group_address, int timeout, int controller_address);
    void initial_ping();

    // Commands
    bool switch_on_off(bool on);
    bool set_voltage_current(float voltage, float current);

    // Template overloads for type-safe command sending
    template <typename PacketType> bool send_command(uint8_t destination_address, bool group = false) {
        // Use static const vector to avoid repeated allocations
        static const std::vector<uint8_t> empty_payload(
            8, 0); // 8 zero bytes for read commands, otherwise the device returns an error
        return send_command_impl(destination_address, PacketType::CMD_ID, empty_payload, group);
    }

    template <typename PacketType>
    bool send_command(uint8_t destination_address, const PacketType& packet, bool group = false) {
        return send_command_impl(destination_address, PacketType::CMD_ID, packet.operator std::vector<uint8_t>(),
                                 group);
    }

    struct Telemetry {
        float voltage{0.};
        float current{0.};
        float v_ext{0.};
        float i_avail{0.};
        bool valid_caps{false};
        float dc_max_output_voltage{0.};
        float dc_min_output_voltage{0.};
        float dc_max_output_current{0.};
        float dc_rated_output_power{0.};
        can_packet_acdc::PowerModuleStatus status;
        std::string serial_number; // Module barcode/serial number for identification
        std::chrono::time_point<std::chrono::steady_clock> last_update;
    };
    typedef std::map<uint8_t, Telemetry> TelemetryMap;
    TelemetryMap telemetries;

    // Data out
    sigslot::signal<TelemetryMap> signalVoltageCurrent;
    sigslot::signal<can_packet_acdc::PowerModuleStatus> signalModuleStatus;
    sigslot::signal<uint8_t, Error, bool> signalError;
    sigslot::signal<TelemetryMap> signalCapabilitiesUpdate;

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload);

private:
    bool initialized{false}; // Set to true when we have received the very first module count packet
    uint8_t controller_address{0};
    std::string can_device{""};
    int group_address{0};
    size_t expected_module_count{0};
    int device_connection_timeout_s{0};
    OperatingMode operating_mode{OperatingMode::FIXED_ADDRESS};

    std::vector<uint8_t> active_module_addresses;
    std::mutex active_modules_mutex;

    void poll_status_handler() override;
    size_t remove_expired_telemetry_entries();

    // Helper methods to reduce code duplication in packet handling
    void handle_module_count_packet(const std::vector<uint8_t>& payload);
    void handle_simple_telemetry_update(uint8_t source_address, const std::vector<uint8_t>& payload,
                                        uint8_t command_number);
    void check_and_signal_error_status_change(uint8_t source_address,
                                              const can_packet_acdc::PowerModuleStatus& new_status,
                                              const can_packet_acdc::PowerModuleStatus& old_status);

    // Helper for standardized module identification in logging
    std::string format_module_id(uint8_t address, const std::string& serial_number = "") const;

    // Private implementation for template methods
    bool send_command_impl(uint8_t destination_address, uint8_t command_number, const std::vector<uint8_t>& payload,
                           bool group = false);
};

#endif // INFY_CAN_DEVICE_HPP