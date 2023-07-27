
import pytest
import asyncio
from datetime import datetime

from ocpp.v201 import call_result, call
from ocpp.v201.datatypes import SetVariableResultType, IdTokenType
from ocpp.v201.enums import SetVariableStatusType, IdTokenType as IdTokenTypeEnum, ClearCacheStatusType, ConnectorStatusType

from everest.testing.ocpp_utils.fixtures import *
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, OcppTestConfiguration, ChargePointInfo
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16


def validate_status_notification_201(meta_data, msg, exp_payload):
    return msg.payload['connectorStatus'] == exp_payload.connector_status and \
        msg.payload['evseId'] == exp_payload.evse_id and \
        msg.payload['connectorId'] == exp_payload.connector_id


@ pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp1.6")
@pytest.mark.everest_core_config("/home/piet/code/everest-workspace/ocpp-testing/test_sets/ocpp_tests/everest-aux/config/everest-config-sil-ocpp.yaml")
async def test_ocpp_16(test_config: OcppTestConfiguration, charge_point_v16: ChargePoint16, test_controller: TestController):
    await charge_point_v16.get_configuration_req(key=["AuthorizeRemoteTxRequests"])
    await charge_point_v16.change_configuration_req(key="MeterValueSampleInterval", value="10")

    # send RemoteStartTransaction.req
    await charge_point_v16.remote_start_transaction_req(id_tag=test_config.authorization_info.valid_id_tag_1, connector_id=1)
    await charge_point_v16.wait_for_specific_message(
        "RemoteStartTransaction", {"status": "Accepted"})

    # start charging session
    test_controller.plug_in()

    # expect StartTransaction.req
    await charge_point_v16.wait_for_specific_message("StartTransaction", {
                                               "connectorId": 1, "idTag": test_config.authorization_info.valid_id_tag_1, "meterStart": 0})

    await charge_point_v16.wait_for_specific_message("StatusNotification", {"connectorId": 1, "status": "Preparing"})
    await charge_point_v16.wait_for_specific_message("StatusNotification", {"connectorId": 1, "status": "Charging"})

    await charge_point_v16.remote_stop_transaction_req(transaction_id=1)
    await charge_point_v16.wait_for_specific_message(
        "RemoteStopTransaction", {"status": "Accepted"})
    
    await charge_point_v16.wait_for_specific_message("StopTransaction", {"reason": "Remote"})
    await charge_point_v16.wait_for_specific_message("StatusNotification", {"connectorId": 1, "status": "Finishing"})

    test_controller.plug_out()

    await charge_point_v16.wait_for_specific_message("StatusNotification", {"connectorId": 1, "status": "Available"})


@ pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("/home/piet/code/everest-workspace/ocpp-testing/test_sets/ocpp_tests/everest-aux/config/everest-config-ocpp201.yaml")
async def test_ocpp_201(charge_point_v201: ChargePoint201, test_controller: TestController, test_utility: TestUtility):
    """This test case tests some requirements around AuthorizationCache of OCPP2.0.1

    Args:
        charge_point_v201 (ChargePoint201): this fixture starts up a OCPP2.0.1 CSMS and everest-core connection to this CSMS using OCPP. The reference can be used to send and receive messages over OCPP
        test_controller (TestController): this fixture is used to control the simulation
        test_utility (TestUtility): this fixture carries meta data of the test case that can be used for validations
    """

    # prepare data for the test
    evse_id = 1
    connector_id = 1

    # Enable AuthCacheCtrlr
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCacheCtrlr", "Enabled", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Enable LocalPreAuthorize
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "LocalPreAuthorize", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Set AuthCacheLifeTime
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCacheCtrlr", "LifeTime", "86400")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Clear cache
    r: call_result.ClearCachePayload = await charge_point_v201.clear_cache_req()
    assert r.status == ClearCacheStatusType.accepted

    test_controller.swipe("DEADBEEF")
    assert await wait_for_and_validate(test_utility, charge_point_v201, "Authorize", call.AuthorizePayload(
        id_token=IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )
    ))

    test_controller.plug_in()
    # eventType=Started
    await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Started"})
    test_utility.messages.clear()
    test_controller.plug_out()
    # eventType=Ended
    await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Ended"})

    test_utility.messages.clear()

    await asyncio.sleep(2)

    # because LocalPreAuthorize is true we dont expect an Authorize.req this time
    test_utility.forbidden_actions.append("Authorize")

    test_controller.swipe("DEADBEEF")
    test_controller.plug_in()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification",
                                       call.StatusNotificationPayload(datetime.now().isoformat(),
                                                                      ConnectorStatusType.occupied, evse_id, connector_id),
                                       validate_status_notification_201)

    # because LocalPreAuthorize is true we dont expect an authorize here
    r: call.TransactionEventPayload = call.TransactionEventPayload(**await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Started"}))

    # Disable LocalPreAuthorize
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "LocalPreAuthorize", "false")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    # Set AuthCacheLifeTime to 1s
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCacheCtrlr", "LifeTime", "1")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    test_utility.messages.clear()
    test_controller.plug_out()

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification",
                                       call.StatusNotificationPayload(datetime.now().isoformat(),
                                                                      ConnectorStatusType.available, evse_id, connector_id),
                                       validate_status_notification_201)

    # eventType=Ended
    await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Ended"})

    test_utility.forbidden_actions.clear()

    test_controller.swipe("DEADBEEF")

    assert await wait_for_and_validate(test_utility, charge_point_v201, "Authorize", call.AuthorizePayload(
        id_token=IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )
    ))

    # Enable LocalPreAuthorize
    r: call_result.SetVariablesPayload = await charge_point_v201.set_config_variables_req("AuthCtrlr", "LocalPreAuthorize", "true")
    set_variable_result: SetVariableResultType = SetVariableResultType(
        **r.set_variable_result[0])
    assert set_variable_result.attribute_status == SetVariableStatusType.accepted

    test_controller.plug_in()
    # eventType=Started
    await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Started"})
    test_utility.messages.clear()
    test_controller.plug_out()
    # eventType=Ended
    await charge_point_v201.wait_for_specific_message("TransactionEvent", {"eventType": "Ended"})

    assert await wait_for_and_validate(test_utility, charge_point_v201, "StatusNotification",
                                       call.StatusNotificationPayload(datetime.now().isoformat(),
                                                                      ConnectorStatusType.available, evse_id, connector_id),
                                       validate_status_notification_201)

    # AuthCacheLifeTime expired so sending Authorize.req
    test_controller.swipe("DEADBEEF")
    assert await wait_for_and_validate(test_utility, charge_point_v201, "Authorize", call.AuthorizePayload(
        id_token=IdTokenType(
            id_token="DEADBEEF",
            type=IdTokenTypeEnum.iso14443
        )
    ))
