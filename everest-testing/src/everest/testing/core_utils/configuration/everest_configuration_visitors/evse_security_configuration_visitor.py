from copy import deepcopy
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Dict, Optional, Union

from everest.testing.core_utils.configuration.everest_configuration_visitors.everest_configuration_visitor import \
    EverestConfigAdjustmentVisitor


@dataclass
class EvseSecurityModuleConfiguration:
    csms_ca_bundle: Optional[str] = None
    mf_ca_bundle: Optional[str] = None
    mo_ca_bundle: Optional[str] = None
    v2g_ca_bundle: Optional[str] = None
    csms_leaf_cert_directory: Optional[str] = None
    csms_leaf_key_directory: Optional[str] = None
    secc_leaf_cert_directory: Optional[str] = None
    secc_leaf_key_directory: Optional[str] = None
    private_key_password: Optional[str] = None


class EvseSecurityModuleConfigurationVisitor(EverestConfigAdjustmentVisitor):
    """ Adjusts the Evse security module configuration in the Everest configuration merging a provided configuration into it (if provided) and adapt
    all paths relative to a certificate target directory (if provided).
    """

    def __init__(self,
                 configuration: Optional[EvseSecurityModuleConfiguration] = None,
                 target_certificates_directory: Optional[Path] = None,
                 source_certificates_directory: Optional[Path] = None,
                 module_id: Optional[str] = None
                 ):
        """

        Args:
            configuration: module configuration. If provided. this will be merged into the template configuration (meaning None values are ignored/taken from the originally provided Everest configuration)
            module_id: Id of security module; if None, auto-detected by module type "EvseSecurity"
            target_certificates_directory: If provided, all configured certificate directories/paths will be changed to point into this folder
            source_certificates_directory: If provided, configured certificate directories/paths will be considered relative to this directory; each relative part is appended to the corresponding target paths
        """
        self._security_module_id = module_id
        self._configuration = configuration
        self._target_certificates_directory = target_certificates_directory
        self._source_certificates_directory = source_certificates_directory

    def _move_paths(self, module_config: Dict):
        def _move(p: Union[str, Path]):
            if self._source_certificates_directory and Path(p).is_relative_to(self._source_certificates_directory):
                p = Path(p).relative_to(self._source_certificates_directory)
            return str(self._target_certificates_directory / p)

        for k in {"csms_ca_bundle",
                   "mf_ca_bundle",
                   "mo_ca_bundle",
                   "v2g_ca_bundle",
                   "csms_leaf_cert_directory",
                   "csms_leaf_key_directory",
                   "secc_leaf_cert_directory",
                   "secc_leaf_key_directory"}:
            if module_config["config_module"].get(k):
                module_config["config_module"][k] = _move(module_config["config_module"][k])

    def _determine_module_id(self, everest_config: Dict):
        if self._security_module_id:
            assert self._security_module_id in everest_config[
                "active_modules"], f"Module id {self._security_module_id} not found in EVerest configuration"
            return self._security_module_id
        else:
            try:
                return next(k for k, v in everest_config["active_modules"].items() if v["module"] == "EvseSecurity")
            except StopIteration:
                raise ValueError("No EvseSecurity module found in EVerest configuration")

    def adjust_everest_configuration(self, everest_config: Dict):

        adjusted_config = deepcopy(everest_config)

        module_cfg = adjusted_config["active_modules"][self._determine_module_id(adjusted_config)]

        if self._configuration:
            module_cfg["config_module"] = {**module_cfg["config_module"],
                                           **{k:v for k,v in asdict(self._configuration).items()
                                              if v is not None}}

        if self._target_certificates_directory:
            self._move_paths(module_cfg)

        return adjusted_config
