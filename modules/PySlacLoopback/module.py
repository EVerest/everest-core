import time

from everest.framework import Module, RuntimeSession, log


class LoopbackTestModule():
    def __init__(self):
        self._session = RuntimeSession()
        self._mod = Module(self._session)
        log.update_process_name(self._mod.info.id)
        self._setup = self._mod.say_hello()
        self._mod.init_done(self._ready)

    def _ready(self):
        WAIT_SECONDS = 0.2
        evse_ff = self._setup.connections['evse_slac'][0]
        ev_ff = self._setup.connections['ev_slac'][0]
        log.info(f'Loopback test driver ready, waiting for {WAIT_SECONDS}s, so EvSlac and EvseSlac get ready')
        time.sleep(WAIT_SECONDS)

        log.info('Calling enter_bcd on EvseSlac')
        self._mod.call_command(evse_ff, 'enter_bcd', {})

        time.sleep(WAIT_SECONDS)
        log.info('Calling trigger_matching on EvSlac')
        self._mod.call_command(ev_ff, 'trigger_matching', {})


LoopbackTestModule()

time.sleep(50)
