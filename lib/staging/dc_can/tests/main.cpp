// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <future>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include <net/if.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <dc_can/dc_can.hpp>

// FIXME (aw): this helper doesn't really belong here
static void exit_with_error(const char* msg) {
    fprintf(stderr, "%s (%s)\n", msg, strerror(errno));
    exit(-EXIT_FAILURE);
}

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
    DcCanBroker(const std::string& interface_name);

    AccessReturnType read_data(dc_can::defs::ReadValueType, float& result);
    AccessReturnType set_data(dc_can::defs::SetValueType, float value);
    void enable();

    ~DcCanBroker();

private:
    constexpr static auto ACCESS_TIMEOUT = std::chrono::milliseconds(250);
    void loop();

    void write_to_can(const struct can_frame& frame);
    AccessReturnType dispatch_frame(const struct can_frame& frame, uint16_t id, uint32_t* result = nullptr);

    void handle_can_input(const struct can_frame& frame);

    bool device_found{false};
    uint8_t device_src;

    std::mutex access_mtx;
    DcCanRequest request;

    const uint8_t monitor_id{0xf0};

    std::thread loop_thread;

    int event_fd{-1};
    int can_fd{-1};
};

DcCanBroker::DcCanBroker(const std::string& interface_name) {
    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (can_fd == -1) {
        exit_with_error("Failed to open socket");
    }

    // retrieve interface index from interface name
    struct ifreq ifr;

    if (interface_name.size() >= sizeof(ifr.ifr_name)) {
        exit_with_error("Interface name too long.");
    } else {
        strcpy(ifr.ifr_name, interface_name.c_str());
    }

    if (ioctl(can_fd, SIOCGIFINDEX, &ifr) == -1) {
        exit_with_error("Failed with ioctl/SIOCGIFINDEX");
    }

    // bind to the interface
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        exit_with_error("Failed with bind");
    }

    event_fd = eventfd(0, 0);

    loop_thread = std::thread(&DcCanBroker::loop, this);
}

DcCanBroker::~DcCanBroker() {
    uint64_t quit_value = 1;
    write(event_fd, &quit_value, sizeof(quit_value));

    loop_thread.join();

    close(can_fd);
    close(event_fd);
}

void DcCanBroker::loop() {
    std::array<struct pollfd, 2> pollfds = {{
        {can_fd, POLLIN, 0},
        {event_fd, POLLIN, 0},
    }};

    while (true) {
        const auto poll_result = poll(pollfds.data(), pollfds.size(), -1);

        if (poll_result == 0) {
            // timeout
            continue;
        }

        if (pollfds[0].revents & POLLIN) {
            struct can_frame frame;
            read(can_fd, &frame, sizeof(frame));

            if (device_found) {
                handle_can_input(frame);
            } else {
                device_src = dc_can::parse_source(frame);
                device_found = true;
                printf("Found device with source number %02X\n", device_src);
            }
        }

        if (pollfds[1].revents & POLLIN) {
            uint64_t tmp;
            read(event_fd, &tmp, sizeof(tmp));
            // new event, for now, we do not care, later on we could check, if it is an exit event code
            return;
        }
    }
}

void DcCanBroker::enable() {
    struct can_frame frame;
    dc_can::power_on(frame, true, true);
    dc_can::set_header(frame, monitor_id, device_src);

    write_to_can(frame);
}

DcCanBroker::AccessReturnType DcCanBroker::dispatch_frame(const struct can_frame& frame, uint16_t id,
                                                          uint32_t* return_payload) {
    if (not device_found) {
        return AccessReturnType::NOT_READY;
    }

    // wait until we get access
    std::lock_guard<std::mutex> access_lock(access_mtx);

    std::unique_lock<std::mutex> request_lock(request.mutex);
    write_to_can(frame);
    request.msg_type = id;
    request.state = DcCanRequest::State::ISSUED;

    const auto finished = request.cv.wait_for(request_lock, ACCESS_TIMEOUT,
                                              [this]() { return request.state != DcCanRequest::State::ISSUED; });

    if (not finished) {
        return AccessReturnType::TIMEOUT;
    }

    if (request.state == DcCanRequest::State::FAILED) {
        return AccessReturnType::FAILED;
    }

    // success
    if (return_payload) {
        memcpy(return_payload, request.response.data(), sizeof(std::remove_pointer_t<decltype(return_payload)>));
    }

    return AccessReturnType::SUCCESS;
}

DcCanBroker::AccessReturnType DcCanBroker::read_data(dc_can::defs::ReadValueType value_type, float& result) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    struct can_frame frame;
    dc_can::request_data(frame, value_type);
    dc_can::set_header(frame, monitor_id, device_src);

    uint32_t tmp;
    const auto status = dispatch_frame(frame, id, &tmp);

    if (status == AccessReturnType::SUCCESS) {
        memcpy(&result, &tmp, sizeof(result));
    }

    return status;
}

DcCanBroker::AccessReturnType DcCanBroker::set_data(dc_can::defs::SetValueType value_type, float payload) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    uint8_t raw_payload[sizeof(payload)];
    memcpy(raw_payload, &payload, sizeof(payload));

    struct can_frame frame;
    dc_can::set_data(frame, value_type, {raw_payload[3], raw_payload[2], raw_payload[1], raw_payload[0]});
    dc_can::set_header(frame, monitor_id, device_src);

    return dispatch_frame(frame, id);
}

void DcCanBroker::write_to_can(const struct can_frame& frame) {
    write(can_fd, &frame, sizeof(frame));
}

void DcCanBroker::handle_can_input(const struct can_frame& frame) {
    if (((frame.can_id >> dc_can::defs::MESSAGE_HEADER_BIT_SHIFT) & dc_can::defs::MESSAGE_HEADER_MASK) !=
        dc_can::defs::MESSAGE_HEADER) {
        return;
    }

    std::unique_lock<std::mutex> request_lock(request.mutex);
    if ((request.state != DcCanRequest::State::ISSUED) or (request.msg_type != dc_can::parse_msg_type(frame))) {
        return;
    }

    if (dc_can::is_error_flag_set(frame)) {
        request.state = DcCanRequest::State::FAILED;
    } else {
        // this is ugly
        for (auto i = 0; i < request.response.size(); ++i) {
            request.response[i] = frame.data[7 - i];
        }
        request.state = DcCanRequest::State::COMPLETED;
    }

    request_lock.unlock();
    request.cv.notify_one();
}

int main(int argc, char* argv[]) {
    struct can_frame frame;

    DcCanBroker broker("can0");
    float result;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    broker.enable();

    // voltage 300 - 1000
    // current 0 - 2

    // while (1) {
    auto success = broker.set_data(dc_can::defs::SetValueType::VOLTAGE, 1000);
    printf("Voltage setting success: %d\n", success);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    success = broker.set_data(dc_can::defs::SetValueType::CURRENT_LIMIT, 0);
    printf("Current setting success: %d\n", success);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }

    broker.read_data(dc_can::defs::ReadValueType::VOLTAGE_LIMIT, result);
    printf("Upper limit point voltage: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::CURRENT_LIMIT, result);
    printf("Current limit: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::VOLTAGE, result);
    printf("Default Voltage: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::ENV_TEMPERATURE, result);
    printf("Env temp: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::CURRENT, result);
    printf("Current: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::VOLTAGE, result);
    printf("Voltage: %f\n", result);

    broker.read_data(dc_can::defs::ReadValueType::AC_VOLTAGE_PHASE_A, result);
    printf("Voltage PH1: %f\n", result);
    broker.read_data(dc_can::defs::ReadValueType::AC_VOLTAGE_PHASE_B, result);
    printf("Voltage PH2: %f\n", result);
    broker.read_data(dc_can::defs::ReadValueType::AC_VOLTAGE_PHASE_C, result);
    printf("Voltage PH3: %f\n", result);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;

    // dc_can::power_on(frame, false, false);
    dc_can::request_data(frame, dc_can::defs::ReadValueType::CURRENT_LIMIT);
    // dc_can::set_data(frame, dc_can::defs::SetValueType::CURRENT_LIMIT, {0x23, 0x24});
    dc_can::set_header(frame, 0xf0, 0b00000100);

    printf("frame is %08lX#", frame.can_id);
    for (auto i = 0; i < sizeof(frame.data); ++i) {
        printf("%02X", frame.data[i]);
    }
    printf("\n");
    printf("frame length: %d\n", frame.can_dlc);

    float foo = 5.0;
    uint32_t bar;
    memcpy(&bar, &foo, sizeof(foo));
    printf("float repr is %08lX\n", (unsigned long)bar);
    // printf("Answer is %d\n", dc_can::dumb_function());

    return 0;
}

//  can0  07078023   [8]  01 F0 10 00 00 00 00 00
//  0b1110000 | 0 | 11110000 | 00000100 | 011

//  0607FF83
//  0b1100000 | 0 | 11111111 | 11110000 | 011

// request: 0b1000 | 01100000 | 1 | 00000100 | 11110000 | 011

//  060F8023   [8]  C1 F2 03 00 00 00 00 00 -> error bit, response request,
// 0b1100000 | 1 | 11110000 | 00000100 | 011
//  can0  07078023   [8]  02 F0 01 00 00 00 00 00
