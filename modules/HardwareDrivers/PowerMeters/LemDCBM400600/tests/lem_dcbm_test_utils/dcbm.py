import asyncio
import logging
import ssl
from datetime import datetime, timezone
from http.client import HTTPSConnection
from urllib.parse import urlparse

import requests
from pydantic import BaseModel


class DCBMInterface:

    def __init__(self, host: str, port: int, enable_tls=False):
        self.host = host
        self.port = port
        self.enable_tls = enable_tls

    @property
    def _base_url(self) -> str:
        return f"{'https' if self.enable_tls else 'http'}://{self.host}:{self.port}"

    async def wait_for_status(self, target_status: int, timeout=5, ) -> int:
        async def check():
            while requests.get(f"{self._base_url}/v1/status", verify=False).json()["status"][
                "value"] != target_status:
                await asyncio.sleep(1)
            return

        try:
            await asyncio.wait_for(check(), timeout)
            return True
        except asyncio.exceptions.TimeoutError:
            return False

    def activate_tls_via_http(self):
        if self._is_https_running():
            return
        logging.info("enabling tls on dcbm device")
        res = requests.put(f"http://{self.host}:{self.port}/v1/settings", json={
            "http": {
                "tls_on": True,
            }
        })
        assert res.ok

    def deactivate_tls_via_https(self):
        if not self._is_https_running():
            return
        logging.info("disabling tls on dcbm device")
        res = requests.put(f"https://{self.host}:{self.port}/v1/settings", json={
            "http": {
                "tls_on": False
            }
        }, verify=False)
        assert res.ok

    def _is_https_running(self) -> bool:
        try:
            HTTPS_URL = urlparse(f"https://{self.host}:{self.port}")
            ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
            ctx.set_ciphers('ALL:@SECLEVEL=1')
            connection = HTTPSConnection(HTTPS_URL.netloc, timeout=2, context=ctx)
            connection.request('HEAD', HTTPS_URL.path)
            if connection.getresponse():
                return True
            else:
                return False
        except:
            return False

    def stop_any_ongoing_transaction(self):
        ongoing = requests.get(f"{self._base_url}/v1/status", verify=False).json()["status"]["bits"][
            "transactionIsOnGoing"]
        if ongoing:
            transaction = requests.get(f"{self._base_url}/v1/legal", verify=False).json()["transactionId"]
            logging.warning(f"stopping transaction {transaction}")
            stop_res = requests.put(f"{self._base_url}/v1/legal?transactionId={transaction}", verify=False,
                                    json={
                                        "running": False
                                    })
            assert stop_res.ok
            asyncio.run(self.wait_for_status(17))

    def reset_device(self):
        """ Reset to http; stop any ongoing transaction
        """
        logging.info("reset DCBM device settings to http; stopping any transaction")
        try:
            self.deactivate_tls_via_https()
        except:
            pass
        self.stop_any_ongoing_transaction()
        self.disable_ntp()
        self.set_time(datetime.utcnow())

    def get_certificate(self) -> str:
        return requests.get(f"http://{self.host}:{self.port}/v1/certificate").json()["certificate"]

    class DCBMStatus(BaseModel):
        status: dict
        time: datetime

    def get_status(self) -> DCBMStatus:
        return self.DCBMStatus(**requests.get(f"{self._base_url}/v1/status", verify=False).json())

    def set_time(self, time: datetime):
        assert requests.put(f"{self._base_url}/v1/settings", verify=False,
                     json={"time": {
                         "utc": time.astimezone(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
                     }}).ok

    def disable_ntp(self):

        assert requests.put(f"{self._base_url}/v1/settings", verify=False,
                     json={"ntp": {
                         "servers": [
                             {
                                 "ipAddress": "",
                                 "port": 123
                             },
                             {
                                 "ipAddress": "",
                                 "port": 123
                             }
                         ],
                         "ntpActivated": False}
                     }).ok

    class DCBMNtpSettings(BaseModel):
        servers: list[dict]
        ntpActivated: bool

    def get_ntp_settings(self) -> DCBMNtpSettings:
        return self.DCBMNtpSettings(**requests.get(f"{self._base_url}/v1/settings", verify=False,
                                                   ).json()["ntp"])
