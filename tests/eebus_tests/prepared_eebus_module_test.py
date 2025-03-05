# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import queue
import threading
import os, pathlib
import logging

from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController

from grpc_servicer.control_service_servicer import ControlServiceServicer
from grpc_server.control_service_server import ControlServiceServer
from grpc_servicer.cs_lpc_control_servicer import CsLpcControlServicer
from grpc_server.cs_lpc_control_server import CsLpcControlServer

from helpers.conversions import convert_external_limits
from helpers import queue_helpers
from helpers.import_helpers import insert_dir_to_sys_path

insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "grpc_libs/generated")
import control_service.messages_pb2 as control_service_messages_pb2
import control_service.types_pb2 as control_service_types_pb2
import usecases.cs.lpc.messages_pb2 as cs_lpc_messages_pb2

class PreparedEEBUSModuleTest:
    """
    This class provides the complete environment for the EEBUS module tests
    and is used to test the EEBUS module.

    It needs to be setup and torn down, before and after the tests.
    """
    class ExtendedProbeModule(ProbeModule):
        """
        This class extends the ProbeModule to add a command for receiving
        external limits from the EEBUS module.

        Received external limits are put into self.external_limits_queue, which can be used
        to check the limits in the test.

        The queue needs to be emptied to delete the ProbeModule.
        """
        def __init__(self, runtime_session):
            super().__init__(runtime_session)


            super().implement_command(
                "eebus_energy_sink",
                "set_external_limits",
                lambda arg: self._set_external_limits(arg),
            )
            self.external_limits_queue = queue.Queue(maxsize=1)

        def _set_external_limits(self, limits):
            limits = convert_external_limits(limits)
            self.external_limits_queue.put_nowait(limits)
            logging.debug(f"Set external limits: {limits}")

        def __del__(self):
            try:
                self.external_limits_queue.shutdown()
                queue_helpers.wait_for_queue_empty(self.external_limits_queue, 30)
            except TimeoutError:
                raise TimeoutError("ProbeModule queues are not empty after 30 seconds")

    def __init__(
            self,
            everest_core: EverestCore,
            test_controller: EverestTestController,
            control_service_server: ControlServiceServer,
            control_service_servicer: ControlServiceServicer,
            cs_lpc_control_server: CsLpcControlServer,
            cs_lpc_control_servicer: CsLpcControlServicer,
            activate_assertions: bool = False,
    ):
        self._setup = False
        self._tear_down = False
        self._assertions = activate_assertions
        
        self.probe_module = None
        self._everest_core = everest_core
        self._test_controller = test_controller

        self._control_service_server = control_service_server
        self._control_service_servicer = control_service_servicer
        self._cs_lpc_control_server = cs_lpc_control_server
        self._cs_lpc_control_servicer = cs_lpc_control_servicer
    
    async def setup(self):
        """
        Sets up the complete environment for the EEBUS module tests.

        It can only be called once and needs to be torn down after the tests.
        """
        if self._setup:
            raise RuntimeError("Already setup")
        if self._tear_down:
            raise RuntimeError("Already torn down")

        self._control_service_server.start()
        self._cs_lpc_control_server.start()
        thread_tc_start = threading.Thread(target=self._test_controller.start).start()

        req = self._control_service_servicer.command_queues["SetConfig"].request_queue.get(timeout=60)
        if self._assertions:
            assert req is not None
            assert req.port == 4715
            assert req.vendor_code == "Demo"
            assert req.device_brand == "Demo"
            assert req.device_model == "EVSE"
            assert req.serial_number == "2345678901"
            assert len(req.device_categories) == 1
            assert req.device_categories[0] == control_service_types_pb2.DeviceCategory.E_MOBILITY
            assert req.device_type == control_service_types_pb2.DeviceType.CHARGING_STATION
            assert len(req.entity_types) == 1
            assert req.entity_types[0] == control_service_types_pb2.EntityType.EVSE
            assert req.heartbeat_timeout_seconds == 4
        self._control_service_servicer.command_queues["SetConfig"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )

        req = self._control_service_servicer.command_queues["StartSetup"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
        self._control_service_servicer.command_queues["StartSetup"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )

        req = self._control_service_servicer.command_queues["RegisterRemoteSki"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert req.remote_ski == "this-is-a-ski-42"  # see config-test-eebus-module-001.yaml
        self._control_service_servicer.command_queues["RegisterRemoteSki"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )

        req = self._control_service_servicer.command_queues["AddUseCase"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert len(req.entity_address.entity_address) == 1
            assert req.entity_address.entity_address[0] == 1
            assert req.use_case.actor == control_service_types_pb2.UseCase.ActorType.ControllableSystem
            assert req.use_case.name == control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption
        res = control_service_messages_pb2.AddUseCaseResponse(
            endpoint=f"localhost:{ self._cs_lpc_control_server.get_port() }"
        )
        self._control_service_servicer.command_queues["AddUseCase"].response_queue.put_nowait(res)

        req = self._cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert req.value == 32000
        
        self._cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetConsumptionNominalMaxResponse()
        )

        req = self._cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert req.load_limit.duration_nanoseconds == 0
            assert req.load_limit.is_changeable == True
            assert req.load_limit.is_active == False
            assert req.load_limit.value == 4200
            assert req.load_limit.delete_duration == False
        self._cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetConsumptionLimitResponse()
        )

        req = self._cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert req.value == 4200
            assert req.is_changeable == True
        self._cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetFailsafeConsumptionActivePowerLimitResponse()
        )

        req = self._cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert req.duration_nanoseconds == 2 * 60 * 60 * 1000 * 1000 * 1000
            assert req.is_changeable == True
        self._cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetFailsafeDurationMinimumResponse()
        )

        req = self._control_service_servicer.command_queues["SubscribeUseCaseEvents"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
            assert len(req.entity_address.entity_address) == 1
            assert req.entity_address.entity_address[0] == 1
            assert req.use_case.actor == control_service_types_pb2.UseCase.ActorType.ControllableSystem
            assert req.use_case.name == control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption
        try:
            thread_tc_start.join(timeout=30)
        except AttributeError:
            logging.debug("Thread already joined")
        
        self.probe_module = self.ExtendedProbeModule(self._everest_core.get_runtime_session())
        self.probe_module.start()
        await self.probe_module.wait_to_be_ready(timeout=30)

        req = self._control_service_servicer.command_queues["StartService"].request_queue.get(timeout=5)
        if self._assertions:
            assert req is not None
        self._control_service_servicer.command_queues["StartService"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )

        self._setup = True

    def tear_down(self):
        """
        It tears down the complete environment for the EEBUS module tests.

        It can only be called once and needs to be setup before the tests.
        """
        if not self._setup:
            raise RuntimeError("Not setup")
        if self._tear_down:
            raise RuntimeError("Already torn down")

        self._control_service_servicer.stop()
        self._test_controller.stop()

        self._tear_down = True

    def __del__(self):
        if self._setup and not self._tear_down:
            raise RuntimeError("EEBUSModule not torn down, before deleting")
        if not self._setup and not self._tear_down:
            logging.warning("EEBUSModule not setup, not used?")
