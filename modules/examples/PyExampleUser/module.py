# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.framework import Module, RuntimeSession, log
import threading
import json

class PyExampleUserModule():
    def __init__(self):
        self._session = RuntimeSession()
        m = Module(self._session)
        self._setup = m.say_hello()
        req_example = self._setup.connections['example'][0]

        self._ready_event = threading.Event()

        # Note: The error handler are called twice for the error type 'example/ExampleErrorA'
        # because it is subscribed to both the specific error type and all errors of the module 'example'
        m.subscribe_error(req_example, 'example/ExampleErrorA', self.handle_error, self.handle_error_cleared)
        m.subscribe_all_errors(req_example, self.handle_error, self.handle_error_cleared)

        self._mod = m
        self._mod.init_done(self._ready)

    def _ready(self):
        self._ready_event.set()

    def handle_error(self, error):
        log.info("Received error: " + json.dumps(error, indent=1))

    def handle_error_cleared(self, error):
        log.info("Received error cleared: " + json.dumps(error, indent=1))


    def start_example(self):
        while True:
            self._ready_event.wait()
            try:
                log.info("Example program started")

            except KeyboardInterrupt:
                log.debug("Example program terminated manually")
                break
            self._ready_event.clear()

py_example_user = PyExampleUserModule()
py_example_user.start_example()
