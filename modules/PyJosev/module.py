from everestpy import log
import asyncio
import logging

import sys, os
JOSEV_WORK_DIR = str(os.getcwd() + '/josev')
sys.path.append(JOSEV_WORK_DIR)

from iso15118.secc import SECCHandler
from iso15118.secc.controller.simulator import SimEVSEController
from iso15118.shared.exificient_exi_codec import ExificientEXICodec

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

def init():
    log.info(f"init")

def ready():
    log.info(f"ready")

    log.info("Josev is starting ...")
    run()

### Charger Command Callbacks ###

def charger_set_EVSEID(EVSEID: str): 
    log.info(f"Received EVSEID: {EVSEID}")

def charger_set_PaymentOptions(PaymentOptions: list):
    print(PaymentOptions)

def charger_set_SupportedEnergyTransferMode(SupportedEnergyTransferMode: list):
    print(SupportedEnergyTransferMode)

def charger_set_AC_EVSENominalVoltage(EVSENominalVoltage: float):
    log.info(f"Received EVSENominalVoltage: {EVSENominalVoltage}")

def charger_set_DC_EVSECurrentRegulationTolerance(EVSECurrentRegulationTolerance: float):
    log.info(f"Received EVSECurrentRegulationTolerance: {EVSECurrentRegulationTolerance}")

def charger_set_DC_EVSEPeakCurrentRipple(EVSEPeakCurrentRipple: float):
    log.info(f"Received EVSEPeakCurrentRipple: {EVSEPeakCurrentRipple}")

def charger_set_ReceiptRequired(ReceiptRequired: bool):
    log.info(f"Received ReceiptRequired: {ReceiptRequired}")

def charger_set_FreeService(FreeService: bool):
    log.info(f"Received FreeService: {FreeService}")

def charger_set_EVSEEnergyToBeDelivered(EVSEEnergyToBeDelivered: float):
    log.info(f"Received EVSEEnergyToBeDelivered: {EVSEEnergyToBeDelivered}")

def charger_enable_debug_mode(debug_mode: str):
    log.info(f"Received debug_mode: {debug_mode}")

def charger_set_Auth_Okay_EIM(auth_okay_eim: bool):
    log.info(f"Received auth_okay_eim: {auth_okay_eim}")

def charger_set_Auth_Okay_PnC(auth_okay_pnc: bool):
    log.info(f"Received auth_okay_pnc: {auth_okay_pnc}")

def charger_set_FAILED_ContactorError(ContactorError: bool):
    log.info(f"Received ContactorError: {ContactorError}")

def charger_set_RCD_Error(RCD: bool):
    log.info(f"Received RCD: {RCD}")

def charger_stop_charging(stop_charging: bool):
    log.info(f"Received stop_charging: {stop_charging}")

def charger_set_DC_EVSEPresentVoltage(EVSEPresentVoltage: float):
    log.info(f"Received EVSEPresentVoltage: {EVSEPresentVoltage}")

def charger_set_DC_EVSEPresentCurrent(EVSEPresentCurrent: float):
    log.info(f"Received EVSEPresentCurrent: {EVSEPresentCurrent}")

def charger_set_AC_EVSEMaxCurrent(EVSEMaxCurrent: float):
    log.info(f"Received EVSEMaxCurrent: {EVSEMaxCurrent}")

def charger_set_DC_EVSEMaximumCurrentLimit(EVSEMaximumCurrentLimit: float):
    log.info(f"Received EVSEMaximumCurrentLimit: {EVSEMaximumCurrentLimit}")

def charger_set_DC_EVSEMaximumPowerLimit(EVSEMaximumPowerLimit: float):
    log.info(f"Received EVSEMaximumPowerLimit: {EVSEMaximumPowerLimit}")

def charger_set_DC_EVSEMaximumVoltageLimit(EVSEMaximumVoltageLimit: float):
    log.info(f"Received EVSEMaximumVoltageLimit: {EVSEMaximumVoltageLimit}")

def charger_set_DC_EVSEMinimumCurrentLimit(EVSEMinimumCurrentLimit: float):
    log.info(f"Received EVSEMinimumCurrentLimit: {EVSEMinimumCurrentLimit}")

def charger_set_DC_EVSEMinimumVoltageLimit(EVSEMinimumVoltageLimit: float):
    log.info(f"Received EVSEMinimumVoltageLimit: {EVSEMinimumVoltageLimit}")

def charger_set_EVSEIsolationStatus(EVSEIsolationStatus: str):
    log.info(f"Received EVSEIsolationStatus: {EVSEIsolationStatus}")

def charger_set_EVSE_UtilityInterruptEvent(EVSE_UtilityInterruptEvent: bool):
    log.info(f"Received EVSE_UtilityInterruptEvent: {EVSE_UtilityInterruptEvent}")

def charger_set_EVSE_Malfunction(EVSE_Malfunction: bool):
    log.info(f"Received EVSE_Malfunction: {EVSE_Malfunction}")

def charger_set_EVSE_EmergencyShutdown(EVSE_EmergencyShutdown: bool):
    log.info(f"Received EVSE_EmergencyShutdown: {EVSE_EmergencyShutdown}")

def charger_set_MeterInfo(powermeter: dict):
    print(powermeter)

def charger_contactor_closed(status: bool):
    log.info(f"Received status: {status}")

def charger_contactor_open(status: bool):
    log.info(f"Received status: {status}")
