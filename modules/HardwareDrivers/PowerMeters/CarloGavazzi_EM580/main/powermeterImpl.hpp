// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../CarloGavazzi_EM580.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "transport.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int powermeter_device_id;
    int communication_retry_count;
    int communication_retry_delay_ms;
    int communication_error_pause_delay_s;
    int initial_connection_retry_count;
    int initial_connection_retry_delay_ms;
    int timezone_offset_minutes;
    int live_measurement_interval_ms;
    int device_state_read_interval_ms;
    std::string public_key_format;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<CarloGavazzi_EM580>& mod, Conf& config) :
        powermeterImplBase(ev, "main"), mod(mod), config(config){};

    // Marker used to append the transaction id to the tariff text (TT field).
    // Format: "<tariff_text><=><transaction_id>"
    static constexpr std::string_view TARIFF_TEXT_TRANSACTION_ID_MARKER = "<=>";

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    ~powermeterImpl() override;
    // Test-only access helpers (used by unit tests to avoid spinning up the full
    // EVerest runtime). These are intentionally narrow: inject transport + tweak
    // minimal internal state + invoke handlers.
    struct TestAccess {
        static void set_modbus_transport(powermeterImpl& self,
                                         std::unique_ptr<transport::AbstractModbusTransport> transport) {
            self.p_modbus_transport = std::move(transport);
        }

        static void set_pending_closed_transaction(powermeterImpl& self, bool pending) {
            self.m_pending_closed_transaction = pending;
        }

        static void set_transaction_id(powermeterImpl& self, std::string transaction_id) {
            self.m_transaction_id = std::move(transaction_id);
            self.m_transaction_active.store(true);
        }

        static void set_public_key_hex(powermeterImpl& self, std::string public_key_hex) {
            self.m_public_key_hex = std::move(public_key_hex);
        }

        static void set_signed_map_word_count(powermeterImpl& self, std::uint16_t signed_map_word_count) {
            self.m_signed_map_word_count = signed_map_word_count;
        }

        static types::powermeter::TransactionStartResponse start_transaction(powermeterImpl& self,
                                                                             types::powermeter::TransactionReq& req) {
            return self.handle_start_transaction(req);
        }

        static types::powermeter::TransactionStopResponse stop_transaction(powermeterImpl& self,
                                                                           std::string& transaction_id) {
            return self.handle_stop_transaction(transaction_id);
        }
    };
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::powermeter::TransactionStartResponse
    handle_start_transaction(types::powermeter::TransactionReq& value) override;
    virtual types::powermeter::TransactionStopResponse handle_stop_transaction(std::string& transaction_id) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<CarloGavazzi_EM580>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    std::unique_ptr<transport::AbstractModbusTransport> p_modbus_transport;

    std::optional<types::units_signed::SignedMeterValue> m_start_signed_meter_value;

    std::uint16_t m_public_key_length_in_bits;
    std::string m_public_key_hex;
    std::string m_transaction_id;
    std::string m_measure_module_firmware_version;
    std::string m_communication_module_firmware_version;
    std::string m_serial_number;
    std::string m_signature_method_string;
    std::uint16_t m_signed_map_word_count{0};

    std::atomic_bool m_transaction_active{false};
    std::atomic_bool m_pending_time_sync{false};
    bool m_pending_closed_transaction{false};

    // Background threads (started in ready(), joined on destruction)
    std::atomic_bool stop_requested_{false};
    std::mutex stop_mutex_;
    std::condition_variable stop_cv_;
    std::thread live_measure_thread_;
    std::thread time_sync_thread_;

    // flag whether transactions are supported
    bool m_transaction_support{true};

    void configure_device();
    void read_signature_config();
    types::units_signed::SignedMeterValue read_signed_meter_value();
    void read_powermeter_values();
    void dump_device_state(void);
    void read_identification();
    void read_firmware_versions();
    void read_serial_number();
    void read_transaction_state_and_id();
    std::string read_ocmf_file();
    void synchronize_time();
    void set_timezone(int offset_minutes);
    void time_sync_thread();
    [[nodiscard]] bool is_transaction_active() const;
    void clear_transaction_states();
    void write_transaction_registers(const types::powermeter::TransactionReq& transaction_req);
    void read_device_state();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
