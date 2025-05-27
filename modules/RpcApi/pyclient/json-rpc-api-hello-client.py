#!/usr/bin/env python

import json
from jsonrpcclient import request, parse
from websockets.sync.client import connect


def hello():
    req = json.dumps(request("API.Hello"))
    with connect("ws://localhost:8080") as websocket:
        websocket.send(req)
        response = websocket.recv()
        print(f"Received: {response}")

        print


hello()
