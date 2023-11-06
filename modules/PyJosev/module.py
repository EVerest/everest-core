# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
import asyncio
import sys
from pathlib import Path
import threading

from everest.framework import Module, RuntimeSession, log

# fmt: off
JOSEV_WORK_DIR = Path(__file__).parent / '../../3rd_party/josev'
sys.path.append(JOSEV_WORK_DIR.as_posix())

from iso15118.shared.settings import set_PKI_PATH
from iso15118.secc.secc_settings import Config
from iso15118.shared.exificient_exi_codec import ExificientEXICodec
from iso15118.secc.controller.simulator import SimEVSEController
from iso15118.secc.everest import context as JOSEV_CONTEXT
from iso15118.secc import SECCHandler

from utilities import setup_everest_logging, patch_josev_config
# fmt: on

setup_everest_logging()


EVEREST_CERTS_SUB_DIR = 'certs'


async def secc_handler_main_loop(module_config: dict):
    """
    Entrypoint function that starts the ISO 15118 code running on
    the SECC (Supply Equipment Communication Controller)
    """

    config = Config()

    patch_josev_config(config, module_config)

    sim_evse_controller = await SimEVSEController.create()
    await SECCHandler(
        exi_codec=ExificientEXICodec(), evse_controller=sim_evse_controller, config=config
    ).start(config.iface)


class PyJosevModule():
    def __init__(self):
        self._cs = JOSEV_CONTEXT.charger_state
        self._session = RuntimeSession()
        m = Module(self._session)

        log.update_process_name(m.info.id)

        self._setup = m.say_hello()

        etc_certs_path = m.info.paths.etc / EVEREST_CERTS_SUB_DIR
        set_PKI_PATH(str(etc_certs_path.resolve()))

        # setup publishing callback
        def publish_callback(variable_name: str, value: any):
            m.publish_variable('charger', variable_name, value)

        JOSEV_CONTEXT.set_publish_callback(publish_callback)

        # setup handlers
        for cmd in m.implementations['charger'].commands:
            m.implement_command(
                'charger', cmd, getattr(self, f'_handler_{cmd}'))

        # init ready event
        self._ready_event = threading.Event()

        self._mod = m
        self._mod.init_done(self._ready)

    def start_secc_handler(self):
        # NOTE (aw): we could also have run the main logic inside the _ready
        # method, but then it would run inside the pybind thread, which doesn't
        # always produce nice tracebacks
        self._ready_event.wait()

        # finally spawn SECC
        try:
            asyncio.run(secc_handler_main_loop(self._setup.configs.module))
        except KeyboardInterrupt:
            log.debug("SECC program terminated manually")

    def _ready(self):
        self._ready_event.set()

    # implementation handlers

    def _handler_set_EVSEID(self, args):
        self._cs.EVSEID = args['EVSEID']
        self._cs.EVSEID_DIN = args['EVSEID_DIN']

    def _handler_set_PaymentOptions(self, args):
        self._cs.PaymentOptions = args['PaymentOptions']

    def _handler_set_SupportedEnergyTransferMode(self, args):
        self._cs.SupportedEnergyTransferMode = args['SupportedEnergyTransferMode']

    def _handler_set_AC_EVSENominalVoltage(self, args):
        self._cs.EVSENominalVoltage = args['EVSENominalVoltage']

    def _handler_set_DC_EVSECurrentRegulationTolerance(self, args):
        self._cs.EVSECurrentRegulationTolerance = args['EVSECurrentRegulationTolerance']

    def _handler_set_DC_EVSEPeakCurrentRipple(self, args):
        self._cs.EVSEPeakCurrentRipple = args['EVSEPeakCurrentRipple']

    def _handler_set_ReceiptRequired(self, args):
        self._cs.ReceiptRequired = args['ReceiptRequired']

    def _handler_set_FreeService(self, args):
        self._cs.FreeService = args['FreeService']

    def _handler_set_EVSEEnergyToBeDelivered(self, args):
        self._cs.EVSEEnergyToBeDelivered = args['EVSEEnergyToBeDelivered']

    def _handler_enable_debug_mode(self, args):
        self._cs.debug_mode = args['debug_mode']

    def _handler_set_Auth_Okay_EIM(self, args):
        self._cs.auth_okay_eim = args['auth_okay_eim']

    def _handler_set_Auth_Okay_PnC(self, args):
        self._cs.auth_pnc_status = args['status']
        self._cs.auth_pnc_certificate_status = args['certificateStatus']

    def _handler_set_FAILED_ContactorError(self, args):
        self._cs.ContactorError = args['ContactorError']

    def _handler_set_RCD_Error(self, args):
        self._cs.RCD_Error = args['RCD']

    def _handler_stop_charging(self, args):
        self._cs.stop_charging = args['stop_charging']

    def _handler_set_DC_EVSEPresentVoltageCurrent(self, args):
        present_values = args['EVSEPresentVoltage_Current']
        self._cs.EVSEPresentVoltage = present_values['EVSEPresentVoltage']
        self._cs.EVSEPresentCurrent = present_values['EVSEPresentCurrent']

    def _handler_set_AC_EVSEMaxCurrent(self, args):
        self._cs.EVSEMaxCurrent = args['EVSEMaxCurrent']

    def _handler_set_DC_EVSEMaximumLimits(self, args):
        limits = args['EVSEMaximumLimits']
        self._cs.EVSEMaximumCurrentLimit = limits['EVSEMaximumCurrentLimit']
        self._cs.EVSEMaximumPowerLimit = limits['EVSEMaximumPowerLimit']
        self._cs.EVSEMaximumVoltageLimit = limits['EVSEMaximumVoltageLimit']

    def _handler_set_DC_EVSEMinimumLimits(self, args):
        limits = args['EVSEMinimumLimits']
        self._cs.EVSEMinimumCurrentLimit = limits['EVSEMinimumCurrentLimit']
        self._cs.EVSEMinimumVoltageLimit = limits['EVSEMinimumVoltageLimit']

    def _handler_set_EVSEIsolationStatus(self, args):
        self._cs.EVSEIsolationStatus = args['EVSEIsolationStatus']

    def _handler_set_EVSE_UtilityInterruptEvent(self, args):
        self._cs.EVSE_UtilityInterruptEvent = args['EVSE_UtilityInterruptEvent']

    def _handler_set_EVSE_Malfunction(self, args):
        self._cs.EVSE_Malfunction = args['EVSE_Malfunction']

    def _handler_set_EVSE_EmergencyShutdown(self, args):
        self._cs.EVSE_EmergencyShutdown = args['EVSE_EmergencyShutdown']

    def _handler_set_MeterInfo(self, args):
        self._cs.powermeter = args['powermeter']

    def _handler_contactor_closed(self, args):
        closed = args['status']
        if closed:
            self._cs.contactorClosed = True
            self._cs.contactorOpen = False

    def _handler_contactor_open(self, args):
        opened = args['status']
        if opened:
            self._cs.contactorOpen = True
            self._cs.contactorClosed = False

    def _handler_cableCheck_Finished(self, args):
        self._cs.cableCheck_Finished = args['status']

    def _handler_set_Certificate_Service_Supported(self, args):
        self._cs.certificate_service_supported = args['status']

    def _handler_set_Get_Certificate_Response(self, args):
        self._cs.existream_status = args['Existream_Status']

    def _handler_dlink_ready(self, args):
        self._cs.dlink_ready = args['value']

    def _handler_supporting_sae_j2847_bidi(self, args):
        log.warning(
            "SAE J2847/2 Bidi Mode is currently not yet supported by the PyJosev module. Use instead EvseV2G.")


py_josev = PyJosevModule()
py_josev.start_secc_handler()
