import asyncio
import logging
from asyncio.queues import Queue
from typing import Any

from everest.framework import Module, RuntimeSession

from lem_dcbm_test_utils.types import Powermeter


class ProbeModule:
    def __init__(self, session: RuntimeSession):
        logging.info("ProbeModule init start")
        m = Module('probe', session)
        self._setup = m.say_hello()
        self._mod = m

        # subscribe to session events
        logging.info(self._setup.connections)
        evse_manager_ff = self._setup.connections['test_control'][0]
        self._mod.subscribe_variable(evse_manager_ff, 'powermeter',
                                     self._handle_evse_manager_powermeter_message)

        self._msg_queue = Queue()
        self._ready_event = asyncio.Event()
        m.init_done(self._ready)
        logging.info("ProbeModule init done")

    def _ready(self):
        logging.info("ProbeModule ready")
        self._ready_event.set()

    def _handle_evse_manager_powermeter_message(self, message):
        asyncio.run(self._msg_queue.put(message))

    async def poll_next_powermeter(self, timeout) -> Powermeter:
        return Powermeter(**(await asyncio.wait_for(self._msg_queue.get(), timeout=timeout)))

    def call_powermeter_command(self, command_name: str, args: dict) -> Any:
        lem_ff = self._setup.connections['test_control'][0]
        try:
            return self._mod.call_command(lem_ff, command_name, args)
        except Exception as e:
            logging.info(f"Exception in calling command {command_name}: {type(e)}: {e}")
            raise e

    async def wait_to_be_ready(self, timeout=3):
        await asyncio.wait_for(self._ready_event.wait(), timeout)
