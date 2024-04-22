import asyncio
import logging
import re
from datetime import timedelta, datetime, timezone
from pathlib import Path

import pytest

from lem_dcbm_test_utils.dcbm import DCBMInterface
from lem_dcbm_test_utils.everest import LemDCBMStandaloneEverestInstance, LecmDDCBMModuleConfig, \
    StartTransactionSuccessResponse, StopTransactionSuccessResponse
from lem_dcbm_test_utils.types import Powermeter


@pytest.fixture()
def dcbm_host(request) -> str:
    host = request.config.getoption("--lem-dcbm-host")
    assert host
    return host


@pytest.fixture()
def dcbm_port(request) -> int:
    port = int(request.config.getoption("--lem-dcbm-port"))
    assert port
    return port


@pytest.fixture(scope="function")
def everest_test_instance(request, dcbm_host, dcbm_port, dcbm) -> LemDCBMStandaloneEverestInstance:
    """Fixture that can be used to start and stop everest-core"""
    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    try:
        dcbm.reset_device()
        with LemDCBMStandaloneEverestInstance(everest_prefix=everest_prefix,
                                              config=LecmDDCBMModuleConfig(ip_address=dcbm_host,
                                                                           port=dcbm_port)) as everest:
            yield everest
    finally:
        dcbm.reset_device()


@pytest.fixture(scope="function")
def everest_test_instance_ntp_configured(request, dcbm_host, dcbm_port, dcbm) -> LemDCBMStandaloneEverestInstance:
    """Fixture that can be used to start and stop everest-core"""
    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    try:
        dcbm.reset_device()
        with LemDCBMStandaloneEverestInstance(everest_prefix=everest_prefix,
                                              config=LecmDDCBMModuleConfig(ip_address=dcbm_host,
                                                                           port=dcbm_port,
                                                                           ntp_server_1_ip_addr="test_ntp_1",
                                                                           ntp_server_1_port=124,
                                                                           ntp_server_2_ip_addr="test_ntp_2",
                                                                           ntp_server_2_port=125
                                                                           )) as everest:
            yield everest
    finally:
        dcbm.reset_device()


@pytest.fixture(scope="function")
def everest_test_instance_tls(request, dcbm_host, dcbm_port, dcbm) -> LemDCBMStandaloneEverestInstance:
    """Fixture that can be used to start and stop everest-core"""
    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    try:
        dcbm.reset_device()
        certificate = dcbm.get_certificate()
        certificate = certificate.replace("-----BEGIN CERTIFICATE-----", "-----BEGIN CERTIFICATE-----\n").replace(
            "-----END CERTIFICATE-----", "\n-----END CERTIFICATE-----")
        dcbm.activate_tls_via_http()
        with LemDCBMStandaloneEverestInstance(everest_prefix=everest_prefix,
                                              config=LecmDDCBMModuleConfig(ip_address=dcbm_host, port=dcbm_port,
                                                                           meter_tls_certificate=certificate)) as everest:
            yield everest
    finally:
        dcbm.reset_device()


@pytest.fixture()
def dcbm(dcbm_host, dcbm_port):
    dcbm = DCBMInterface(host=dcbm_host, port=dcbm_port)
    return dcbm


@pytest.mark.asyncio
async def test_lem_dcbm_e2e_powermeter_does_regular_publish(everest_test_instance, dcbm):
    for i in range(2):
        logging.info(f"waiting for {i + 1}th powermeter publications")
        await everest_test_instance.probe_module.poll_next_powermeter(1.25)


@pytest.mark.asyncio
async def test_lem_dcbm_e2e_powermeter_meterid_correct(everest_test_instance):
    power_meter: Powermeter = await everest_test_instance.probe_module.poll_next_powermeter(1.25)
    assert re.match(
        r"^\d+$", power_meter.meter_id), f"got unexpected meter_id {power_meter.meter_id}"


@pytest.mark.asyncio
async def test_lem_dcbm_e2e_start_stop_transaction(everest_test_instance, dcbm):
    assert await dcbm.wait_for_status(17), "device has invalid status before transaction start"

    start_result = everest_test_instance.probe_module.call_powermeter_command('start_transaction',
                                                                              {"value": {
                                                                                  "evse_id": "mock_evse_id",
                                                                                  "transaction_id": "e2e_test_transaction",
                                                                                  "identification_status": "ASSIGNED",
                                                                                  "identification_flags": []
                                                                              }})

    parsed_start_result = StartTransactionSuccessResponse(**start_result)
    assert 48 * 60 - 3.1 < ((
        parsed_start_result.transaction_max_stop_time - parsed_start_result.transaction_min_stop_time).total_seconds() / 60) <= 48 * 60 - 2.9

    logging.info("started transaction 'e2e_test_transaction'")

    assert await dcbm.wait_for_status(21), "device has invalid status after transaction start"

    stop_result = everest_test_instance.probe_module.call_powermeter_command('stop_transaction',
                                                                             {"transaction_id": "e2e_test_transaction"}
                                                                             )

    StopTransactionSuccessResponse(**stop_result)

    logging.info("stopped transaction 'e2e_test_transaction'")

    assert await dcbm.wait_for_status(17), "device has invalid status after transaction stop"


@pytest.mark.asyncio
async def test_lem_dcbm_e2e_time_sync(everest_test_instance, dcbm):
    """ Check time gets synced per default

    :param everest_test_instance:
    :param dcbm:
    :return:
    """

    # start transaction to enforce early sync; tidied up by fixture
    assert everest_test_instance.probe_module.call_powermeter_command('start_transaction',
                                                                      {"value": {
                                                                          "evse_id": "mock_evse_id",
                                                                          "transaction_id": "e2e_test_transaction",
                                                                          "identification_status": "ASSIGNED",
                                                                          "identification_flags": [],
                                                                          "identification_type": "ISO14443"
                                                                      }})["status"] == "OK"

    dcbm.set_time(datetime.now() - timedelta(days=365))

    async def check_time():
        while ((dcbm.get_status().time.astimezone(timezone.utc) - datetime.now(timezone.utc)).total_seconds() > 60):
            await asyncio.sleep(0.25)

    await asyncio.wait_for(check_time(), 2)


@pytest.mark.asyncio
async def test_lem_dcbm_e2e_ntp_setup(everest_test_instance_ntp_configured, dcbm):
    """ Test ntp is setup correctly and activated if configured. """

    # start transaction to enforce early sync; tidied up by fixture
    assert everest_test_instance_ntp_configured.probe_module.call_powermeter_command('start_transaction',
                                                                                     {"value": {
                                                                                         "evse_id": "mock_evse_id",
                                                                                         "transaction_id": "e2e_test_transaction",
                                                                                         "identification_status": "ASSIGNED",
                                                                                         "identification_flags": [],
                                                                                         "identification_type": "ISO14443"
                                                                                     }})["status"] == "OK"

    async def check():
        while not (ntp_settings := dcbm.get_ntp_settings()).ntpActivated:
            await asyncio.sleep(0.25)
        return ntp_settings

    ntp_settings = await asyncio.wait_for(check(), timeout=2)
    assert ntp_settings.ntpActivated is True
    assert ntp_settings.servers == [{
        "ipAddress": "test_ntp_1",
        "port": 124
    },
        {
            "ipAddress": "test_ntp_2",
            "port": 125
    }]


@pytest.mark.asyncio
async def test_lem_dcbm_2e_get_powermeter_tls(everest_test_instance_tls):
    power_meter: Powermeter = await everest_test_instance_tls.probe_module.poll_next_powermeter(1.25)
    assert re.match(
        r"^\d+$", power_meter.meter_id), f"got unexpected meter_id {power_meter.meter_id}"
