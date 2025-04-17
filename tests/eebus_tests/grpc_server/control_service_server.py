# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import grpc_servicer.control_service_servicer as control_service_servicer
import grpc
from concurrent import futures
import logging
import threading

import os, pathlib
from helpers.import_helpers import insert_dir_to_sys_path
insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")

import control_service.control_service_pb2_grpc as control_service_pb2_grpc

class ControlServiceServer:
    """
    This class is a wrapper around the gRPC server for the Control Service.
    It is used to start and stop the server, and to get the port it is running on.
    It takes a servicer as an argument, which is used to handle the requests.
    The server is started in a separate thread, and the port is assigned dynamically.
    """
    def __init__(self, servicer, port: int | None = None):
        self._started = False
        self._server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
        control_service_pb2_grpc.add_ControlServiceServicer_to_server(
            servicer,
            self._server
        )
        if port is not None:
            self._port = self._server.add_insecure_port(f'[::]:{port}')
        else:
            self._port = self._server.add_insecure_port('[::]:0')

    def __del__(self):
        if self._started:
            # self.stop()
            return
        
    def get_port(self) -> int:
        return self._port

    def start(self):
        logging.debug("Starting ControlServiceServer")
        self._started = True
        self._server.start()
        channel = grpc.insecure_channel(f'localhost:{self._port}')
        ready_event = threading.Event()
        def wait_for_ready(channel_connectivity):
            if channel_connectivity == grpc.ChannelConnectivity.TRANSIENT_FAILURE:
                logging.error("ControlServiceServer did not start in time")
                raise TimeoutError("ControlServiceServer did not start in time")
            elif channel_connectivity == grpc.ChannelConnectivity.IDLE:
                logging.debug("ControlServiceServer started successfully")
                ready_event.set()
            else:
                logging.debug(f"ControlServiceServer channel state: {channel_connectivity}")
        channel.subscribe(wait_for_ready)
        res = ready_event.wait(timeout = 50)
        if not res:
            logging.error("ControlServiceServer did not start in time")
            raise TimeoutError("ControlServiceServer did not start in time")
        logging.debug("ControlServiceServer started successfully")
        channel.close()

    def stop(self):
        logging.debug("Stopping ControlServiceServer")
        self._server.stop(None)
        self._started = False
