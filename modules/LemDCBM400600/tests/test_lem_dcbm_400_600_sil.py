import logging
from pathlib import Path

import pytest

from lem_dcbm_test_utils.everest import LemDCBMStandaloneEverestInstance, LecmDDCBMModuleConfig, \
    StartTransactionSuccessResponse, StopTransactionSuccessResponse


@pytest.fixture(scope="function")
def everest_test_instance(request, lem_dcbm_mock) -> LemDCBMStandaloneEverestInstance:
    """Fixture that can be used to start and stop everest-core"""
    everest_prefix = Path(request.config.getoption("--everest-prefix"))
    with LemDCBMStandaloneEverestInstance(everest_prefix=everest_prefix,
                                          config=LecmDDCBMModuleConfig(ip_address="localhost",
                                                                       port=8000)) as everest:
        yield everest


@pytest.mark.asyncio
async def test_get_powermeter(everest_test_instance):
    for i in range(2):
        logging.info(f"waiting for {i + 1}th powermeter publications")
        await everest_test_instance.probe_module.poll_next_powermeter(1.25)


def test_start_transaction(everest_test_instance):
    res = everest_test_instance.probe_module.call_powermeter_command('start_transaction',
                                                                     {"value": {
                                                                         "evse_id": "mock_evse_id",
                                                                         "transaction_id": "e2e_test_transaction",
                                                                         "identification_status": "ASSIGNED",
                                                                         "identification_flags": [],
                                                                         "identification_type": "ISO14443"
                                                                     }})

    parsed_start_result = StartTransactionSuccessResponse(**res)
    assert 48 * 60 - 3.1 < ((
        parsed_start_result.transaction_max_stop_time - parsed_start_result.transaction_min_stop_time).total_seconds() / 60) <= 48 * 60 - 2.9


def test_stop_transaction(everest_test_instance):
    res = everest_test_instance.probe_module.call_powermeter_command('stop_transaction',
                                                                     {"transaction_id": "mock_transaction_id"}
                                                                     )
    StopTransactionSuccessResponse(**res)
