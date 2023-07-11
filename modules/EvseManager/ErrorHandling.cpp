// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "ErrorHandling.hpp"

namespace module {

ErrorHandling::ErrorHandling(const std::unique_ptr<evse_board_supportIntf>& _r_bsp,
                             const std::vector<std::unique_ptr<ISO15118_chargerIntf>>& _r_hlc,
                             const std::vector<std::unique_ptr<connector_lockIntf>>& _r_connector_lock,
                             const std::vector<std::unique_ptr<ac_rcdIntf>>& _r_ac_rcd,
                             const std::unique_ptr<evse_managerImplBase>& _p_evse) :
    r_bsp(_r_bsp), r_hlc(_r_hlc), r_connector_lock(_r_connector_lock), r_ac_rcd(_r_ac_rcd), p_evse(_p_evse) {

    if (r_hlc.size() > 0) {
        hlc = true;
    }

    // Subscribe to bsp driver to receive Errors from the bsp hardware
    r_bsp->subscribe_all_errors(
        [this](const Everest::error::Error& error) {
            if (modify_error_bsp(error, true)) {
                // signal to charger a new error has been set
                signal_error();
            };
        },
        [this](const Everest::error::Error& error) {
            modify_error_bsp(error, false);

            if (active_errors.all_cleared()) {
                // signal to charger that all errors are cleared now
                signal_all_errors_cleared();
                // clear errors with HLC stack
                if (hlc) {
                    r_hlc[0]->call_reset_error();
                }
            }
        });

    // Subscribe to connector lock to receive errors from connector lock hardware
    if (r_connector_lock.size() > 0) {
        r_connector_lock[0]->subscribe_all_errors(
            [this](const Everest::error::Error& error) {
                if (modify_error_connector_lock(error, true)) {
                    // signal to charger a new error has been set
                    signal_error();
                };
            },
            [this](const Everest::error::Error& error) {
                modify_error_connector_lock(error, false);

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    signal_all_errors_cleared();
                    // clear errors with HLC stack
                    if (hlc) {
                        r_hlc[0]->call_reset_error();
                    }
                }
            });
    }

    // Subscribe to ac_rcd to receive errors from AC RCD hardware
    if (r_ac_rcd.size() > 0) {
        r_ac_rcd[0]->subscribe_all_errors(
            [this](const Everest::error::Error& error) {
                if (modify_error_ac_rcd(error, true)) {
                    // signal to charger a new error has been set
                    signal_error();
                };
            },
            [this](const Everest::error::Error& error) {
                modify_error_ac_rcd(error, false);

                if (active_errors.all_cleared()) {
                    // signal to charger that all errors are cleared now
                    signal_all_errors_cleared();
                    // clear errors with HLC stack
                    if (hlc) {
                        r_hlc[0]->call_reset_error();
                    }
                }
            });
    }
}

void ErrorHandling::raise_overcurrent_error(const std::string& description) {
    // raise externally
    p_evse->raise_evse_manager_MREC4OverCurrentFailure("Slow overcurrent detected", Everest::error::Severity::High);

    if (modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", true)) {
        // signal to charger a new error has been set
        signal_error();
    };
}

void ErrorHandling::clear_overcurrent_error() {
    // clear externally
    p_evse->request_clear_all_evse_manager_MREC4OverCurrentFailure();

    modify_error_evse_manager("evse_manager/MREC4OverCurrentFailure", false);

    if (active_errors.all_cleared()) {
        // signal to charger that all errors are cleared now
        signal_all_errors_cleared();
        // clear errors with HLC stack
        if (hlc) {
            r_hlc[0]->call_reset_error();
        }
    }
}

void ErrorHandling::raise_internal_error(const std::string& description) {
    // raise externally
    // FIXME raise_evse_manager_MREC4OverCurrentFailure(description);

    if (modify_error_evse_manager("evse_manager/Internal", true)) {
        // signal to charger a new error has been set
        signal_error();
    };
}

void ErrorHandling::clear_internal_error() {
    // clear externally
    // FIXME clear_evse_manager_MREC4OverCurrentFailure();

    modify_error_evse_manager("evse_manager/Internal", false);

    if (active_errors.all_cleared()) {
        // signal to charger that all errors are cleared now
        signal_all_errors_cleared();
        // clear errors with HLC stack
        if (hlc) {
            r_hlc[0]->call_reset_error();
        }
    }
}

bool ErrorHandling::modify_error_bsp(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "evse_board_support/DiodeFault") {
        active_errors.bsp.DiodeFault = active;
    } else if (error_type == "evse_board_support/VentilationNotAvailable") {
        active_errors.bsp.VentilationNotAvailable = active;
    } else if (error_type == "evse_board_support/BrownOut") {
        active_errors.bsp.BrownOut = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/EnergyManagement") {
        active_errors.bsp.EnergyManagement = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/PermanentFault") {
        active_errors.bsp.PermanentFault = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC2GroundFailure") {
        active_errors.bsp.MREC2GroundFailure = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC4OverCurrentFailure") {
        active_errors.bsp.MREC4OverCurrentFailure = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC5OverVoltage") {
        active_errors.bsp.MREC5OverVoltage = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC6UnderVoltage") {
        active_errors.bsp.MREC6UnderVoltage = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC8EmergencyStop") {
        active_errors.bsp.MREC8EmergencyStop = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_EmergencyShutdown);
        }
    } else if (error_type == "evse_board_support/MREC10InvalidVehicleMode") {
        active_errors.bsp.MREC10InvalidVehicleMode = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC14PilotFault") {
        active_errors.bsp.MREC14PilotFault = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC15PowerLoss") {
        active_errors.bsp.MREC15PowerLoss = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC17EVSEContactorFault") {
        active_errors.bsp.MREC17EVSEContactorFault = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Contactor);
        }
    } else if (error_type == "evse_board_support/MREC19CableOverTempStop") {
        active_errors.bsp.MREC19CableOverTempStop = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC20PartialInsertion") {
        active_errors.bsp.MREC20PartialInsertion = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC23ProximityFault") {
        active_errors.bsp.MREC23ProximityFault = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC24ConnectorVoltageHigh") {
        active_errors.bsp.MREC24ConnectorVoltageHigh = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC25BrokenLatch") {
        active_errors.bsp.MREC25BrokenLatch = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/MREC26CutCable") {
        active_errors.bsp.MREC26CutCable = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "evse_board_support/VendorError") {
        active_errors.bsp.VendorError = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        return false; // Error does not stop charging, ignored here
    }
    return true;      // Error stops charging
};

bool ErrorHandling::modify_error_connector_lock(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "connector_lock/ConnectorLockCapNotCharged") {
        active_errors.connector_lock.ConnectorLockCapNotCharged = active;
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedClose") {
        active_errors.connector_lock.ConnectorLockUnexpectedClose = active;
    } else if (error_type == "connector_lock/ConnectorLockUnexpectedOpen") {
        active_errors.connector_lock.ConnectorLockUnexpectedOpen = active;
    } else if (error_type == "connector_lock/ConnectorLockFailedLock") {
        active_errors.connector_lock.ConnectorLockFailedLock = active;
    } else if (error_type == "connector_lock/ConnectorLockFailedUnlock") {
        active_errors.connector_lock.ConnectorLockFailedUnlock = active;
    } else if (error_type == "connector_lock/MREC1ConnectorLockFailure") {
        active_errors.connector_lock.MREC1ConnectorLockFailure = active;
    } else if (error_type == "connector_lock/VendorError") {
        active_errors.connector_lock.VendorError = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else {
        return false; // Error does not stop charging, ignored here
    }
    return true;      // Error stops charging
};

bool ErrorHandling::modify_error_ac_rcd(const Everest::error::Error& error, bool active) {
    const std::string& error_type = error.type;

    if (active) {
        EVLOG_error << "Raised error " << error_type << ": " << error.description << " (" << error.message << ")";
    } else {
        EVLOG_info << "Cleared error " << error_type << ": " << error.description << " (" << error.message << ")";
    }

    if (error_type == "ac_rcd/MREC2GroundFailure") {
        active_errors.ac_rcd.MREC2GroundFailure = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/VendorError") {
        active_errors.ac_rcd.VendorError = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/Selftest") {
        active_errors.ac_rcd.Selftest = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }
    } else if (error_type == "ac_rcd/AC") {
        active_errors.ac_rcd.AC = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else if (error_type == "ac_rcd/DC") {
        active_errors.ac_rcd.DC = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_RCD);
        }
    } else {
        return false; // Error does not stop charging, ignored here
    }
    return true;      // Error stops charging
};

bool ErrorHandling::modify_error_evse_manager(const std::string& error_type, bool active) {
    if (error_type == "evse_manager/MREC4OverCurrentFailure") {
        active_errors.bsp.MREC4OverCurrentFailure = active;
        if (hlc && active) {
            r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
        }

    } else {
        return false; // Error does not stop charging, ignored here
    }
    return true;      // Error stops charging
};

} // namespace module
