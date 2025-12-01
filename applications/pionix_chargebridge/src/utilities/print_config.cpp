// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "protocol/cb_config.h"
#include <charge_bridge/charge_bridge.hpp>
#include <sstream>

namespace charge_bridge::utilities {

std::string to_string(CbCanBaudrate value) {
    switch (value) {
    case CBCBR_125000:
        return "125000";
    case CBCBR_250000:
        return "250000";
    case CBCBR_500000:
        return "500000";
    case CBCBR_1000000:
        return "1000000";
    default:
        break;
    }
    return "Invalid bitrate";
}

std::string to_string(CbUartBaudrate value) {
    switch (value) {
    case CBUBR_9600:
        return "9600";
    case CBUBR_19200:
        return "19200";
    case CBUBR_38400:
        return "38400";
    case CBUBR_57600:
        return "57600";
    case CBUBR_115200:
        return "115200";
    case CBUBR_230400:
        return "230400";
    case CBUBR_250000:
        return "250000";
    case CBUBR_460800:
        return "460800";
    case CBUBR_500000:
        return "500000";
    case CBUBR_1000000:
        return "1000000";
    case CBUBR_2000000:
        return "2000000";
    case CBUBR_3000000:
        return "3000000";
    case CBUBR_4000000:
        return "4000000";
    case CBUBR_6000000:
        return "6000000";
    case CBUBR_8000000:
        return "8000000";
    case CBUBR_10000000:
        return "10000000";
    default:
        break;
    }
    return "Invalid baudrate";
}

std::string to_string(CbUartParity value) {
    switch (value) {
    case CBUP_None:
        return "N";
    case CBUP_Odd:
        return "O";
    case CBUP_Even:
        return "E";
    default:
        break;
    }
    return "Invalid parity";
}

std::string to_string(CbUartStopbits value) {
    switch (value) {
    case CBUS_OneStopBit:
        return "1";
    case CBUS_TwoStopBits:
        return "2";
    default:
        break;
    }
    return "Invalid parity";
}

std::string to_string(CbUartConfig const& value) {
    std::stringstream data;
    data << to_string(value.baudrate) << " 8" << to_string(value.parity) << to_string(value.stopbits);
    return data.str();
}

} // namespace charge_bridge::utilities
