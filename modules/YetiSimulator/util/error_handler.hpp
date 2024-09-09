// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/interfaces/ac_rcd/Implementation.hpp"
#include "generated/interfaces/connector_lock/Implementation.hpp"
#include "generated/interfaces/evse_board_support/Implementation.hpp"

namespace module {

class ErrorHandler {
public:
    ErrorHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                 connector_lockImplBase* p_connector_lock);

    void error_DiodeFault(bool raise);
    void error_BrownOut(bool raise);
    void error_EnergyManagement(bool raise);
    void error_PermanentFault(bool raise);
    void error_MREC2GroundFailure(bool raise);
    void error_MREC3HighTemperature(bool raise);
    void error_MREC4OverCurrentFailure(bool raise);
    void error_MREC5OverVoltage(bool raise);
    void error_MREC6UnderVoltage(bool raise);
    void error_MREC8EmergencyStop(bool raise);
    void error_MREC10InvalidVehicleMode(bool raise);
    void error_MREC14PilotFault(bool raise);
    void error_MREC15PowerLoss(bool raise);
    void error_MREC17EVSEContactorFault(bool raise);
    void error_MREC18CableOverTempDerate(bool raise);
    void error_MREC19CableOverTempStop(bool raise);
    void error_MREC20PartialInsertion(bool raise);
    void error_MREC23ProximityFault(bool raise);
    void error_MREC24ConnectorVoltageHigh(bool raise);
    void error_MREC25BrokenLatch(bool raise);
    void error_MREC26CutCable(bool raise);
    void error_ac_rcd_MREC2GroundFailure(bool raise);
    void error_ac_rcd_VendorError(bool raise);
    void error_ac_rcd_Selftest(bool raise);
    void error_ac_rcd_AC(bool raise);
    void error_ac_rcd_DC(bool raise);
    void error_lock_ConnectorLockCapNotCharged(bool raise);
    void error_lock_ConnectorLockUnexpectedOpen(bool raise);
    void error_lock_ConnectorLockUnexpectedClose(bool raise);
    void error_lock_ConnectorLockFailedLock(bool raise);
    void error_lock_ConnectorLockFailedUnlock(bool raise);
    void error_lock_MREC1ConnectorLockFailure(bool raise);
    void error_lock_VendorError(bool raise);

private:
    evse_board_supportImplBase* p_board_support;
    ac_rcdImplBase* p_rcd;
    connector_lockImplBase* p_connector_lock;
};

} // namespace module
