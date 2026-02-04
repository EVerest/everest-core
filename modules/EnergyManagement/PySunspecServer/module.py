# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.framework import Module, RuntimeSession, log
from everest_types import Powermeter, ExternalLimits, ScheduleSetpointEntry, SetpointType
import threading
from datetime import datetime, timezone

"""Example Skeleton for PySunspecServer Module"""
class PySunspecServerModule():
    def __init__(self) -> None:
        self._session = RuntimeSession()
        m = Module(self._session)
        log.update_process_name(m.info.id)
        self._setup = m.say_hello()
        self._ready_event = threading.Event()
        self._mod = m

        self._init_fulfillments()

        self._mod.init_done(self._ready)

    def _init_fulfillments(self):
        # evse_manager fulfillment, TOOD: support multiple connections
        evse_manager_ff = self._setup.connections['evse_manager'][0]

        # external_energy_limits fulfillment, TOOD: support multiple connections
        self._external_energy_limits_ff = self._setup.connections['external_energy_limits'][0]

        # subscribe to powermeter messages from evse_manager
        self._mod.subscribe_variable(evse_manager_ff, 'powermeter',
                                     self._handle_evse_manager_powermeter_message)
        
    def _ready(self):
        print("PySunspecServerModule ready")
        self._ready_event.set()

    def start(self):
        while True:
            # Required to keep the module running
            self._ready_event.wait()
            self._ready_event.clear()

    def _handle_evse_manager_powermeter_message(self, message):
        # https://everest.github.io/nightly/reference/types/powermeter.html#powermeter-powermeter
        pm = Powermeter.from_dict(message)
        print(f"Received powermeter at {pm.timestamp}")
        print(f"  Energy imported: {pm.energy_Wh_import.total} Wh")

    def _call_set_external_limits(self, max_current_A: float, max_power_W: float):
        """Example: Set external limits for an EVSE"""
        # https://everest.github.io/nightly/reference/interfaces/external_energy_limits.html
        now = datetime.now(timezone.utc)

        external_limits = ExternalLimits(
            schedule_import=[],
            schedule_export=[],
            schedule_setpoints=[
                ScheduleSetpointEntry(timestamp=now, setpoint=SetpointType(
                    ac_current_A=max_current_A,
                    total_power_W=max_power_W,
                    source="PySunspecServer",
                    priority=1
                ))
            ],
        )

        # Call the set_external_limits command
        self._mod.call_command(
            self._external_energy_limits_ff,
            'set_external_limits',
            {'value': external_limits.to_dict()}
        )

        print(f"Set external limits: {max_current_A}A, {max_power_W}W")

pysunspec_module = PySunspecServerModule()
pysunspec_module.start()
