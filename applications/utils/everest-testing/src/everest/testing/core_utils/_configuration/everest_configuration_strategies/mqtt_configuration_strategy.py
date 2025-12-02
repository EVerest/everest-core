from copy import deepcopy
from typing import Dict

from everest.testing.core_utils._configuration.everest_configuration_strategies.everest_configuration_strategy import \
    EverestConfigAdjustmentStrategy


class EverestMqttConfigurationAdjustmentStrategy(EverestConfigAdjustmentStrategy):
    """ Adjusts the Everest configuration by manipulating the "settings" block to use the prober Everest UUID and
    external prefix.

    """

    def __init__(self, everest_uuid: str, mqtt_external_prefix: str):
        self._everest_uuid = everest_uuid
        self._mqtt_external_prefix = mqtt_external_prefix

    def _find_jscarv2g_module_ids(self, config: Dict):
        return [k for k, v in config["active_modules"].items()
                if v.get("module") == "JsCarV2G"]

    def adjust_everest_configuration(self, config: Dict) -> Dict:
        adjusted_everest_config = deepcopy(config)
        adjusted_everest_config["settings"] = {}
        adjusted_everest_config["settings"]["mqtt_everest_prefix"] = f"everest_{self._everest_uuid}"
        adjusted_everest_config["settings"]["mqtt_external_prefix"] = self._mqtt_external_prefix
        adjusted_everest_config["settings"]["telemetry_prefix"] = f"telemetry_{self._everest_uuid}"

        # make sure controller starts with a dynamic port
        adjusted_everest_config["settings"]["controller_port"] = 0

        for car_module_id in self._find_jscarv2g_module_ids(adjusted_everest_config):
            adjusted_everest_config["active_modules"][car_module_id]\
                .setdefault("config_implementation",{})\
                .setdefault("main", {})["mqtt_prefix"] = self._mqtt_external_prefix

        return adjusted_everest_config
