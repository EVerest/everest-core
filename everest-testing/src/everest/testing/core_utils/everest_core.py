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
from typing import List, TypedDict, Optional
import uuid
import yaml
import selectors
from signal import SIGINT

STARTUP_TIMEOUT = 30


class TestControlModuleConnection(TypedDict):
    evse_manager_id: str
    car_simulator_id: str


class StatusFifoListener:
    def __init__(self, status_fifo_path: Path):
        os.mkfifo(status_fifo_path)

        # note: open doesn't support non-blocking, so we use os.open to get the fd
        fd = os.open(status_fifo_path, flags=(os.O_RDONLY | os.O_NONBLOCK))
        self._file_obj = open(fd)

        selector = selectors.DefaultSelector()
        selector.register(self._file_obj, selectors.EVENT_READ)
        self._selector = selector

    def wait_for_status(self, timeout: float, match_status: list[str]) -> Optional[list[str]]:
        if match_status is None:
            match_status = []

        end_time = time.time() + timeout

        while True:
            for _key, _mask in self._selector.select(timeout):
                data = self._file_obj.read()
                if len(data) == 0:
                    return None

                # plural!
                received_status = data.splitlines()

                if len(match_status) == 0:
                    # we're not trying to match any messages
                    return received_status

                # return the filtered matched messages
                matched_status = [status for status in match_status if status in received_status]
                if len(matched_status) > 0:
                    return matched_status

            timeout = end_time - time.time()

            if timeout < 0:
                return []


class EverestCore:
    """This class can be used to configure, start and stop a full build of everest-core
    """

    def __init__(self, prefix_path: Path, config_path: Path = None) -> None:
        """Initialize EVerest using everest_core_path and everest_config_path

        Args:
            everest_prefix (Path): location of installed everest distribution".
        """

        self.process = None
        self.everest_uuid = uuid.uuid4().hex
        self.temp_dir = Path(tempfile.mkdtemp(prefix=self.everest_uuid))
        self.temp_everest_config_file = tempfile.NamedTemporaryFile(
            delete=False, mode="w+", suffix=".yaml", dir=self.temp_dir)
        self.everest_core_user_config_path = Path(
            self.temp_everest_config_file.name).parent / 'user-config'
        self.everest_core_user_config_path.mkdir(parents=True, exist_ok=True)
        self.prefix_path = prefix_path
        self.etc_path = Path('/etc/everest') if prefix_path == '/usr' else prefix_path / 'etc/everest'

        if config_path is None:
            config_path = self.etc_path / 'config-sil.yaml'

        everest_config = yaml.safe_load(config_path.read_text())

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
        logging.info(f"temp everest config: {self.everest_config_path} based on {config_path}")

        self.test_control_modules = None

        self.log_reader_thread: Thread = None
        self.everest_running = False

    def start(self, standalone_module: Optional[str] = None, modules_to_test: List[TestControlModuleConnection] = None):
        """Starts everest-core in a subprocess

        Args:
            standalone_module (str, optional): If set, a submodule can be started separately. EVerest will then wait for the submodule to be started.
             Defaults to None.
        """

        manager_path = self.prefix_path / 'bin/manager'

        logging.info(f'config: {self.everest_config_path}')

        # FIXME (aw): clean up passing of modules_to_test
        self.test_control_modules = modules_to_test
        self.create_testing_user_config()

        status_fifo_path = self.temp_dir / "status.fifo"
        status_listener = StatusFifoListener(status_fifo_path)
        logging.info(status_fifo_path)

        args = [str(manager_path.resolve()), '--config', str(self.everest_config_path),
                '--status-fifo', str(status_fifo_path)]

        if standalone_module is not None:
            logging.info(f"Standalone module {standalone_module} was specified")
            args.extend(['--standalone', standalone_module])

        logging.info(" ".join(args))

        logging.info('Starting EVerest...')
        logging.info('  '.join(args))

        self.process = subprocess.Popen(
            args, cwd=self.prefix_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        self.log_reader_thread = Thread(target=self.read_everest_log)
        self.log_reader_thread.start()

        expected_status = 'ALL_MODULES_STARTED' if standalone_module == None else 'WAITING_FOR_STANDALONE_MODULES'

        status = status_listener.wait_for_status(STARTUP_TIMEOUT, [expected_status])
        if status == None or len(status) == 0:
            raise TimeoutError("Timeout while waiting for EVerest to start")

        logging.info("EVerest has started")

    def read_everest_log(self):
        while self.process.poll() == None:
            stderr_raw = self.process.stderr.readline()
            stderr_formatted = stderr_raw.strip().decode()
            logging.debug(f'  {stderr_formatted}')

        if self.process.returncode == 0:
            logging.info("EVerest stopped with return code 0")
        elif self.process.returncode < 0:
            logging.info(f"EVerest stopped by signal {signal.Signals(-self.process.returncode).name}")
        else:
            logging.warning(f"EVerest stopped with return code: {self.process.returncode}")

        logging.debug("EVerest output stopped")

    def stop(self):
        """Stops execution of EVerest by signaling SIGINT
        """
        logging.debug("CONTROLLER stop() function called...")
        if self.process:
            # NOTE (aw): we could also call process.kill()
            self.process.send_signal(SIGINT)
            self.process.wait()

        if self.log_reader_thread:
            self.log_reader_thread.join()

    def create_testing_user_config(self):
        """Creates a user-config file to include the PyTestControlModule in the current SIL simulation.
        If a user-config already exists, it will be re-named
        """
        file = self.everest_core_user_config_path / self.everest_config_path.name
        logging.info(f"temp everest user-config: {file.resolve()}")
        if self.test_control_modules:
            logging.info(f"Adding test control module(s) to user-config: {self.test_control_modules}")
            user_config = {"active_modules": {}}
            connections = {"connector_1": [], "test_control": []}
            for test_control_module in self.test_control_modules:
                connections["connector_1"].append(
                    {"implementation_id": "evse", "module_id": test_control_module["evse_manager_id"]})
                connections["test_control"].append(
                    {"implementation_id": "main", "module_id": test_control_module["car_simulator_id"]})

            user_config["active_modules"]["probe_module"] = {
                "config_module": {"device": "auto"},
                "connections": connections,
                "module": "PyProbeModule"}

            file.write_text(yaml.dump(user_config))
