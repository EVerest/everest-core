# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""Shared fixtures for EEBus reference control box tests."""

import os
import signal
import socket
import subprocess
import time
from pathlib import Path

import pytest
from copy import deepcopy
from everest.testing.core_utils._configuration.everest_configuration_strategies.everest_configuration_strategy import (
    EverestConfigAdjustmentStrategy,
)

CERT_DIR = Path(__file__).parent
CONTROLBOX_CERT_SKI = "5f4db7163af56b9f24ccf9eac9c1758bcde762de"


class EebusPortStrategy(EverestConfigAdjustmentStrategy):
    """Rewrites grpc_port and eebus_service_port in EEBUS module configs
    to use dynamically allocated ports, enabling parallel test execution."""

    def __init__(self, grpc_port: int, eebus_service_port: int):
        self._grpc_port = grpc_port
        self._eebus_service_port = eebus_service_port

    def adjust_everest_configuration(self, everest_config: dict) -> dict:
        adjusted = deepcopy(everest_config)
        for module_def in adjusted.get("active_modules", {}).values():
            if module_def.get("module") != "EEBUS":
                continue
            config = module_def.setdefault("config_module", {})
            config["grpc_port"] = self._grpc_port
            config["eebus_service_port"] = self._eebus_service_port
        return adjusted


class EebusModuleConfigStrategy(EverestConfigAdjustmentStrategy):
    """Applies a dict of config_module overrides to every EEBUS module in the
    test config. Lets individual tests customize the allowlist / accept_unknown
    flag / etc. without maintaining a separate yaml per scenario.

    Keys set to ``None`` are removed from the module config (lets a test drop
    keys set to "" drop the override so the base yaml value applies).
    """

    def __init__(self, overrides: dict):
        self._overrides = overrides

    def adjust_everest_configuration(self, everest_config: dict) -> dict:
        adjusted = deepcopy(everest_config)
        for module_def in adjusted.get("active_modules", {}).values():
            if module_def.get("module") != "EEBUS":
                continue
            config = module_def.setdefault("config_module", {})
            for key, value in self._overrides.items():
                if value is None:
                    config.pop(key, None)
                else:
                    config[key] = value
        return adjusted


def get_free_port() -> int:
    """Return an unused TCP port."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("", 0))
        return s.getsockname()[1]


@pytest.fixture
def eebus_grpc_port():
    """Allocate a free port for the EEBUS gRPC control service."""
    return get_free_port()


@pytest.fixture
def eebus_service_port():
    """Allocate a free port for the EEBUS SHIP service."""
    return get_free_port()


@pytest.fixture
def everest_config_strategies(request, eebus_grpc_port, eebus_service_port):
    """Override base fixture to inject EebusPortStrategy with dynamic ports.

    This replaces the base everest_config_strategies from
    everest.testing.core_utils.fixtures, adding EEBUS-specific port
    rewriting. NetworkIsolationStrategy is not needed for EEBUS tests.
    """
    strategies = []
    marker = request.node.get_closest_marker("everest_config_adaptions")
    if marker:
        for v in marker.args:
            assert isinstance(v, EverestConfigAdjustmentStrategy), (
                "Arguments to 'everest_config_adaptions' must all be "
                "instances of EverestConfigAdjustmentStrategy"
            )
            strategies.append(v)
    strategies.append(EebusPortStrategy(eebus_grpc_port, eebus_service_port))
    return strategies


def extract_ski_from_cert(cert_path: Path) -> str:
    """Extract Subject Key Identifier from a PEM certificate file."""
    from cryptography import x509
    cert = x509.load_pem_x509_certificate(cert_path.read_bytes())
    ski_ext = cert.extensions.get_extension_for_class(x509.SubjectKeyIdentifier)
    return ski_ext.value.digest.hex()


def read_everest_ski(everest_prefix: Path) -> str:
    """Read EVerest's EEBUS SKI from the installed cert directory."""
    ski_file = everest_prefix / "etc" / "everest" / "certs" / "eebus" / "evse_ski"
    if ski_file.exists():
        return ski_file.read_text().strip()
    # SKI file may not exist if certs are runtime-generated; extract from cert.
    cert_file = everest_prefix / "etc" / "everest" / "certs" / "eebus" / "evse_cert"
    if cert_file.exists():
        return extract_ski_from_cert(cert_file)
    raise FileNotFoundError(
        f"Neither SKI file ({ski_file}) nor certificate ({cert_file}) found"
    )


def wait_for_everest_ski(everest_prefix: Path, timeout: float = 10) -> str:
    """Wait for EVerest's EEBUS cert to appear and return the SKI.

    The sidecar generates certs synchronously before starting the gRPC
    server, so this polls until the cert file exists.
    """
    cert_file = everest_prefix / "etc" / "everest" / "certs" / "eebus" / "evse_cert"
    deadline = time.monotonic() + timeout
    while not cert_file.exists() and time.monotonic() < deadline:
        time.sleep(0.5)
    return read_everest_ski(everest_prefix)


class ReferenceControlBox:
    """Manage the eebus-go controlbox binary lifecycle."""

    def __init__(self, binary: Path, port: int, remote_ski: str,
                 cert: Path = CERT_DIR / "eebus.crt",
                 key: Path = CERT_DIR / "eebus.key"):
        self.binary = binary
        self.port = port
        self.remote_ski = remote_ski
        self.cert = cert
        self.key = key
        self.process = None

    def start(self, timeout: float = 5.0):
        self.process = subprocess.Popen(
            [str(self.binary), str(self.port), self.remote_ski,
             str(self.cert), str(self.key)],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            preexec_fn=os.setsid,
        )
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if self.process.poll() is not None:
                raise RuntimeError(f"controlbox exited with code {self.process.returncode}")
            if self._port_listening():
                return
            time.sleep(0.2)
        raise RuntimeError(f"controlbox not listening on port {self.port} after {timeout}s")

    def stop(self):
        if self.process and self.process.poll() is None:
            os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
            try:
                self.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(os.getpgid(self.process.pid), signal.SIGKILL)
                self.process.wait(timeout=2)
        self.process = None

    def is_running(self) -> bool:
        return self.process is not None and self.process.poll() is None

    def _port_listening(self) -> bool:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            return s.connect_ex(("localhost", self.port)) == 0


@pytest.fixture(scope="session")
def controlbox_binary():
    """Locate the controlbox binary or skip."""
    candidates = [
        Path(__file__).parent / "controlbox",
        Path.home() / "go" / "bin" / "controlbox",
    ]
    for p in candidates:
        if p.exists() and os.access(p, os.X_OK):
            return p
    pytest.skip("controlbox binary not found")


@pytest.fixture
def reference_control_box(request, controlbox_binary):
    """Create and yield a ReferenceControlBox, stopping it on teardown.

    The control box is NOT auto-started because the SKI may only become
    available after EVerest starts and the sidecar generates certs.
    Call box.start() in the test body after extracting the SKI.
    """
    port = get_free_port()
    # remote_ski will be set by the test before calling start()
    box = ReferenceControlBox(controlbox_binary, port, remote_ski="")
    yield box
    box.stop()
