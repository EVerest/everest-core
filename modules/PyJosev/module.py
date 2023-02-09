from everestpy import log
import asyncio
import logging
import netifaces
from pathlib import Path

import sys
JOSEV_WORK_DIR = Path(__file__).parent / '../../3rd_party/josev'
sys.path.append(JOSEV_WORK_DIR.as_posix())

EVEREST_ETC_CERTS_PATH = "etc/everest/certs"

from iso15118.secc import SECCHandler
from iso15118.secc.controller.simulator import SimEVSEController
from iso15118.shared.exificient_exi_codec import ExificientEXICodec
from iso15118.secc.secc_settings import Config
from iso15118.shared.settings import set_PKI_PATH

from everest_iso15118 import init_Setup
from everest_iso15118 import ChargerWrapper

logger = logging.getLogger(__name__)

async def main():
    """
    Entrypoint function that starts the ISO 15118 code running on
    the SECC (Supply Equipment Communication Controller)
    """
    config = Config()
    config.load_everest_config(module_config_)

    sim_evse_controller = await SimEVSEController.create()
    await SECCHandler(
        exi_codec=ExificientEXICodec(), evse_controller=sim_evse_controller, config=config
    ).start(config.iface)


def run():
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.debug("SECC program terminated manually")


setup_ = None
module_config_ = None
impl_configs_ = None
module_info_ = None

def pre_init(setup, module_config, impl_configs, _module_info):
    global setup_, module_config_, impl_configs_, module_info_
    setup_ = setup
    module_config_ = module_config
    impl_configs_ = impl_configs
    module_info_ = _module_info
    init_Setup(setup_)

def init():
    log.debug("init")

    etc_certs_path = module_info_.everest_prefix + "/" + EVEREST_ETC_CERTS_PATH + "/"
    set_PKI_PATH(etc_certs_path)

    net_iface: str = str(module_config_["device"])
    if net_iface == "auto":
        module_config_["device"] = choose_first_ipv6_local()
    elif not check_network_interfaces(net_iface=net_iface):
        log.warning(f"The network interface {net_iface} was not found!")

def ready():
    log.debug("ready")

    log.debug("Josev is starting ...")
    run()

### Charger Command Callbacks ###

def charger_set_EVSEID(EVSEID: str, EVSEID_DIN: str):
    ChargerWrapper.set_EVSEID(EVSEID)
    ChargerWrapper.set_EVSEID_DIN(EVSEID_DIN)

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

def charger_set_Auth_Okay_PnC(status: str, certificateStatus: str):
    ChargerWrapper.set_Auth_PnC_Status(status, certificateStatus)

def charger_set_FAILED_ContactorError(ContactorError: bool):
    ChargerWrapper.set_FAILED_ContactorError(ContactorError)

def charger_set_RCD_Error(RCD: bool):
    ChargerWrapper.set_RCD_Error(RCD)

def charger_stop_charging(stop_charging: bool):
    ChargerWrapper.set_stop_charging(stop_charging)

def charger_set_DC_EVSEPresentVoltageCurrent(EVSEPresentVoltage_Current: dict):
    ChargerWrapper.set_DC_EVSEPresentVoltage(EVSEPresentVoltage_Current["EVSEPresentVoltage"])
    ChargerWrapper.set_DC_EVSEPresentCurrent(EVSEPresentVoltage_Current["EVSEPresentCurrent"])

def charger_set_AC_EVSEMaxCurrent(EVSEMaxCurrent: float):
    ChargerWrapper.set_AC_EVSEMaxCurrent(EVSEMaxCurrent)

def charger_set_DC_EVSEMaximumLimits(EVSEMaximumLimits: dict):
    ChargerWrapper.set_DC_EVSEMaximumCurrentLimit(EVSEMaximumLimits["EVSEMaximumCurrentLimit"])
    ChargerWrapper.set_DC_EVSEMaximumPowerLimit(EVSEMaximumLimits["EVSEMaximumPowerLimit"])
    ChargerWrapper.set_DC_EVSEMaximumVoltageLimit(EVSEMaximumLimits["EVSEMaximumVoltageLimit"])

def charger_set_DC_EVSEMinimumLimits(EVSEMinimumLimits: dict):
    ChargerWrapper.set_DC_EVSEMinimumCurrentLimit(EVSEMinimumLimits["EVSEMinimumCurrentLimit"])
    ChargerWrapper.set_DC_EVSEMinimumVoltageLimit(EVSEMinimumLimits["EVSEMinimumVoltageLimit"])

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

def charger_cableCheck_Finished(status: bool):
    ChargerWrapper.set_cableCheck_Finished(status)

def charger_set_Certificate_Service_Supported(status: bool):
    ChargerWrapper.set_Certificate_Service_Supported(status)

def charger_set_Get_Certificate_Response(Existream_Status: dict):
    ChargerWrapper.set_Certificate_Response(Existream_Status)

def check_network_interfaces(net_iface: str) -> bool:
    if (net_iface in netifaces.interfaces()) is True:
        return True
    return False

def choose_first_ipv6_local() -> str:
    for iface in netifaces.interfaces():
        if netifaces.AF_INET6 in netifaces.ifaddresses(iface):
            for netif_inet6 in netifaces.ifaddresses(iface)[netifaces.AF_INET6]:
                if 'fe80' in netif_inet6['addr']:
                    return iface

    log.warning("No necessary IPv6 link-local address was found!")
    return "eth0"
