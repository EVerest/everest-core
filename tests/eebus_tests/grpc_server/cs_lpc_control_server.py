# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import asyncio
import logging
import os
import pathlib

import grpc.aio as grpc_aio

from helpers.import_helpers import insert_dir_to_sys_path

insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")

import usecases.cs.lpc.service_pb2_grpc as cs_lpc_service_pb2_grpc


class CsLpcControlServer:
    """
    This class is a wrapper around the asyncio gRPC server for the Controllable System
    Limit Power Consumption (LPC) use case.
    """

    def __init__(self, servicer, port: int | None = None):
        self._server = grpc_aio.server()
        cs_lpc_service_pb2_grpc.add_ControllableSystemLPCControlServicer_to_server(
            servicer,
            self._server
        )
        if port is not None:
            self._port = self._server.add_insecure_port(f'[::]:{port}')
        else:
            self._port = self._server.add_insecure_port('[::]:0')

    def get_port(self) -> int:
        return self._port

    async def start(self):
        await self._server.start()

    async def stop(self):
        await self._server.stop(0)
