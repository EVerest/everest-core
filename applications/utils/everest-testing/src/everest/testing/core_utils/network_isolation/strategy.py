# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""
EverestConfigAdjustmentStrategy that rewrites the `device` field in ISO 15118
modules to use a specific network interface, enabling parallel test execution.

Usage:
    @pytest.mark.everest_config_adaptions(NetworkIsolationStrategy("ev_test0"))
"""

from copy import deepcopy
from typing import Dict

from everest.testing.core_utils import EverestConfigAdjustmentStrategy

# Module types that use the `device` config field for ISO 15118 communication
ISO15118_MODULE_TYPES = frozenset({"EvseV2G", "Evse15118D20", "PyEvJosev"})


class NetworkIsolationStrategy(EverestConfigAdjustmentStrategy):
    """Rewrites `device` in ISO 15118 modules to use a dedicated network interface.

    This enables multiple ISO 15118 test sessions to run in parallel by assigning
    each test to a separate virtual ethernet (veth) interface, avoiding port and
    multicast conflicts.

    Args:
        interface_name: The network interface name to assign (e.g., "ev_test0").
    """

    def __init__(self, interface_name: str):
        self._interface_name = interface_name

    def adjust_everest_configuration(self, everest_config: Dict) -> Dict:
        adjusted_config = deepcopy(everest_config)

        for module_id, module_def in adjusted_config.get("active_modules", {}).items():
            module_type = module_def.get("module", "")
            if module_type in ISO15118_MODULE_TYPES:
                config_module = module_def.get("config_module", {})
                if "device" in config_module:
                    config_module["device"] = self._interface_name

        return adjusted_config
