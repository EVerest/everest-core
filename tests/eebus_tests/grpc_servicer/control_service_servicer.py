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

import control_service.control_service_pb2_grpc as control_service_pb2_grpc
from .common import CommandQueues, default_command_func


class ControlServiceServicer(control_service_pb2_grpc.ControlServiceServicer):
    """
    This class implements the asyncio gRPC service for the Control Service.
    """

    def __init__(self):
        self._stopped = False
        self._commands = [
            "StartService",
            "StopService",
            "SetConfig",
            "StartSetup",
            "AddEntity",
            "RemoveEntity",
            "RegisterRemoteSki",
            "AddUseCase",
            "SubscribeUseCaseEvents",
        ]
        self.command_queues = {}

        for command in self._commands:
            if command == "SubscribeUseCaseEvents":
                self.command_queues[command] = CommandQueues(
                    request_queue=asyncio.Queue(maxsize=1),
                    response_queue=asyncio.Queue()
                )
            else:
                self.command_queues[command] = CommandQueues(
                    request_queue=asyncio.Queue(maxsize=1),
                    response_queue=asyncio.Queue(maxsize=1)
                )

    def stop(self):
        logging.info("Stop called on ControlServiceServicer")
        self._stopped = True

    async def StartService(self, request, context):
        return await default_command_func(self, request, context, "StartService")

    async def StopService(self, request, context):
        return await default_command_func(self, request, context, "StopService")

    async def SetConfig(self, request, context):
        return await default_command_func(self, request, context, "SetConfig")

    async def StartSetup(self, request, context):
        return await default_command_func(self, request, context, "StartSetup")

    async def AddEntity(self, request, context):
        return await default_command_func(self, request, context, "AddEntity")

    async def RemoveEntity(self, request, context):
        return await default_command_func(self, request, context, "RemoveEntity")

    async def RegisterRemoteSki(self, request, context):
        return await default_command_func(self, request, context, "RegisterRemoteSki")

    async def AddUseCase(self, request, context):
        return await default_command_func(self, request, context, "AddUseCase")

    async def SubscribeUseCaseEvents(self, request, context):
        logging.info("SubscribeUseCaseEvents called")
        try:
            self.command_queues["SubscribeUseCaseEvents"].request_queue.put_nowait(request)
        except asyncio.QueueFull:
            raise asyncio.QueueFull("SubscribeUseCaseEvents request queue is full, not able to put request")

        while not self._stopped:
            try:
                res = await asyncio.wait_for(self.command_queues["SubscribeUseCaseEvents"].response_queue.get(), timeout=15)
                yield res
            except asyncio.TimeoutError:
                continue
