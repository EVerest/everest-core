from copy import deepcopy
from dataclasses import dataclass, asdict
from typing import Union, Dict

from everest.testing.core_utils.common import OCPPVersion
from everest.testing.core_utils._configuration.everest_configuration_strategies.everest_configuration_strategy import EverestConfigAdjustmentStrategy



@dataclass
class OCPPModuleConfigurationBase:
    MessageLogPath: str

@dataclass
class OCPPModulePaths16(OCPPModuleConfigurationBase):
    ChargePointConfigPath: str
    UserConfigPath: str
    DatabasePath: str

@dataclass
class OCPPModulePaths201(OCPPModuleConfigurationBase):
    DeviceModelConfigPath: str
    CoreDatabasePath: str
    DeviceModelDatabasePath: str


class OCPPModuleConfigurationStrategy(EverestConfigAdjustmentStrategy):
    """ Adjusts the Everest configuration by manipulating the OCPP module configuration to use proper (temporary test) paths.

    """


    def __init__(self, ocpp_paths: Union[OCPPModulePaths16, OCPPModulePaths201],
                 ocpp_module_id: str,
                 ocpp_version: OCPPVersion):
        self._ocpp_paths = ocpp_paths
        self._ocpp_module_id = ocpp_module_id
        self._ocpp_version = ocpp_version

    def adjust_everest_configuration(self, everest_config: Dict):
        """ Changes the provided configuration of the Everest "OCPP" module .

        Creates the TEST_LOGS_DIR if not existent
        """

        adjusted_config = deepcopy(everest_config)

        self._verify_module_config(adjusted_config)

        module_config = adjusted_config["active_modules"][self._ocpp_module_id]

        module_config["config_module"] = {**module_config["config_module"],
                                          **asdict(self._ocpp_paths)}

        return adjusted_config

    def _verify_module_config(self, everest_config):
        """ Verify the provided config fits the provided OCPP version """
        assert "active_modules" in everest_config and self._ocpp_module_id in everest_config[
            "active_modules"], "OCPP Module is missing from EVerest config"
        ocpp_module = everest_config["active_modules"][self._ocpp_module_id]["module"]
        assert (ocpp_module == "OCPP" and self._ocpp_version == OCPPVersion.ocpp16) or (
                ocpp_module == "OCPP201" and self._ocpp_version == OCPPVersion.ocpp201), \
            f"Invalid OCCP Module {ocpp_module} for provided OCCP version {self._ocpp_version}"


