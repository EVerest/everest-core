from copy import deepcopy
from typing import Dict, List

from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils._configuration.everest_configuration_strategies.everest_configuration_strategy import EverestConfigAdjustmentStrategy


class ProbeModuleConfigurationStrategy(EverestConfigAdjustmentStrategy):
    """ Adjusts the Everest configuration by adding the probe module into an EVerest config """

    def __init__(self,
                 connections: Dict[str, List[Requirement]],
                 module_id: str = "probe"
                 ):
        self._module_id = module_id
        self._connections = connections

    def adjust_everest_configuration(self, everest_config: Dict) -> Dict:
        adjusted_config = deepcopy(everest_config)

        probe_connections = {}
        for k, v in self._connections.items():
            probe_connections[k] = [{"module_id": v.module_id, "implementation_id": v.implementation_id}]

        active_modules = adjusted_config.setdefault("active_modules", {})
        active_modules[self._module_id] = {
            'connections': probe_connections,
            'module': 'ProbeModule'
        }

        return adjusted_config
