// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <vector>

namespace everest::lib::io::can {

/**
 * @var can_payload
 * @brief Payload of a CAN message
 */
using can_payload = std::vector<uint8_t>;

/**
 * @struct can_dataset
 * Dataset for socket_can. Includes CAN ID, optional DLC8, and the actual message / payload
 */
class can_dataset {
public:
    /**
     * @brief Create a dataset from CAN id, DLC8 and the message
     * @details Creates a dataset with 11bit SSF (standard frame format) can_id. Any flags
     * implicitly given by the can_id are discarded.
     * @param[in] can_id_ CAN id
     * @param[in] len8_dlc_ optional for payloads of size 8 but higher dlc
     * @param[in] payload_ message to be transmitted
     */
    can_dataset(uint32_t can_id_, uint8_t len8_dlc_, can_payload const& payload_);

    can_dataset() = default;

    /**
     * @brief Get the can_id without any implicit flags
     * @details Get the can_id with EFF, ERR, RTR flags cleared. This takes the objects EFF
     * flag into account and strips any extra bits.
     * @return The plain id.
     */
    uint32_t get_can_id() const;
    /**
     * @brief Get the can_id with with all implicit flags included
     * @details Get the can_id including EFF, ERR, RTR flags. This takes the objects EFF
     * flag into account and strips any extra bits.
     * @return The plain id.
     */
    uint32_t get_can_id_with_flags() const;

    /**
     * @brief Set the can_id without any implicit flags.
     * @details This take the objects EFF flag into account and strips data accordingly.
     * @param[in] id The can_id
     */
    void set_can_id(uint32_t id);
    /**
     * @brief Set the can_id including all implicit flags.
     * @details This take the implicit EFF flag into account and strips data accordingly.
     * Implicit EFF, RTR and ERR flags are extracted and set acoordingly.
     * @param[in] id The can_id
     */
    void set_can_id_with_flags(uint32_t id);
    /**
     * @brief Set can_id and explicit flags
     * @details This sets the can_id and the EFF, RTR and ERR flags explicitly. Implicitly flags
     * from the can_id are discarded and the id is stripped according to the EFF flag given.
     * @param[in] id Description
     * @param[in] eff Description
     * @param[in] rtr Description
     * @param[in] err Description
     */
    void set_can_id_with_flags(uint32_t id, bool eff, bool rtr, bool err);

    /**
     * @brief Status of the extended frame format (EFF) flag;
     * @return EFF flag;
     */
    bool eff_flag() const;
    /**
     * @brief Status of the remote transmission request (RTR) flag;
     * @return RTR flag;
     */
    bool rtr_flag() const;
    /**
     * @brief Status of the error (ERR) flag.
     * @return The ERR flag;
     */
    bool err_flag() const;

    /**
     * @brief payload of up to 8 bytes
     */
    can_payload payload{};
    /**
     * @brief optional DLC for payloads of size 8
     */
    uint8_t len8_dlc{0};

private:
    /**
     * @brief CAN ID
     */
    uint32_t can_id{0};
    /**
     * @brief If 'true' use EFF (extended frame format). SFF (standard frame format otherwise)
     */
    bool eff{false};
    /**
     * @brief Remote transmission flag (RTR flag) is set.
     */
    bool rtr{false};
    /**
     * @brief Error flag (ERR) is set.
     */
    bool err{false};
};
} // namespace everest::lib::io::can
