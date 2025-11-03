import pytest
import pytest_asyncio

from copy import deepcopy
from typing import Dict, List

from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.core_utils.probe_module import ProbeModule
from everest.testing.core_utils import EverestConfigAdjustmentStrategy


@pytest.fixture
def probe_module(started_test_controller, everest_core) -> ProbeModule:
    # initiate the probe module, connecting to the same runtime session the test controller started
    module = ProbeModule(everest_core.get_runtime_session())

    return module


@pytest_asyncio.fixture
async def chargepoint_with_pm(central_system: CentralSystem, probe_module: ProbeModule):
    """Fixture for ChargePoint201. Requires central_system_v201
    """
    # wait for libocpp to go online
    cp = await central_system.wait_for_chargepoint()
    yield cp
    await cp.stop()


class ProbeModuleCostAndPriceMetervaluesConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    """
    Probe module to be able to 'inject' metervalues
    """
    def __init__(self, evse_manager_ids: List[str]):
        self.evse_manager_ids = evse_manager_ids

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["grid_connection_point"]["connections"]["powermeter"] = [
            {"module_id": "probe", "implementation_id": "ProbeModulePowerMeter"}]

        for evse_manager_id in self.evse_manager_ids:
            adjusted_config["active_modules"][evse_manager_id]["connections"]["powermeter_grid_side"] = [
                {"module_id": "probe", "implementation_id": "ProbeModulePowerMeter"}]

        return adjusted_config


class ProbeModuleCostAndPriceDisplayMessageConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    """
    Probe module to be able to mock display messages
    """

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["ocpp"]["connections"]["display_message"] = [
            {"module_id": "probe", "implementation_id": "ProbeModuleDisplayMessage"}]

        return adjusted_config


class ProbeModuleCostAndPriceSessionCostConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    """
    Probe module to be able to mock the session cost interface calls
    """

    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["probe"]["connections"]["session_cost"] = [
            {"module_id": "ocpp", "implementation_id": "session_cost"}]

        return adjusted_config
