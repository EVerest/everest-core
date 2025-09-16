// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <string>

namespace everest::lib::io::mqtt {

/**
 *  @enum Qos
 *  These values are hardcoded according to the MQTT specification
 *  the underlying mqtt-c library doesn't expose these as constants
 */
enum class Qos {
    AtMostOnce = 0,
    AtLeastOnce = 1,
    ExactlyOnce = 2
};

/**
 * @struct Dataset
 * Dataset for mqtt client mqtt::mqtt_handler
 */
struct Dataset {
    /** Topic */
    std::string topic;
    /** message */
    std::string message;
    /** QOS */
    Qos qos{Qos::AtMostOnce};
};

} // namespace everest::lib::io::mqtt
