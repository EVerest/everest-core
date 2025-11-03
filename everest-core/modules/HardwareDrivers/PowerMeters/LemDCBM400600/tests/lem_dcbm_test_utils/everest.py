import asyncio
import contextlib
import logging
from datetime import datetime
from pathlib import Path
from tempfile import NamedTemporaryFile

import yaml
from everest.framework import RuntimeSession
from everest.testing.core_utils.everest_core import EverestCore, Requirement
from pydantic import BaseModel, Extra, Field

from lem_dcbm_test_utils.probe_module import ProbeModule


class LecmDDCBMModuleConfig(BaseModel):
    ip_address: str
    port: int
    meter_tls_certificate: str | None = None
    ntp_server_1_ip_addr: str | None = None
    ntp_server_1_port: int | None = None
    ntp_server_2_ip_addr: str | None = None
    ntp_server_2_port: int | None = None

class StartTransactionSuccessResponse(BaseModel, extra=Extra.forbid):
    status: str = Field("OK", const=True, strict=True)
    transaction_max_stop_time: datetime
    transaction_min_stop_time: datetime


class StopTransactionSuccessResponse(BaseModel, extra=Extra.allow):
    status: str = Field("OK", const=True, strict=True)


class LemDCBMStandaloneEverestInstance(contextlib.ContextDecorator):

    def __init__(self, everest_prefix: Path, config: LecmDDCBMModuleConfig):
        self.everest_prefix = everest_prefix
        self.config = config
        self._stack = None
        self._stack = None


    def _write_config(self, target_file: Path):
        template_file = Path(__file__).parent / "../resources/config-standalone-lemdcbm400600.yaml"
        config = yaml.safe_load(template_file.read_text(encoding="utf-8"))
        module_config = config["active_modules"]["lem_dcbm_controller"]["config_module"] = {
            **config["active_modules"]["lem_dcbm_controller"]["config_module"],
            **self.config.dict(exclude_none=True)
        }

        logging.info(f"writing config ip_address={self.config.ip_address} port={self.config.port} into {target_file}")
        with target_file.open("w") as f:
            yaml.dump(config, f)

    def __enter__(self):
        self._stack = contextlib.ExitStack()
        file = Path(self._stack.enter_context(NamedTemporaryFile()).name)
        self._write_config(file)
        self._everest = EverestCore(self.everest_prefix, file)
        self._everest.start(standalone_module='probe', test_connections={
            'test_control': [Requirement('lem_dcbm_controller', 'main')]
        })
        if self._everest.status_listener.wait_for_status(3, ["ALL_MODULES_STARTED"]):
            self._everest.all_modules_started_event.set()
            logging.info("set all modules started event...")
        self._probe_module = self._create_probe_module()

        return self

    def __exit__(self, *exc):
        self._everest.stop()
        self._stack.__exit__(*exc)
        self._stack = None
        self._everest = None
        self._probe_module = None
        return False

    @property
    def probe_module(self) -> ProbeModule:
        assert self._probe_module
        return self._probe_module

    def _get_session(self) -> RuntimeSession:
        assert self._everest
        return RuntimeSession(str(self._everest.prefix_path),
                              str(self._everest.everest_config_path))

    def _create_probe_module(self) -> ProbeModule:
        session = self._get_session()
        module = ProbeModule(session)
        asyncio.run(module.wait_to_be_ready())
        return module
