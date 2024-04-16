# Tests to run on the real hardware. This assumes that the Iskra meter is
# connected to the machine on the port specified under `tests.yaml`.
import logging
import pytest
import json
from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore, Requirement
from everest.testing.core_utils.probe_module import ProbeModule
from everest.framework import RuntimeSession


logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()


@pytest.mark.asyncio
async def test_meter(everest_core: EverestCore):
    """ """
    logger.info("Starting test")
    test_connections = {
        "meter": [Requirement("power_meter_1", "meter")],
    }
    everest_core.start(standalone_module="probe", test_connections=test_connections)

    session = RuntimeSession(
        str(everest_core.prefix_path), str(everest_core.everest_config_path)
    )
    probe = ProbeModule(session)
    probe.start()

    logger.info("Waiting for the init")
    if everest_core.status_listener.wait_for_status(5, ["ALL_MODULES_STARTED"]):
        everest_core.all_modules_started_event.set()

    await probe.wait_to_be_ready()
    start_request = {
        "value": {
            "evse_id": "ABC1234ABC1",
            "transaction_id": "MY_TRANSACTION_ID",
            "client_id": "MY_CLIENT_ID",
            "tariff_id": 1,
            "cable_id": 2,
            "user_data": "MY_USER_DATA",
        }
    }

    start_transaction_response = await probe.call_command(
        "meter",
        "start_transaction",
        start_request,
    )

    assert start_transaction_response["status"] == "OK"
    assert start_transaction_response.get("error", None) is None

    stop_request = {"transaction_id": "MY_TRANSACTION_ID"}
    stop_transaction_response = await probe.call_command(
        "meter",
        "stop_transaction",
        stop_request,
    )

    assert stop_transaction_response["status"] == "OK"
    assert stop_transaction_response.get("error", None) is None
    signed_meter_value = stop_transaction_response["signed_meter_value"]
    splits = signed_meter_value["signed_meter_data"].split("|")[1]
    data = json.loads(splits)

    assert start_request["value"]["evse_id"] in data["ID"]
    assert len(data["RD"]) == 2
