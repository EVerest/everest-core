# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, sys
current_dir = os.path.dirname(os.path.realpath(__file__))
generated_dir = os.path.join(current_dir, "..", "grpc_libs", "generated")
sys.path.insert(0, generated_dir)

import control_service.control_service_pb2_grpc as control_service_pb2_grpc
import control_service.messages_pb2 as messages_pb2
import logging
import queue
import time
import helpers.queue_helpers as queue_helpers
from .common import CommandQueues, default_command_func

class ControlServiceServicer(control_service_pb2_grpc.ControlServiceServicer):
    """
    This class implements the gRPC service for the Control Service Service.

    It puts received requests into a request_queue and waits for a response
    in the response_queue. The response is then returned to the client.

    Before desctruction, it needs to be stopped by calling the stop() method.
    On destruction, the queues are shut down and waited for to be empty.
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
                continue
            self.command_queues[command] = CommandQueues(
                request_queue=queue.Queue(maxsize=1),
                response_queue=queue.Queue(maxsize=1)
            )
            setattr(self, command, lambda request, context, cmd=command: self._default_command_func(request, context, cmd))
        
        self.command_queues["SubscribeUseCaseEvents"] = CommandQueues(
            request_queue=queue.Queue(maxsize=1),
            response_queue=queue.Queue()
        )

    def __del__(self):
        try:
            for command in self._commands:
                self.command_queues[command].request_queue.shutdown()
                self.command_queues[command].response_queue.shutdown()
                queue_helpers.wait_for_queue_empty(self.command_queues[command].request_queue, 30)
                queue_helpers.wait_for_queue_empty(self.command_queues[command].response_queue, 30)

        except TimeoutError:
            raise TimeoutError("ControlServiceServicer queues are not empty after 30 seconds")

    def _default_command_func(self, request, context, command):
        return default_command_func(self, request, context, command)
    
    def stop(self):
        logging.info("Stop called")
        self._stopped = True

    def SubscribeUseCaseEvents(self, request, context):
        logging.info("SubscribeUseCaseEvents called")
        try:
            self.command_queues["SubscribeUseCaseEvents"].request_queue.put(request, timeout=30)
        except queue.Full:
            raise queue.Full("SubscribeUseCaseEvents request queue is full, not able to put request")
        while(True):
            if self._stopped:
                return
            try:
                res = self.command_queues["SubscribeUseCaseEvents"].response_queue.get(timeout=5)
                yield res
            except queue.Empty:
                continue
