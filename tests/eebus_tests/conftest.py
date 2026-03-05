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

CERT_DIR = Path(__file__).parent
CONTROLBOX_CERT_SKI = "5f4db7163af56b9f24ccf9eac9c1758bcde762de"


def get_free_port() -> int:
    """Return an unused TCP port."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("", 0))
        return s.getsockname()[1]


def read_everest_ski(everest_prefix: Path) -> str:
    """Read EVerest's EEBUS SKI from the installed cert directory."""
    ski_file = everest_prefix / "etc" / "everest" / "certs" / "eebus" / "evse_ski"
    if not ski_file.exists():
        raise FileNotFoundError(f"EVerest SKI file not found at {ski_file}")
    return ski_file.read_text().strip()


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


@pytest.fixture(scope="session")
def everest_ski(request):
    """Read EVerest's EEBUS SKI so the control box can trust it."""
    prefix = Path(request.config.getoption("--everest-prefix")).resolve()
    return read_everest_ski(prefix)


@pytest.fixture
def reference_control_box(controlbox_binary, everest_ski):
    """Start the eebus-go controlbox on a free port, stop on teardown.

    The controlbox is given EVerest's SKI as the remote-ski argument so that
    the SHIP trust handshake can complete.
    """
    port = get_free_port()
    box = ReferenceControlBox(controlbox_binary, port, remote_ski=everest_ski)
    box.start()
    yield box
    box.stop()
