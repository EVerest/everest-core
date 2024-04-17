// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVSE_BOARD_SUPPORTINTFSTUB_H_
#define EVSE_BOARD_SUPPORTINTFSTUB_H_

#include "ModuleAdapterStub.hpp"
#include <generated/interfaces/evse_board_support/Interface.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

struct evse_board_supportIntfStub : public evse_board_supportIntf {
    ModuleAdapterStub module_adapter;
    Requirement req{"requirement", 1};

    evse_board_supportIntfStub() : evse_board_supportIntf(&module_adapter, req, "EvseManager") {
    }
    evse_board_supportIntfStub(ModuleAdapterStub& adapter) : evse_board_supportIntf(&adapter, req, "EvseManager") {
    }
};

} // namespace module::stub

#endif
