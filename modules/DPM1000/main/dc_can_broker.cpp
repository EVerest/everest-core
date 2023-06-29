#include "dc_can_broker.hpp"

#include <cstring>
#include <stdexcept>

#include <net/if.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

// FIXME (aw): this helper doesn't really belong here
static void throw_with_error(const std::string& msg) {
    throw std::runtime_error(msg + ": (" + std::string(strerror(errno)) + ")");
}

DcCanBroker::DcCanBroker(const std::string& interface_name, uint8_t _device_src) : device_src(_device_src) {
    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (can_fd == -1) {
        throw_with_error("Failed to open socket");
    }

    // retrieve interface index from interface name
    struct ifreq ifr;

    if (interface_name.size() >= sizeof(ifr.ifr_name)) {
        throw_with_error("Interface name too long: " + interface_name);
    } else {
        strcpy(ifr.ifr_name, interface_name.c_str());
    }

    if (ioctl(can_fd, SIOCGIFINDEX, &ifr) == -1) {
        throw_with_error("Failed with ioctl/SIOCGIFINDEX on interface " + interface_name);
    }

    // bind to the interface
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        throw_with_error("Failed with bind");
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
            handle_can_input(frame);
        }

        if (pollfds[1].revents & POLLIN) {
            uint64_t tmp;
            read(event_fd, &tmp, sizeof(tmp));
            // new event, for now, we do not care, later on we could check, if it is an exit event code
            return;
        }
    }
}

void DcCanBroker::set_state(bool enabled) {
    struct can_frame frame;
    dc_can::power_on(frame, enabled, enabled);
    dc_can::set_header(frame, monitor_id, device_src);

    write_to_can(frame);

    // Do an extra module ON command as sometimes the bits in the header are not enough to actually switch on
    auto status = set_data_int(dc_can::defs::SetValueType::SWITCH_ON_OFF_SETTING, (enabled ? 0 : 1));
}

DcCanBroker::AccessReturnType DcCanBroker::dispatch_frame(const struct can_frame& frame, uint16_t id,
                                                          uint32_t* return_payload) {
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

DcCanBroker::AccessReturnType DcCanBroker::read_data_int(dc_can::defs::ReadValueType value_type, uint32_t& result) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    struct can_frame frame;
    dc_can::request_data(frame, value_type);
    dc_can::set_header(frame, monitor_id, device_src);

    uint32_t tmp;
    const auto status = dispatch_frame(frame, id, &tmp);

    if (status == AccessReturnType::SUCCESS) {
        result = tmp;
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

DcCanBroker::AccessReturnType DcCanBroker::set_data_int(dc_can::defs::SetValueType value_type, uint32_t payload) {
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

#if 0

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
    printf("float repr is %08lX\n", bar);
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

#endif
