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

    def _handler_setup(self, args):
        evseid = args['evse_id']
        self._cs.EVSEID = evseid['EVSE_ID']
        self._cs.EVSEID_DIN = evseid['EVSEID_DIN']

        self._cs.SupportedEnergyTransferMode = args['supported_energy_transfer_modes']
        self._cs.debug_mode = args['debug_mode']

        physical_values = args['physical_values']
        if 'ac_max_current' in physical_values:
            self._cs.EVSEMaxCurrent = physical_values['ac_max_current']
        if 'ac_nominal_voltage' in physical_values:
            self._cs.EVSENominalVoltage = physical_values['ac_nominal_voltage']
        if 'dc_current_regulation_tolerance' in physical_values:
            self._cs.EVSECurrentRegulationTolerance = physical_values[
                'dc_current_regulation_tolerance']
        if 'dc_peak_current_ripple' in physical_values:
            self._cs.EVSEPeakCurrentRipple = physical_values['dc_peak_current_ripple']
        if 'dc_energy_to_be_delivered' in physical_values:
            self._cs.EVSEEnergyToBeDelivered = physical_values['dc_energy_to_be_delivered']
        if 'dc_minimum_limits' in physical_values:
            limits = physical_values['dc_minimum_limits']
            self._cs.EVSEMinimumCurrentLimit = limits['EVSEMinimumCurrentLimit']
            self._cs.EVSEMinimumVoltageLimit = limits['EVSEMinimumVoltageLimit']
        if 'dc_maximum_limits' in physical_values:
            limits = physical_values['dc_maximum_limits']
            self._cs.EVSEMaximumCurrentLimit = limits['EVSEMaximumCurrentLimit']
            self._cs.EVSEMaximumPowerLimit = limits['EVSEMaximumPowerLimit']
            self._cs.EVSEMaximumVoltageLimit = limits['EVSEMaximumVoltageLimit']

        sae_mode = args['sae_j2847_mode']

        if sae_mode == 'V2H' or sae_mode == 'V2G':
            log.warning(
                "SAE J2847/2 Bidi Mode is currently not yet supported by the PyJosev module. Use instead EvseV2G.")

    def _handler_session_setup(self, args):
        self._cs.PaymentOptions = args['payment_options']
        self._cs.certificate_service_supported = args['supported_certificate_service']

    def _handler_certificate_response(self, args):
        self._cs.existream_status = args['exi_stream_status']

    def _handler_authorization_response(self, args):
        self._cs.auth_status = args['authorization_status']
        self._cs.certificate_status = args['certificate_status']

    def _handler_ac_contactor_closed(self, args):
        closed = args['status']
        if closed:
            self._cs.contactorClosed = True
            self._cs.contactorOpen = False
        else:
            self._cs.contactorClosed = False
            self._cs.contactorOpen = True

    def _handler_dlink_ready(self, args):
        self._cs.dlink_ready = args['value']

    def _handler_cable_check_finished(self, args):
        self._cs.cableCheck_Finished = args['status']

    def _handler_receipt_is_required(self, args):
        self._cs.ReceiptRequired = args['receipt_required']

    def _handler_stop_charging(self, args):
        self._cs.stop_charging = args['stop']

    def _handler_update_ac_max_current(self, args):
        self._cs.EVSEMaxCurrent = args['max_current']

    def _handler_update_dc_maximum_limits(self, args):
        limits = args['maximum_limits']
        self._cs.EVSEMaximumCurrentLimit = limits['EVSEMaximumCurrentLimit']
        self._cs.EVSEMaximumPowerLimit = limits['EVSEMaximumPowerLimit']
        self._cs.EVSEMaximumVoltageLimit = limits['EVSEMaximumVoltageLimit']

    def _handler_update_dc_minimum_limits(self, args):
        limits = args['minimum_limits']
        self._cs.EVSEMinimumCurrentLimit = limits['EVSEMinimumCurrentLimit']
        self._cs.EVSEMinimumVoltageLimit = limits['EVSEMinimumVoltageLimit']

    def _handler_update_isolation_status(self, args):
        self._cs.EVSEIsolationStatus = args['isolation_status']

    def _handler_update_dc_present_values(self, args):
        present_values = args['present_voltage_current']
        self._cs.EVSEPresentVoltage = present_values['EVSEPresentVoltage']
        self._cs.EVSEPresentCurrent = present_values['EVSEPresentCurrent']

    def _handler_update_meter_info(self, args):
        self._cs.powermeter = args['powermeter']

    def _handler_send_error(self, args):
        error = args['error']
        if error == 'Error_Contactor':
            self._cs.ContactorError = True
        elif error == 'Error_RCD':
            self._cs.RCD_Error = True
        elif error == 'Error_UtilityInterruptEvent':
            self._cs.EVSE_UtilityInterruptEvent = True
        elif error == 'Error_Malfunction':
            self._cs.EVSE_Malfunction = True
        elif error == 'Error_EmergencyShutdown':
            self._cs.EVSE_EmergencyShutdown = True

    def _handler_reset_error(self, args):
        self._cs.ContactorError = False
        self._cs.RCD_Error = False
        self._cs.EVSE_UtilityInterruptEvent = False
        self._cs.EVSE_Malfunction = False
        self._cs.EVSE_EmergencyShutdown = False


py_josev = PyJosevModule()
py_josev.start_secc_handler()
