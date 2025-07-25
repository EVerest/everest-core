# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
from copy import deepcopy
from typing import Dict
from datetime import datetime
from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.ocpp_utils.fixtures import central_system_v201, CentralSystem
from everest.testing.core_utils import EverestConfigAdjustmentStrategy
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from validations import validate_notify_report_data_201

from ocpp.v201 import call
from ocpp.v201 import call_result as call_result
from ocpp.v201.datatypes import *
from ocpp.v201.enums import (
    GetVariableStatusEnumType,
    AttributeEnumType,
    SetVariableStatusEnumType,
    ReportBaseEnumType,
    MutabilityEnumType,
    DataEnumType,
    GenericDeviceModelStatusEnumType,
)


class OCPP201ModuleAccessConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["ocpp"]["access"] = {
            "config": {
                "allow_global_read": True,
                "allow_global_write": True,
                "allow_set_read_only": True,
            }
        }

        return adjusted_config


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_config_adaptions(OCPP201ModuleAccessConfigurationAdjustment())
async def test_set_get_variable_01(
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
    test_controller: TestController,
    central_system_v201: CentralSystem,
):
    """Test setting and getting variables in the Everest Device Model.
    This test gets the initial values of the `session_logging` and `session_logging_path` variables,
    sets new values for them, and then verifies that the values are correctly set after a restart .
    """

    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging_path"),
                attribute_type=AttributeEnumType.actual,
            ),
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="true",
                ),
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging_path"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="/tmp",
                ),
            ]
        ),
    )

    await charge_point_v201.set_variables_req(
        set_variable_data=[
            SetVariableDataType(
                attribute_value="false",
                attribute_type=AttributeEnumType.actual,
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging"),
            ),
            SetVariableDataType(
                attribute_value="/another/path",
                attribute_type=AttributeEnumType.actual,
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging_path"),
            ),
        ]
    )

    await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "SetVariables",
        call_result.SetVariables(
            set_variable_result=[
                SetVariableResultType(
                    attribute_status=SetVariableStatusEnumType.reboot_required,
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging"),
                    attribute_type=AttributeEnumType.actual,
                ),
                SetVariableResultType(
                    attribute_status=SetVariableStatusEnumType.reboot_required,
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging_path"),
                    attribute_type=AttributeEnumType.actual,
                ),
            ]
        ),
    )

    # Restart the charge point to apply the changes
    test_controller.stop()
    test_controller.start()
    charge_point_v201 = await central_system_v201.wait_for_chargepoint(
        wait_for_bootnotification=True
    )

    # After restart, the values should be set to the new values
    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging_path"),
                attribute_type=AttributeEnumType.actual,
            ),
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="false",
                ),
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging_path"),
                    attribute_status=GetVariableStatusEnumType.accepted,
                    attribute_type=AttributeEnumType.actual,
                    attribute_value="/another/path",
                ),
            ]
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
async def test_set_get_variable_02(
    charge_point_v201: ChargePoint201,
    test_utility: TestUtility,
):
    """Test getting variables when access to the Everest Device Model is not allowed (pytest marker is not used here).
    This test attempts to get the `session_logging` and `session_logging_path` variables,
    which should return an unknown component status since access is not allowed.
    """
    await charge_point_v201.get_variables_req(
        get_variable_data=[
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging"),
                attribute_type=AttributeEnumType.actual,
            ),
            GetVariableDataType(
                component=ComponentType(name="EvseManager", instance="connector_1"),
                variable=VariableType(name="session_logging_path"),
                attribute_type=AttributeEnumType.actual,
            ),
        ]
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetVariables",
        call_result.GetVariables(
            get_variable_result=[
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging"),
                    attribute_status=GetVariableStatusEnumType.unknown_component,
                    attribute_type=AttributeEnumType.actual,
                ),
                GetVariableResultType(
                    component=ComponentType(name="EvseManager", instance="connector_1"),
                    variable=VariableType(name="session_logging_path"),
                    attribute_status=GetVariableStatusEnumType.unknown_component,
                    attribute_type=AttributeEnumType.actual,
                ),
            ]
        ),
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_config_adaptions(OCPP201ModuleAccessConfigurationAdjustment())
async def test_get_base_report(
    charge_point_v201: ChargePoint201, test_utility: TestUtility
):
    """Test getting a base report from the Everest Device Model.
    This test requests a full inventory report and verifies that at least one expected report data is returned.
    It checks for the `csms_ca_bundle` variable in the `EvseSecurity` component.
    """

    await charge_point_v201.get_base_report_req(
        request_id=1, report_base=ReportBaseEnumType.full_inventory
    )

    await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "GetBaseReport",
        call_result.GetBaseReport(status=GenericDeviceModelStatusEnumType.accepted),
    )

    exp_single_report_data = ReportDataType(
        component=ComponentType(name="EvseSecurity", instance="evse_security"),
        variable=VariableType(name="csms_ca_bundle"),
        variable_attribute=VariableAttributeType(
            type=AttributeEnumType.actual,
            value="ca/csms/CSMS_ROOT_CA.pem",
            mutability=MutabilityEnumType.read_only,
        ),
        variable_characteristics=VariableCharacteristicsType(
            data_type=DataEnumType.string, supports_monitoring=False
        ),
    )

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v201,
        "NotifyReport",
        call.NotifyReport(
            request_id=1,
            generated_at=datetime.now().isoformat(),
            seq_no=0,
            report_data=[exp_single_report_data],
        ),
        validate_notify_report_data_201,
    )
