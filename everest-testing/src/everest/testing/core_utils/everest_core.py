# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import logging
import os
import signal
from threading import Thread
import time
import subprocess
from pathlib import Path
import tempfile
from typing import List, TypedDict
import uuid
import yaml

STARTUP_TIMEOUT = 30


class TestControlModuleConnection(TypedDict):
    evse_manager_id: str
    car_simulator_id: str


class EverestCore:
    """This class can be used to configure, start and stop a full build of everest-core
    """

    def __init__(self, everest_core_path=Path("~/checkout/everest-workspace/everest-core"),
                 everest_config_path=Path("~/checkout/everest-workspace/everest-core/config/config-sil.yaml"),
                 test_control_modules: List[TestControlModuleConnection] = [TestControlModuleConnection(evse_manager_id="connector_1", car_simulator_id="car_simulator")]) -> None:
        """Initialize EVerest using everest_core_path and everest_config_path

        Args:
            everest_core_path (Path, optional): Full path to everest-core. Defaults to "~/checkout/everest-workspace/everest-core".
            everest_config_path (Path, optional): Full path to EVerest config file. Defaults to "~/checkout/everest-workspace/everest-core/config/config-sil.yaml".
        """
        self.process = None
        self.everest_uuid = uuid.uuid4().hex
        self.temp_everest_config_dir = tempfile.mkdtemp(prefix=self.everest_uuid)
        self.temp_everest_config_file = tempfile.NamedTemporaryFile(
            delete=False, mode="w+", suffix=".yaml", dir=self.temp_everest_config_dir)
        self.everest_core_user_config_path = Path(
            self.temp_everest_config_file.name).parent / 'user-config'
        self.everest_core_user_config_path.mkdir(parents=True, exist_ok=True)

        # namespace everest with uids
        everest_config = yaml.safe_load(everest_config_path.read_text())

        self.mqtt_external_prefix = f"external_{self.everest_uuid}"
        everest_config["settings"] = {}
        everest_config["settings"]["mqtt_everest_prefix"] = f"everest_{self.everest_uuid}"
        everest_config["settings"]["mqtt_external_prefix"] = self.mqtt_external_prefix
        everest_config["settings"]["telemetry_prefix"] = f"telemetry_{self.everest_uuid}"

        # make sure controller starts with a dynamic port
        everest_config["settings"]["controller_port"] = 0

        try:
            everest_config["active_modules"]["iso15118_car"]["config_implementation"]["main"]["mqtt_prefix"] = self.mqtt_external_prefix
        except KeyError:
            logging.warning("Missing key in iso15118_car config")

        yaml.dump(everest_config, self.temp_everest_config_file)

        self.everest_config_path = Path(self.temp_everest_config_file.name)

        logging.info(f"everest uuid: {self.everest_uuid}")
        logging.info(f"temp everest config: {self.everest_config_path} based on {everest_config_path}")

        self.everest_core_path = everest_core_path

        self.everest_core_build_path = Path(
            self.everest_core_path / 'build')

        self.test_control_modules = test_control_modules
        self.log_reader_thread = None
        self.everest_running = False

    def start(self, standalone_module=None):
        """Starts everest-core in a subprocess

        Args:
            standalone_module (str, optional): If set, a submodule can be started separately. EVerest will then wait for the submodule to be started.
             Defaults to None.
        """
        LD_LIBRARY_PATH = self.everest_core_build_path / \
            Path('_deps/everest-framework-build/lib')
        manager_bin_path = Path('dist/bin/manager')

        logging.info(f'config: {self.everest_config_path}')

        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = LD_LIBRARY_PATH

        everest_dist_path = self.everest_core_build_path / 'dist'
        logging.info(everest_dist_path)
        args = [manager_bin_path, '--prefix',
                everest_dist_path, '--conf', str(self.everest_config_path)]

        logging.info(f"{manager_bin_path} --prefix {everest_dist_path} --conf {str(self.everest_config_path)}")
        self.create_testing_user_config()
        expected_log = 'Ready to start charging'
        if standalone_module is not None:
            logging.info(f"Standalone module {standalone_module} was specified")
            args.append('--standalone')
            args.append(str(standalone_module))
            expected_log = 'Modules started by manager are ready, waiting for standalone modules.'

        logging.info("Starting EVerest...")
        self.process = subprocess.Popen(
            args, env=env, cwd=self.everest_core_build_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        t_timeout = time.time() + STARTUP_TIMEOUT
        output = ''
        while expected_log not in output and time.time() < t_timeout:
            rl = self.process.stderr.readline()
            if rl == b'' and self.process.poll() is not None:
                continue
            if rl:
                output = rl.strip().decode('utf-8')
                logging.debug(f"  {output}")

        if expected_log not in output:
            logging.error("Timeout while waiting for EVerest to start")
            raise TimeoutError("Timeout while waiting for EVerest to start")

        self.everest_running = True
        logging.info("EVerest has started")

        self.log_reader_thread = Thread(target=self.read_everest_log).start()

    def read_everest_log(self):
        while self.everest_running:
            rl = self.process.stderr.readline()
            if rl == b'' and self.process.poll() is not None:
                if self.process.returncode == 0:
                    logging.info("EVerest stopped with return code 0")
                elif self.process.returncode < 0:
                    logging.info(f"EVerest stopped by signal {signal.Signals(-self.process.returncode).name}")
                else:
                    logging.warning(f"EVerest stopped with return code: {self.process.returncode}")
                break
            if rl:
                output = rl.strip().decode('utf-8')
                logging.debug(f"  {output}")
        logging.debug("EVerest output stopped")

    def stop(self):
        """Stops execution of EVerest by signaling SIGINT
        """
        logging.debug("CONTROLLER stop() function called...")
        if self.process != None:
            import signal
            self.process.send_signal(signal.SIGINT)
            self.process.wait()
        self.everest_running = False

    def create_testing_user_config(self):
        """Creates a user-config file to include the PyTestControlModule in the current SIL simulation.
        If a user-config already exists, it will be re-named
        """
        file = self.everest_core_user_config_path / self.everest_config_path.name
        filename = str(file)
        logging.info(f"temp everest user-config: {filename}")
        if self.test_control_modules:
            logging.info(f"Adding test control module(s) to user-config: {self.test_control_modules}")
            user_config = {"active_modules": {}}
            connections = {"connector_1": [], "test_control": []}
            for test_control_module in self.test_control_modules:
                connections["connector_1"].append(
                    {"implementation_id": "evse", "module_id": test_control_module["evse_manager_id"]})
                connections["test_control"].append(
                    {"implementation_id": "main", "module_id": test_control_module["car_simulator_id"]})
            user_config["active_modules"]["test_control_module"] = {
                "config_module": {"device": "auto"},
                "connections": connections,
                "module": "PyTestControlModule"}

            with open(file, "a") as f:
                yaml.dump(user_config, f)
