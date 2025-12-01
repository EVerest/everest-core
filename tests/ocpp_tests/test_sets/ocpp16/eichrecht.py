import pytest

from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.central_system import ChargePoint16
from everest.testing.ocpp_utils.fixtures import test_utility, charge_point_v16
from everest_test_utils import get_everest_config_path_str

from ocpp.v16 import call_result


@pytest.mark.asyncio
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-two-connectors.yaml")
)
async def test_meter_public_key(
    charge_point_v16: ChargePoint16, test_utility: TestUtility
):
    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[1]"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfiguration(
            [{"key": "MeterPublicKey[1]", "readonly": True, "value": "TESTPUBLICKEY1"}]
        ),
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[2]"])
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        call_result.GetConfiguration(
            [{"key": "MeterPublicKey[2]", "readonly": True, "value": "TESTPUBLICKEY2"}]
        ),
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[3]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[3]"]}
    )

    test_utility.messages.clear()

    response : call_result.GetConfiguration = await charge_point_v16.get_configuration_req()

    assert any(
        entry['key'] == "MeterPublicKey[1]" and entry['value'] == "TESTPUBLICKEY1"
        for entry in response.configuration_key)

    assert any(
        entry['key'] == "MeterPublicKey[2]" and entry['value'] == "TESTPUBLICKEY2"
        for entry in response.configuration_key)
    
    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[]"]}
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[MeterPublicKey[1]]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[MeterPublicKey[1]]"]}
    )
    
    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[1X"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[1X"]}
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[1X]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[1X]"]}
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[banana]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[banana]"]}
    )

    await charge_point_v16.get_configuration_req(key=["MeterPublicKey[0]"])

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "GetConfiguration",
        {"unknownKey": ["MeterPublicKey[0]"]}
    )

    await charge_point_v16.change_configuration_req(
        key="MeterPublicKey[1]", value="TEST"
    )
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "ChangeConfiguration",
        {"status": "Rejected"}
    )
