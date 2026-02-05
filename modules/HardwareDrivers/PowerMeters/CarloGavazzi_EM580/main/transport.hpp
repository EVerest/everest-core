// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

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
#include <cstdint>
#include <everest/logging.hpp>
#include <functional>
#include <generated/interfaces/serial_communication_hub/Interface.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace transport {

using DataVector = std::vector<std::uint8_t>;

// Custom exception to distinguish timeout errors from other Modbus errors
class ModbusTimeoutException : public std::runtime_error {
public:
    explicit ModbusTimeoutException(const std::string& message) : std::runtime_error(message) {
    }
};

// Error handler callback type: void(error_message)
using ErrorHandler = std::function<void(const std::string&)>;
// Clear error callback type: void()
using ClearErrorHandler = std::function<void()>;

class AbstractModbusTransport {

public:
    AbstractModbusTransport() = default;
    virtual ~AbstractModbusTransport() = default;

    AbstractModbusTransport(const AbstractModbusTransport&) = delete;
    AbstractModbusTransport& operator=(const AbstractModbusTransport&) = delete;
    AbstractModbusTransport(AbstractModbusTransport&&) = delete;
    AbstractModbusTransport& operator=(AbstractModbusTransport&&) = delete;

    virtual transport::DataVector fetch(std::int32_t address, std::uint16_t register_count) = 0;
    virtual void write_multiple_registers(std::int32_t address, const std::vector<std::uint16_t>& data) = 0;
};

/**
 * data transport via SerialComHub
 */

class SerialCommHubTransport : public AbstractModbusTransport {

private:
    serial_communication_hubIntf& m_serial_hub;
    std::int32_t m_device_id;
    std::int32_t m_base_address;

    // Retry configuration
    std::int32_t m_initial_retry_count;
    std::int32_t m_initial_retry_delay_ms;
    std::int32_t m_normal_retry_count;
    std::int32_t m_normal_retry_delay_ms;

    // State tracking
    std::atomic_bool m_initial_connection_mode{true};

    // Error handling callbacks (optional)
    ErrorHandler m_error_handler;
    ClearErrorHandler m_clear_error_handler;

    // Internal retry helper for functions that return a value
    template <typename Func> auto retry_with_config(Func&& func) -> decltype(std::forward<Func>(func)()) {
        const bool is_initial = m_initial_connection_mode.load();
        const int max_retries = is_initial ? m_initial_retry_count : m_normal_retry_count;
        const int delay_ms = is_initial ? m_initial_retry_delay_ms : m_normal_retry_delay_ms;
        const bool infinite_retries = is_initial && (m_initial_retry_count == 0);

        // For initial connection, 0 means infinite retries
        int attempt = 1;
        while (infinite_retries || attempt <= max_retries) {
            try {
                auto result = std::forward<Func>(func)();
                // First successful call - switch to normal mode
                bool was_initial = m_initial_connection_mode.exchange(false);
                // Clear CommunicationFault error if communication is restored
                // Only clear if we're not in initial connection mode (i.e., we've had
                // at least one successful operation)
                if (m_clear_error_handler && !was_initial) {
                    m_clear_error_handler();
                }
                return result;
            } catch (const ModbusTimeoutException& e) {
                // Timeout errors should raise CommunicationFault
                const bool should_retry = infinite_retries ? true : attempt < max_retries;
                if (should_retry) {
                    if (infinite_retries) {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt
                                      << ", infinite retries): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    } else {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                      << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    // Raise CommunicationFault error for timeout errors
                    if (m_error_handler) {
                        m_error_handler("Modbus communication error: " + std::string(e.what()));
                    }
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            } catch (const std::exception& e) {
                // Other errors (non-timeout) should not raise CommunicationFault
                const bool should_retry = infinite_retries ? true : attempt < max_retries;
                if (should_retry) {
                    if (infinite_retries) {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt
                                      << ", infinite retries): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    } else {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                      << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    // Don't raise CommunicationFault for non-timeout errors
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            }
        }
        // This should never be reached, but needed to satisfy compiler
        throw std::runtime_error("Retry loop exited unexpectedly");
    }

    // Internal retry helper for void functions
    template <typename Func> void retry_with_config_void(Func&& func) {
        const bool is_initial = m_initial_connection_mode.load();
        const int max_retries = is_initial ? m_initial_retry_count : m_normal_retry_count;
        const int delay_ms = is_initial ? m_initial_retry_delay_ms : m_normal_retry_delay_ms;
        const bool infinite_retries = is_initial && (m_initial_retry_count == 0);

        // For initial connection, 0 means infinite retries
        int attempt = 1;
        while (infinite_retries || attempt <= max_retries) {
            try {
                std::forward<Func>(func)();
                // First successful call - switch to normal mode
                bool was_initial = m_initial_connection_mode.exchange(false);
                // Clear CommunicationFault error if communication is restored
                // Only clear if we're not in initial connection mode (i.e., we've had
                // at least one successful operation)
                if (m_clear_error_handler && !was_initial) {
                    m_clear_error_handler();
                }
                return;
            } catch (const ModbusTimeoutException& e) {
                // Timeout errors should raise CommunicationFault
                const bool should_retry = infinite_retries ? true : attempt < max_retries;
                if (should_retry) {
                    if (infinite_retries) {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt
                                      << ", infinite retries): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    } else {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                      << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    // Raise CommunicationFault error for timeout errors
                    if (m_error_handler) {
                        m_error_handler("Modbus communication error: " + std::string(e.what()));
                    }
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            } catch (const std::exception& e) {
                // Other errors (non-timeout) should not raise CommunicationFault
                const bool should_retry = infinite_retries ? true : attempt < max_retries;
                if (should_retry) {
                    if (infinite_retries) {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt
                                      << ", infinite retries): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    } else {
                        EVLOG_warning << "Modbus operation failed (attempt " << attempt << "/" << max_retries
                                      << "): " << e.what() << ". Retrying in " << delay_ms << "ms...";
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                } else {
                    EVLOG_error << "Modbus operation failed after " << attempt << " attempts: " << e.what();
                    // Don't raise CommunicationFault for non-timeout errors
                    rethrow_exception(std::current_exception());
                }
                attempt++;
            }
        }
    }

public:
    struct RetryConfig {
        std::int32_t initial_retry_count;
        std::int32_t initial_retry_delay_ms;
        std::int32_t normal_retry_count;
        std::int32_t normal_retry_delay_ms;
    };

    struct TransportConfig {
        std::int32_t device_id;
        std::int32_t base_address;
        RetryConfig retry;
    };

    SerialCommHubTransport(serial_communication_hubIntf& serial_hub, TransportConfig config) :
        SerialCommHubTransport(serial_hub, config, nullptr, nullptr) {
    }

    SerialCommHubTransport(serial_communication_hubIntf& serial_hub, TransportConfig config, ErrorHandler error_handler,
                           ClearErrorHandler clear_error_handler) :
        m_serial_hub(serial_hub),
        m_device_id(config.device_id),
        m_base_address(config.base_address),
        m_initial_retry_count(config.retry.initial_retry_count),
        m_initial_retry_delay_ms(config.retry.initial_retry_delay_ms),
        m_normal_retry_count(config.retry.normal_retry_count),
        m_normal_retry_delay_ms(config.retry.normal_retry_delay_ms),
        m_error_handler(std::move(error_handler)),
        m_clear_error_handler(std::move(clear_error_handler)) {
    }

    transport::DataVector fetch(std::int32_t address, std::uint16_t register_count) override;
    void write_multiple_registers(std::int32_t address, const std::vector<std::uint16_t>& data) override;
};

} // namespace transport

#endif // POWERMETER_TRANSPORT_HPP
