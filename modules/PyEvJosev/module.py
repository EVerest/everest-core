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

from iso15118.evcc import EVCCHandler
from iso15118.evcc.controller.simulator import SimEVController
from iso15118.evcc.evcc_config import EVCCConfig
from iso15118.evcc.everest import context as JOSEV_CONTEXT
from iso15118.shared.exificient_exi_codec import ExificientEXICodec
from iso15118.shared.settings import set_PKI_PATH

from utilities import (
    setup_everest_logging,
    determine_network_interface,
    patch_josev_config
)

setup_everest_logging()

EVEREST_CERTS_SUB_DIR = 'certs'

async def evcc_handler_main_loop(module_config: dict):
    """
    Entrypoint function that starts the ISO 15118 code running on
    the EVCC (EV Communication Controller)
    """
    iface = determine_network_interface(module_config['device'])

    evcc_config = EVCCConfig()
    patch_josev_config(evcc_config, module_config)

    await EVCCHandler(
        evcc_config=evcc_config,
        iface=iface,
        exi_codec=ExificientEXICodec(),
        ev_controller=SimEVController(evcc_config),
    ).start()


class PyEVJosevModule():
    def __init__(self) -> None:
        self._es = JOSEV_CONTEXT.ev_state
        self._session = RuntimeSession()
        m = Module(self._session)

        log.update_process_name(m.info.id)

        self._setup = m.say_hello()

        etc_certs_path = m.info.paths.etc / EVEREST_CERTS_SUB_DIR
        set_PKI_PATH(str(etc_certs_path.resolve()))

        # setup publishing callback
        def publish_callback(variable_name: str, value: any):
            m.publish_variable('ev', variable_name, value)

        # set publish callback for context
        JOSEV_CONTEXT.set_publish_callback(publish_callback)

        # setup handlers
        for cmd in m.implementations['ev'].commands:
            m.implement_command(
                'ev', cmd, getattr(self, f'_handler_{cmd}'))

        # init ready event
        self._ready_event = threading.Event()

        self._mod = m
        self._mod.init_done(self._ready)

    def start_evcc_handler(self):
        while True:
            self._ready_event.wait()
            try:
                asyncio.run(evcc_handler_main_loop(self._setup.configs.module))
                self._mod.publish_variable('ev', 'V2G_Session_Finished', None)
            except KeyboardInterrupt:
                log.debug("SECC program terminated manually")
                break
            self._ready_event.clear()

    def _ready(self):
        log.debug("ready!")

    # implementation handlers

    def _handler_start_charging(self, args) -> bool:

        self._es.EnergyTransferMode = args['EnergyTransferMode']

        self._ready_event.set()

        return True

    def _handler_stop_charging(self, args):
        self._es.StopCharging = True

    def _handler_pause_charging(self, args):
        self._es.Pause = True

    def _handler_set_fault(self, args):
        pass

    def _handler_set_dc_params(self, args):
        parameters = args['EV_Parameters']
        self._es.dc_max_current_limit = parameters['MaxCurrentLimit']
        self._es.dc_max_power_limit = parameters['MaxPowerLimit']
        self._es.dc_max_voltage_limit = parameters['MaxVoltageLimit']
        self._es.dc_energy_capacity = parameters['EnergyCapacity']
        self._es.dc_target_current = parameters['TargetCurrent']
        self._es.dc_target_voltage = parameters['TargetVoltage']

    def _handler_set_bpt_dc_params(self, args):
        parameters = args['EV_BPT_Parameters']
        self._es.dc_discharge_max_current_limit = parameters["DischargeMaxCurrentLimit"]
        self._es.dc_discharge_max_power_limit = parameters['DischargeMaxPowerLimit']
        self._es.dc_discharge_target_current = parameters['DischargeTargetCurrent']
        self._es.minimal_soc = parameters["DischargeMinimalSoC"]

    def _handler_enable_sae_j2847_v2g_v2h(self, args):
        self._es.SAEJ2847_V2H_V2G_Active = True

py_ev_josev = PyEVJosevModule()
py_ev_josev.start_evcc_handler()
