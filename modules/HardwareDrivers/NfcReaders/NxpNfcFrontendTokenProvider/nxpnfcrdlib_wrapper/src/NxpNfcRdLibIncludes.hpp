// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef SRC_NXPNFCRDLIBINCLUDES_HPP_
#define SRC_NXPNFCRDLIBINCLUDES_HPP_

#include <string>

extern "C" {
#include <phDriver.h>
#include <phDriver_Linux_Int.h>
#include <phNfcLib.h>
#include <ph_Status.h>

#include <BoardSelection.h>
}

// undo definition from NxpNfcRdLib which defines bool as unsigned char
#ifdef bool
#undef bool
#endif

#define HI_LEVEL_LOOP_DELAY_MS 200

struct ErrorInfo {
    std::string component;
    std::string type;
};

ErrorInfo decodeErrorCode(phStatus_t);

#endif // SRC_NXPNFCRDLIBINCLUDES_HPP_
