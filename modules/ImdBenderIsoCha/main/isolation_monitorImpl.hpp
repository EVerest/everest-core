// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_ISOLATION_MONITOR_IMPL_HPP
#define MAIN_ISOLATION_MONITOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/isolation_monitor/Implementation.hpp>

#include "../ImdBenderIsoCha.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int imd_device_id;
    int threshold_resistance_kohm;
};

class isolation_monitorImpl : public isolation_monitorImplBase {
public:
    isolation_monitorImpl() = delete;
    isolation_monitorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<ImdBenderIsoCha>& mod, Conf& config) :
        isolation_monitorImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_start() override;
    virtual void handle_stop() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<ImdBenderIsoCha>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    constexpr static uint8_t ALARM_BITS_MASK{0b00000111};
    constexpr static uint8_t ALARM_AND_TEST_BITS_MASK{0b11000111};
    constexpr static uint8_t PRE_WARNING_STATE{0b00000001};
    constexpr static uint8_t DEVICE_ERROR_STATE{0b00000010};
    constexpr static uint8_t WARNING_STATE{0b00000100};
    constexpr static uint8_t ALARM_STATE{0b00000101};
    constexpr static uint8_t INTERNAL_TEST_STATE{0b00000001};
    constexpr static uint8_t EXTERNAL_TEST_STATE{0b00000010};

    enum class ValueStatus : uint8_t {
        VALUE_IS_TRUE = 0,
        TRUE_VALUE_SMALLER = 0b00000001,
        TRUE_VALUE_LARGER = 0b00000010,
        VALUE_INVALID = 0b00000011
    };

    enum class ImdRegisters : uint16_t {
        RESISTANCE_R_F_OHM = 1000,
        VOLTAGE_U_N_V = 1008,
        VOLTAGE_U_L1E_V = 1016,
        VOLTAGE_U_L2E_V = 1020,
        RESISTANCE_R_FU_OHM = 1028
    };

    enum class ModbusFunctionType : uint8_t {
        READ_HOLDING_REGISTER = 0x03,
        READ_INPUT_REGISTER = 0x04,
        WRITE_MULTIPLE_REGISTERS = 0x10,
        REGISTER_TYPE_UNDEFINED = 0x00
    };

    struct RegisterData {
        float multiplier;
        ImdRegisters type;
        uint16_t start_register;
        ModbusFunctionType register_function;
        uint16_t num_registers;
    };

    struct ImdDataSet {
        double resistance_R_F_ohm{0.0f};
        double resistance_R_FU_ohm{0.0f};
        double voltage_U_N_V{0.0f};
        double voltage_U_L1e_V{0.0f};
        double voltage_U_L2e_V{0.0f};
    };

    std::vector<RegisterData> imd_read_configuration;
    bool config_loaded_successfully = {false};

    ImdDataSet imd_last_values;
    types::isolation_monitor::IsolationMeasurement isolation_measurement_data{};

    std::thread output_thread;
    bool enable_publishing{false};
    bool dead_time_flag{false};
    constexpr static uint8_t DEAD_TIME_S{3};

    void init_default_values();
    void init_registers();
    void init_device();
    void dead_time_wait();
    void request_start();
    void request_stop();
    void enable_device();
    void disable_device();
    void send_to_imd(const uint16_t& command, const uint16_t& value);
    void add_register_to_query(const ImdRegisters register_type);
    void read_imd_values();
    void readRegister(const RegisterData& register_config);
    void process_response(const RegisterData& message_type,
                          const types::serial_comm_hub_requests::Result& register_message);
    static double extract_register_values(const RegisterData& reg_data,
                                          const types::serial_comm_hub_requests::Result& reg_message);
    static std::string get_alarm_status(const uint8_t& alarm_reg);
    static ValueStatus get_value_status(const uint8_t& unit_reg);
    static void output_error_with_content(const types::serial_comm_hub_requests::Result& response);
    void update_IsolationMeasurement();
    // void read_measurement_resistances();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_ISOLATION_MONITOR_IMPL_HPP
