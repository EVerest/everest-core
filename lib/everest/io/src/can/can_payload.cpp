// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/can/can_payload.hpp>
#include <linux/can.h>

namespace everest::lib::io::can {

can_dataset::can_dataset(uint32_t can_id_, uint8_t len8_dlc_, can_payload const& payload_) {
    payload = payload_;
    len8_dlc = len8_dlc_;
    set_can_id(can_id_);
}

uint32_t can_dataset::get_can_id() const {
    if (eff) {
        return can_id & CAN_EFF_MASK;
    } else {
        return can_id & CAN_SFF_MASK;
    }
}

uint32_t can_dataset::get_can_id_with_flags() const {
    auto tmp = get_can_id();
    if (eff) {
        tmp |= CAN_EFF_FLAG;
    }
    if (rtr) {
        tmp |= CAN_RTR_FLAG;
    }
    if (err) {
        tmp |= CAN_ERR_FLAG;
    }
    return tmp;
}

void can_dataset::set_can_id(uint32_t id) {
    can_id = id;
}

void can_dataset::set_can_id_with_flags(uint32_t id) {
    eff = id & CAN_EFF_FLAG;
    rtr = id & CAN_RTR_FLAG;
    err = id & CAN_ERR_FLAG;
    set_can_id(id);
}

void can_dataset::set_can_id_with_flags(uint32_t id, bool eff_, bool rtr_, bool err_) {
    eff = eff_;
    rtr = rtr_;
    err = err_;
    set_can_id(id);
}

bool can_dataset::eff_flag() const {
    return eff;
}
bool can_dataset::rtr_flag() const {
    return rtr;
}
bool can_dataset::err_flag() const {
    return err;
}

} // namespace everest::lib::io::can
