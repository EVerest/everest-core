#include "can_broker.hpp"

#include <cstring>
#include <stdexcept>

#include <net/if.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace dpm1000 = can::protocol::dpm1000;

// FIXME (aw): this helper doesn't really belong here
static void throw_with_error(const std::string& msg) {
    throw std::runtime_error(msg + ": (" + std::string(strerror(errno)) + ")");
}

CanBroker::CanBroker(const std::string& interface_name, uint8_t _device_src) : device_src(_device_src) {
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

    loop_thread = std::thread(&CanBroker::loop, this);
}

CanBroker::~CanBroker() {
    uint64_t quit_value = 1;
    write(event_fd, &quit_value, sizeof(quit_value));

    loop_thread.join();

    close(can_fd);
    close(event_fd);
}

void CanBroker::loop() {
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

void CanBroker::set_state(bool enabled) {
    struct can_frame frame;
    dpm1000::power_on(frame, enabled, enabled);
    dpm1000::set_header(frame, monitor_id, device_src);

    write_to_can(frame);

    // Do an extra module ON command as sometimes the bits in the header are not enough to actually switch on
    set_data_int(dpm1000::def::SetValueType::SWITCH_ON_OFF_SETTING, (enabled ? 0 : 1));
}

CanBroker::AccessReturnType CanBroker::dispatch_frame(const struct can_frame& frame, uint16_t id,
                                                      uint32_t* return_payload) {
    // wait until we get access
    std::lock_guard<std::mutex> access_lock(access_mtx);

    std::unique_lock<std::mutex> request_lock(request.mutex);
    write_to_can(frame);
    request.msg_type = id;
    request.state = CanRequest::State::ISSUED;

    const auto finished = request.cv.wait_for(request_lock, ACCESS_TIMEOUT,
                                              [this]() { return request.state != CanRequest::State::ISSUED; });

    if (not finished) {
        return AccessReturnType::TIMEOUT;
    }

    if (request.state == CanRequest::State::FAILED) {
        return AccessReturnType::FAILED;
    }

    // success
    if (return_payload) {
        memcpy(return_payload, request.response.data(), sizeof(std::remove_pointer_t<decltype(return_payload)>));
    }

    return AccessReturnType::SUCCESS;
}

CanBroker::AccessReturnType CanBroker::read_data(dpm1000::def::ReadValueType value_type, float& result) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    struct can_frame frame;
    dpm1000::request_data(frame, value_type);
    dpm1000::set_header(frame, monitor_id, device_src);

    uint32_t tmp;
    const auto status = dispatch_frame(frame, id, &tmp);

    if (status == AccessReturnType::SUCCESS) {
        memcpy(&result, &tmp, sizeof(result));
    }

    return status;
}

CanBroker::AccessReturnType CanBroker::read_data_int(dpm1000::def::ReadValueType value_type, uint32_t& result) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    struct can_frame frame;
    dpm1000::request_data(frame, value_type);
    dpm1000::set_header(frame, monitor_id, device_src);

    uint32_t tmp;
    const auto status = dispatch_frame(frame, id, &tmp);

    if (status == AccessReturnType::SUCCESS) {
        result = tmp;
    }

    return status;
}

CanBroker::AccessReturnType CanBroker::set_data(dpm1000::def::SetValueType value_type, float payload) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    uint8_t raw_payload[sizeof(payload)];
    memcpy(raw_payload, &payload, sizeof(payload));

    struct can_frame frame;
    dpm1000::set_data(frame, value_type, {raw_payload[3], raw_payload[2], raw_payload[1], raw_payload[0]});
    dpm1000::set_header(frame, monitor_id, device_src);

    return dispatch_frame(frame, id);
}

CanBroker::AccessReturnType CanBroker::set_data_int(dpm1000::def::SetValueType value_type, uint32_t payload) {
    const auto id = static_cast<std::underlying_type_t<decltype(value_type)>>(value_type);

    uint8_t raw_payload[sizeof(payload)];
    memcpy(raw_payload, &payload, sizeof(payload));

    struct can_frame frame;
    dpm1000::set_data(frame, value_type, {raw_payload[3], raw_payload[2], raw_payload[1], raw_payload[0]});
    dpm1000::set_header(frame, monitor_id, device_src);

    return dispatch_frame(frame, id);
}

void CanBroker::write_to_can(const struct can_frame& frame) {
    write(can_fd, &frame, sizeof(frame));
}

void CanBroker::handle_can_input(const struct can_frame& frame) {
    if (((frame.can_id >> dpm1000::def::MESSAGE_HEADER_BIT_SHIFT) & dpm1000::def::MESSAGE_HEADER_MASK) !=
        dpm1000::def::MESSAGE_HEADER) {
        return;
    }

    std::unique_lock<std::mutex> request_lock(request.mutex);
    if ((request.state != CanRequest::State::ISSUED) or (request.msg_type != dpm1000::parse_msg_type(frame))) {
        return;
    }

    if (dpm1000::is_error_flag_set(frame)) {
        request.state = CanRequest::State::FAILED;
    } else {
        // this is ugly
        for (auto i = 0; i < request.response.size(); ++i) {
            request.response[i] = frame.data[7 - i];
        }
        request.state = CanRequest::State::COMPLETED;
    }

    request_lock.unlock();
    request.cv.notify_one();
}
