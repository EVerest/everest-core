from everestpy import log
import asyncio
import logging

import sys, os
JOSEV_WORK_DIR = str(os.getcwd() + '/josev')
sys.path.append(JOSEV_WORK_DIR)

from iso15118.secc import SECCHandler
from iso15118.secc.controller.simulator import SimEVSEController
from iso15118.shared.exificient_exi_codec import ExificientEXICodec

from everest_iso15118 import init_Setup
from everest_iso15118 import ChargerWrapper

logger = logging.getLogger(__name__)

async def main():
    """
    Entrypoint function that starts the ISO 15118 code running on
    the SECC (Supply Equipment Communication Controller)
    """
    sim_evse_controller = await SimEVSEController.create()
    env_path = str(JOSEV_WORK_DIR + '/.env')
    await SECCHandler(
        exi_codec=ExificientEXICodec(), evse_controller=sim_evse_controller, env_path=env_path
    ).start()


def run():
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.debug("SECC program terminated manually")

setup_ = None

def pre_init(setup):
    global setup_
    setup_ = setup
    init_Setup(setup_)

def init():
    log.info(f"init")

def ready():
    log.info(f"ready")

    log.info("Josev is starting ...")
    run()

### Charger Command Callbacks ###

def charger_set_EVSEID(EVSEID: str): 
    ChargerWrapper.set_EVSEID(EVSEID)

def charger_set_PaymentOptions(PaymentOptions: list):
    ChargerWrapper.set_PaymentOptions(PaymentOptions)

def charger_set_SupportedEnergyTransferMode(SupportedEnergyTransferMode: list):
    ChargerWrapper.set_SupportedEnergyTransferMode(SupportedEnergyTransferMode)

def charger_set_AC_EVSENominalVoltage(EVSENominalVoltage: float):
    ChargerWrapper.set_AC_EVSENominalVoltage(EVSENominalVoltage)

def charger_set_DC_EVSECurrentRegulationTolerance(EVSECurrentRegulationTolerance: float):
    ChargerWrapper.set_DC_EVSECurrentRegulationTolerance(EVSECurrentRegulationTolerance)

def charger_set_DC_EVSEPeakCurrentRipple(EVSEPeakCurrentRipple: float):
    ChargerWrapper.set_DC_EVSEPeakCurrentRipple(EVSEPeakCurrentRipple)

def charger_set_ReceiptRequired(ReceiptRequired: bool):
    ChargerWrapper.set_ReceiptRequired(ReceiptRequired)

def charger_set_FreeService(FreeService: bool):
    ChargerWrapper.set_FreeService(FreeService)

def charger_set_EVSEEnergyToBeDelivered(EVSEEnergyToBeDelivered: float):
    ChargerWrapper.set_EVSEEnergyToBeDelivered(EVSEEnergyToBeDelivered)

def charger_enable_debug_mode(debug_mode: str):
    ChargerWrapper.enable_debug_mode(debug_mode)

def charger_set_Auth_Okay_EIM(auth_okay_eim: bool):
    ChargerWrapper.set_Auth_Okay_EIM(auth_okay_eim)

def charger_set_Auth_Okay_PnC(auth_okay_pnc: bool):
    ChargerWrapper.set_Auth_Okay_PnC(auth_okay_pnc)

def charger_set_FAILED_ContactorError(ContactorError: bool):
    ChargerWrapper.set_FAILED_ContactorError(ContactorError)

def charger_set_RCD_Error(RCD: bool):
    ChargerWrapper.set_RCD_Error(RCD)

def charger_stop_charging(stop_charging: bool):
    ChargerWrapper.stop_charging(stop_charging)

def charger_set_DC_EVSEPresentVoltage(EVSEPresentVoltage: float):
    ChargerWrapper.set_DC_EVSEPresentVoltage(EVSEPresentVoltage)

def charger_set_DC_EVSEPresentCurrent(EVSEPresentCurrent: float):
    ChargerWrapper.set_DC_EVSEPresentCurrent(EVSEPresentCurrent)

def charger_set_AC_EVSEMaxCurrent(EVSEMaxCurrent: float):
    ChargerWrapper.set_AC_EVSEMaxCurrent(EVSEMaxCurrent)

def charger_set_DC_EVSEMaximumCurrentLimit(EVSEMaximumCurrentLimit: float):
    ChargerWrapper.set_DC_EVSEMaximumCurrentLimit(EVSEMaximumCurrentLimit)

def charger_set_DC_EVSEMaximumPowerLimit(EVSEMaximumPowerLimit: float):
    ChargerWrapper.set_DC_EVSEMaximumPowerLimit(EVSEMaximumPowerLimit)

def charger_set_DC_EVSEMaximumVoltageLimit(EVSEMaximumVoltageLimit: float):
    ChargerWrapper.set_DC_EVSEMaximumVoltageLimit(EVSEMaximumVoltageLimit)

def charger_set_DC_EVSEMinimumCurrentLimit(EVSEMinimumCurrentLimit: float):
    ChargerWrapper.set_DC_EVSEMinimumCurrentLimit(EVSEMinimumCurrentLimit)

def charger_set_DC_EVSEMinimumVoltageLimit(EVSEMinimumVoltageLimit: float):
    ChargerWrapper.set_DC_EVSEMinimumVoltageLimit(EVSEMinimumVoltageLimit)

def charger_set_EVSEIsolationStatus(EVSEIsolationStatus: str):
    ChargerWrapper.set_EVSEIsolationStatus(EVSEIsolationStatus)

def charger_set_EVSE_UtilityInterruptEvent(EVSE_UtilityInterruptEvent: bool):
    ChargerWrapper.set_EVSE_UtilityInterruptEvent(EVSE_UtilityInterruptEvent)

def charger_set_EVSE_Malfunction(EVSE_Malfunction: bool):
    ChargerWrapper.set_EVSE_Malfunction(EVSE_Malfunction)

def charger_set_EVSE_EmergencyShutdown(EVSE_EmergencyShutdown: bool):
    ChargerWrapper.set_EVSE_EmergencyShutdown(EVSE_EmergencyShutdown)

def charger_set_MeterInfo(powermeter: dict):
    ChargerWrapper.set_MeterInfo(powermeter)

def charger_contactor_closed(status: bool):
    ChargerWrapper.contactor_closed(status)

def charger_contactor_open(status: bool):
    ChargerWrapper.contactor_open(status)
