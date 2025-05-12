# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, sys
current_dir = os.path.dirname(os.path.realpath(__file__))
generated_dir = os.path.join(current_dir, "..", "grpc_libs", "generated")
sys.path.insert(0, generated_dir)

import usecases.cs.lpc.service_pb2_grpc as cs_lpc_service_pb2_grpc
import usecases.cs.lpc.messages_pb2 as cs_lpc_messages_pb2

import logging
import queue
import time
import helpers.queue_helpers as queue_helpers

from .common import CommandQueues, default_command_func

class CsLpcControlServicer(cs_lpc_service_pb2_grpc.ControllableSystemLPCControlServicer):
    """
    This class implements the gRPC service for the Controllable System Limit Power Consumption use case.

    It puts received requests into a request_queue and waits for a response
    in the response_queue. The response is then returned to the client.

    On destruction, the queues are shut down and waited for to be empty.
    """
    def __init__(self):
        self._commands = [
            "ConsumptionLimit",
            "SetConsumptionLimit",
            "PendingConsumptionLimit",
            "ApproveOrDenyConsumptionLimit",
            "FailsafeConsumptionActivePowerLimit",
            "SetFailsafeConsumptionActivePowerLimit",
            "FailsafeDurationMinimum",
            "SetFailsafeDurationMinimum",
            "StartHeartbeat",
            "StopHeartbeat",
            "IsHeartbeatWithinDuration",
            "ConsumptionNominalMax",
            "SetConsumptionNominalMax"
        ]
        self.command_queues = {}

        for command in self._commands:
            self.command_queues[command] = CommandQueues(
                request_queue=queue.Queue(maxsize=1),
                response_queue=queue.Queue(maxsize=1)
            )
            setattr(self, command, lambda request, context, cmd=command: self._default_command_func(request, context, cmd))

    def __del__(self):
        try:
            for command in self._commands:
                queue_helpers.wait_for_queue_empty(self.command_queues[command].request_queue, 30)
                queue_helpers.wait_for_queue_empty(self.command_queues[command].response_queue, 30)
        except TimeoutError:
            raise TimeoutError("CsLpcControlServicer queues are not empty after 30 seconds")

    def _default_command_func(self, request, context, command):
        return default_command_func(self, request, context, command)
