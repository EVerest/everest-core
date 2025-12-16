// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_TRANSPORT_HPP
#define POWERMETER_TRANSPORT_HPP

/**
 * Baseclass for transport classes.
 *
 * Transports are:
 * - direct connection via modbus
 * - connection via SerialComHub
 */

#include <atomic>
#include <chrono>
#include <everest/logging.hpp>
#include <generated/interfaces/serial_communication_hub/Interface.hpp>
#include <thread>

namespace transport {

using DataVector = std::vector<std::uint8_t>;

class AbstractModbusTransport {

public:
    virtual transport::DataVector fetch(int address, int register_count) = 0;
    virtual void write_multiple_registers(int address, const std::vector<uint16_t>& data) = 0;
};

/**
 * data transport via SerialComHub
 */

class SerialCommHubTransport : public AbstractModbusTransport {

protected:
    serial_communication_hubIntf& m_serial_hub;
    int m_device_id;
    int m_base_address;

    // Retry configuration
    int m_initial_retry_count;
    int m_initial_retry_delay_ms;
    int m_normal_retry_count;
    int m_normal_retry_delay_ms;

    // State tracking
    std::atomic_bool m_initial_connection_mode{true};

    // Internal retry helper for functions that return a value
    template <typename Func> auto retry_with_config(Func&& func) -> decltype(std::forward<Func>(func)()) {
        bool is_initial = m_initial_connection_mode.load();
        int max_retries = is_initial ? m_initial_retry_count : m_normal_retry_count;
        int delay_ms = is_initial ? m_initial_retry_delay_ms : m_normal_retry_delay_ms;

        // For initial connection, 0 means infinite retries
        int attempt = 1;
        while (m_initial_retry_count == 0 ? true : attempt <= max_retries) {
            try {
                auto result = std::forward<Func>(func)();
                // First successful call - switch to normal mode
                if (m_initial_connection_mode.exchange(false)) {
                    // Switched from initial to normal mode
                }
                return result;
            } catch (const std::exception& e) {
                bool should_retry = is_initial && m_initial_retry_count == 0 ? true : attempt < max_retries;
                if (should_retry) {
                    EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                  << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            }
        }
    }

    // Internal retry helper for void functions
    template <typename Func> void retry_with_config_void(Func&& func) {
        bool is_initial = m_initial_connection_mode.load();
        int max_retries = is_initial ? m_initial_retry_count : m_normal_retry_count;
        int delay_ms = is_initial ? m_initial_retry_delay_ms : m_normal_retry_delay_ms;

        // For initial connection, 0 means infinite retries
        int attempt = 1;
        while (m_initial_retry_count == 0 ? true : attempt <= max_retries) {
            try {
                std::forward<Func>(func)();
                // First successful call - switch to normal mode
                if (m_initial_connection_mode.exchange(false)) {
                    // Switched from initial to normal mode
                }
                return;
            } catch (const std::exception& e) {
                bool should_retry = is_initial && m_initial_retry_count == 0 ? true : attempt < max_retries;
                if (should_retry) {
                    EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                  << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            }
        }
    }

public:
    SerialCommHubTransport(serial_communication_hubIntf& serial_hub, int device_id, int base_address,
                           int initial_retry_count, int initial_retry_delay_ms, int normal_retry_count,
                           int normal_retry_delay_ms) :
        m_serial_hub(serial_hub),
        m_device_id(device_id),
        m_base_address(base_address),
        m_initial_retry_count(initial_retry_count),
        m_initial_retry_delay_ms(initial_retry_delay_ms),
        m_normal_retry_count(normal_retry_count),
        m_normal_retry_delay_ms(normal_retry_delay_ms) {
    }

    virtual transport::DataVector fetch(int address, int register_count) override;
    virtual void write_multiple_registers(int address, const std::vector<uint16_t>& data) override;
};

} // namespace transport

#endif // POWERMETER_TRANSPORT_HPP
