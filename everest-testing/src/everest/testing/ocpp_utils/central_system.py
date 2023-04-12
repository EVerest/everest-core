# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import asyncio
import time
import logging
import websockets
from ocpp.routing import create_route_map
from ocpp.charge_point import ChargePoint

from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201


logging.basicConfig(level=logging.debug)


class CentralSystem:

    """Wrapper for CSMS websocket server. Holds a reference to a single connected chargepoint
    """

    def __init__(self, port, chargepoint_id, ocpp_version):
        self.name = "CentralSystem"
        self.port = port
        self.chargepoint_id = chargepoint_id
        self.ocpp_version = ocpp_version
        self.ws_server = None
        self.chargepoint = None
        self.chargepoint_set_event = asyncio.Event()
        self.function_overrides = []

    async def on_connect(self, websocket, path):
        """ For every new charge point that connects, create a ChargePoint
        instance and start listening for messages.
        """
        chargepoint_id = path.strip('/')
        if chargepoint_id == self.chargepoint_id:
            logging.debug(f"Chargepoint {chargepoint_id} connected")
            try:
                requested_protocols = websocket.request_headers[
                    'Sec-WebSocket-Protocol']
            except KeyError:
                logging.error(
                    "Client hasn't requested any Subprotocol. Closing Connection"
                )
                return await websocket.close()
            if websocket.subprotocol:
                logging.debug("Protocols Matched: %s", websocket.subprotocol)
            else:
                # In the websockets lib if no subprotocols are supported by the
                # client and the server, it proceeds without a subprotocol,
                # so we have to manually close the connection.
                logging.warning('Protocols Mismatched | Expected Subprotocols: %s,'
                                ' but client supports  %s | Closing connection',
                                websocket.available_subprotocols,
                                requested_protocols)
                return await websocket.close()

            if self.ocpp_version == 'ocpp1.6':
                cp = ChargePoint16(chargepoint_id, websocket)
            else:
                cp = ChargePoint201(chargepoint_id, websocket)
            self.chargepoint = cp
            self.chargepoint.pipe = True
            for override in self.function_overrides:
                setattr(self.chargepoint, override[0], override[1])
            self.chargepoint.route_map = create_route_map(self.chargepoint)

            self.chargepoint_set_event.set()
            await self.chargepoint.start()
        else:
            logging.warning(
                f"Connection on invalid path {chargepoint_id} received. Check the configuration of the ChargePointId.")
            return await websocket.close()

    async def wait_for_chargepoint(self, timeout=30, wait_for_bootnotification=True) -> ChargePoint:
        """Waits for the chargepoint to connect to the CSMS

        Args:
            timeout (int, optional): time in seconds until timeout occurs. Defaults to 30.
            wait_for_bootnotification (bool, optional): Indiciates if this method should wait until the chargepoint sends a BootNotification. Defaults to True.

        Returns:
            ChargePoint: reference to ChargePoint16 or ChargePoint201
        """
        try:
            logging.debug("Waiting for chargepoint to connect")
            await asyncio.wait_for(self.chargepoint_set_event.wait(), timeout)
            logging.debug("Chargepoint connected!")
            self.chargepoint_set_event.clear()
        except asyncio.exceptions.TimeoutError:
            raise asyncio.exceptions.TimeoutError("Timeout while waiting for the chargepoint to connect.")

        if wait_for_bootnotification:
            t_timeout = time.time() + timeout
            received_boot_notification = False
            while (time.time() < t_timeout and not received_boot_notification):
                raw_message = await asyncio.wait_for(self.chargepoint.wait_for_message(), timeout=timeout)
                # FIXME(piet): Make proper check for BootNotification
                received_boot_notification = "BootNotification" in raw_message

            if not received_boot_notification:
                raise asyncio.exceptions.TimeoutError("Timeout while waiting for BootNotification.")

        await asyncio.sleep(1)
        return self.chargepoint

    async def start(self, ssl_context=None):
        """Starts the websocket server
        """
        self.ws_server = await websockets.serve(
            self.on_connect,
            '127.0.0.1',
            self.port,
            subprotocols=[self.ocpp_version],
            ssl=ssl_context
        )
        if self.port is None:
            self.port = self.ws_server.sockets[0].getsockname()[1]
            logging.info(f"Server port was not set, setting to {self.port}")
        logging.debug(f"Server Started listening to new {self.ocpp_version} connections.")

    async def stop(self):
        """Stops the websocket server
        """
        self.ws_server.close()
        await self.ws_server.wait_closed()
