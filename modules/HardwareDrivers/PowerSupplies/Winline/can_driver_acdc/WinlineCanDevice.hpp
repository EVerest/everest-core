// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef WINLINE_CAN_DEVICE_HPP
#define WINLINE_CAN_DEVICE_HPP

#include "CanBus.hpp"
#include <chrono>
#include <deque> // Added for status history
#include <linux/can.h>
#include <map>
#include <mutex>
#include <sigslot/signal.hpp>
#include <vector>

class WinlineCanDevice : public CanBus {
public:
    WinlineCanDevice();
    ~WinlineCanDevice();

    enum class Error {
        OverVoltage,
        UnderVoltage,
        OverTemperature,
        FanFault,
        InputPhaseLoss,
        CommunicationFault,
        InternalFault,
        OverCurrent,
        InputVoltage,
        VendorError,
        VendorWarning
    };

    enum class OperatingMode {
        FIXED_ADDRESS,
        GROUP_DISCOVERY
    };

    void set_can_device(const std::string& dev);
    void set_config_values(const std::string& addrs, int group_address, int timeout, int controller_address,
                           int power_state_grace_period_ms, int altitude_setting_m, const std::string& input_mode,
                           double module_current_limit_point);
    void initial_ping();

    // Commands
    bool switch_on_off(bool on);
    bool set_voltage_current(float voltage, float current);

    // Enhanced Winline group operations
    bool discover_group_modules();

    // Winline error recovery operations
    bool reset_overvoltage_protection(uint8_t module_address);
    bool reset_short_circuit_protection(uint8_t module_address);

    // Altitude setting operations
    bool set_altitude_all_modules();

    // Current limit point setting operations
    bool set_current_limit_point_all_modules();

    // Input mode setting operations
    bool set_input_mode_all_modules();

    // Winline register-based command functions
    bool send_read_register(uint8_t destination_address, uint16_t register_number, bool group = false);
    bool send_set_register_float(uint8_t destination_address, uint16_t register_number, float value,
                                 bool group = false);
    bool send_set_register_integer(uint8_t destination_address, uint16_t register_number, uint32_t value,
                                   bool group = false);

    // Enhanced Winline status monitoring capabilities
    bool perform_comprehensive_status_check(uint8_t module_address);
    bool analyze_status_trends(uint8_t module_address);
    void log_status_diagnostics(uint8_t module_address, const can_packet_acdc::PowerModuleStatus& status);
    std::string get_status_summary(uint8_t module_address) const;

    // Enhanced Winline power control capabilities
    bool verify_power_state(uint8_t module_address, bool expected_on_state);
    bool handle_power_transition(bool target_state);
    void track_power_state_change(uint8_t module_address, bool new_power_state);

    // Template overloads for type-safe command sending (DEPRECATED - use register functions)
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
        // Core telemetry values
        float voltage{0.};
        float current{0.};
        float current_limit_point{0.};

        // Legacy InfyPower fields (retained for compatibility)
        float v_ext{0.};
        float i_avail{0.};
        bool valid_caps{false};

        // Module capabilities and limits (Winline protocol provides current and power only)
        float dc_max_output_current{0.};
        float dc_rated_output_power{0.};

        // Temperature monitoring (Winline-specific)
        float dc_board_temperature{0.};
        float ambient_temperature{0.};
        float pfc_board_temperature{0.};

        // Status and diagnostic information
        can_packet_acdc::PowerModuleStatus status;

        // Module identification (Winline dual-register serial number)
        std::string serial_number; // Complete formatted serial number
        uint32_t serial_low{0};    // Low bytes from register 0x0054
        uint32_t serial_high{0};   // High bytes from register 0x0055

        // Version information
        uint16_t dcdc_version{0};
        uint16_t pfc_version{0};

        // Winline-specific settings
        uint32_t altitude_setting{1000}; // Working altitude in meters
        uint32_t input_mode{1};          // 1=AC, 2=DC
        uint8_t group_number{0};         // Module group assignment
        uint8_t dip_address{0};          // DIP switch address

        // Enhanced status monitoring
        struct StatusHistory {
            std::deque<can_packet_acdc::PowerModuleStatus> recent_status; // Last 10 status readings
            uint32_t fault_count{0};                                      // Total fault occurrences
            uint32_t recovery_count{0};                                   // Successful recovery attempts
            std::chrono::time_point<std::chrono::steady_clock> last_fault_time;
            std::chrono::time_point<std::chrono::steady_clock> last_recovery_time;
        } status_history;

        struct StatusMetrics {
            uint32_t status_reads_total{0};  // Total status reads
            uint32_t status_errors_total{0}; // Status read errors
            std::chrono::time_point<std::chrono::steady_clock> last_status_read;
            float status_read_success_rate{100.0f}; // Success rate percentage
        } status_metrics;

        // Enhanced power control tracking
        struct PowerStateTracking {
            bool expected_power_state{false};   // Expected power state (what we commanded)
            bool actual_power_state{false};     // Actual power state (from status register)
            bool power_state_verified{false};   // Whether power state has been verified
            uint32_t power_commands_sent{0};    // Total power commands sent
            uint32_t power_state_mismatches{0}; // Power state verification failures
            std::chrono::time_point<std::chrono::steady_clock> last_power_command;
            std::chrono::time_point<std::chrono::steady_clock> last_power_verification;
        } power_tracking;

        // Timing
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
    int power_state_grace_period_ms{0};
    int altitude_setting_m{0};
    double module_current_limit_point{0.};
    std::string input_mode{"AC"};
    OperatingMode operating_mode{OperatingMode::FIXED_ADDRESS};

    std::vector<uint8_t> active_module_addresses;
    std::vector<uint8_t> configured_module_addresses; // Store original configured addresses for recovery
    std::mutex active_modules_mutex;

    void poll_status_handler() override;
    size_t remove_expired_telemetry_entries();

    // Helper methods to reduce code duplication in packet handling
    void check_and_signal_error_status_change(uint8_t source_address,
                                              const can_packet_acdc::PowerModuleStatus& new_status,
                                              const can_packet_acdc::PowerModuleStatus& old_status);

    // Helper for standardized module identification in logging
    std::string format_module_id(uint8_t address, const std::string& serial_number = "") const;

    // Helper to check and update capabilities when capability data is received
    void check_and_update_capabilities(uint8_t source_address);

    // Private implementation for template methods
    bool send_command_impl(uint8_t destination_address, uint8_t command_number, const std::vector<uint8_t>& payload,
                           bool group = false);
};

#endif // WINLINE_CAN_DEVICE_HPP