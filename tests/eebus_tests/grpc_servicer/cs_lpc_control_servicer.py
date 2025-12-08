# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os
import sys
import pathlib
import asyncio
import logging

# Add generated protobuf code to path
current_dir = os.path.dirname(os.path.realpath(__file__))
generated_dir = os.path.join(current_dir, "..", "grpc_libs", "generated")
sys.path.insert(0, generated_dir)

import usecases.cs.lpc.service_pb2_grpc as cs_lpc_service_pb2_grpc
from .common import CommandQueues, default_command_func


class CsLpcControlServicer(cs_lpc_service_pb2_grpc.ControllableSystemLPCControlServicer):
    """
    This class implements the asyncio gRPC service for the Controllable System
    Limit Power Consumption (LPC) use case.
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
                request_queue=asyncio.Queue(maxsize=1),
                response_queue=asyncio.Queue(maxsize=1)
            )

    async def ConsumptionLimit(self, request, context):
        return await default_command_func(self, request, context, "ConsumptionLimit")

    async def SetConsumptionLimit(self, request, context):
        return await default_command_func(self, request, context, "SetConsumptionLimit")

    async def PendingConsumptionLimit(self, request, context):
        return await default_command_func(self, request, context, "PendingConsumptionLimit")

    async def ApproveOrDenyConsumptionLimit(self, request, context):
        return await default_command_func(self, request, context, "ApproveOrDenyConsumptionLimit")

    async def FailsafeConsumptionActivePowerLimit(self, request, context):
        return await default_command_func(self, request, context, "FailsafeConsumptionActivePowerLimit")

    async def SetFailsafeConsumptionActivePowerLimit(self, request, context):
        return await default_command_func(self, request, context, "SetFailsafeConsumptionActivePowerLimit")

    async def FailsafeDurationMinimum(self, request, context):
        return await default_command_func(self, request, context, "FailsafeDurationMinimum")

    async def SetFailsafeDurationMinimum(self, request, context):
        return await default_command_func(self, request, context, "SetFailsafeDurationMinimum")

    async def StartHeartbeat(self, request, context):
        return await default_command_func(self, request, context, "StartHeartbeat")

    async def StopHeartbeat(self, request, context):
        return await default_command_func(self, request, context, "StopHeartbeat")

    async def IsHeartbeatWithinDuration(self, request, context):
        return await default_command_func(self, request, context, "IsHeartbeatWithinDuration")

    async def ConsumptionNominalMax(self, request, context):
        return await default_command_func(self, request, context, "ConsumptionNominalMax")

    async def SetConsumptionNominalMax(self, request, context):
        return await default_command_func(self, request, context, "SetConsumptionNominalMax")
