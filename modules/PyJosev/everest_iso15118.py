Setup = None
ChargerWrapper = None

INT_16_MAX = 2**15 - 1

def init_Setup(new_setup):
    global Setup
    Setup = new_setup

def p_Charger():
    return Setup.p_charger

class Charger_Wrapper(object):

    def __init__(self) -> None:
        # Common
        self.EVSEID = ""
        self.PaymentOptions = []
        self.SupportedEnergyTransferMode = []
        self.ReceiptRequired = False
        self.FreeService = False
        self.EVSEEnergyToBeDelivered = 0
        self.debug_mode = "None"
        self.stop_charging = False
        self.auth_okay_eim = False
        self.auth_okay_pnc = False
        self.EVSE_UtilityInterruptEvent = False
        self.EVSE_EmergencyShutdown = False
        self.EVSE_Malfunction = False
        self.powermeter = dict()

        # AC
        self.EVSENominalVoltage = 0
        self.ContactorError = False
        self.RCD_Error = False
        self.EVSEMaxCurrent = 0
        self.contactorClosed = False
        self.contactorOpen = True

        # DC
        self.EVSEPeakCurrentRipple = 0
        self.EVSECurrentRegulationTolerance = 0
        self.EVSEPresentVoltage = 0
        self.EVSEPresentCurrent = 0
        self.EVSEMaximumCurrentLimit = 0
        self.EVSEMaximumPowerLimit = 0
        self.EVSEMaximumVoltageLimit = 0
        self.EVSEMinimumCurrentLimit = 0
        self.EVSEMinimumVoltageLimit = 0
        self.EVSEIsolationStatus = "Invalid"
        self.cableCheck_Finished = False

    # Reset values every SessionSetup
    def reset(self) -> None:
        # Common
        self.stop_charging = False
        self.auth_okay_eim = False
        self.auth_okay_pnc = False
        self.EVSE_UtilityInterruptEvent = False
        self.EVSE_EmergencyShutdown = False
        self.EVSE_Malfunction = False

        # AC
        self.ContactorError = False
        self.contactorClosed = False
        self.contactorOpen = True
        self.RCD_Error = False

        # DC
        self.EVSEIsolationStatus = "Invalid"
        self.cableCheck_Finished = False

    # SessionSetup, CurrentDemand, ChargingStatus
    def set_EVSEID(self, EVSEID: str):
        self.EVSEID = EVSEID
    def get_EVSEID(self) -> str:
        return self.EVSEID
    
    # ServiceDiscovery, PaymentServiceSelection
    def set_PaymentOptions(self, PaymentOptions: list):
        self.PaymentOptions = PaymentOptions
    def get_PaymentOptions(self) -> list:
        return self.PaymentOptions
    
    # ServiceDisovery, ChargeParameterDiscovery
    def set_SupportedEnergyTransferMode(self, SupportedEnergyTransferMode: list):
        self.SupportedEnergyTransferMode = SupportedEnergyTransferMode
    def get_SupportedEnergyTransferMode(self) -> list:
        return self.SupportedEnergyTransferMode

    # ChargeParameterDiscovery
    def set_AC_EVSENominalVoltage(self, EVSENominalVoltage: float):
        self.EVSENominalVoltage = EVSENominalVoltage
    def get_AC_EVSENominalVoltage(self) -> float:
        return self.EVSENominalVoltage

    # ChargeParameterDiscovery
    def set_DC_EVSECurrentRegulationTolerance(self, EVSECurrentRegulationTolerance: float):
        self.EVSECurrentRegulationTolerance = EVSECurrentRegulationTolerance
    def get_DC_EVSECurrentRegulationTolerance(self) -> float:
        return self.EVSECurrentRegulationTolerance

    # ChargeParameterDisovery
    def set_DC_EVSEPeakCurrentRipple(self, EVSEPeakCurrentRipple: float):
        self.EVSEPeakCurrentRipple = EVSEPeakCurrentRipple
    def get_DC_EVSEPeakCurrentRipple(self) -> float:
        return self.EVSEPeakCurrentRipple

    # ChargingStatus and CurrentDemand
    def set_ReceiptRequired(self, ReceiptRequired: bool):
        self.ReceiptRequired = ReceiptRequired
    def get_ReceiptRequired(self) -> bool:
        return self.ReceiptRequired

    # Authorization, ServiceDiscovery
    def set_FreeService(self, FreeService: bool):
        self.FreeService = FreeService
    def get_FreeService(self) -> bool:
        return self.FreeService

    # ChargeParameterDisovery
    def set_EVSEEnergyToBeDelivered(self, EVSEEnergyToBeDelivered: float):
        self.EVSEEnergyToBeDelivered = EVSEEnergyToBeDelivered
    def get_EVSEEnergyToBeDelivered(self) -> float:
        return self.EVSEEnergyToBeDelivered

    # V2GCommunicationSessionSECC IncomingMessage, Send
    def enable_debug_mode(self, debug_mode: str):
        self.debug_mode = debug_mode
    def get_debug_mode(self) -> str:
        return self.debug_mode

    # Authorization 
    def set_Auth_Okay_EIM(self, auth_okay_eim: bool):
        self.auth_okay_eim = auth_okay_eim
    def get_Auth_Okay_EIM(self) -> bool:
        return self.auth_okay_eim

    # Authorization
    def set_Auth_Okay_PnC(self, auth_okay_pnc: bool):
        self.auth_okay_pnc = auth_okay_pnc
    def get_Auth_Okay_PnC(self) -> bool:
        return self.auth_okay_pnc

    # TODO: Not yet implemented
    def set_FAILED_ContactorError(self, ContactorError: bool):
        self.ContactorError = ContactorError
    def get_FAILED_ContactorError(self) -> bool:
        return self.ContactorError
        
    # ChargeParameter, ChargingStatus, MeteringReceipt, PowerDelivery
    def set_RCD_Error(self, RCD: bool):
        self.RCD_Error = RCD
    def get_RCD_Error(self) -> bool:
        return self.RCD_Error

    # CableCheck, ChargingStatus, CurrentDemand, PowerDelivery, PreCharge, WeldingDetection
    # ChargeParameterDiscovery
    def set_stop_charging(self, stop_charging: bool):
        self.stop_charging = stop_charging
    def get_stop_charging(self) -> bool:
        return self.stop_charging

    # CurrentDemand, PreCharge, WeldingDetection 
    def set_DC_EVSEPresentVoltage(self, EVSEPresentVoltage: float):
        self.EVSEPresentVoltage = EVSEPresentVoltage
    def get_DC_EVSEPresentVoltage(self) -> float:
        return self.EVSEPresentVoltage

    # CurrentDemand 
    def set_DC_EVSEPresentCurrent(self, EVSEPresentCurrent: float):
        self.EVSEPresentCurrent = EVSEPresentCurrent
    def get_DC_EVSEPresentCurrent(self) -> float:
        return self.EVSEPresentCurrent

    # ChargingStatus, ChargeParameter 
    def set_AC_EVSEMaxCurrent(self, EVSEMaxCurrent: float):
        self.EVSEMaxCurrent = EVSEMaxCurrent
    def get_AC_EVSEMaxCurrent(self) -> float:
        return self.EVSEMaxCurrent

    # ChargeParameter, CurrentDemand
    def set_DC_EVSEMaximumCurrentLimit(self, EVSEMaximumCurrentLimit: float):
        self.EVSEMaximumCurrentLimit = EVSEMaximumCurrentLimit
    def get_DC_EVSEMaximumCurrentLimit(self) -> float:
        return self.EVSEMaximumCurrentLimit

    # ChargeParameter, CurrentDemand
    def set_DC_EVSEMaximumPowerLimit(self, EVSEMaximumPowerLimit: float):
        self.EVSEMaximumPowerLimit = EVSEMaximumPowerLimit
    def get_DC_EVSEMaximumPowerLimit(self) -> float:
        return self.EVSEMaximumPowerLimit

    # ChargeParameter, CurrentDemand
    def set_DC_EVSEMaximumVoltageLimit(self, EVSEMaximumVoltageLimit: float):
        self.EVSEMaximumVoltageLimit = EVSEMaximumVoltageLimit
    def get_DC_EVSEMaximumVoltageLimit(self) -> float:
        return self.EVSEMaximumVoltageLimit

    # ChargeParameter
    def set_DC_EVSEMinimumCurrentLimit(self, EVSEMinimumCurrentLimit: float):
        self.EVSEMinimumCurrentLimit = EVSEMinimumCurrentLimit
    def get_DC_EVSEMinimumCurrentLimit(self) -> float:
        return self.EVSEMinimumCurrentLimit

    # ChargeParameter
    def set_DC_EVSEMinimumVoltageLimit(self, EVSEMinimumVoltageLimit: float):
        self.EVSEMinimumVoltageLimit = EVSEMinimumVoltageLimit
    def get_DC_EVSEMinimumVoltageLimit(self) -> float:
        return self.EVSEMinimumVoltageLimit

    # CableCheck, CurrentDemand, MeteringReceipt, PowerDelivery, PreCharge, WeldingDetection
    def set_EVSEIsolationStatus(self, EVSEIsolationStatus: str):
        self.EVSEIsolationStatus = EVSEIsolationStatus
    def get_EVSEIsolationStatus(self) -> str:
        return self.EVSEIsolationStatus 

    # CableCheck, CurrentDemand, MeteringReceipt, PowerDelivery, PreCharge, WeldingDetection
    def set_EVSE_UtilityInterruptEvent(self, EVSE_UtilityInterruptEvent: bool):
        self.EVSE_UtilityInterruptEvent = EVSE_UtilityInterruptEvent
    def get_EVSE_UtilityInterruptEvent(self) -> bool:
        return self.EVSE_UtilityInterruptEvent

    # CableCheck, CurrentDemand, MeteringReceipt, PowerDelivery, PreCharge, WeldingDetection
    def set_EVSE_Malfunction(self, EVSE_Malfunction: bool):
        self.EVSE_Malfunction = EVSE_Malfunction
    def get_EVSE_Malfunction(self) -> bool:
        return self.EVSE_Malfunction

    # CableCheck, CurrentDemand, MeteringReceipt, PowerDelivery, PreCharge, WeldingDetection
    def set_EVSE_EmergencyShutdown(self, EVSE_EmergencyShutdown: bool):
        self.EVSE_EmergencyShutdown = EVSE_EmergencyShutdown
    def get_EVSE_EmergencyShutdown(self) -> bool:
        return self.EVSE_EmergencyShutdown

    # ChargingStatus, CurrentDemand
    def set_MeterInfo(self, powermeter: dict):
        self.powermeter = powermeter
    def get_MeterInfo(self) -> dict:
        return self.powermeter

    # PowerDelivery
    def contactor_closed(self, status: bool):
        if (status == True):
            self.contactorClosed = True
            self.contactorOpen = False
    def contactor_open(self, status: bool):
        if (status == True):
            self.contactorClosed = False
            self.contactorOpen = True
    def get_contactor_closed_status(self) -> bool:
        return self.contactorClosed
    def get_contactor_opened_status(self) -> bool:
        return self.contactorOpen

    def set_cableCheck_Finished(self, status: bool):
        self.cableCheck_Finished = status
    def get_cableCheck_Finished(self) -> bool:
        return self.cableCheck_Finished
    
ChargerWrapper = Charger_Wrapper()

def float2Value_Multiplier(value:float):
    p_value: int = 0
    p_multiplier: int = 0
    exponent: int = 0
    
    # Check if it is an integer or a decimal number
    if (value - int(value)) != 0:
        exponent = 2
    
    for x in range(exponent, -4, -1):
        if (value * pow(10, x)) < INT_16_MAX:
            exponent = x
            break
    
    p_multiplier = int(-exponent)
    p_value = int(value * pow(10, exponent))

    return p_value, p_multiplier

def PhysicalValueType2float(p_value:int, p_multiplier:int) -> float:
    value:float = p_value * pow(10, p_multiplier)
    return value
