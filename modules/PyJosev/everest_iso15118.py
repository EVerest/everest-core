Setup = None
ChargerWrapper = None

def init_Setup(new_setup):
    global Setup
    Setup = new_setup

def p_Charger():
    return Setup.p_charger

class Charger_Wrapper(object):

    def set_EVSEID(self, EVSEID: str):
        self.EVSEID = EVSEID
    def get_EVSEID(self) -> str:
        return self.EVSEID

    def set_PaymentOptions(self, PaymentOptions: list):
        self.PaymentOptions = PaymentOptions
    def get_PaymentOptions(self) -> list:
        return self.PaymentOptions

    def set_SupportedEnergyTransferMode(self, SupportedEnergyTransferMode: list):
        self.SupportedEnergyTransferMode = SupportedEnergyTransferMode
    def get_SupportedEnergyTransferMode(self) -> list:
        return self.SupportedEnergyTransferMode

    def set_AC_EVSENominalVoltage(self, EVSENominalVoltage: float):
        self.EVSENominalVoltage = EVSENominalVoltage
    def get_AC_EVSENominalVoltage(self) -> float:
        return self.EVSENominalVoltage

    def set_DC_EVSECurrentRegulationTolerance(self, EVSECurrentRegulationTolerance: float):
        self.EVSECurrentRegulationTolerance = EVSECurrentRegulationTolerance
    def get_DC_EVSECurrentRegulationTolerance(self) -> float:
        return self.EVSECurrentRegulationTolerance

    def set_DC_EVSEPeakCurrentRipple(self, EVSEPeakCurrentRipple: float):
        self.EVSEPeakCurrentRipple = EVSEPeakCurrentRipple
    def get_DC_EVSEPeakCurrentRipple(self) -> float:
        return self.EVSEPeakCurrentRipple

    def set_ReceiptRequired(self, ReceiptRequired: bool):
        self.ReceiptRequired = ReceiptRequired
    def get_ReceiptRequired(self) -> bool:
        return self.ReceiptRequired

    def set_FreeService(self, FreeService: bool):
        self.FreeService = FreeService
    def get_FreeService(self) -> bool:
        return self.FreeService
    
    def set_EVSEEnergyToBeDelivered(self, EVSEEnergyToBeDelivered: float):
        self.EVSEEnergyToBeDelivered = EVSEEnergyToBeDelivered
    def get_EVSEEnergyToBeDelivered(self) -> float:
        return self.EVSEEnergyToBeDelivered

    def enable_debug_mode(self, debug_mode: str):
        self.debug_mode = debug_mode
    def get_debug_mode(self) -> str:
        return self.debug_mode

    def set_Auth_Okay_EIM(self, auth_okay_eim: bool):
        self.auth_okay_eim = auth_okay_eim
    def get_Auth_Okay_EIM(self) -> bool:
        return self.auth_okay_eim

    def set_Auth_Okay_PnC(self, auth_okay_pnc: bool):
        self.auth_okay_pnc = auth_okay_pnc
    def get_Auth_Okay_PnC(self) -> bool:
        return self.auth_okay_pnc

    def set_FAILED_ContactorError(self, ContactorError: bool):
        self.ContactorError = ContactorError
    def get_FAILED_ContactorError(self) -> bool:
        return self.ContactorError
    
    def set_RCD_Error(self, RCD: bool):
        self.RCD = RCD
    def get_RCD_Error(self) -> bool:
        return self.RCD

    def stop_charging(self, stop_charging: bool):
        self.stop_charging = stop_charging
    def get_stop_charging(self) -> bool:
        return self.stop_charging

    def set_DC_EVSEPresentVoltage(self, EVSEPresentVoltage: float):
        self.EVSEPresentVoltage = EVSEPresentVoltage
    def get_DC_EVSEPresentVoltage(self) -> float:
        return self.EVSEPresentVoltage

    def set_DC_EVSEPresentCurrent(self, EVSEPresentCurrent: float):
        self.EVSEPresentCurrent = EVSEPresentCurrent
    def get_DC_EVSEPresentCurrent(self) -> float:
        return self.EVSEPresentCurrent
    
    def set_AC_EVSEMaxCurrent(self, EVSEMaxCurrent: float):
        self.EVSEMaxCurrent = EVSEMaxCurrent
    def get_AC_EVSEMaxCurrent(self) -> float:
        return self.EVSEMaxCurrent

    def set_DC_EVSEMaximumCurrentLimit(self, EVSEMaximumCurrentLimit: float):
        self.EVSEMaximumCurrentLimit = EVSEMaximumCurrentLimit
    def get_DC_EVSEMaximumCurrentLimit(self) -> float:
        return self.EVSEMaximumCurrentLimit
    
    def set_DC_EVSEMaximumPowerLimit(self, EVSEMaximumPowerLimit: float):
        self.EVSEMaximumPowerLimit = EVSEMaximumPowerLimit
    def get_DC_EVSEMaximumPowerLimit(self) -> float:
        return self.EVSEMaximumPowerLimit
        
    def set_DC_EVSEMaximumVoltageLimit(self, EVSEMaximumVoltageLimit: float):
        self.EVSEMaximumVoltageLimit = EVSEMaximumVoltageLimit
    def get_DC_EVSEMaximumVoltageLimit(self) -> float:
        return self.EVSEMaximumVoltageLimit
        
    def set_DC_EVSEMinimumCurrentLimit(self, EVSEMinimumCurrentLimit: float):
        self.EVSEMinimumCurrentLimit = EVSEMinimumCurrentLimit
    def get_DC_EVSEMinimumCurrentLimit(self) -> float:
        return self.EVSEMinimumCurrentLimit
        
    def set_DC_EVSEMinimumVoltageLimit(self, EVSEMinimumVoltageLimit: float):
        self.EVSEMinimumVoltageLimit = EVSEMinimumVoltageLimit
    def get_DC_EVSEMinimumVoltageLimit(self) -> float:
        return self.EVSEMinimumVoltageLimit

    def set_EVSEIsolationStatus(self, EVSEIsolationStatus: str):
        self.EVSEIsolationStatus = EVSEIsolationStatus
    def get_EVSEIsolationStatus(self) -> str:
        return self.EVSEIsolationStatus
    
    def set_EVSE_UtilityInterruptEvent(self, EVSE_UtilityInterruptEvent: bool):
        self.EVSE_UtilityInterruptEvent = EVSE_UtilityInterruptEvent
    def get_EVSE_UtilityInterruptEvent(self) -> bool:
        return self.EVSE_UtilityInterruptEvent
    
    def set_EVSE_Malfunction(self, EVSE_Malfunction: bool):
        self.EVSE_Malfunction = EVSE_Malfunction
    def get_EVSE_Malfunction(self) -> bool:
        return self.EVSE_Malfunction
    
    def set_EVSE_EmergencyShutdown(self, EVSE_EmergencyShutdown: bool):
        self.EVSE_EmergencyShutdown = EVSE_EmergencyShutdown
    def get_EVSE_EmergencyShutdown(self) -> bool:
        return self.EVSE_EmergencyShutdown
    
    def set_MeterInfo(self, powermeter: dict):
        self.powermeter = powermeter
    def get_MeterInfo(self) -> dict:
        return self.powermeter
    
    def contactor_closed(self, status: bool):
        if (status == True):
            self.contactorClosed = True
            self.contactorOpen = False
    def contactor_open(self, status: bool):
        if (status == True):
            self.contactorClosed = False
            self.contactorOpen = True
    def get_contactor_status(self) -> list:
        return [self.contactorClosed, self.contactorClosed]

ChargerWrapper = Charger_Wrapper()