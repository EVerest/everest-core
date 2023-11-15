# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.framework import Module, RuntimeSession, log
import threading
import time

class PyExampleModule():
    def __init__(self):
        self._session = RuntimeSession()
        m = Module(self._session)
        self._setup = m.say_hello()

        self._ready_event = threading.Event()

        self._mod = m
        self._mod.init_done(self._ready)

    def _ready(self):
        self._ready_event.set()

    def start_example(self):
        while True:
            self._ready_event.wait()
            try:
                log.info("Example program started")
                self._mod.publish_variable('example', 'max_current', 20)
                err_handle = self._mod.raise_error('example', 'example/ExampleErrorA', 'Example error message', 'Low')
                self._mod.raise_error('example', 'example/ExampleErrorA', 'Example error message', 'Medium')
                self._mod.raise_error('example', 'example/ExampleErrorA', 'Example error message', 'High')
                time.sleep(1)
                self._mod.request_clear_error_uuid('example', err_handle)
                time.sleep(1)
                self._mod.request_clear_error_all_of_type('example', 'example/ExampleErrorA')
                time.sleep(1)
                self._mod.raise_error('example', 'example/ExampleErrorB', 'Example error message', 'Low')
                self._mod.raise_error('example', 'example/ExampleErrorC', 'Example error message', 'Medium')
                time.sleep(1)
                self._mod.request_clear_error_all_of_module('example')

            except KeyboardInterrupt:
                log.debug("Example program terminated manually")
                break
            self._ready_event.clear()

py_example = PyExampleModule()
py_example.start_example()
