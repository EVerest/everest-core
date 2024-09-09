// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "error_handler.hpp"

namespace module {

using Everest::error::Severity;

ErrorHandler::ErrorHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                           connector_lockImplBase* p_connector_lock) :
    p_board_support(p_board_support), p_rcd(p_rcd), p_connector_lock(p_connector_lock) {
}

void ErrorHandler::error_DiodeFault(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/DiodeFault", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/DiodeFault");
    }
}

void ErrorHandler::error_BrownOut(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/BrownOut", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/BrownOut");
    }
}

void ErrorHandler::error_EnergyManagement(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/EnergyManagement", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/EnergyManagement");
    }
}

void ErrorHandler::error_PermanentFault(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/PermanentFault", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/PermanentFault");
    }
}

void ErrorHandler::error_MREC2GroundFailure(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC2GroundFailure", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC2GroundFailure");
    }
}

void ErrorHandler::error_MREC3HighTemperature(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC3HighTemperature", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC3HighTemperature");
    }
}

void ErrorHandler::error_MREC4OverCurrentFailure(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC4OverCurrentFailure",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC4OverCurrentFailure");
    }
}

void ErrorHandler::error_MREC5OverVoltage(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC5OverVoltage", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC5OverVoltage");
    }
}

void ErrorHandler::error_MREC6UnderVoltage(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC6UnderVoltage", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC6UnderVoltage");
    }
}

void ErrorHandler::error_MREC8EmergencyStop(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC8EmergencyStop", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC8EmergencyStop");
    }
}

void ErrorHandler::error_MREC10InvalidVehicleMode(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC10InvalidVehicleMode",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC10InvalidVehicleMode");
    }
}

void ErrorHandler::error_MREC14PilotFault(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC14PilotFault", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC14PilotFault");
    }
}

void ErrorHandler::error_MREC15PowerLoss(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC15PowerLoss", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC15PowerLoss");
    }
}

void ErrorHandler::error_MREC17EVSEContactorFault(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC17EVSEContactorFault",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC17EVSEContactorFault");
    }
}

void ErrorHandler::error_MREC18CableOverTempDerate(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC18CableOverTempDerate",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC18CableOverTempDerate");
    }
}

void ErrorHandler::error_MREC19CableOverTempStop(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC19CableOverTempStop",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC19CableOverTempStop");
    }
}

void ErrorHandler::error_MREC20PartialInsertion(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC20PartialInsertion", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC20PartialInsertion");
    }
}

void ErrorHandler::error_MREC23ProximityFault(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC23ProximityFault", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC23ProximityFault");
    }
}

void ErrorHandler::error_MREC24ConnectorVoltageHigh(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC24ConnectorVoltageHigh",
                                                                        "", "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC24ConnectorVoltageHigh");
    }
}

void ErrorHandler::error_MREC25BrokenLatch(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC25BrokenLatch", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC25BrokenLatch");
    }
}

void ErrorHandler::error_MREC26CutCable(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("evse_board_support/MREC26CutCable", "",
                                                                        "Simulated fault event", Severity::High);
        p_board_support->raise_error(error);
    } else {
        p_board_support->clear_error("evse_board_support/MREC26CutCable");
    }
}

void ErrorHandler::error_ac_rcd_MREC2GroundFailure(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("ac_rcd/MREC2GroundFailure", "",
                                                                        "Simulated fault event", Severity::High);
        p_rcd->raise_error(error);
    } else {
        p_rcd->clear_error("ac_rcd/MREC2GroundFailure");
    }
}

void ErrorHandler::error_ac_rcd_VendorError(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("ac_rcd/VendorError", "",
                                                                        "Simulated fault event", Severity::High);
        p_rcd->raise_error(error);
    } else {
        p_rcd->clear_error("ac_rcd/VendorError");
    }
}

void ErrorHandler::error_ac_rcd_Selftest(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("ac_rcd/Selftest", "", "Simulated fault event",
                                                                        Severity::High);
        p_rcd->raise_error(error);
    } else {
        p_rcd->clear_error("ac_rcd/Selftest");
    }
}

void ErrorHandler::error_ac_rcd_AC(bool raise) {
    if (raise) {
        const auto error =
            p_board_support->error_factory->create_error("ac_rcd/AC", "", "Simulated fault event", Severity::High);
        p_rcd->raise_error(error);
    } else {
        p_rcd->clear_error("ac_rcd/AC");
    }
}

void ErrorHandler::error_ac_rcd_DC(bool raise) {
    if (raise) {
        const auto error =
            p_board_support->error_factory->create_error("ac_rcd/DC", "", "Simulated fault event", Severity::High);
        p_rcd->raise_error(error);
    } else {
        p_rcd->clear_error("ac_rcd/DC");
    }
}

void ErrorHandler::error_lock_ConnectorLockCapNotCharged(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/ConnectorLockCapNotCharged", "",
                                                                        "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/ConnectorLockCapNotCharged");
    }
}

void ErrorHandler::error_lock_ConnectorLockUnexpectedOpen(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/ConnectorLockUnexpectedOpen",
                                                                        "", "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/ConnectorLockUnexpectedOpen");
    }
}

void ErrorHandler::error_lock_ConnectorLockUnexpectedClose(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/ConnectorLockUnexpectedClose",
                                                                        "", "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/ConnectorLockUnexpectedClose");
    }
}

void ErrorHandler::error_lock_ConnectorLockFailedLock(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/ConnectorLockFailedLock", "",
                                                                        "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/ConnectorLockFailedLock");
    }
}

void ErrorHandler::error_lock_ConnectorLockFailedUnlock(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/ConnectorLockFailedUnlock", "",
                                                                        "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/ConnectorLockFailedUnlock");
    }
}

void ErrorHandler::error_lock_MREC1ConnectorLockFailure(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/MREC1ConnectorLockFailure", "",
                                                                        "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/MREC1ConnectorLockFailure");
    }
}

void ErrorHandler::error_lock_VendorError(bool raise) {
    if (raise) {
        const auto error = p_board_support->error_factory->create_error("connector_lock/VendorError", "",
                                                                        "Simulated fault event", Severity::High);
        p_connector_lock->raise_error(error);
    } else {
        p_connector_lock->clear_error("connector_lock/VendorError");
    }
}

} // namespace module