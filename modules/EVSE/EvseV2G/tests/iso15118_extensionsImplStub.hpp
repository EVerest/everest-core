// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ISO15118_EXTENTSIONSIMPLSTUB_H
#define ISO15118_EXTENTSIONSIMPLSTUB_H

#include <iostream>

#include <generated/interfaces/iso15118_extensions/Implementation.hpp>

//-----------------------------------------------------------------------------
namespace module::stub {

class iso15118_extensionsImplStub : public iso15118_extensionsImplBase {
public:
    iso15118_extensionsImplStub() : iso15118_extensionsImplBase(nullptr, "EvseV2G"){};

    virtual void init() {
    }
    virtual void ready() {
    }

    virtual void handle_set_get_certificate_response(types::iso15118::ResponseExiStreamStatus& certificate_response) {
        std::cout << "iso15118_extensionsImplBase::handle_set_get_certificate_response called" << std::endl;
    }

    virtual void handle_set_notify_ev_schedule_status(types::iso15118::NotifyEvScheduleStatus& status) {
        std::cout << "iso15118_extensionsImplBase::handle_set_notify_ev_schedule_status called" << std::endl;
    }

    virtual types::iso15118::SetChargingSchedulesResult
    handle_set_ev_charging_schedules(types::iso15118::OcppEvChargingSchedules& charging_schedules) {
        std::cout << "iso15118_extensionsImplBase::handle_set_ev_charging_schedules called" << std::endl;
        return {types::iso15118::SetChargingSchedulesStatus::Rejected, std::nullopt};
    }
};

} // namespace module::stub

#endif // ISO15118_EXTENTSIONSIMPLSTUB_H
