// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DPM1000_MAIN_DC_CAN_BROKER_HPP
#define DPM1000_MAIN_DC_CAN_BROKER_HPP
#include <array>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include <dc_can/dc_can.hpp>

struct DcCanRequest {
    enum class State {
        IDLE,
        ISSUED,
        COMPLETED,
        FAILED,
    } state{State::IDLE};

    uint16_t msg_type;
    std::array<uint8_t, 4> response;
    std::condition_variable cv;
    std::mutex mutex;
};

class DcCanBroker {
public:

    enum class AccessReturnType {
        SUCCESS,
        FAILED,
        TIMEOUT,
        NOT_READY,
    };
    DcCanBroker(const std::string& interface_name, uint8_t _device_src);

    AccessReturnType read_data(dc_can::defs::ReadValueType, float& result);
    AccessReturnType set_data(dc_can::defs::SetValueType, float value);
    void set_state(bool enabled);

    ~DcCanBroker();

private:
    constexpr static auto ACCESS_TIMEOUT = std::chrono::milliseconds(250);
    void loop();

    void write_to_can(const struct can_frame& frame);
    AccessReturnType dispatch_frame(const struct can_frame& frame, uint16_t id, uint32_t* result = nullptr);

    void handle_can_input(const struct can_frame& frame);

    uint8_t device_src;

    std::mutex access_mtx;
    DcCanRequest request;

    const uint8_t monitor_id{0xf0};

    std::thread loop_thread;

    int event_fd{-1};
    int can_fd{-1};

    
};

#endif // DPM1000_MAIN_DC_CAN_BROKER_HPP
