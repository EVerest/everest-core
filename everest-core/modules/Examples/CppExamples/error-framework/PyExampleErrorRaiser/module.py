# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.framework import Module, RuntimeSession, log, error
import time, threading

condition_lists = [
    {
        "name": "Only A is active",
        "conditions": [
            error.ErrorStateCondition("example/ExampleErrorA", "example sub type", True),
            error.ErrorStateCondition("example/ExampleErrorB", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorC", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorD", "example sub type", False),
        ],
    },
    {
        "name": "Only B is active",
        "conditions": [
            error.ErrorStateCondition("example/ExampleErrorA", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorB", "example sub type", True),
            error.ErrorStateCondition("example/ExampleErrorC", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorD", "example sub type", False),
        ],
    },
    {
        "name": "Only C & D are active",
        "conditions": [
            error.ErrorStateCondition("example/ExampleErrorA", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorB", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorC", "example sub type", True),
            error.ErrorStateCondition("example/ExampleErrorD", "example sub type", True),
        ],
    },
    {
        "name": "No error is active",
        "conditions": [
            error.ErrorStateCondition("example/ExampleErrorA", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorB", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorC", "example sub type", False),
            error.ErrorStateCondition("example/ExampleErrorD", "example sub type", False),
        ],
    }
]

class PyExampleErrorRaiserModule():
    def __init__(self) -> None:
        self._session = RuntimeSession()
        m = Module(self._session)

        log.update_process_name(m.info.id)

        self._ready_event = threading.Event()

        self._setup = m.say_hello()

        self._mod = m
        self._mod.init_done(self._ready)

    def _ready(self):
        log.debug("ready!")
        self._ready_event.set()

    def check_conditions(self):
        monitor = self._mod.get_error_state_monitor_impl("example_raiser")
        for entry in condition_lists:
            if monitor.is_condition_satisfied(entry["conditions"]):
                log.info(f"Condition '{entry['name']}' is satisfied")
            else:
                log.info(f"Condition '{entry['name']}' is not satisfied")

    def start_example(self):
        while True:
            self._ready_event.wait()
            try:
                error_factory = self._mod.get_error_factory("example_raiser")
                error = error_factory.create_error("example/ExampleErrorA", "example sub type", "example error message")
                self._mod.raise_error("example_raiser", error)
                self.check_conditions()
                time.sleep(1)
                self._mod.clear_error("example_raiser", "example/ExampleErrorA", "example sub type")
                self.check_conditions()
                time.sleep(1)
                error = error_factory.create_error("example/ExampleErrorB", "example sub type", "example error message")
                self._mod.raise_error("example_raiser", error)
                self.check_conditions()
                time.sleep(1)
                self._mod.clear_error("example_raiser", "example/ExampleErrorB", "example sub type")
                self.check_conditions()
                time.sleep(1)
                error = error_factory.create_error("example/ExampleErrorC", "example sub type", "example error message")
                self._mod.raise_error("example_raiser", error)
                self.check_conditions()
                time.sleep(1)
                error = error_factory.create_error("example/ExampleErrorD", "example sub type", "example error message")
                self._mod.raise_error("example_raiser", error)
                self.check_conditions()
                time.sleep(1)
                self._mod.clear_all_errors_of_impl("example_raiser")
                self.check_conditions()
                time.sleep(1)
            except KeyboardInterrupt:
                log.debug("Example program terminated manually")
                break
            self._ready_event.clear()

py_example_error_raiser = PyExampleErrorRaiserModule()
py_example_error_raiser.start_example()
