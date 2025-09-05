// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

#include "NxpNfcRdLibIncludes.hpp"

namespace {
const std::unordered_map<phStatus_t, std::string> componentMapping = {{PH_COMP_BAL, "BAL"},
                                                                      {PH_COMP_HAL, "HAL"},
                                                                      {PH_COMP_PAL_ISO14443P3A, "PAL_ISO14443P3A"},
                                                                      {PH_COMP_PAL_ISO14443P3B, "PAL_ISO14443P3B"},
                                                                      {PH_COMP_PAL_ISO14443P4A, "PAL_ISO14443P4A"},
                                                                      {PH_COMP_PAL_ISO14443P4, "PAL_ISO14443P4"},
                                                                      {PH_COMP_PAL_FELICA, "PAL_FELICA"},
                                                                      {PH_COMP_PAL_EPCUID, "PAL_EPCUID"},
                                                                      {PH_COMP_PAL_SLI15693, "PAL_SLI15693"},
                                                                      {PH_COMP_PAL_I18000P3M3, "PAL_I18000P3M3"},
                                                                      {PH_COMP_PAL_I18092MPI, "PAL_I18092MPI"},
                                                                      {PH_COMP_PAL_I18092MT, "PAL_I18092MT"},
                                                                      {PH_COMP_PAL_I14443P4MC, "PAL_I14443P4MC"},
                                                                      {PH_COMP_AC_DISCLOOP, "AC_DISCLOOP"},
                                                                      {PH_COMP_OSAL, "OSAL"},
                                                                      {PH_COMP_DRIVER, "DRIVER"}};

static const std::unordered_map<phStatus_t, std::string> phOsalErrorTypeMapping = {
    {PH_OSAL_IO_TIMEOUT, "IO_TIMEOUT"},
    {PH_OSAL_ERROR, "ERROR"},
    {PH_OSAL_FAILURE, "FAILURE"},
    {PH_OSAL_UNSUPPORTED_COMMAND, "UNSUPPORTED_COMMAND"}};

static const std::unordered_map<phStatus_t, std::string> balErrorTypeMapping = {{PH_DRIVER_TIMEOUT, "TIMEOUT"},
                                                                                {PH_DRIVER_ABORTED, "ABORTED"},
                                                                                {PH_DRIVER_ERROR, "ERROR"},
                                                                                {PH_DRIVER_FAILURE, "FAILURE"}};

static const std::unordered_map<phStatus_t, std::string> errorTypeMapping = {
    {PH_ERR_IO_TIMEOUT, "IO_TIMEOUT"},
    {PH_ERR_INTEGRITY_ERROR, "INTEGRITY_ERROR"},
    {PH_ERR_COLLISION_ERROR, "COLLISION_ERROR"},
    {PH_ERR_BUFFER_OVERFLOW, "BUFFER_OVERFLOW"},
    {PH_ERR_FRAMING_ERROR, "FRAMING_ERROR"},
    {PH_ERR_PROTOCOL_ERROR, "PROTOCOL_ERROR"},
    {PH_ERR_AUTH_ERROR, "AUTH_ERROR"},
    {PH_ERR_READ_WRITE_ERROR, "READ_WRITE_ERROR"},
    {PH_ERR_TEMPERATURE_ERROR, "TEMPERATURE_ERROR"},
    {PH_ERR_RF_ERROR, "RF_ERROR"},
    {PH_ERR_INTERFACE_ERROR, "INTERFACE_ERROR"},
    {PH_ERR_LENGTH_ERROR, "LENGTH_ERROR"},
    {PH_ERR_RESOURCE_ERROR, "RESOURCE_ERROR"},
    {PH_ERR_TX_NAK_ERROR, "TX_NAK_ERROR"},
    {PH_ERR_RX_NAK_ERROR, "RX_NAK_ERROR"},
    {PH_ERR_EXT_RF_ERROR, "EXT_RF_ERROR"},
    {PH_ERR_NOISE_ERROR, "NOISE_ERROR"},
    {PH_ERR_ABORTED, "ABORTED"},
    {PH_ERR_LPCD_ABORTED, "LPCD_ABORTED"},

    {PH_ERR_INTERNAL_ERROR, "INTERNAL_ERROR"},
    {PH_ERR_INVALID_DATA_PARAMS, "INVALID_DATA_PARAMS"},
    {PH_ERR_INVALID_PARAMETER, "INVALID_PARAMETER"},
    {PH_ERR_PARAMETER_OVERFLOW, "PARAMETER_OVERFLOW"},
    {PH_ERR_UNSUPPORTED_PARAMETER, "UNSUPPORTED_PARAMETER"},
    {PH_ERR_UNSUPPORTED_COMMAND, "UNSUPPORTED_COMMAND"},
    {PH_ERR_USE_CONDITION, "USE_CONDITION"},
    {PH_ERR_KEY, "KEY"},
    {PH_ERR_PARAMETER_SIZE, "PARAMETER_SIZE"},
    {PH_ERR_UNKNOWN, "UNKNOWN"},

    {PH_ERR_INTERNAL_ERROR, "INTERNAL_ERROR"},
    {PH_ERR_AUTH_DELAY, "AUTH_DELAY"}};
} // namespace

ErrorInfo decodeErrorCode(phStatus_t wStatus) {
    phStatus_t componentCode = wStatus & PH_COMP_MASK;
    phStatus_t errorTypeCode = wStatus & PH_ERR_MASK;

    std::string component;
    std::string type;

    try {
        component = componentMapping.at(componentCode);
    } catch (const std::out_of_range& e) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << "0x" << std::uppercase << std::hex << componentCode;
        component = oss.str();
    }

    try {
        if (componentCode == PH_COMP_OSAL) {
            type = phOsalErrorTypeMapping.at(errorTypeCode);
        } else if (componentCode == PH_COMP_DRIVER) {
            type = balErrorTypeMapping.at(errorTypeCode);
        } else {
            type = errorTypeMapping.at(errorTypeCode);
        }
    } catch (const std::out_of_range& e) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << "0x" << std::uppercase << std::hex << errorTypeCode;
        type = oss.str();
    }

    return {component, type};
}
