# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from everest.framework import Module, RuntimeSession, log, error
import threading

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

class PyExampleErrorSubscriberModule():
    def __init__(self) -> None:
        self._session = RuntimeSession()
        m = Module(self._session)

        log.update_process_name(m.info.id)

        self._setup = m.say_hello()

        self._ready_event = threading.Event()

        self._req_example_raiser = self._setup.connections['example_raiser'][0]
        m.subscribe_error(
            self._req_example_raiser,
            'example/ExampleErrorA',
            self.handle_error,
            self.handle_error_cleared
        )
        m.subscribe_all_errors(
           self._req_example_raiser,
           self.handle_error,
           self.handle_error_cleared
        )

        self._mod = m
        self._mod.init_done(self._ready)

    def handle_error(self, error):
        log.info("Received error: '" + error.type + "' '" + error.sub_type + "' '" + error.message + "'")
        self.check_conditions()

    def handle_error_cleared(self, error):
        log.info("Received error cleared: '" + error.type + "' '" + error.sub_type + "' '" + error.message + "'")
        self.check_conditions()

    def _ready(self):
        self._ready_event.set()
        log.debug("ready!")

    def start_example(self):
        while True:
            self._ready_event.wait()
            try:
                log.info("Example program started")

            except KeyboardInterrupt:
                log.debug("Example program terminated manually")
                break
            self._ready_event.clear()

    def check_conditions(self):
        monitor = self._mod.get_error_state_monitor_req(self._req_example_raiser)
        for entry in condition_lists:
            if monitor.is_condition_satisfied(entry["conditions"]):
                log.info(f"Condition '{entry['name']}' is satisfied")
            else:
                log.info(f"Condition '{entry['name']}' is not satisfied")
        log.info("")
        log.info("")

py_example_error_subscriber = PyExampleErrorSubscriberModule()
py_example_error_subscriber.start_example()
