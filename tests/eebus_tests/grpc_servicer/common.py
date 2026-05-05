# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from dataclasses import dataclass
import asyncio
import logging

@dataclass
class CommandQueues:
    """
    Class to hold the queues for the commands
    """
    request_queue: asyncio.Queue
    response_queue: asyncio.Queue

async def default_command_func(self, request, context, command):
    """
    Default command function to handle the request and response queues
    """
    logging.info(f"Command {command} called")
    try:
        self.command_queues[command].request_queue.put_nowait(request)
    except asyncio.QueueFull:
        raise asyncio.QueueFull(f"{command} request queue is full, not able to put request")
    try:
        response = await asyncio.wait_for(self.command_queues[command].response_queue.get(), timeout=30)
    except asyncio.TimeoutError:
        raise asyncio.TimeoutError(f"{command} response queue is empty, not able to get response")
    return response
