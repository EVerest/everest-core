from copy import deepcopy
from typing import Dict, List, Optional

from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils._configuration.everest_configuration_strategies.everest_configuration_strategy import \
    EverestConfigAdjustmentStrategy


class ProbeModuleConfigurationStrategy(EverestConfigAdjustmentStrategy):
    """ Adjusts the Everest configuration by adding the probe module into an EVerest config """

    def __init__(self,
                 connections: Dict[str, List[Requirement]],
                 module_id: str = "probe",
                 access: Optional[Dict] = None
                 ):
        self._module_id = module_id
        self._connections = connections
        self._access = access

    def adjust_everest_configuration(self, everest_config: Dict) -> Dict:
        adjusted_config = deepcopy(everest_config)

        probe_connections = {
            requirement_id: [{"module_id": requirement.module_id, "implementation_id": requirement.implementation_id}
                             for requirement in requirements_list]
            for requirement_id, requirements_list in self._connections.items()}

        active_modules = adjusted_config.setdefault("active_modules", {})
        probe_config = {
            'connections': probe_connections,
            'module': 'ProbeModule'
        }
        if self._access is not None:
            probe_config['access'] = self._access

        active_modules[self._module_id] = probe_config

        return adjusted_config
