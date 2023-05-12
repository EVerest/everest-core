# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import logging
import os
import time
from pathlib import Path
import importlib
import sys
from os import environ


class TestControlModule:
    """This class can be used to configure, start and stop the EVerest PyTestControlModule
    """
    __test__ = False

    def __init__(self, everest_core_path: Path) -> None:
        """Initialization method for TestControlModule
        """
        self.process = None
        self.current_file_path = Path(__file__).absolute()
        self.everest_core_path = everest_core_path
        self.workspace_path = everest_core_path.parent
        self.everest_core_build_path = self.everest_core_path / 'build'
        self.everest_core_build_dist_path = self.everest_core_build_path / 'dist'
        self.everest_core_module_path = self.everest_core_build_dist_path / 'libexec/everest/modules'
        self.test_control_module_bin_path = self.everest_core_module_path / 'PyTestControlModule/module.py'
        self.everest_py_path =  self.everest_core_build_dist_path / 'lib/everest/everestpy/everest.py'
        self.event_list = []


    def get_event_list(self):
        """This method returns the event list filled by the PyTestControlModule via callback function.

        Returns:
            list: Event list of TestControlModule
        """
        return self.event_list


    def clear_event_list(self):
        """This method clears the internal event list.
        """
        self.event_list = []


    def event_occurred(self, event):
        logging.debug(f"Event occured {event}")
        self.event_list.append(event)


    def run(self, config_file):
        """This method starts the everest-core "test_control_module" in a subprocess.

        Args:
            config_file (str): Full path to everest-core configuration file

        Returns:
            _type_: The TestControlModule subprocess
        """

        LD_LIBRARY_PATH = self.everest_core_build_path / '_deps/everest-framework-build/lib'
        env = dict(os.environ)
        env['LD_LIBRARY_PATH'] = LD_LIBRARY_PATH

        environ["EV_PYTHON_MODULE"] = self.test_control_module_bin_path.as_posix()

        # make sure everest.py path is available and import module
        sys.path.append(self.everest_py_path.parent.as_posix())
        sys.path.append(self.everest_py_path.parent.parent.as_posix())
        spec = importlib.util.spec_from_file_location("everest", self.everest_py_path)
        self.everest = importlib.util.module_from_spec(spec)
        sys.modules["everest"] = self.everest
        spec.loader.exec_module(self.everest)

        environ["EV_MODULE"] = "test_control_module"
        environ["EV_PREFIX"] = self.everest_core_build_dist_path.as_posix()
        environ["EV_CONF_FILE"] = config_file.as_posix()

        EV_MODULE = environ.get('EV_MODULE')
        EV_PREFIX = environ.get('EV_PREFIX')
        EV_CONF_FILE = environ.get('EV_CONF_FILE')

        self.everest.everestpy.register_module_adapter_callback(self.everest.register_module_adapter)
        self.everest.everestpy.register_everest_register_callback(self.everest.register_everest_register)
        self.everest.everestpy.register_init_callback(self.everest.register_init)
        self.everest.everestpy.register_pre_init_callback(self.everest.register_pre_init)
        self.everest.everestpy.register_ready_callback(self.everest.register_ready)

        logging.debug("Starting everest-core test control module...")
        self.everest.everestpy.init(EV_PREFIX, EV_CONF_FILE, EV_MODULE)
        return self.everest.module


    def start(self, everest_config_path):
        """Starts the TestControlModule submodule

        Returns:
            _type_: The subprocess
        """

        self.process = self.run(everest_config_path)
        time.sleep(2) #FIXME: wait properly for module start
        self.process.set_event_callback(self.event_occurred)
        return self.process


    def stop(self):
        """Stops the TestControlModule submodule
        """
        logging.debug("CONTROL MODULE stop() function called...")

