# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""
Pytest plugin for network-isolated parallel ISO 15118 test execution.

This plugin integrates with pytest-xdist to:
1. Detect pre-existing veth pairs (created externally via setup-network-isolation.sh)
2. Assign a unique interface to each xdist worker via workerinput
3. Strip @pytest.mark.xdist_group(name="ISO15118") markers so those tests
   can be distributed freely across workers
4. Automatically inject a NetworkIsolationStrategy via the everest_config_strategies
   fixture override in conftest.py

When veth pairs are NOT available, the plugin is a no-op and the xdist_group
markers remain — tests fall back to sequential execution within their group.

The plugin never creates or destroys network interfaces. That is done externally
via `setup-network-isolation.sh` (which requires sudo / CAP_NET_ADMIN).
"""

import logging
import os
import subprocess
from typing import Optional

# Naming convention for veth pairs: ev_test0/ev_test0_peer, ev_test1/ev_test1_peer, ...
VETH_PREFIX = "ev_test"

# Environment variable used to pass the assigned interface from controller to workers
WORKER_INTERFACE_ENV = "EVEREST_TEST_NETWORK_INTERFACE"

# The xdist_group name used by ISO 15118 tests
ISO15118_XDIST_GROUP = "ISO15118"


def interface_exists(name: str) -> bool:
    """Check if a network interface exists."""
    result = subprocess.run(
        ["ip", "link", "show", name],
        capture_output=True, text=True, check=False,
    )
    return result.returncode == 0


def get_worker_index(worker_id: str) -> Optional[int]:
    """Extract numeric index from xdist worker_id (e.g., 'gw0' -> 0).

    Returns None if not running under xdist (worker_id == 'master').
    """
    if worker_id == "master":
        return None
    # worker_id format: "gw0", "gw1", etc.
    return int(worker_id.replace("gw", ""))


def get_worker_interface() -> Optional[str]:
    """Get the network interface assigned to the current xdist worker.

    Returns None if network isolation is not active.
    """
    return os.environ.get(WORKER_INTERFACE_ENV)


class NetworkIsolationPlugin:
    """Pytest plugin that detects pre-existing veth pairs and enables parallel
    ISO 15118 test execution.

    Behavior:
        --network-isolation passed AND veth pairs exist:
            -> Adopts interfaces, assigns one per worker, strips xdist_group markers
        --network-isolation passed but NO veth pairs:
            -> Logs a warning, xdist_group markers stay (sequential fallback)
        --network-isolation NOT passed:
            -> Plugin is not registered, everything works as before
    """

    def __init__(self):
        self._num_workers: int = 0
        self._active = False  # True only if interfaces were found

    @staticmethod
    def register(config):
        """Register this plugin with pytest if --network-isolation is passed."""
        if config.getoption("--network-isolation", default=False):
            plugin = NetworkIsolationPlugin()
            config.pluginmanager.register(plugin, "network_isolation")

    def pytest_configure_node(self, node):
        """Called on the controller for each xdist worker node.

        Passes the assigned interface name via workerinput.
        """
        if not self._active:
            return
        worker_id = node.workerinput["workerid"]
        idx = get_worker_index(worker_id)
        if idx is not None and idx < self._num_workers:
            interface = f"{VETH_PREFIX}{idx}"
            node.workerinput["network_interface"] = interface

    @staticmethod
    def pytest_configure(config):
        """On worker nodes, read the assigned interface and set the env var.

        xdist workers have `config.workerinput` populated by the controller's
        `pytest_configure_node` hook.
        """
        workerinput = getattr(config, "workerinput", None)
        if workerinput and "network_interface" in workerinput:
            interface = workerinput["network_interface"]
            os.environ[WORKER_INTERFACE_ENV] = interface

    def pytest_sessionstart(self, session):
        """Detect and adopt pre-existing veth pairs at session start."""
        # Determine worker count
        num_workers = session.config.getoption("numprocesses", default=None)
        if num_workers is None or num_workers == 0:
            num_workers = os.cpu_count() or 4
        if isinstance(num_workers, str):
            num_workers = os.cpu_count() or 4 if num_workers == "auto" else int(num_workers)

        # Check if interfaces exist (created by setup-network-isolation.sh)
        first_iface = f"{VETH_PREFIX}0"
        if not interface_exists(first_iface):
            logging.warning(
                "--network-isolation was passed but no veth interfaces found "
                "(expected %s). ISO 15118 tests will fall back to sequential "
                "execution via xdist_group. Run 'sudo ./setup-network-isolation.sh "
                "setup %d' first to enable parallel execution.",
                first_iface,
                num_workers,
            )
            return

        self._num_workers = num_workers
        self._active = True

    @staticmethod
    def _is_iso15118_xdist_marker(marker) -> bool:
        """Check if a marker is @pytest.mark.xdist_group(name="ISO15118")."""
        return (
            marker.name == "xdist_group"
            and (
                marker.kwargs.get("name") == ISO15118_XDIST_GROUP
                or (marker.args and marker.args[0] == ISO15118_XDIST_GROUP)
            )
        )

    def _strip_marker_from_node(self, node) -> bool:
        """Remove ISO15118 xdist_group marker from a single node. Returns True if removed."""
        original_len = len(node.own_markers)
        node.own_markers = [
            m for m in node.own_markers
            if not self._is_iso15118_xdist_marker(m)
        ]
        return len(node.own_markers) < original_len

    def pytest_collection_modifyitems(self, config, items):
        """Remove xdist_group('ISO15118') markers when network isolation is active.

        This allows ISO 15118 tests to be distributed freely across workers
        instead of being forced into a single sequential group.

        Markers can live on test functions, classes, or modules, so we strip
        from the item and all its parent nodes.
        """
        if not self._active:
            return

        stripped_count = 0
        processed_parents = set()

        for item in items:
            # Check if this item inherits an ISO15118 xdist_group marker
            has_iso_group = any(
                self._is_iso15118_xdist_marker(m)
                for m in item.iter_markers("xdist_group")
            )
            if not has_iso_group:
                continue

            # Strip from the item itself
            self._strip_marker_from_node(item)

            # Strip from parent nodes (class, module) — but only once per parent
            parent = item.parent
            while parent is not None:
                parent_id = id(parent)
                if parent_id not in processed_parents:
                    processed_parents.add(parent_id)
                    self._strip_marker_from_node(parent)
                parent = parent.parent

            stripped_count += 1

