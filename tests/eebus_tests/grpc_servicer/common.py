# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from dataclasses import dataclass
import queue
import logging

@dataclass
class CommandQueues:
    """
    Class to hold the queues for the commands
    """
    request_queue: queue.Queue
    response_queue: queue.Queue

def default_command_func(self, request, context, command):
    """
    Default command function to handle the request and response queues
    """
    logging.info(f"Command {command} called")
    try:
        self.command_queues[command].request_queue.put(request, timeout=30)
    except queue.Full:
        raise queue.Full(f"{command} request queue is full, not able to put request")
    try:
        response = self.command_queues[command].response_queue.get(timeout=30)
    except queue.Empty:
        raise queue.Empty(f"{command} response queue is empty, not able to get response")
    return response
