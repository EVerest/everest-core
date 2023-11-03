from copy import deepcopy
from pathlib import Path
from typing import Dict, Optional

from everest.testing.core_utils.configuration.everest_configuration_visitors.everest_configuration_visitor import \
    EverestConfigAdjustmentVisitor


class PersistentStoreConfigurationVisitor(EverestConfigAdjustmentVisitor):
    """ Adjusts the Everest configuration by manipulating the PersistentStore module configuration to point
    to the desired (temporary) storage

    """

    def __init__(self,
                 sqlite_db_file_path: Path,
                 module_id: Optional[str] = None):
        """

        Args:
            sqlite_db_file_path: database to be used in configuration
            module_id: Id of security module; if None, auto-detected by module type "EvseSecurity
        """
        self._module_id = module_id
        self._sqlite_db_file_path = sqlite_db_file_path

    def _determine_module_id(self, everest_config: Dict):
        if self._module_id:
            assert self._module_id in everest_config[
                "active_modules"], f"Module id {self._module_id} not found in EVerest configuration"
            return self._module_id
        else:
            try:
                return next(k for k, v in everest_config["active_modules"].items() if v["module"] == "PersistentStore")
            except StopIteration:
                raise ValueError("No PersistentStore module found in EVerest configuration")

    def adjust_everest_configuration(self, everest_config: Dict):

        adjusted_config = deepcopy(everest_config)

        module_cfg = adjusted_config["active_modules"][self._determine_module_id(adjusted_config)]

        module_cfg.setdefault("config_module", {})["sqlite_db_file_path"] = str(self._sqlite_db_file_path)

        return adjusted_config
